/*=========================================================================================================================================

This is flow_erosion.cpp: it calculates erosion by overland flow for RillGrow

Copyright (C) 2025 David Favis-Mortlock

==========================================================================================================================================

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
#include "rg.h"
#include "simulation.h"
#include "cell.h"

//=========================================================================================================================================
//! This method erodes a cell as a result of downhill flow; it uses a probabilistic equation from Nearing (1991)
//=========================================================================================================================================
void CSimulation::DoCellFlowErosion(int const nX, int const nY, int const nLowX, int const nLowY, int const nDirection, double const dS, double const dHLen, double const dVel, double const dSedimentLoadWt, double const dMoveDepth)
{
   /*
   For detachment, use the relationship:

      e = K C P (h**0.5) (S**1.5)                                    (1)

   which is equation 10 in Nearing, M.A. (1991). A probabilistic model of soil detachment by shallow turbulent flow. Transactions of the American Society of Agricultural Engineers 34(1), 81-85.

   In equation (1), h is depth (m), S is slope (m/m i.e. tan), P is a function of T, tau-b, S-t and S-tau-b (see below). Assume:

      K = 440.54 kg m**-3

   from Lei, T., Nearing, M.A., Haghighi, K. and Bralts, V.F. (1998). Rill erosion and morphological evolution: a simulation model. Water Resources Research 34(11), 3157-3168.

   Also in (1), C is Chezy coefficient, given by:

      C = (8 g / f)**0.5                                             (2)

   In equation (2), g is gravitational acceleration (m s**-2), and f is the Darcy-Weisbach friction factor, given by:

      f = (8 g h S) / u**2                                           (3)

   from Abrahams, A.D., Li, G. and Parsons, A.J. (1996). Rill hydraulics on a semiarid hillslope, southern Arizona. Earth Surface Processes and Landforms 21(1), 35-47.

   In equation (3), g is gravitational acceleration (m s**-2), h is depth (m), S is slope (m/m i.e. tan), and u is flow speed (m s**-1).

   Substituting (3) into (2), and the result into (1) [see Mathcad doc], get:

      e = K u P S                                                    (4)

   P is given in Nearing (1991) by:

      P = 0.5 - psi[(T - tau-b) / ((S-t**2) + (S-tau-b**2))]         (5a)

   this is for the situation where 'stress' is always less than 'strength'. Here 'stress' may exceed 'strength', so this uses a commonly-found variant from reliability analysis:

      P = 1 - psi[(T - tau-b) / sqrt((S-t**2) + (S-tau-b**2))]       (5b)

   Psi is the cumulative probability function of a standard normal deviate (here calculated using an Abramowitz and Stegun approximation).

   In (5a  or 5b):

      T = about 1100 Pa

   from Table 2 in Nearing, M.A., Parker, S.C., Bradford, J.M. and Elliot, W.J. (1991). Tensile strength of thirty-three saturated repacked soils. Soil Science Society of America Journal 55(6), 1546-1551.

      tau-b = 150 tau

   from equation 12 in Nearing (1991), and:

      tau = rho g h S

   where rho is the density of water. The coefficient of variation of T, CV-T (= standard deviation of T / mean T) was set at 0.4 by Lei et al. (1998). Thus:

      S-t = 0.4 . 1100 Pa

   The coefficient of variation of tau-b CV-tau-b (= standard deviation of tau-b / mean tau-b) was also set at 0.4 by Lei et al. (1998). Thus:

      S-tau-b = 0.4 . 150 rho g h S
   */

   double const TAUB_CONST = 150;

   // For dTau, must divide by 1000 to convert dMoveDepth to m, and divide by 1000 as m_dRho is in t/m3
   double
      dTau = m_dRho * m_dG * dMoveDepth * dS * 1e-6,
      dTaub = TAUB_CONST * dTau,
      dSTaub = m_dCVTaub * dTaub;

   // Save the shear stress
   if (m_bSlumping)
      // Allocate shear stress to this and adjacent cells
      DoDistributeShearStress(nX, nY, dTau);
   else
      // Just write the shear stress (both this-iteration and cumulative) to this cell
      m_Cell[nX][nY].pGetSoil()->IncShearStress(dTau);

/*
   // TESTING
   // m_dT = soil strength   m_dTaub = stress
   double dZ = (dTaub - m_dT) / sqrt((m_dST2)+(dSTaub*dSTaub));
   double dP = dGetCGaussianPDF(dZ);
   double dE = m_dK * dVel * dP * dS /1000;
   static bool sbFirst = true;
   if (sbFirst)
   {
      sbFirst = false;
      m_ofsLog << "m_dT" << '\t' << "m_dST" << endl;
      m_ofsLog << m_dT << '\t' << sqrt(m_dST2) << endl;
      m_ofsLog << "dTaub" << '\t' << "dSTaub" << '\t' << "dZ"  << '\t' << "dP" << '\t' << "dE" << endl;
   }
   if (0 == m_ulIter % 10)
      m_ofsLog << dTaub << '\t' << dSTaub << '\t' << dZ  << '\t' << dP << '\t' << dE << endl;
*/
   // Use Nearing's equation to calculate soil loss as detachment rate per unit area of soil surface i.e. in kg/(sec m2). First calculate the difference between burst shear stress and soil strength, normalize this to SD units then use the normalized value to sample a value from the cumulative probability distribution function of a standard Gaussian (normal) distribution
   // This value of P is thus calculated using a function similar to, but not identical with, equation 9 in Nearing (1991). The approach used here is closer to the standard relationship used in reliability analysis. Note must divide by 1000, since dVel in mm/sec
   double dE = m_dK * dVel * dGetCGaussianPDF((dTaub - m_dT) / sqrt((m_dST2) + (dSTaub * dSTaub))) * dS * 0.001;
   // assert(dE >= 0);

   // Pre-existing sediment load of 'this' cell is assumed to produce a linear decrease in detachment i.e  dE(actual) = dE * (1 - (dSedimentLoad/dTransportCapacity)) following equation 11 in Lei et al. (1998)
   dE *= dSedimentLoadWt;                                        // kg/(sec m2)

   // Get the bulk density (in kg/m3) of the topmost non-zero layer
   double dBulkDensity = m_Cell[nX][nY].pGetSoil()->dGetBulkDensityOfTopNonZeroLayer();
   if (dBulkDensity < 0)
   {
      // We are down to unerodible basement, so no sediment to be transported
      return;
   }

   // Now convert the total thickness of soil lost per sec; to be split between 'this' and 'next' cells
   double dThickLost = 1000 * dE / dBulkDensity;       // mm/sec
   // assert(dThickLost >= 0);

//   m_ofsLog << m_ulIter << " [" << nX << "][" << nY << "] dThickLost="  << dThickLost << endl;

   // Is this an edge cell?
   if (nLowX < 0)
   {
      // It is an edge cell. Do we have a baselevel?
      if (m_bHaveBaseLevel)
      {
         // We do, so make sure that we do not cut down below the baselevel
         double dTmp = m_Cell[nX][nY].pGetSoil()->dGetSoilSurfaceElevation() - m_dBaseLevel;
         if (dTmp <= 0)
            return;

         dThickLost = tMin(dTmp, dThickLost);
      }

      // Erode 'this' cell, also add to sediment load and totals
      m_Cell[nX][nY].pGetSoil()->DoFlowDetach(dThickLost, true);
   }
   else
   {
      // It is not an edge cell, so erode 'this' (source) and 'next' (destination) cells equally
      double dHalfThickLost = dThickLost * 0.5;

      // Erode 'this' cell, and add to sediment load and totals
      // assert(dHalfThickLost >= 0);
      m_Cell[nX][nY].pGetSoil()->DoFlowDetach(dHalfThickLost, false);

      // Also erode the 'next' cell, and add to sediment load and totals
      m_Cell[nLowX][nLowY].pGetSoil()->DoFlowDetach(dHalfThickLost, false);
   }

   if (m_bHeadcutRetreat)
   {
      if (nLowX >= 0)
      {
         // OK, now add to the potential headcut erosion: dElevSlope is in radians TODO try using water surface gradient (i.e. the hydraulic gradient) rather than soil surface gradient here, but made v little difference
         double dElevSlope = atan((m_Cell[nX][nY].pGetSoil()->dGetSoilSurfaceElevation() - m_Cell[nLowX][nLowY].pGetSoil()->dGetSoilSurfaceElevation()) / dHLen);

         // TODO BODGE
         double dConst = 0.01;
         double dThisRetreat = dConst * sin(dElevSlope);
   //          m_ofsLog << m_ulIter << " [" << nX << "][" << nY << "] dElevSlope (degrees)=" << dElevSlope * 180 / PI << " dThisRetreat=" << dThisRetreat << endl;

         // Calculate the opposite direction to flow
         int nHeadcutDirection = (nDirection + 4) % 8;
         m_Cell[nX][nY].AddToStoredRetreat(nHeadcutDirection, dThisRetreat);
      }
   }
}

