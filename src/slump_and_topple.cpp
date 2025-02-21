/*=========================================================================================================================================

This is slump_and_topple.cpp: it simulates two kinds of mass movements (saturated slumping of e.g. rill sidewalls, and toppling due to oversteep gradients) for RillGrow

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
//! This file-local function calculates the 'distance' between a given cell of the soil sear stress weigh quadrant, and the corner of the quadrant
//=========================================================================================================================================
double dDistToCorner(int const nX, int const nY)
{
   return sqrt((nX * nX) + (nY * nY));
}

//=========================================================================================================================================
//! This CSimulation member function initializes various things (including the quadrant of soil shear stress weights) ready to do slumping
//=========================================================================================================================================
int CSimulation::nInitSlumping(void)
{
   m_dSlumpAngleOfRestDiff      = m_dCellSide * m_dSlumpAngleOfRest / 100;
   m_dSlumpAngleOfRestDiffDiag  = m_dCellDiag * m_dSlumpAngleOfRest / 100;
   m_dToppleCritDiff            = m_dCellSide * m_dToppleCriticalAngle / 100;
   m_dToppleCritDiffDiag        = m_dCellDiag * m_dToppleCriticalAngle / 100;
   m_dToppleAngleOfRestDiff     = m_dCellSide * m_dToppleAngleOfRest / 100;
   m_dToppleAngleOfRestDiffDiag = m_dCellDiag * m_dToppleAngleOfRest / 100;

   m_nSSSQuadrantSize = tMax(1, static_cast<int>(floor(m_dSSSPatchSize / m_dCellSide)));

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

   // Calculate a weight for each cell in the quadrant. If m_SSSWeightQuadrant[0][0] is considered to be the bottom-left corner, then the weights of the top-left and bottom-right corners are zero. The weights of the cells along the left column and bottom row are interpolated linearly
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
      if (bFpEQ(m_SSSWeightQuadrant[n][0], 0.0, TOLERANCE))
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

//=========================================================================================================================================
//! This member function of CSimulation distributes shear stress in a patch around a target cell
//=========================================================================================================================================
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

//=========================================================================================================================================
//! This member function of CSimulation simulates slumping (as occurs in e.g. rill sidewalls) for all cells
//=========================================================================================================================================
void CSimulation::DoAllSlump(void)
{
   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      for (int nY = 0; nY < m_nYGridMax; nY++)
      {
         if (m_Cell[nX][nY].bIsMissingValue())
            continue;

         // Set the this-operation (actually they are kept for several iterations) values for slumping and toppling
         m_Cell[nX][nY].pGetSoil()->ZeroThisOperationSlump();

         // Now total up the shear stress in this and adjacent cells
         int nXTmp, nYTmp, nCount = 0;;
         double dThisStress = m_Cell[nX][nY].pGetSoil()->dGetShearStress();

         // Planview bottom
         nXTmp = nX;
         nYTmp = nY+1;
         if ((nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && (m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
         {
            dThisStress += m_Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();
            nCount++;
         }

         // Planview bottom right
         nXTmp = nX+1;
         nYTmp = nY+1;
         if ((nXTmp < m_nXGridMax) && (nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && (m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
         {
            dThisStress += m_Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();
            nCount++;
         }

         // Planview bottom left
         nXTmp = nX-1;
         nYTmp = nY+1;
         if ((nXTmp >= 0) && (nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && (m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
         {
            dThisStress += m_Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();
            nCount++;
         }

         // Planview right
         nXTmp = nX+1;
         nYTmp = nY;
         if ((nXTmp < m_nXGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && (m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
         {
            dThisStress += m_Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();
            nCount++;
         }

         // Planview left
         nXTmp = nX-1;
         nYTmp = nY;
         if ((nXTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && (m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
         {
            dThisStress += m_Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();
            nCount++;
         }

         // Planview top right
         nXTmp = nX+1;
         nYTmp = nY-1;
         if ((nXTmp < m_nXGridMax) && (nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && (m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
         {
            dThisStress += m_Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();
            nCount++;
         }

         // Planview top left
         nXTmp = nX-1;
         nYTmp = nY-1;
         if ((nXTmp >= 0) && (nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && (m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
         {
            dThisStress += m_Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();
            nCount++;
         }

         // Planview top
         nXTmp = nX;
         nYTmp = nY-1;
         if ((nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && (m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
         {
            dThisStress += m_Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();
            nCount++;
         }

         // Any shear stress?
         if (bFpEQ(dThisStress, 0.0, TOLERANCE))
            continue;

         if (nCount < 8)
         {
            // At least one of the surrounding eight cells could not be read, which means that we are near an edge. So compensate for this
            double dAvgStress = dThisStress / nCount;
            dThisStress = dAvgStress * 8;
         }

         // OK, we have some shear stress. Now get the water content of this cell, as a fraction. But if we aren't simulating infilt, assume the soil is saturated
         // TODO get working for multiple soil layers
         double dWaterFrac = 1;
         if (m_bDoInfiltration)
         {
            // Calculate the saturated (maximum) soil water content (a depth equivalent) for the top soil layer on this cell
            CCellSoilLayer const* pTopLayer = m_Cell[nX][nY].pGetSoil()->pLayerGetLayer(0);

            // Get the current water content (a depth equivalent) for the top soil layer
            double dTopLayerSoilWater = pTopLayer->dGetSoilWater();

            // Now calculate the saturated (maximum) soil water content (a depth equivalent) for this layer
            double
               dTopLayerThickness = pTopLayer->dGetLayerThickness(),
               dTopLayerMaxSoilWater = m_VdInputSoilLayerInfiltSatWater[0] * dTopLayerThickness;

            // And so get the fraction saturated
            dWaterFrac = dTopLayerSoilWater / dTopLayerMaxSoilWater;
         }

         // Assume that shear stress (and hence slumping) is linearly proportional to the water content of this cell TODO check this
         dThisStress *= dWaterFrac;
         dThisStress /= (m_dSimulatedTimeElapsed - m_dLastSlumpCalcTime);

         // So we have the total shear stress in this cell, from all wet cells adjacent to this cell. Does this exceed the threshold shear stress for slumping?
         if (dThisStress >= m_dCritSSSForSlump)
         {
            // Yes, it does: so do some slumping
            bool bDiag = false;
            double dDiff;

            // Find the adjacent wet cell with the steepest downhill soil-surface gradient, however this must not be an edge cell
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

                  // Remove the soil from the higher cell, and move it to the lower cCell
                  double
                     dClayDetached = 0,
                     dSiltDetached = 0,
                     dSandDetached = 0;

                  m_Cell[nX][nY].pGetSoil()->DoSlumpDetach(dDiff, dClayDetached, dSiltDetached, dSandDetached);

                  // Add to 'since last' values
                  m_dEndOfIterClaySlumpDetach += dClayDetached;
                  m_dEndOfIterSiltSlumpDetach += dSiltDetached;
                  m_dEndOfIterSandSlumpDetach += dSandDetached;

                  // Now add the detached soil to the 'To' cell (as part of the top soil layer if dry, or to the sediment load if wet)
                  double
                     dClayDeposited = 0,
                     dSiltDeposited = 0,
                     dSandDeposited = 0,
                     dClayToSedLoad = 0,
                     dSiltToSedLoad = 0,
                     dSandToSedLoad = 0;

                  m_Cell[nXTmp][nYTmp].pGetSoil()->DoSlumpDepositOrToSedLoad(dClayDetached, dSiltDetached, dSandDetached, dClayDeposited, dSiltDeposited, dSandDeposited, dClayToSedLoad, dSiltToSedLoad, dSandToSedLoad);

                  // Add to 'since last' values
                  m_dEndOfIterClaySlumpDeposit += dClayDeposited;
                  m_dEndOfIterSiltSlumpDeposit += dSiltDeposited;
                  m_dEndOfIterSandSlumpDeposit += dSandDeposited;
                  m_dEndOfIterClaySlumpToSedLoad += dClayToSedLoad;
                  m_dEndOfIterSiltSlumpToSedLoad += dSiltToSedLoad;
                  m_dEndOfIterSandSlumpToSedLoad += dSandToSedLoad;

                  // Has this slumping also triggered toppling of any now-unstable upslope cells?
                  int nRecursionDepth = MAX_RECURSION_DEPTH;
                  TryToppleCellsAbove(nX, nY, nRecursionDepth);
               }
            }
         }
      }
   }
}

//=========================================================================================================================================
//! Identifies the adjacent wet cell which has the steepest downhill soil-surface gradient. However this must not be an edge cell
//=========================================================================================================================================
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
      dSlope = dTmpDiff * m_dInvCellSide;                                                       // is tan(top slope)
      nLowX = nXTmp;
      nLowY = nYTmp;
      dDiff = dTmpDiff;
      nDir = DIRECTION_BOTTOM;
      bDiag = false;
   }

   // Planview bottom right
   nXTmp = nX+1;
   nYTmp = nY+1;
   if ((nXTmp < m_nXGridMax) && (nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellDiag) > dSlope))
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
   if ((nXTmp >= 0) && (nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellDiag) > dSlope))
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
   if ((nXTmp < m_nXGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellSide) > dSlope))
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
   if ((nXTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellSide) > dSlope))
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
   if ((nXTmp < m_nXGridMax) && (nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellDiag) > dSlope))
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
   if ((nXTmp >= 0) && (nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellDiag) > dSlope))
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
   if ((nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && m_Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0) && ((dTmpDiff * m_dInvCellSide) > dSlope))
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

//=========================================================================================================================================
//! Topples all unstable cells above a point of slumping; calls itself recursively until there are no more cells to topple. However, no toppling is allowed on edge cells, to avoid edge effects
//=========================================================================================================================================
void CSimulation::TryToppleCellsAbove(int const nX, int const nY, int nRecursionDepth)
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
      // It's uphill, and the slope is above the critical toppling value, so topple cells
      DoToppleCells(nX, nY, nXTmp, nYTmp, dDiff, false);

      // Now call this routine recursively
      TryToppleCellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }

   // The cell at planview bottom right
   nXTmp = nX+1;
   nYTmp = nY+1;
   if ((nXTmp < m_nXGridMax) && (nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dDiff = m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiffDiag))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple cells
      DoToppleCells(nX, nY, nXTmp, nYTmp, dDiff, true);

      // Now call this routine recursively
      TryToppleCellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }

   // The cell at planview bottom left
   nXTmp = nX-1;
   nYTmp = nY+1;
   if ((nXTmp >= 0) && (nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dDiff = m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiffDiag))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple cells
      DoToppleCells(nX, nY, nXTmp, nYTmp, dDiff, true);

      // Now call this routine recursively
      TryToppleCellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }

   // The cell at planview right
   nXTmp = nX+1;
   nYTmp = nY;
   if ((nXTmp < m_nXGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dDiff = m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiff))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple cells
      DoToppleCells(nX, nY, nXTmp, nYTmp, dDiff, false);

      // Now call this routine recursively
      TryToppleCellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }

   // The cell at planview left
   nXTmp = nX-1;
   nYTmp = nY;
   if ((nXTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dDiff = m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiff))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple cells
      DoToppleCells(nX, nY, nXTmp, nYTmp, dDiff, false);

      // Now call this routine recursively
      TryToppleCellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }

   // The cell at planview top right
   nXTmp = nX+1;
   nYTmp = nY-1;
   if ((nXTmp < m_nXGridMax) && (nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dDiff = m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiffDiag))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple cells
      DoToppleCells(nX, nY, nXTmp, nYTmp, dDiff, true);

      // Now call this routine recursively
      TryToppleCellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }

   // The cell at planview top left
   nXTmp = nX-1;
   nYTmp = nY-1;
   if ((nXTmp >= 0) && (nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dDiff = m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiffDiag))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple cells
      DoToppleCells(nX, nY, nXTmp, nYTmp, dDiff, true);

      // Now call this routine recursively
      TryToppleCellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }

   // The cell at planview top
   nXTmp = nX;
   nYTmp = nY-1;
   if ((nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dDiff = m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiff))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple cells
      DoToppleCells(nX, nY, nXTmp, nYTmp, dDiff, false);

      // Now call this routine recursively
      TryToppleCellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }
}

//=========================================================================================================================================
//! Does toppling, i.e. moves sediment from an over-steepened cell to an adjacent cell. If the target cell is wet, then the sediment is moved into that cell's sediment load
//=========================================================================================================================================
void CSimulation::DoToppleCells(int const nX, int const nY, int const nXTmp, int const nYTmp, double dDiff, bool const bDiag)
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
   m_dEndOfIterClayToppleDetach += dClayDetached;
   m_dEndOfIterSiltToppleDetach += dSiltDetached;
   m_dEndOfIterSandToppleDetach += dSandDetached;

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
   m_dEndOfIterClayToppleDeposit += dClayDeposited;
   m_dEndOfIterSiltToppleDeposit += dSiltDeposited;
   m_dEndOfIterSandToppleDeposit += dSandDeposited;
   m_dEndOfIterClayToppleToSedLoad += dClayToSedLoad;
   m_dEndOfIterSiltToppleToSedLoad += dSiltToSedLoad;
   m_dEndOfIterSandToppleToSedLoad += dSandToSedLoad;
}

