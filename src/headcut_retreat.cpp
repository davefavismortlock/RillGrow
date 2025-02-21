/*=========================================================================================================================================

This is headcut_retreat.cpp: it simulates headcut retreat for RillGrow

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
//! This member function of CSimulation simulates headcut retreat on all cells
//=========================================================================================================================================
void CSimulation::DoAllHeadcutRetreat(void)
{
   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      for (int nY = 0; nY < m_nYGridMax; nY++)
      {
         if (m_Cell[nX][nY].bIsMissingValue())
         {
            // Don't do headcut retreat on edge cells, this is to avoid edge effects TODO May be OK as long as retreat direction is not towards the edge
            continue;
         }

         // OK check to see if this cell is ready for headcut collapse
         for (int nDir = 0; nDir < 8; nDir++)
         {
            double dSize = m_dCellSide;
            if ((nDir % 2) != 0)
               dSize = m_dCellDiag;

            if (m_Cell[nX][nY].bIsReadyForHeadcutRetreat(nDir, dSize))
            {
               //                m_ofsLog << m_ulIter << " [" << nX << "][" << nY << "] ready for headcut collapse in direction " << nDir << endl;

               // Get the coords of the 'upstream' cell
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

               // Safety check
               if ((nUpStreamX < 0) || (nUpStreamY < 0) || (nUpStreamX > m_nXGridMax-1) || (nUpStreamY > m_nYGridMax-1))
                  continue;

               // Get the soil surface elevation of this cell
               double dThisElev = m_Cell[nX][nY].pGetSoil()->dGetSoilSurfaceElevation();

               // Get the soil surface elevation of the 'upstream' cell
               double dUpStreamElev = m_Cell[nUpStreamX][nUpStreamY].pGetSoil()->dGetSoilSurfaceElevation();

               // And calculate the elevation difference
               double dElevDiff = dUpStreamElev - dThisElev;

               // Is the elevation difference nearly zero?
               if (bFpEQ(dElevDiff, 0.0, SEDIMENT_TOLERANCE))
               {
                  // Just try the next direction. Note that we don't zero the retreat for this direction since the elvation of the upstream cell may change later
                  continue;
               }

               // OK, we have a reasonable elevation difference
               if (dElevDiff < 0)
               {
                  // The upstream cell is lower than this cell, so this cell is a effectively a watershed boundary
                  //                  m_ofsLog << m_ulIter << " [" << nX << "][" << nY << "] (dThisElev=" << dThisElev << ") is a watershed boundary headcut, with [" << nUpStreamX << "][" << nUpStreamY << "] (dUpStreamElev=" << dUpStreamElev << "), dElevDiff=" << dElevDiff << endl;

                  // Move soil from this cell to the upstream cell
                  DoHeadcutRetreatMoveSoil(nDir, nX, nY, nUpStreamX, nUpStreamY, -dElevDiff * m_dHeadcutRetreatConst);
               }
               else
               {
                  // The upstream cell is higher than this cell, so this is a normal headcut
                  //                  m_ofsLog << m_ulIter << " [" << nX << "][" << nY << "] (dThisElev=" << dThisElev << ") is a normal headcut, with [" << nUpStreamX << "][" << nUpStreamY << "] (dUpStreamElev=" << dUpStreamElev << "), dElevDiff=" << dElevDiff << endl;

                  // Move soil from the upstream cell to this cell
                  DoHeadcutRetreatMoveSoil(nDir, nUpStreamX, nUpStreamY, nX, nY, dElevDiff * m_dHeadcutRetreatConst);
               }

               // Set the flag
               m_Cell[nX][nY].SetHasHadHeadcutRetreat();
            }
         }
      }
   }
}

//=========================================================================================================================================
//! Moves soil from one cell to an adjacent cell for headcut retreat. If the target cell is wet, the sediment is added to the cell's sediment load
//=========================================================================================================================================
void CSimulation::DoHeadcutRetreatMoveSoil(int const nDir, int const nFromX, int const nFromY, int const nToX, int const nToY, double const dDiff)
{
   double
      dHalfDiff = dDiff * 0.5,
      dClayDetached = 0,
      dSiltDetached = 0,
      dSandDetached = 0;

   // Remove soil from the 'From' cell
   m_Cell[nFromX][nFromY].pGetSoil()->DoHeadcutRetreatDetach(dHalfDiff, dClayDetached, dSiltDetached, dSandDetached);

   // Add to the 'since last' values
   m_dEndOfIterClayHeadcutDetach += dClayDetached;
   m_dEndOfIterSiltHeadcutDetach += dSiltDetached;
   m_dEndOfIterSandHeadcutDetach += dSandDetached;

   // Now add the detached soil to the 'To' cell: as part of the top soil layer if dry, or as sediment load if wet
   double
      dClayDeposited = 0,
      dSiltDeposited = 0,
      dSandDeposited = 0,
      dClayToSedLoad = 0,
      dSiltToSedLoad = 0,
      dSandToSedLoad = 0;

   m_Cell[nToX][nToY].pGetSoil()->DoHeadcutRetreatDepositOrToSedLoad(dClayDetached, dSiltDetached, dSandDetached, dClayDeposited, dSiltDeposited, dSandDeposited, dClayToSedLoad, dSiltToSedLoad, dSandToSedLoad);

   m_dEndOfIterClayHeadcutDeposit += dClayDeposited;
   m_dEndOfIterSiltHeadcutDeposit += dSiltDeposited;
   m_dEndOfIterSandHeadcutDeposit += dSandDeposited;
   m_dEndOfIterClayHeadcutToSedLoad += dClayToSedLoad;
   m_dEndOfIterSiltHeadcutToSedLoad += dSiltToSedLoad;
   m_dEndOfIterSandHeadcutToSedLoad += dSandToSedLoad;

   m_Cell[nFromX][nFromY].SetStoredRetreat(nDir, 0);
   m_Cell[nToX][nToY].SetStoredRetreat(nDir, 0);

   //    m_ofsLog << m_ulIter << " headcut retreat, from [" << nFromX << "][" << nFromY << "] to [" << nToX << "][" << nToY << "] depth=" << dHalfDiff << " (clay=" << dClayDetached << ", silt=" << dSiltDetached << ", sand=" << dSandDetached << ")" << endl;
}

