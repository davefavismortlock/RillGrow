/*=========================================================================================================================================

 This is do_sediment.cpp: it handles erosion and deposition

 Copyright (C) 2018 David Favis-Mortlock

 ==========================================================================================================================================

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation,
 Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
#include "rg.h"
#include "simulation.h"
#include "cell.h"


/*=========================================================================================================================================

 Calculates transport capacity in a single wet cell to decide whether erosion or deposition should occur there

=========================================================================================================================================*/
void CSimulation::CalcTransportCapacity(int const nX, int const nY, int const nLowX, int const nLowY, double const dWaterDepth, double const dTopDiff, double const dTopSlope, double const dHLen, double const dVel, double const dMoveDepth)
{
   // For transport capacity, use the equation (5) in Nearing, M.A., Norton, L.D., Bulgakov, D.A., Larionov, G.A., West, L.T. and Dontsova, K. (1997). Hydraulics and erosion in eroding rills. Water Resources Research 33(4), 865-876.

   // Because this equation uses cgs units, all variables used in this subroutine's calculations must be temporarily transformed into cgs units. First calculate unit discharge dQ in cm2/s. Note that velocity used in this calculation is depth-dependent
   double dQ = dMoveDepth * dVel * 0.01;                                         // div by 100 -> cm2/sec

   // Now calculate stream power dW
   double dW = m_dRho * m_dG * dTopSlope * dQ * 0.01;                            // in g/s3, x 10 since m_dG in m/s2, and divide by 1000 since m_dRho is in kg/m3

   // This flow is potentially erosive, save stream power value for later display (in kg/s3)
   Cell[nX][nY].pGetSurfaceWater()->SetStreamPower(dW * 0.001);

   // Calculate potential unit sediment load dQs using Nearing et al. 1997 equation
   double 
      dTmp1 = exp(m_dGamma + (m_dDelta * log(dW))),
      dTmp2 = dTmp1 + 1,
      dQs = exp(((m_dAlpha * dTmp2) + (m_dBeta * dTmp1)) / dTmp2);

   // This is for unit (cm) width, so calculate total for full width of cell
   double dTmp3 = dQs * m_dCellSide * 0.1;                                     // div by 10 -> g/s

   // Calculate amount which could be transported while travelling across cell
   dTmp3 *= sqrt((dHLen * dHLen) + (dTopDiff * dTopDiff)) / dVel;             // g

   // Get the bulk density of the topmost non-zero soil layer
   // TODO This may be wrong, since bulk density of transported material probably isn't the same as bulk density of its parent soil
   double dBulkDensity = Cell[nX][nY].pGetSoil()->dGetBulkDensityOfTopNonZeroLayer();
   if (dBulkDensity < 0)
   {
      // We are down to unerodible basement, so no sediment to be transported
      return;      
   }
   
   // Convert this amount to a volumetric measure in mm3; div by 1/1000 since bulk density was previously multiplied by 1000, and also multiplied by 1000 to convert cm3 -> mm3
   dTmp3 *= (1e6 / dBulkDensity);

   // Convert again to a depth measure -- this is the transport capacity expressed as a depth equivalent
   double dTransportCapacity = dTmp3 * m_dInvCellSquare;                                      // in mm

   // Save this
   Cell[nX][nY].pGetSurfaceWater()->SetTransportCapacity(dTransportCapacity);

   // Get the total sediment load (this id for all size classes)
   double dSedimentLoad = Cell[nX][nY].pGetSediment()->dGetAllSizeSedimentLoad();

   // Finally, compare sediment load and transport capacity
   if (dSedimentLoad > dTransportCapacity) 
   {
      // Sediment load is more than transport capacity, so do some deposition
      DoCellDeposition(nX, nY, dWaterDepth, dSedimentLoad, dTransportCapacity);
   }
   else
   {
      // Sediment load is less than transport capacity and the adjacent cell is downhill from this one, so do some erosion
      DoCellErosion(nX, nY, nLowX, nLowY, dTopSlope, dHLen, dVel, 1.0 - (dSedimentLoad / dTransportCapacity), dMoveDepth);
   }
}


/*=========================================================================================================================================

 This method erodes a cell as a result of downhill flow; it uses a probabilistic equation from Nearing (1991)

=========================================================================================================================================*/
void CSimulation::DoCellErosion(int const nX, int const nY, int const nLowX, int const nLowY, double const dS, double const dHLen, double const dVel, double const dSedimentLoadWt, double const dMoveDepth)
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

   // Save the shear stress (both this-iteration and cumulative total) TODO is this OK? Should it be cumulative?
   Cell[nX][nY].pGetSoil()->IncShearStress(dTau);
   Cell[nX][nY].pGetSoil()->IncTotShearStress(dTau);

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
      LogStream << "m_dT" << '\t' << "m_dST" << endl;
      LogStream << m_dT << '\t' << sqrt(m_dST2) << endl;
      LogStream << "dTaub" << '\t' << "dSTaub" << '\t' << "dZ"  << '\t' << "dP" << '\t' << "dE" << endl;
   }
   if (0 == m_ulIter % 10)
      LogStream << dTaub << '\t' << dSTaub << '\t' << dZ  << '\t' << dP << '\t' << dE << endl;
*/

   // Use Nearing's equation to calculate soil loss as detachment rate per unit area of soil surface i.e. in kg/(sec m2). First calculate the difference between burst shear stress and soil strength, normalize this to SD units then use the normalized value to sample a value from the cumulative probability distribution function of a standard Gaussian (normal) distribution
   // This value of P is thus calculated using a function similar to, but not identical with, equation 9 in Nearing (1991). The approach used here is closer to the standard relationship used in reliability analysis. Note must divide by 1000, since dVel in mm/sec
   double dE = m_dK * dVel * dGetCGaussianPDF((dTaub - m_dT) / sqrt((m_dST2) + (dSTaub * dSTaub))) * dS * 0.001;

   // Pre-existing sediment load of 'this' cell is assumed to produce a linear decrease in detachment i.e  dE(actual) = dE * (1 - (dSedimentLoad/dTransportCapacity)) following equation 11 in Lei et al. (1998)
   dE *= dSedimentLoadWt;                                        // kg/(sec m2)

   // Get the bulk density (in kg/m3) of the topmost non-zero layer
   double dBulkDensity = Cell[nX][nY].pGetSoil()->dGetBulkDensityOfTopNonZeroLayer();
   if (dBulkDensity < 0)
   {
      // We are down to unerodible basement, so no sediment to be transported
      return;      
   }
   
   // Now convert the total thickness of soil lost per sec; to be split between 'this' and 'next' cells
   double dThickLost = 1000 * dE / dBulkDensity;       // mm/sec

//   LogStream << m_ulIter << " [" << nX << "][" << nY << "] dThickLost="  << dThickLost << endl;

   // Is this an edge cell?
   if (nLowX < 0)
   {
      // It is an edge cell. Do we have a baselevel?
      if (m_bHaveBaseLevel)
         // We do, so make sure that we do not cut down below the baselevel
         dThickLost = tMin(Cell[nX][nY].pGetSoil()->dGetSoilSurfaceElevation() - m_dBaseLevel, dThickLost);
      
      // Erode 'this' cell, also add to sediment load and totals
      if (dThickLost > TOLERANCE)
         Cell[nX][nY].pGetSoil()->DoFlowDetach(dThickLost);      
   }
   else
   {
      // It is not an edge cell, so calculate the relative amounts to be removed from source and destination cells: as the soil surface slope gets steeper, relatively more is removed from 'this' cell than from the 'next' cell. Helps both to avoid excavating pits, and in the simulation of headcut retreat
      // Note: tried using water surface gradient (i.e. the hydraulic gradient) rather than soil surface gradient here, but made v little difference TODO again
      double dElevSlope = (Cell[nX][nY].pGetSoil()->dGetSoilSurfaceElevation() - Cell[nLowX][nLowY].pGetSoil()->dGetSoilSurfaceElevation()) / dHLen;
         
      if (dElevSlope > 0)
      {
         // Need to check dElevSlope, because it is possible that both cells may have exactly the same elevation
         double dSourceWeight = tMin(0.5 + (0.5 * dElevSlope / m_dSourceDestCritAngle), 1.0);
//         double dSourceWeight = 0.5 + (0.5 * exp(-1.0 / (dElevSlope * m_dSourceDestCritAngle)));
         double dDestWeight = 1.0 - dSourceWeight;
//         LogStream << m_ulIter << " [" << nX << "][" << nY << "] dSourceWeight=" << dSourceWeight << " fDestWeight=" << dSourceWeight << endl;
         
         if (dThickLost > TOLERANCE)
         {
            // Erode 'this' cell, and add to sediment load and totals
            Cell[nX][nY].pGetSoil()->DoFlowDetach(dThickLost * dSourceWeight);
            
            // Also erode the 'next' cell, and add to sediment load and totals
            Cell[nLowX][nLowY].pGetSoil()->DoFlowDetach(dThickLost * dDestWeight);
         }
      }
   }
}


/*=========================================================================================================================================

 Calculates deposition in a single wet cell

=========================================================================================================================================*/
void CSimulation::DoCellDeposition(int const nX, int const nY, double const dWaterDepth, double const dSedimentLoad, double const dTransportCapacity)
{
   // First calculate Reynold's Number, and a drag coefficient
   double 
      dReynoldsNumber = Cell[nX][nY].pGetSurfaceWater()->dGetReynolds(m_dNu),
      dDragCoefficient = 24.0 / dReynoldsNumber;

   // Calculate the settling speed for the three sediment size classes using the terminal velocity equation: see https://en.wikipedia.org/wiki/Terminal_velocity
   double
      dClaySettlingSpeed = 1000 * sqrt((4 * m_dG * m_dClayDiameter) * m_dDensityDiffExpression / ( 3 * dDragCoefficient)),     // in mm/sec
      dSiltSettlingSpeed = 1000 * sqrt((4 * m_dG * m_dSiltDiameter) * m_dDensityDiffExpression / ( 3 * dDragCoefficient)),
      dSandSettlingSpeed = 1000 * sqrt((4 * m_dG * m_dSandDiameter) * m_dDensityDiffExpression / ( 3 * dDragCoefficient));
   
   // Now calculate the distance fallen during this timestep by a grain of each sediment size class
   double
      dClayDistance = m_dTimeStep * dClaySettlingSpeed,
      dSiltDistance = m_dTimeStep * dSiltSettlingSpeed,
      dSandDistance = m_dTimeStep * dSandSettlingSpeed;
   
   // Assume that, for all sediment size classes, deposition is a linear function of the difference between sediment load and transport capacity: see equation 12 in Lei et al. (1998)
   double dDiff = dSedimentLoad - dTransportCapacity;
   
   // Also assume that the sediment in each size class is well mixed vertically, and that the fraction deposited is proportional to distance fallen divided by water depth
   double
      dClayFrac = tMin(1.0, dClayDistance / dWaterDepth) * dDiff,
      dSiltFrac = tMin(1.0, dSiltDistance / dWaterDepth) * dDiff,
      dSandFrac = tMin(1.0, dSandDistance / dWaterDepth) * dDiff;
   
   // Finally, deposit these fractions of each sediment size class. Note that if there isn't enough being transported, then we will reduce the amount (= depth) which gets deposited
   // TODO Note too that what is implied here is that deposited sediment becomes indistinguishable from the original soil. To be considered in a later version
   Cell[nX][nY].pGetSoil()->DoFlowDeposit(dClayFrac, dSiltFrac, dSandFrac);
}


/*=========================================================================================================================================
 
 Adds to the this-iteration flow detachment value for clay-sized sediment
 
=========================================================================================================================================*/
void CSimulation::AddThisIterClayFlowDetach(double const dDetach)
{
   m_dThisIterClayFlowDetach += dDetach;
}

/*=========================================================================================================================================
 
 Adds to the this-iteration flow detachment value for silt-sized sediment
 
=========================================================================================================================================*/
void CSimulation::AddThisIterSiltFlowDetach(double const dDetach)
{
   m_dThisIterSiltFlowDetach += dDetach;
}

/*=========================================================================================================================================
 
 Adds to the this-iteration flow detachment value for sand-sized sediment
 
=========================================================================================================================================*/
void CSimulation::AddThisIterSandFlowDetach(double const dDetach)
{
   m_dThisIterSandFlowDetach += dDetach;
}

/*=========================================================================================================================================
 
 Adds to the this-iteration flow deposition value for clay-sized sediment
 
=========================================================================================================================================*/
void CSimulation::AddThisIterClayFlowDeposit(double const dDeposit)
{
   m_dThisIterClayFlowDeposit += dDeposit;
}

/*=========================================================================================================================================
 
 Adds to the this-iteration flow deposition value for silt-sized sediment
 
=========================================================================================================================================*/
void CSimulation::AddThisIterSiltFlowDeposit(double const dDeposit)
{
   m_dThisIterSiltFlowDeposit += dDeposit;
}

/*=========================================================================================================================================
 
 Adds to the this-iteration flow deposition value for sand-sized sediment
 
=========================================================================================================================================*/
void CSimulation::AddThisIterSandFlowDeposit(double const dDeposit)
{
   m_dThisIterSandFlowDeposit += dDeposit;
}


/*=========================================================================================================================================
 
 Adds to the this-iteration clay-sized sediment load
 
=========================================================================================================================================*/
void CSimulation::AddThisIterClaySedimentLoad(double const dChange)
{
   m_dThisIterClaySedLoad += dChange;
}

/*=========================================================================================================================================
 
 Adds to the this-iteration silt-sized sediment load
 
=========================================================================================================================================*/
void CSimulation::AddThisIterSiltSedimentLoad(double const dChange)
{
   m_dThisIterSiltSedLoad += dChange;
}

/*=========================================================================================================================================
 
 Adds to the this-iteration sand-sized sediment load
 
=========================================================================================================================================*/
void CSimulation::AddThisIterSandSedimentLoad(double const dChange)
{
   m_dThisIterSandSedimentLoad += dChange;
}
