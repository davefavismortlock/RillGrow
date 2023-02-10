/*=========================================================================================================================================

 This is simulate_mass_movement.cpp: it handles slumping of e.g. rill sidewalls, toppling, and headcut retreat

 Copyright (C) 2023 David Favis-Mortlock

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

 This file-local function calculates the 'distance' between a given m_Cell of the soil sear stress weigh quadrant, and the corner of the quadrant

=========================================================================================================================================*/
double dDistToCorner(int const nX, int const nY)
{
   return sqrt((nX * nX) + (nY * nY));
}


/*=========================================================================================================================================

 This CSimulation member function initializes various things (including the quadrant of soil shear stress weights) ready to do slumping

=========================================================================================================================================*/
int CSimulation::nInitSlumping(void)
{
   m_dSlumpAngleOfRestDiff      = m_dm_CellSide * m_dSlumpAngleOfRest / 100;
   m_dSlumpAngleOfRestDiffDiag  = m_dm_CellDiag * m_dSlumpAngleOfRest / 100;
   m_dToppleCritDiff            = m_dm_CellSide * m_dToppleCriticalAngle / 100;
   m_dToppleCritDiffDiag        = m_dm_CellDiag * m_dToppleCriticalAngle / 100;
   m_dToppleAngleOfRestDiff     = m_dm_CellSide * m_dToppleAngleOfRest / 100;
   m_dToppleAngleOfRestDiffDiag = m_dm_CellDiag * m_dToppleAngleOfRest / 100;

   m_nSSSQuadrantSize = tMax(1, static_cast<int>(floor(m_dSSSPatchSize / m_dm_CellSide)));

   // Create a 2D array for one quadrant of weights for the soil shear stress spatial distribution
   m_SSSWeightQuadrant = new double*[m_nSSSQuadrantSize];
   if (NULL == m_SSSWeightQuadrant)
   {
      // Error, can't allocate memory
      cerr << ERR << "cannot allocate memory for " << m_nSSSQuadrantSize << " x " << m_nSSSQuadrantSize << " soil shear stress array" << endl;
      return (RTN_ERR_MEMALLOC);
   }

   for (int nX = 0; nX < m_nSSSQuadrantSize; nX++)
   {
      m_SSSWeightQuadrant[nX] = new double[m_nSSSQuadrantSize];
      if (NULL == m_SSSWeightQuadrant[nX])
      {
         // Error, can't allocate memory
         cerr << ERR << "cannot allocate memory for " << m_nSSSQuadrantSize << " x " << m_nSSSQuadrantSize << " soil shear stress array" << endl;
         return (RTN_ERR_MEMALLOC);
      }
   }

   // Calculate a weight for each m_Cell in the quadrant. If m_SSSWeightQuadrant[0][0] is considered to be the bottom-left corner, then the weights of the top-left and bottom-right corners are zero. The weights of the cells along the left column and bottom row are interpolated linearly
   double dTotWeight = m_SSSWeightQuadrant[0][0] = 1;

   // Set the N-S and W-E weights, and add to the total of weights
   for (int n = 1; n < m_nSSSQuadrantSize; n++)
   {
      double dWeight = 1.0 - (dDistToCorner(n, 0) / m_nSSSQuadrantSize);

      m_SSSWeightQuadrant[n][0] = m_SSSWeightQuadrant[0][n] = dWeight;

      // Add to the total of weights in the whole (four quadrant) patch
      dTotWeight += 4 * dWeight;
   }

   // Next do the same for the in-between weights
   for (int n = 1; n < m_nSSSQuadrantSize; n++)
   {
      if (m_SSSWeightQuadrant[n][0] == 0)
         break;

      for (int m = 1; m < m_nSSSQuadrantSize; m++)
      {

         double dWeight = tMax(0.0, 1.0 - (dDistToCorner(n, m) / m_nSSSQuadrantSize));

         m_SSSWeightQuadrant[n][m] = dWeight;
         dTotWeight += 4 * dWeight;
      }
   }

   // See the result
//    for (int nY = 0; nY < m_nSSSQuadrantSize; nY++)
//    {
//       for (int nX = 0; nX < m_nSSSQuadrantSize; nX++)
//       {
//          cout << m_SSSWeightQuadrant[nX][nY] << "\t";
//       }
//       cout << endl;
//    }
//    cout << endl;

   // Replace weights with normalized weights (total is one)
   for (int nX = 0; nX < m_nSSSQuadrantSize; nX++)
   {
      for (int nY = 0; nY < m_nSSSQuadrantSize; nY++)
      {
         m_SSSWeightQuadrant[nX][nY] /= dTotWeight;
      }
   }

   // See the result
//    for (int nY = 0; nY < m_nSSSQuadrantSize; nY++)
//    {
//       for (int nX = 0; nX < m_nSSSQuadrantSize; nX++)
//       {
//          m_ofsLog << m_SSSWeightQuadrant[nX][nY] << "\t";
//       }
//       m_ofsLog << endl;
//    }
//    m_ofsLog << endl << endl;

   return RTN_OK;
}


/*=========================================================================================================================================

 This member function of CSimulation distributes shear stress in a patch around a target m_Cell

=========================================================================================================================================*/
void CSimulation::DoDistributeShearStress(int const nX, int const nY, double const dTau)
{
   m_Cell[nX][nY].pGetSoil()->IncShearStress(dTau * m_SSSWeightQuadrant[0][0]);
//    cerr << "[" << nX << "][" << nY << "] " << m_SSSWeightQuadrant[0][0] << endl << endl;

   for (int m = 1; m < m_nSSSQuadrantSize; m++)
   {
      for (int n = 0; n < m_nSSSQuadrantSize; n++)
      {
         double dThisTau = dTau * m_SSSWeightQuadrant[m][n];

         int nXTmp = nX + m;
         int nYTmp = nY + n;
         if ((nXTmp < m_nXGridMax) && (nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()))
         {
            m_Cell[nX][nY].pGetSoil()->IncShearStress(dThisTau);
//             cerr << "[" << nXTmp << "][" << nYTmp << "] " << m_SSSWeightQuadrant[m][n] << endl;
         }

         nXTmp = nX - m;
         nYTmp = nY - n;
         if ((nXTmp >= 0) && (nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()))
         {
            m_Cell[nX][nY].pGetSoil()->IncShearStress(dThisTau);
//             cerr << "[" << nXTmp << "][" << nYTmp << "] " << m_SSSWeightQuadrant[m][n] << endl;
         }

         nXTmp = nX + n;
         nYTmp = nY - m;
         if ((nXTmp < m_nXGridMax) && (nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()))
         {
            m_Cell[nX][nY].pGetSoil()->IncShearStress(dThisTau);
//             cerr << "[" << nXTmp << "][" << nYTmp << "] " << m_SSSWeightQuadrant[m][n] << endl;
         }

         nXTmp = nX - n;
         nYTmp = nY + m;
         if ((nXTmp >= 0) && (nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()))
         {
            m_Cell[nX][nY].pGetSoil()->IncShearStress(dThisTau);
//             cerr << "[" << nXTmp << "][" << nYTmp << "] " << m_SSSWeightQuadrant[m][n] << endl << endl;
         }
      }
   }

//    cerr << "-------------------------------------" << endl;
}


/*=========================================================================================================================================

 This member function of CSimulation simulates slumping of e.g. rill sidewalls, for all m_Cells

=========================================================================================================================================*/
void CSimulation::DoAllSlump(void)
{
   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      for (int nY = 0; nY < m_nYGridMax; nY++)
      {
         if (m_Cell[nX][nY].bIsMissingValue())
         {
            // Don't start slumping on edge m_Cells, this is to avoid edge effects
            continue;
         }

         // Set the this-operation (actually they are kept for several iterations) values for slumping and toppling
         m_Cell[nX][nY].pGetSoil()->ZeroThisOperationSlump();

         // Now total up the shear stress in this and adjacent m_Cells
         int nXTmp, nYTmp;
         double dThisStress = m_Cell[nX][nY].pGetSoil()->dGetShearStress();

         // Planview bottom
         nXTmp = nX;
         nYTmp = nY+1;
         if ((nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && (m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
            dThisStress += m_Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();

         // Planview bottom right
         nXTmp = nX+1;
         nYTmp = nY+1;
         if ((nXTmp < m_nXGridMax) && (nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && (m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
            dThisStress += m_Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();

         // Planview bottom left
         nXTmp = nX-1;
         nYTmp = nY+1;
         if ((nXTmp >= 0) && (nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && (m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
            dThisStress += m_Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();

         // Planview right
         nXTmp = nX+1;
         nYTmp = nY;
         if ((nXTmp < m_nXGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && (m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
            dThisStress += m_Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();

         // Planview left
         nXTmp = nX-1;
         nYTmp = nY;
         if ((nXTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && (m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
            dThisStress += m_Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();

         // Planview top right
         nXTmp = nX+1;
         nYTmp = nY-1;
         if ((nXTmp < m_nXGridMax) && (nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && (m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
            dThisStress += m_Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();

         // Planview top left
         nXTmp = nX-1;
         nYTmp = nY-1;
         if ((nXTmp >= 0) && (nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && (m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
            dThisStress += m_Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();

         // Planview top
         nXTmp = nX;
         nYTmp = nY-1;
         if ((nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && (m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
            dThisStress += m_Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();

         // Any shear stress?
         if (dThisStress == 0)
            continue;

         // OK, we have some shear stress. Now get the water content of this m_Cell, as a fraction. But if we aren't simulating infilt, assume the soil is saturated
         // TODO get working for multiple soil layers
         double dWaterFrac = 1;
         if (m_bDoInfiltration)
         {
            // Calculate the saturated (maximum) soil water content (a depth equivalent) for the top soil layer on this cell
            CCellSoilLayer* pTopLayer = m_Cell[nX][nY].pGetSoil()->pLayerGetLayer(0);

            // Get the current water content (a depth equivalent) for the top soil layer
            double dTopLayerSoilWater = pTopLayer->dGetSoilWater();

            // Now calculate the saturated (maximum) soil water content (a depth equivalent) for this layer
            double
               dTopLayerThickness = pTopLayer->dGetLayerThickness(),
               dTopLayerMaxSoilWater = m_VdInputSoilLayerInfiltSatWater[0] * dTopLayerThickness;

            // And so get the fraction saturated
            dWaterFrac = dTopLayerSoilWater / dTopLayerMaxSoilWater;
         }

         // Assume that shear stress (and hence slumping) is linearly proportional to the water content of this m_Cell TODO check this
         dThisStress *= dWaterFrac;
         dThisStress /= (m_dSimulatedTimeElapsed - m_dLastSlumpCalcTime);

         // So we have the total shear stress in this m_Cell, from all wet m_Cells adjacent to this m_Cell. Does this exceed the threshold shear stress for slumping?
         if (dThisStress >= m_dCritSSSForSlump)
         {
            // Yes, it does: so do some slumping
            bool bDiag = false;
            double dDiff;

            // Find the adjacent wet m_Cell with the steepest downhill soil-surface gradient, however this must not be an edge m_Cell
            if ((nFindSteepestSoilSurface(nX, nY, m_Cell[nX][nY].pGetSoil()->dGetSoilSurfaceElevation(), nXTmp, nYTmp, dDiff, bDiag)) != DIRECTION_NONE)
            {
               // Assume that soil is saturated, and flows hydrostatically (i.e. it wants to get to angle of rest) down the steepest soil-surface gradient
               double dCrit = 0;
               if (bDiag)
                  // Steepest downhill soil surface slope is NW-SE or SW-NE (i.e. diagonally) planview
                  dCrit = m_dSlumpAngleOfRestDiffDiag;
               else
                  // Steepest downhill soil surface slope is N-S or W-E planview
                  dCrit = m_dSlumpAngleOfRestDiff;

               if (dDiff > dCrit)
               {
                  dDiff -= dCrit;
                  dDiff /= 2;

                  // Remove the soil from the higher m_Cell, and move it to the lower m_Cell
                  double
                     dClayDetached = 0,
                     dSiltDetached = 0,
                     dSandDetached = 0;

                  m_Cell[nX][nY].pGetSoil()->DoSlumpDetach(dDiff, dClayDetached, dSiltDetached, dSandDetached);

                  // Add to 'since last' values
                  m_dSinceLastClaySlumpDetach += dClayDetached;
                  m_dSinceLastSiltSlumpDetach += dSiltDetached;
                  m_dSinceLastSandSlumpDetach += dSandDetached;

                  // Now add the detached soil to the 'To' m_Cell (as part of the top soil layer if dry, or to the sediment load if wet)
                  double
                     dClayDeposited = 0,
                     dSiltDeposited = 0,
                     dSandDeposited = 0,
                     dClayToSedLoad = 0,
                     dSiltToSedLoad = 0,
                     dSandToSedLoad = 0;

                  m_Cell[nXTmp][nYTmp].pGetSoil()->DoSlumpDepositOrToSedLoad(dClayDetached, dSiltDetached, dSandDetached, dClayDeposited, dSiltDeposited, dSandDeposited, dClayToSedLoad, dSiltToSedLoad, dSandToSedLoad);

                  // Add to 'since last' values
                  m_dSinceLastClaySlumpDeposit += dClayDeposited;
                  m_dSinceLastSiltSlumpDeposit += dSiltDeposited;
                  m_dSinceLastSandSlumpDeposit += dSandDeposited;
                  m_dSinceLastClaySlumpToSedLoad += dClayToSedLoad;
                  m_dSinceLastSiltSlumpToSedLoad += dSiltToSedLoad;
                  m_dSinceLastSandSlumpToSedLoad += dSandToSedLoad;

                  // Has this slumping also triggered toppling of any now-unstable upslope m_Cells?
                  int nRecursionDepth = MAX_RECURSION_DEPTH;
                  TryTopplem_CellsAbove(nX, nY, nRecursionDepth);
               }
            }
         }
      }
   }
}



/*=========================================================================================================================================

 Identifies the adjacent wet m_Cell which has the steepest downhill soil-surface gradient. However this must not be an edge m_Cell

=========================================================================================================================================*/
int CSimulation::nFindSteepestSoilSurface(int const nX, int const nY, double const dThisElev, int& nLowX, int& nLowY, double& dDiff, bool& bDiag)
{
   int nXTmp,
      nYTmp,
      nDir = DIRECTION_NONE;
   double
      dTmpDiff,
      dTanX,
      dSlope = 0;

   // Planview bottom
   nXTmp = nX;
   nYTmp = nY+1;
   if ((nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0))
   {
      // It's wet, it's downhill, and being the first one checked it must be the steepest so far
      dSlope = dTmpDiff * m_dInvm_CellSide;                                                       // is tan(top slope)
      nLowX = nXTmp;
      nLowY = nYTmp;
      dDiff = dTmpDiff;
      nDir = DIRECTION_BOTTOM;
      bDiag = false;
   }

   // Planview bottom right
   nXTmp = nX+1;
   nYTmp = nY+1;
   if ((nXTmp < m_nXGridMax) && (nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvm_CellDiag) > dSlope))
   {
      // It's wet and the steepest so far
      dSlope = dTanX;                                                                           // is tan(top slope)
      nLowX = nXTmp;
      nLowY = nYTmp;
      dDiff = dTmpDiff;
      nDir = DIRECTION_BOTTOM_RIGHT;
      bDiag = true;
   }

   // Planview bottom left
   nXTmp = nX-1;
   nYTmp = nY+1;
   if ((nXTmp >= 0) && (nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvm_CellDiag) > dSlope))
   {
      // It's wet and the steepest so far
      dSlope = dTanX;                                                                           // is tan(top slope)
      nLowX = nXTmp;
      nLowY = nYTmp;
      dDiff = dTmpDiff;
      nDir = DIRECTION_BOTTOM_LEFT;
      bDiag = true;
   }

   // Planview right
   nXTmp = nX+1;
   nYTmp = nY;
   if ((nXTmp < m_nXGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvm_CellSide) > dSlope))
   {
      // It's wet and the steepest so far
      dSlope = dTanX;                                                                           // is tan(top slope)
      nLowX = nXTmp;
      nLowY = nYTmp;
      dDiff = dTmpDiff;
      nDir = DIRECTION_RIGHT;
      bDiag = false;
   }

   // Planview left
   nXTmp = nX-1;
   nYTmp = nY;
   if ((nXTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvm_CellSide) > dSlope))
   {
      // It's wet and the steepest so far
      dSlope = dTanX;                                                                           // is tan(top slope)
      nLowX = nXTmp;
      nLowY = nYTmp;
      dDiff = dTmpDiff;
      nDir = DIRECTION_LEFT;
      bDiag = false;
   }

   // Planview top right
   nXTmp = nX+1;
   nYTmp = nY-1;
   if ((nXTmp < m_nXGridMax) && (nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvm_CellDiag) > dSlope))
   {
      // It's wet and the steepest so far
      dSlope = dTanX;                                                                           // is tan(top slope)
      nLowX = nXTmp;
      nLowY = nYTmp;
      dDiff = dTmpDiff;
      nDir = DIRECTION_TOP_RIGHT;
      bDiag = true;
   }

   // Planview top left
   nXTmp = nX-1;
   nYTmp = nY-1;
   if ((nXTmp >= 0) && (nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvm_CellDiag) > dSlope))
   {
      // It's wet and the steepest so far
      dSlope = dTanX;                                                                           // is tan(top slope)
      nLowX = nXTmp;
      nLowY = nYTmp;
      dDiff = dTmpDiff;
      nDir = DIRECTION_TOP_LEFT;
      bDiag = true;
   }

   // Planview top
   nXTmp = nX;
   nYTmp = nY-1;
   if ((nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0) && ((dTmpDiff * m_dInvm_CellSide) > dSlope))
   {
      // It's wet and the steepest so far
      nLowX = nXTmp;
      nLowY = nYTmp;
      dDiff = dTmpDiff;
      nDir = DIRECTION_TOP;
      bDiag = false;
   }

   return (nDir);
}


/*=========================================================================================================================================

 Topples all unstable m_Cells above a point of slumping; calls itself recursively until there are no more m_Cells to topple. However, no toppling is allowed in edge m_Cells, to avoid edge effects

=========================================================================================================================================*/
void CSimulation::TryTopplem_CellsAbove(int const nX, int const nY, int nRecursionDepth)
{
   if (nRecursionDepth <= 0)
      return;

   nRecursionDepth--;

   int
      nXTmp,
      nYTmp;
   double
      dThisElev = m_Cell[nX][nY].pGetSoil()->dGetSoilSurfaceElevation(),
      dDiff;

   // The cell at planview bottom
   nXTmp = nX;
   nYTmp = nY+1;
   if ((nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dDiff = m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiff))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple m_Cells
      DoTopplem_Cells(nX, nY, nXTmp, nYTmp, dDiff, false);

      // Now call this routine recursively
      TryTopplem_CellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }

   // The cell at planview bottom right
   nXTmp = nX+1;
   nYTmp = nY+1;
   if ((nXTmp < m_nXGridMax) && (nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dDiff = m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiffDiag))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple m_Cells
      DoTopplem_Cells(nX, nY, nXTmp, nYTmp, dDiff, true);

      // Now call this routine recursively
      TryTopplem_CellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }

   // The cell at planview bottom left
   nXTmp = nX-1;
   nYTmp = nY+1;
   if ((nXTmp >= 0) && (nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dDiff = m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiffDiag))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple m_Cells
      DoTopplem_Cells(nX, nY, nXTmp, nYTmp, dDiff, true);

      // Now call this routine recursively
      TryTopplem_CellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }

   // The cell at planview right
   nXTmp = nX+1;
   nYTmp = nY;
   if ((nXTmp < m_nXGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dDiff = m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiff))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple m_Cells
      DoTopplem_Cells(nX, nY, nXTmp, nYTmp, dDiff, false);

      // Now call this routine recursively
      TryTopplem_CellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }

   // The cell at planview left
   nXTmp = nX-1;
   nYTmp = nY;
   if ((nXTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dDiff = m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiff))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple m_Cells
      DoTopplem_Cells(nX, nY, nXTmp, nYTmp, dDiff, false);

      // Now call this routine recursively
      TryTopplem_CellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }

   // The cell at planview top right
   nXTmp = nX+1;
   nYTmp = nY-1;
   if ((nXTmp < m_nXGridMax) && (nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dDiff = m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiffDiag))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple m_Cells
      DoTopplem_Cells(nX, nY, nXTmp, nYTmp, dDiff, true);

      // Now call this routine recursively
      TryTopplem_CellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }

   // The cell at planview top left
   nXTmp = nX-1;
   nYTmp = nY-1;
   if ((nXTmp >= 0) && (nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dDiff = m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiffDiag))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple m_Cells
      DoTopplem_Cells(nX, nY, nXTmp, nYTmp, dDiff, true);

      // Now call this routine recursively
      TryTopplem_CellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }

   // The cell at planview top
   nXTmp = nX;
   nYTmp = nY-1;
   if ((nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dDiff = m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiff))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple m_Cells
      DoTopplem_Cells(nX, nY, nXTmp, nYTmp, dDiff, false);

      // Now call this routine recursively
      TryTopplem_CellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }
}


/*=========================================================================================================================================

 Does toppling, i.e. moves sediment from an over-steepened m_Cell to an adjacent m_Cell. If the target m_Cell is wet, then the sediment is moved into that m_Cell's sediment load

=========================================================================================================================================*/
void CSimulation::DoTopplem_Cells(int const nX, int const nY, int const nXTmp, int const nYTmp, double dDiff, bool const bDiag)
{
   double dRestDiff = 0;
   if (bDiag)
      dRestDiff = m_dToppleAngleOfRestDiffDiag;
   else
      dRestDiff = m_dToppleAngleOfRestDiff;

   dDiff -= dRestDiff;
   dDiff /= 2;

   double
      dClayDetached = 0,
      dSiltDetached = 0,
      dSandDetached = 0;

   // Do the detachment
   m_Cell[nXTmp][nYTmp].pGetSoil()->DoToppleDetach(dDiff, dClayDetached, dSiltDetached, dSandDetached);

   // Add to the 'since'last' values
   m_dSinceLastClayToppleDetach += dClayDetached;
   m_dSinceLastSiltToppleDetach += dSiltDetached;
   m_dSinceLastSandToppleDetach += dSandDetached;

   // And now do the deposition
   double
      dClayDeposited = 0,
      dSiltDeposited = 0,
      dSandDeposited = 0,
      dClayToSedLoad = 0,
      dSiltToSedLoad = 0,
      dSandToSedLoad = 0;

   m_Cell[nX][nY].pGetSoil()->DoToppleDepositOrToSedLoad(dClayDetached, dSiltDetached, dSandDetached, dClayDeposited, dSiltDeposited, dSandDeposited, dClayToSedLoad, dSiltToSedLoad, dSandToSedLoad);

   // Add to the 'since'last' values
   m_dSinceLastClayToppleDeposit += dClayDeposited;
   m_dSinceLastSiltToppleDeposit += dSiltDeposited;
   m_dSinceLastSandToppleDeposit += dSandDeposited;
   m_dSinceLastClayToppleToSedLoad += dClayToSedLoad;
   m_dSinceLastSiltToppleToSedLoad += dSiltToSedLoad;
   m_dSinceLastSandToppleToSedLoad += dSandToSedLoad;
}


/*=========================================================================================================================================

 This member function of CSimulation simulates headcut retreat on all m_Cells

=========================================================================================================================================*/
void CSimulation::DoAllHeadcutRetreat(void)
{
   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      for (int nY = 0; nY < m_nYGridMax; nY++)
      {
         if (m_Cell[nX][nY].bIsMissingOrEdge())
         {
            // Don't do headcut retreat on edge m_Cells, this is to avoid edge effects TODO May be OK as long as retreat direction is not towards the edge
            continue;
         }

         // OK check to see if this m_Cell is ready for headcut collapse
         for (int nDir = 0; nDir < 8; nDir++)
         {
            double dSize = m_dm_CellSide;
            if ((nDir % 2) != 0)
               dSize = m_dm_CellDiag;

            if (m_Cell[nX][nY].bIsReadyForHeadcutRetreat(nDir, dSize))
            {
               //                m_ofsLog << m_ulIter << " [" << nX << "][" << nY << "] ready for headcut collapse in direction " << nDir << endl;

               // Get the coords of the 'upstream' m_Cell
               int
               nUpStreamX = nX,
               nUpStreamY = nY;

               switch (nDir)
               {
                  case (DIRECTION_TOP) :
                     nUpStreamY = nY - 1;
                     break;

                  case (DIRECTION_TOP_RIGHT) :
                     nUpStreamX = nX + 1;
                     nUpStreamY = nY - 1;
                     break;

                  case (DIRECTION_RIGHT) :
                     nUpStreamX = nX + 1;
                     break;

                  case (DIRECTION_BOTTOM_RIGHT) :
                     nUpStreamX = nX + 1;
                     nUpStreamY = nY + 1;
                     break;

                  case (DIRECTION_BOTTOM) :
                     nUpStreamY = nY + 1;
                     break;

                  case (DIRECTION_BOTTOM_LEFT) :
                     nUpStreamX = nX - 1;
                     nUpStreamY = nY + 1;
                     break;

                  case (DIRECTION_LEFT) :
                     nUpStreamX = nX - 1;
                     break;

                  case (DIRECTION_TOP_LEFT) :
                     nUpStreamX = nX - 1;
                     nUpStreamY = nY - 1;
                     break;
               }

               // Get the soil surface elevation of this m_Cell
               double dThisElev = m_Cell[nX][nY].dGetSoilSurfaceElevation();

               // Get the soil surface elevation of the 'upstream' m_Cell
               double dUpStreamElev = m_Cell[nUpStreamX][nUpStreamY].dGetSoilSurfaceElevation();

               // And calculate the elevation difference
               double dElevDiff = dUpStreamElev - dThisElev;

               // Is the elevation difference nearly zero?
               if (bFPIsEqual(dElevDiff, 0.0, SEDIMENT_TOLERANCE))
               {
                  // Just try the next direction. Note that we don't zero the retreat for this direction since the elvation of the upstream m_Cell may change later
                  continue;
               }

               // OK, we have a reasonable elevation difference
               if (dElevDiff < 0)
               {
                  // The upstream m_Cell is lower than this m_Cell, so this m_Cell is a effectively a watershed boundary
                  //                  m_ofsLog << m_ulIter << " [" << nX << "][" << nY << "] (dThisElev=" << dThisElev << ") is a watershed boundary headcut, with [" << nUpStreamX << "][" << nUpStreamY << "] (dUpStreamElev=" << dUpStreamElev << "), dElevDiff=" << dElevDiff << endl;

                  // Move soil from this m_Cell to the upstream m_Cell
                  DoHeadcutRetreatMoveSoil(nDir, nX, nY, nUpStreamX, nUpStreamY, -dElevDiff);
               }
               else
               {
                  // The upstream m_Cell is higher than this m_Cell, so this is a normal headcut
                  //                  m_ofsLog << m_ulIter << " [" << nX << "][" << nY << "] (dThisElev=" << dThisElev << ") is a normal headcut, with [" << nUpStreamX << "][" << nUpStreamY << "] (dUpStreamElev=" << dUpStreamElev << "), dElevDiff=" << dElevDiff << endl;

                  // Move soil from the upstream m_Cell to this m_Cell
                  DoHeadcutRetreatMoveSoil(nDir, nUpStreamX, nUpStreamY, nX, nY, dElevDiff);
               }

               // Set the flag
               m_Cell[nX][nY].SetHasHadHeadcutRetreat();
            }
         }
      }
   }
}


/*=========================================================================================================================================

 Moves soil from one m_Cell to an adjacent m_Cell for headcut retreat. If the target m_Cell is wet, the sediment is added to the cell's sediment load

=========================================================================================================================================*/
void CSimulation::DoHeadcutRetreatMoveSoil(int const nDir, int const nFromX, int const nFromY, int const nToX, int const nToY, double const dDiff)
{
   double
      dHalfDiff = dDiff * 0.5,
      dClayDetached = 0,
      dSiltDetached = 0,
      dSandDetached = 0;

   // Remove soil from the 'From' m_Cell
   m_Cell[nFromX][nFromY].pGetSoil()->DoHeadcutRetreatDetach(dHalfDiff, dClayDetached, dSiltDetached, dSandDetached);

   // Add to the 'since last' values
   m_dSinceLastClayHeadcutDetach += dClayDetached;
   m_dSinceLastSiltHeadcutDetach += dSiltDetached;
   m_dSinceLastSandHeadcutDetach += dSandDetached;

   // Now add the detached soil to the 'To' m_Cell: as part of the top soil layer if dry, or as sediment load if wet
   double
      dClayDeposited = 0,
      dSiltDeposited = 0,
      dSandDeposited = 0,
      dClayToSedLoad = 0,
      dSiltToSedLoad = 0,
      dSandToSedLoad = 0;

   m_Cell[nToX][nToY].pGetSoil()->DoHeadcutRetreatDepositOrToSedLoad(dClayDetached, dSiltDetached, dSandDetached, dClayDeposited, dSiltDeposited, dSandDeposited, dClayToSedLoad, dSiltToSedLoad, dSandToSedLoad);

   m_dSinceLastClayHeadcutDeposit += dClayDeposited;
   m_dSinceLastSiltHeadcutDeposit += dSiltDeposited;
   m_dSinceLastSandHeadcutDeposit += dSandDeposited;
   m_dSinceLastClayHeadcutToSedLoad += dClayToSedLoad;
   m_dSinceLastSiltHeadcutToSedLoad += dSiltToSedLoad;
   m_dSinceLastSandHeadcutToSedLoad += dSandToSedLoad;

   m_Cell[nFromX][nFromY].SetStoredRetreat(nDir, 0);
   m_Cell[nToX][nToY].SetStoredRetreat(nDir, 0);

   //    m_ofsLog << m_ulIter << " headcut retreat, from [" << nFromX << "][" << nFromY << "] to [" << nToX << "][" << nToY << "] depth=" << dHalfDiff << " (clay=" << dClayDetached << ", silt=" << dSiltDetached << ", sand=" << dSandDetached << ")" << endl;
}

