/*=========================================================================================================================================

This is transport_capacity.cpp: it calculates sediment tansport capacity for RillGrow

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
//! Calculates transport capacity for a single wet cell to decide whether erosion or deposition should occur there. Uses equation (5) in Nearing, M.A., Norton, L.D., Bulgakov, D.A., Larionov, G.A., West, L.T. and Dontsova, K. (1997). Hydraulics and erosion in eroding rills. Water Resources Research 33(4), 865-876.
//=========================================================================================================================================
void CSimulation::CalcTransportCapacity(int const nX, int const nY, int const nLowX, int const nLowY, int const nDirection, double const dWaterDepth, double const dTopDiff, double const dTopSlope, double const dHLen, double const dVel, double const dMoveDepth)
{
   // Because the Nearing et al. equation uses cgs units, all variables used in this subroutine's calculations must be temporarily transformed into cgs units. First calculate unit discharge dQ in cm**2/s. Note that velocity used in this calculation is depth-dependent
   double dQ = dMoveDepth * dVel * 0.01;                                         // div by 100 to give cm**2/sec

   // Now calculate stream power dW (in g/s**3). Need to multiply by 10 since m_dG in m/s**2, and divide by 1000 since m_dRho is in kg/m**3 i.e. divide by 100
   double dW = m_dRho * m_dG * dTopSlope * dQ * 0.01;                            // in g/s**3

   // This flow is potentially erosive, save stream power value for later display (save stream power in kg/s3)
   m_Cell[nX][nY].pGetSurfaceWater()->SetStreamPower(dW * 0.001);

   // Calculate potential unit sediment load dQs (cgs units) using the Nearing et al. equation
   double
      dTmp1 = exp(m_dGamma + (m_dDelta * log(dW))),
      dTmp2 = dTmp1 + 1,
      dQs = exp(((m_dAlpha * dTmp2) + (m_dBeta * dTmp1)) / dTmp2);

   // OK, dQS is for unit (cm) width, so calculate total for full width of cell. Need to divide by 10 since m_dCellSide is in mm
   double dTmp3 = dQs * m_dCellSide * 0.1;                                     // in g/s

   // Now calculate the amount which could be transported while travelling across cell
   dTmp3 *= sqrt((dHLen * dHLen) + (dTopDiff * dTopDiff)) / dVel;             // in g

   // Get the bulk density of the topmost non-zero soil layer: divide by 1000 to convert from kg/m**3 to t/m**3 (same as g/cm**3). NOTE this needs improvement, since bulk density of transported material probably isn't the same as bulk density of its parent soil
   double dBulkDensity = m_Cell[nX][nY].pGetSoil()->dGetBulkDensityOfTopNonZeroLayer() * 0.001;    // in g/cm**3
   if (dBulkDensity < 0)
   {
      // We are down to unerodible basement, so no sediment to be transported
      return;
   }

   // Convert the amount which could be transported while travelling across cell (in g) to a volumetric measure in cm**3: vol = mass / density then multiply by 1000 to convert cm**3 to mm**3
   double dTmp4 = 1000 * dTmp3 / dBulkDensity;     // in mm**3

   // Now convert to a depth measure: this is the transport capacity expressed as a depth equivalent
   double dTransportCapacity = dTmp4 * m_dInvCellSquare;                                      // in mm depth

   // Save transport capacity (mm depth)
   m_Cell[nX][nY].pGetSurfaceWater()->SetTransportCapacity(dTransportCapacity);

   // Get the total sediment load (for all sediment size classes) as a depth equivalent
   double dSedimentLoad = m_Cell[nX][nY].pGetSedLoad()->dGetLastIterAllSizeSedLoad();

   // Are we at the edge of the grid?
   if (nLowX == -1)
   {
      // Cannot have deposition at grid edge, but might have erosion
      double dSedimentLoadWt = tMax(1.0 - (dSedimentLoad / dTransportCapacity), 0.0);
      if (bFpEQ(dSedimentLoadWt, 0.0, TOLERANCE))      // Ensure that dSedimentLoadWt is never -ve
         return;

      // OK, do some erosion
      DoCellFlowErosion(nX, nY, nLowX, nLowY, nDirection, dTopSlope, dHLen, dVel, dSedimentLoadWt, dMoveDepth);
      return;
   }

   // We are not at the grid edge: compare sediment load and transport capacity
   if (dSedimentLoad > dTransportCapacity)
   {
      // Sediment load is more than transport capacity, so do some deposition
      DoCellSedLoadDeposition(nX, nY, dWaterDepth, dSedimentLoad, dTransportCapacity);
   }
   else
   {
      if (m_bFlowErosion)
      {
         // Sediment load is less than transport capacity and the adjacent cell is downhill from this one. First do a safety check to ensure that dSedimentLoadWt is never -ve
         double dSedimentLoadWt = tMax(1.0 - (dSedimentLoad / dTransportCapacity), 0.0);
         if (bFpEQ(dSedimentLoadWt, 0.0, TOLERANCE))
            return;

         // OK, do some erosion
         DoCellFlowErosion(nX, nY, nLowX, nLowY, nDirection, dTopSlope, dHLen, dVel, dSedimentLoadWt, dMoveDepth);
      }
   }
}

