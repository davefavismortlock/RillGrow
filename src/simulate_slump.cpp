/*=========================================================================================================================================

 This is do_slump.cpp: it handles slumping of e.g. rill sidewalls

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

 This member function of CSimulation simulates slumping

=========================================================================================================================================*/
void CSimulation::DoAllSlump(void)
{
   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      for (int nY = 0; nY < m_nYGridMax; nY++)
      {
         if (Cell[nX][nY].bIsMissingOrEdge())
         {
            // Don't start slumping on edge cells, this is to avoid edge effects
            continue;
         }
         
         // Set this this-iteration (actually they are kept for several iterations) values for slumping and toppling
         Cell[nX][nY].pGetSoil()->ZeroThisIterSlump();

         // Now total up the shear stress in this and adjacent cells
         int nXTmp, nYTmp;
         double dThisStress = Cell[nX][nY].pGetSoil()->dGetShearStress();

         // Planview bottom
         nXTmp = nX;
         nYTmp = nY+1;
         if ((nYTmp < m_nYGridMax) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && (Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
            dThisStress += Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();

         // Planview bottom right
         nXTmp = nX+1;
         nYTmp = nY+1;
         if ((nXTmp < m_nXGridMax) && (nYTmp < m_nYGridMax) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && (Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
            dThisStress += Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();

         // Planview bottom left
         nXTmp = nX-1;
         nYTmp = nY+1;
         if ((nXTmp >= 0) && (nYTmp < m_nYGridMax) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && (Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
            dThisStress += Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();

         // Planview right
         nXTmp = nX+1;
         nYTmp = nY;
         if ((nXTmp < m_nXGridMax) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && (Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
            dThisStress += Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();

         // Planview left
         nXTmp = nX-1;
         nYTmp = nY;
         if ((nXTmp >= 0) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && (Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
            dThisStress += Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();

         // Planview top right
         nXTmp = nX+1;
         nYTmp = nY-1;
         if ((nXTmp < m_nXGridMax) && (nYTmp >= 0) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && (Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
            dThisStress += Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();

         // Planview top left
         nXTmp = nX-1;
         nYTmp = nY-1;
         if ((nXTmp >= 0) && (nYTmp >= 0) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && (Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
            dThisStress += Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();

         // Planview top
         nXTmp = nX;
         nYTmp = nY-1;
         if ((nYTmp >= 0) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && (Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet()))
            dThisStress += Cell[nXTmp][nYTmp].pGetSoil()->dGetShearStress();
         
         // Any shear stress?
         if (dThisStress == 0)
            continue;

         // OK, we have some shear stress. Now get the water content of this cell, as a fraction. But if we aren't simulating infiltration, assume the soil is saturated
         // TODO get working for multiple soil layers
         double dWaterFrac = 1;
         if (m_bDoInfiltration)
         {
            // Calculate the saturated (maximum) soil water content (a depth equivalent) for the top soil layer on this cell
            CLayer* pTopLayer = Cell[nX][nY].pGetSoil()->pLayerGetLayer(0);      
            
            // Get the current subsurface water content (a depth equivalent) for this layer
            double dTopLayerSoilWater = pTopLayer->dGetSoilWater();
            
            // Now calculate the saturated (maximum) soil water content (a depth equivalent) for this layer
            double 
               dTopLayerThickness = pTopLayer->dGetLayerThickness(),
               dTopLayerMaxSoilWater = m_VdInputSoilLayerInfiltSatWater[0] * dTopLayerThickness;

            dWaterFrac = dTopLayerSoilWater / dTopLayerMaxSoilWater;
         }

         // Assume that shear stress (and hence slumping) is linearly proportional to the water content of this cell
         // TODO check this
         dThisStress *= dWaterFrac;
         dThisStress /= (m_dSimulatedTimeElapsed - m_dLastSlumpCalcTime);

         // So we have the total shear stress in this cell, from all wet cells adjacent to this cell. Is this total shear stress of this cell above the threshold?
         if (dThisStress >= m_dSlumpCritShearStress)
         {
            // Yes, it is: so do some slumping
            bool bDiag = false;
            double dDiff;

            // Find the adjacent wet cell with the steepest downhill soil-surface gradient, however this must not be an edge cell
            if ((nFindSteepestSoilSurface(nX, nY, Cell[nX][nY].pGetSoil()->dGetSoilSurfaceElevation(), nXTmp, nYTmp, dDiff, bDiag)) != DIRECTION_NONE)
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

                  // Remove the soil from the higher cell, and move it (as sediment load) to the lower cell
                  double 
                     dClayDetached = 0,
                     dSiltDetached = 0,
                     dSandDetached = 0;
                     
                  Cell[nX][nY].pGetSoil()->DoSlumpDetach(dDiff, dClayDetached, dSiltDetached, dSandDetached);
                  
                  // Add the detached soil to the downhill wet cell's sediment load
                  Cell[nXTmp][nYTmp].pGetSediment()->dChangeClaySedimentLoad(dClayDetached);
                  Cell[nXTmp][nYTmp].pGetSediment()->dChangeSiltSedimentLoad(dSiltDetached);
                  Cell[nXTmp][nYTmp].pGetSediment()->dChangeSandSedimentLoad(dSandDetached);
                  
                  // To keep the mass balance calcs working correctly, also store slump deposition (even tho' the slump-detached sediment has been moved to the wet cell's sediment load
                  Cell[nXTmp][nYTmp].pGetSoil()->AddSlumpDeposition(dClayDetached, dSiltDetached, dSandDetached);
                  m_dThisIterClaySlumpDeposit += dClayDetached;
                  m_dThisIterSiltSlumpDeposit += dSiltDetached;
                  m_dThisIterSandSlumpDeposit += dSandDetached;                 
                  
//                   LogStream << m_ulIter << " [" << nX << "][" << nY << "] slump detach="  << -dDiff << ", [" << nXTmp << "][" << nYTmp << "] deposit=" << dDiff << endl;

                  // Has this slumping also triggered toppling of now-unstable upslope cells?
                  int nRecursionDepth = MAX_RECURSION_DEPTH;        
                  TryToppleCellsAbove(nX, nY, nRecursionDepth);
               }
            }
         }
      }
   }
}



/*=========================================================================================================================================

 Identifies the adjacent wet cell which has the steepest downhill soil-surface gradient. However this must not be an edge cell

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
   if ((nYTmp < m_nYGridMax) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0))
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
   if ((nXTmp < m_nXGridMax) && (nYTmp < m_nYGridMax) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellDiag) > dSlope))
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
   if ((nXTmp >= 0) && (nYTmp < m_nYGridMax) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellDiag) > dSlope))
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
   if ((nXTmp < m_nXGridMax) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellSide) > dSlope))
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
   if ((nXTmp >= 0) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellSide) > dSlope))
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
   if ((nXTmp < m_nXGridMax) && (nYTmp >= 0) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellDiag) > dSlope))
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
   if ((nXTmp >= 0) && (nYTmp >= 0) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellDiag) > dSlope))
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
   if ((nYTmp >= 0) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && Cell[nXTmp][nYTmp].pGetSurfaceWater()->bIsWet() && ((dTmpDiff = dThisElev - Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellSide) > dSlope))
   {
      // It's wet and the steepest so far
//       dSlope = dTanX;                                                                           // is tan(top slope)
      nLowX = nXTmp;
      nLowY = nYTmp;
      dDiff = dTmpDiff;
      nDir = DIRECTION_TOP;
      bDiag = false;
   }

   return (nDir);
}


/*=========================================================================================================================================

 Topples all unstable cells above a point of slumping; calls itself recursively until there are no more cells to topple. However, no toppling is allowed in edge cells, to avoid edge effects

=========================================================================================================================================*/
void CSimulation::TryToppleCellsAbove(int const nX, int const nY, int nRecursionDepth)
{
   if (nRecursionDepth <= 0)
      return;

   nRecursionDepth--;

   int
      nXTmp,
      nYTmp;
   double
      dThisElev = Cell[nX][nY].pGetSoil()->dGetSoilSurfaceElevation(),
      dDiff;

   // Cell at planview bottom
   nXTmp = nX;
   nYTmp = nY+1;
   if ((nYTmp < m_nYGridMax) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && ((dDiff = Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiff))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple cells
      DoToppleCells(nX, nY, nXTmp, nYTmp, dDiff, false);

      // Now call this routine recursively
      TryToppleCellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }

   // Cell at planview bottom right
   nXTmp = nX+1;
   nYTmp = nY+1;
   if ((nXTmp < m_nXGridMax) && (nYTmp < m_nYGridMax) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && ((dDiff = Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiffDiag))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple cells
      DoToppleCells(nX, nY, nXTmp, nYTmp, dDiff, true);

      // Now call this routine recursively
      TryToppleCellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }

   // Cell at planview bottom left
   nXTmp = nX-1;
   nYTmp = nY+1;
   if ((nXTmp >= 0) && (nYTmp < m_nYGridMax) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && ((dDiff = Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiffDiag))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple cells
      DoToppleCells(nX, nY, nXTmp, nYTmp, dDiff, true);

      // Now call this routine recursively
      TryToppleCellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }

   // Cell at planview right
   nXTmp = nX+1;
   nYTmp = nY;
   if ((nXTmp < m_nXGridMax) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && ((dDiff = Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiff))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple cells
      DoToppleCells(nX, nY, nXTmp, nYTmp, dDiff, false);

      // Now call this routine recursively
      TryToppleCellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }

   // Cell at planview left
   nXTmp = nX-1;
   nYTmp = nY;
   if ((nXTmp >= 0) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && ((dDiff = Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiff))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple cells
      DoToppleCells(nX, nY, nXTmp, nYTmp, dDiff, false);

      // Now call this routine recursively
      TryToppleCellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }

   // Cell at planview top right
   nXTmp = nX+1;
   nYTmp = nY-1;
   if ((nXTmp < m_nXGridMax) && (nYTmp >= 0) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && ((dDiff = Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiffDiag))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple cells
      DoToppleCells(nX, nY, nXTmp, nYTmp, dDiff, true);

      // Now call this routine recursively
      TryToppleCellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }

   // Cell at planview top left
   nXTmp = nX-1;
   nYTmp = nY-1;
   if ((nXTmp >= 0) && (nYTmp >= 0) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && ((dDiff = Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiffDiag))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple cells
      DoToppleCells(nX, nY, nXTmp, nYTmp, dDiff, true);

      // Now call this routine recursively
      TryToppleCellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }

   // Cell at planview top
   nXTmp = nX;
   nYTmp = nY-1;
   if ((nYTmp >= 0) && (! Cell[nXTmp][nYTmp].bIsMissingOrEdge()) && ((dDiff = Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation() - dThisElev) > m_dToppleCritDiff))
   {
      // It's uphill, and the slope is above the critical toppling value, so topple cells
      DoToppleCells(nX, nY, nXTmp, nYTmp, dDiff, false);

      // Now call this routine recursively
      TryToppleCellsAbove(nXTmp, nYTmp, nRecursionDepth);
   }
}


/*=========================================================================================================================================

 Does toppling, i.e. moves sediment from one cell onto another

=========================================================================================================================================*/
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
   Cell[nXTmp][nYTmp].pGetSoil()->DoToppleDetach(dDiff, dClayDetached, dSiltDetached, dSandDetached);
      
   // And now do the deposition
   Cell[nX][nY].pGetSoil()->DoClayToppleDeposit(dClayDetached);
   Cell[nX][nY].pGetSoil()->DoSiltToppleDeposit(dSiltDetached);
   Cell[nX][nY].pGetSoil()->DoSandToppleDeposit(dSandDetached);
   
   // Add to this-iteration totals
   m_dThisIterClayToppleDeposit += dClayDetached;
   m_dThisIterSiltToppleDeposit += dSiltDetached;
   m_dThisIterSandToppleDeposit += dSandDetached;
   
//#if defined _DEBUG
//   LogStream << m_ulIter << " [" << nX << "][" << nY << "] topple deposit="  << dDiff << ", [" << nXTmp << "][" << nYTmp << "] detach=" << -dDiff << endl;
//#endif
}


/*=========================================================================================================================================
 
 Adds to the this-iteration slump detachment value for clay-sized sediment
 
=========================================================================================================================================*/
void CSimulation::AddThisIterClaySlumpDetach(double const dDetach)
{
   m_dThisIterClaySlumpDetach += dDetach;
}

/*=========================================================================================================================================
 
 Adds to the this-iteration slump detachment value for silt-sized sediment
 
=========================================================================================================================================*/
void CSimulation::AddThisIterSiltSlumpDetach(double const dDetach)
{
   m_dThisIterSiltSlumpDetach += dDetach;
}

/*=========================================================================================================================================
 
 Adds to the this-iteration slump detachment value for sand-sized sediment
 
=========================================================================================================================================*/
void CSimulation::AddThisIterSandSlumpDetach(double const dDetach)
{
   m_dThisIterSandSlumpDetach += dDetach;
}

/*=========================================================================================================================================
 
 Adds to the this-iteration toppling detachment value for clay-sized sediment
 
=========================================================================================================================================*/
void CSimulation::AddThisIterClayToppleDetach(double const dDetach)
{
   m_dThisIterClayToppleDetach += dDetach;
}

/*=========================================================================================================================================
 
 Adds to the this-iteration toppling detachment value for silt-sized sediment
 
=========================================================================================================================================*/
void CSimulation::AddThisIterSiltToppleDetach(double const dDetach)
{
   m_dThisIterSiltToppleDetach += dDetach;
}

/*=========================================================================================================================================
 
 Adds to the this-iteration toppling detachment value for sand-sized sediment
 
=========================================================================================================================================*/
void CSimulation::AddThisIterSandToppleDetach(double const dDetach)
{
   m_dThisIterSandToppleDetach += dDetach;
}
