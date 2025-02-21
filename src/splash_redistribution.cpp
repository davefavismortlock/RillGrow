/*=========================================================================================================================================

This is splash_redistribution.cpp: it handles splash redistribution for RillGrow

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
//! This member function of CSimulation does splash redistribution on the whole grid at each timestep
//=========================================================================================================================================
void CSimulation::DoAllSplash(void)
{
   m_dEndOfIterKE = 0;

   if (m_bPoesenSplashEqn)
   {
      // // TEST
      // double dTotClayDetach = 0;
      // double dTotSiltDetach = 0;
      // double dTotSandDetach = 0;
      // double dTotClayDeposit = 0;
      // double dTotSiltDeposit = 0;
      // double dTotSandDeposit = 0;
      // double dTotClayToSedLoad = 0;
      // double dTotSiltToSedLoad = 0;
      // double dTotSandToSedLoad = 0;
      // double dTotClayOffEdge = 0;
      // double dTotSiltOffEdge = 0;
      // double dTotSandOffEdge = 0;

      for (int nX = 0; nX < m_nXGridMax; nX++)
      {
         for (int nY = 0; nY < m_nYGridMax; nY++)
         {
            if (m_Cell[nX][nY].bIsMissingValue())
               continue;

            // Get the depth of rain on this cell during this iteration, if any
            double dRain = m_Cell[nX][nY].pGetRainAndRunon()->dGetRain();

            if (bFpEQ(dRain, 0.0, TOLERANCE))
               continue;

            // OK, some rain fell on this cell. So calculate the kinetic energy of the rain = 0.5 m v**2
            double dKE = m_dPartKE * dRain;
            m_dEndOfIterKE += dKE;

            // Now calculate the decrease in elevation due to splash
            double dSplashErosion = dKE * m_dSplashConstantNormalized;

            // We have splash detachment. Attenuate the decrease in elevation depending on the depth of surface water
            dSplashErosion *= dCalcSplashCubicSpline(m_Cell[nX][nY].pGetSurfaceWater()->dGetSurfaceWaterDepth());

            // Now do the splash detachment
            double dClayDetach = 0;
            double dSiltDetach = 0;
            double dSandDetach = 0;
            m_Cell[nX][nY].pGetSoil()->DoSplashDetach(dSplashErosion, dClayDetach, dSiltDetach, dSandDetach);

            // // And add to totals detached
            // dTotClayDetach += dClayDetach;
            // dTotSiltDetach += dSiltDetach;
            // dTotSandDetach += dSandDetach;

            // Next, distribute the detached soil onto the adjacent cells, either as deposition or (if the cell is wet) as sediment load
            double dThisElev = m_Cell[nX][nY].pGetSoil()->dGetSoilSurfaceElevation();

            for (int nDirection = 0; nDirection < 4; nDirection++)
            {
               int nXAdjOneSide;
               int nYAdjOneSide;
               int nXAdjOtherSide;
               int nYAdjOtherSide;
               bool bOffEdgeThis = false;
               bool bOffEdgeOpposite = false;
               bool bDownSlopeThis = false;
               bool bDownSlopeOpposite = false;
               double dTanBetaThis;
               double dTanBetaOpposite;

               if (nDirection == DIRECTION_TOP)
               {
                  // Planview top
                  nXAdjOneSide = nX;
                  nYAdjOneSide = nY-1;
                  bOffEdgeThis = false;
                  bDownSlopeThis = true;

                  if ((m_Cell[nX][nY].nGetEdge() == DIRECTION_TOP) || (nY == 0))
                  {
                     // This is a cell on the planview top edge
                     bOffEdgeThis = true;
                  }
                  else
                  {
                     // Not on planview top edge. Get the elevation difference
                     double dElevDiff = dThisElev - m_Cell[nXAdjOneSide][nYAdjOneSide].pGetSoil()->dGetSoilSurfaceElevation();

                     // Is there a downslope gradient to the adjacent cell?
                     if (dElevDiff < 0)
                        bDownSlopeThis = false;

                     dTanBetaThis = dElevDiff * m_dInvCellSide;
                  }

                  // Now deal with the opposite direction, which is planview bottom
                  nXAdjOtherSide = nX;
                  nYAdjOtherSide = nY+1;
                  bOffEdgeOpposite = false;
                  bDownSlopeOpposite = true;

                  if ((m_Cell[nX][nY].nGetEdge() == DIRECTION_BOTTOM) || (nY == m_nYGridMax-1))
                  {
                     // This is a cell on the planview bottom edge
                     bOffEdgeOpposite = true;
                  }
                  else
                  {
                     // Not on planview bottom edge. Get the elevation difference
                     double dElevDiff = dThisElev - m_Cell[nXAdjOtherSide][nYAdjOtherSide].pGetSoil()->dGetSoilSurfaceElevation();

                     // Is there a downslope gradient to the adjacent cell?
                     if (dElevDiff < 0)
                        bDownSlopeOpposite = false;

                     dTanBetaOpposite = dElevDiff * m_dInvCellSide;
                  }
               }

               else if (nDirection == DIRECTION_TOP_RIGHT)
               {
                  // Planview top right
                  nXAdjOneSide = nX+1;
                  nYAdjOneSide = nY-1;
                  bOffEdgeThis = false;
                  bDownSlopeThis = true;

                  int nEdge = m_Cell[nX][nY].nGetEdge();
                  if ((nEdge == DIRECTION_TOP) || (nEdge == DIRECTION_RIGHT) || (nY == 0) || (nX == m_nXGridMax-1))
                  {
                     // This is a cell on the planview top edge or the planview right edge
                     bOffEdgeThis = true;
                  }
                  else
                  {
                     // Not on planview top edge or planview right edge. Get the elevation difference
                     double dElevDiff = dThisElev - m_Cell[nXAdjOneSide][nYAdjOneSide].pGetSoil()->dGetSoilSurfaceElevation();

                     // Is there a downslope gradient to the adjacent cell?
                     if (dElevDiff < 0)
                        bDownSlopeThis = false;

                     dTanBetaThis = dElevDiff * m_dInvCellSide;
                  }

                  // Now deal with the opposite direction, which is planview bottom left
                  nXAdjOtherSide = nX-1;
                  nYAdjOtherSide = nY+1;
                  bOffEdgeOpposite = false;
                  bDownSlopeOpposite = true;

                  nEdge = m_Cell[nX][nY].nGetEdge();
                  if ((nEdge == DIRECTION_BOTTOM) || (nEdge == DIRECTION_LEFT) || (nY == m_nYGridMax-1) || (nX == 0))
                  {
                     // This is a cell on the planview bottom edge or the planview left edge
                     bOffEdgeOpposite = true;
                  }
                  else
                  {
                     // Not on planview bottom edge or the planview left edge. Get the elevation difference
                     double dElevDiff = dThisElev - m_Cell[nXAdjOtherSide][nYAdjOtherSide].pGetSoil()->dGetSoilSurfaceElevation();

                     // Is there a downslope gradient to the adjacent cell?
                     if (dElevDiff < 0)
                        bDownSlopeOpposite = false;

                     dTanBetaOpposite = dElevDiff * m_dInvCellSide;
                  }
               }

               else if (nDirection == DIRECTION_RIGHT)
               {
                  // Planview right
                  nXAdjOneSide = nX+1;
                  nYAdjOneSide = nY;
                  bOffEdgeThis = false;
                  bDownSlopeThis = true;

                  if ((m_Cell[nX][nY].nGetEdge() == DIRECTION_RIGHT) || (nX == m_nXGridMax-1))
                  {
                     // This is a cell on the planview right edge
                     bOffEdgeThis = true;
                  }
                  else
                  {
                     // Not on planview right edge. Get the elevation difference
                     double dElevDiff = dThisElev - m_Cell[nXAdjOneSide][nYAdjOneSide].pGetSoil()->dGetSoilSurfaceElevation();

                     // Is there a downslope gradient to the adjacent cell?
                     if (dElevDiff < 0)
                        bDownSlopeThis = false;

                     dTanBetaThis = dElevDiff * m_dInvCellSide;
                  }

                  // Now deal with the opposite direction, which is planview left
                  nXAdjOtherSide = nX-1;
                  nYAdjOtherSide = nY;
                  bOffEdgeOpposite = false;
                  bDownSlopeOpposite = true;

                  if ((m_Cell[nX][nY].nGetEdge() == DIRECTION_LEFT) || (nX == 0))
                  {
                     // This is a cell on the planview left edge
                     bOffEdgeOpposite = true;
                  }
                  else
                  {
                     // Not on planview left edge. Get the elevation difference
                     double dElevDiff = dThisElev - m_Cell[nXAdjOtherSide][nYAdjOtherSide].pGetSoil()->dGetSoilSurfaceElevation();

                     // Is there a downslope gradient to the adjacent cell?
                     if (dElevDiff < 0)
                        bDownSlopeOpposite = false;

                     dTanBetaOpposite = dElevDiff * m_dInvCellSide;
                  }
               }

               else if (nDirection == DIRECTION_BOTTOM_RIGHT)
               {
                  // Planview bottom right
                  nXAdjOneSide = nX+1;
                  nYAdjOneSide = nY+1;
                  bOffEdgeThis = false;
                  bDownSlopeThis = true;

                  int nEdge = m_Cell[nX][nY].nGetEdge();
                  if ((nEdge == DIRECTION_BOTTOM) || (nEdge == DIRECTION_RIGHT) || (nY == m_nYGridMax-1) || (nX == m_nXGridMax-1))
                  {
                     // This is a cell on the planview bottom edge or the planview right edge
                     bOffEdgeThis = true;
                  }
                  else
                  {
                     // Not on planview bottom edge or planview right edge. Get the elevation difference
                     double dElevDiff = dThisElev - m_Cell[nXAdjOneSide][nYAdjOneSide].pGetSoil()->dGetSoilSurfaceElevation();

                     // Is there a downslope gradient to the adjacent cell?
                     if (dElevDiff < 0)
                        bDownSlopeThis = false;

                     dTanBetaThis = dElevDiff * m_dInvCellSide;
                  }

                  // Now deal with the opposite direction, which is planview top left
                  nXAdjOtherSide = nX-1;
                  nYAdjOtherSide = nY-1;
                  bOffEdgeOpposite = false;
                  bDownSlopeOpposite = true;

                  nEdge = m_Cell[nX][nY].nGetEdge();
                  if ((nEdge == DIRECTION_TOP) || (nEdge == DIRECTION_LEFT) || (nX == 0) || (nY == 0))
                  {
                     // This is a cell on the planview top edge or the planview left edge
                     bOffEdgeOpposite = true;
                  }
                  else
                  {
                     // Not on planview top edge or the planview left edge. Get the elevation difference
                     double dElevDiff = dThisElev - m_Cell[nXAdjOtherSide][nYAdjOtherSide].pGetSoil()->dGetSoilSurfaceElevation();

                     // Is there a downslope gradient to the adjacent cell?
                     if (dElevDiff < 0)
                        bDownSlopeOpposite = false;

                     dTanBetaOpposite = dElevDiff * m_dInvCellSide;
                  }
               }

               // Assume that splash is radially symmetrical, so allocate 1/4 of total sediment detached to the four angular directions: this is then split between 'this' and 'opposite' directions
               double dClayToDepositBothSides = dClayDetach / 4;
               double dSiltToDepositBothSides = dSiltDetach / 4;
               double dSandToDepositBothSides = dSandDetach / 4;

               double dClayToDepositThis;
               double dClayToDepositOpposite;
               double dSiltToDepositThis;
               double dSiltToDepositOpposite;
               double dSandToDepositThis;
               double dSandToDepositOpposite;

               if (bOffEdgeThis && bOffEdgeOpposite)
               {
                  // Only applies to corner cells going diaginally (i.e. TL to BR or TR to BL), all goes off-edge
                  m_Cell[nX][nY].pGetSoil()->DoClaySplashOffEdge(dClayToDepositBothSides);
                  m_Cell[nX][nY].pGetSoil()->DoSiltSplashOffEdge(dSiltToDepositBothSides);
                  m_Cell[nX][nY].pGetSoil()->DoSandSplashOffEdge(dSandToDepositBothSides);

                  // dTotClayOffEdge += dClayToDepositBothSides;
                  // dTotSiltOffEdge += dSiltToDepositBothSides;
                  // dTotSandOffEdge += dSandToDepositBothSides;
               }
               else if (bOffEdgeThis)
               {
                  // Assume that half goes off-edge on this side
                  dClayToDepositThis = dClayToDepositOpposite = dClayToDepositBothSides / 2;
                  dSiltToDepositThis = dSiltToDepositOpposite = dSiltToDepositBothSides / 2;
                  dSandToDepositThis = dSandToDepositOpposite = dSandToDepositBothSides / 2;

                  m_Cell[nX][nY].pGetSoil()->DoClaySplashOffEdge(dClayToDepositThis);
                  m_Cell[nX][nY].pGetSoil()->DoSiltSplashOffEdge(dSiltToDepositThis);
                  m_Cell[nX][nY].pGetSoil()->DoSandSplashOffEdge(dSandToDepositThis);

                  // dTotClayOffEdge += dClayToDepositThis;
                  // dTotSiltOffEdge += dSiltToDepositThis;
                  // dTotSandOffEdge += dSandToDepositThis;

                  // And half is either deposited or moved to sediment load on the opposite side
                  bool bToSedLoad = false;
                  m_Cell[nXAdjOtherSide][nYAdjOtherSide].pGetSoil()->DoSplashToSedLoadOrDeposit(dClayToDepositOpposite, dSiltToDepositOpposite, dSandToDepositOpposite, bToSedLoad);

                  // And add to totals
                  if (bToSedLoad)
                  {
                     // dTotClayToSedLoad += dClayToDepositOpposite;
                     // dTotSiltToSedLoad += dSiltToDepositOpposite;
                     // dTotSandToSedLoad += dSandToDepositOpposite;
                  }
                  else
                  {
                     // dTotClayDeposit += dClayToDepositOpposite;
                     // dTotSiltDeposit += dSiltToDepositOpposite;
                     // dTotSandDeposit += dSandToDepositOpposite;
                  }
               }
               else if (bOffEdgeOpposite)
               {
                  // Assume that half goes off-edge on the opposite side
                  dClayToDepositThis = dClayToDepositOpposite = dClayToDepositBothSides / 2;
                  dSiltToDepositThis = dSiltToDepositOpposite = dSiltToDepositBothSides / 2;
                  dSandToDepositThis = dSandToDepositOpposite = dSandToDepositBothSides / 2;

                  m_Cell[nX][nY].pGetSoil()->DoClaySplashOffEdge(dClayToDepositOpposite);
                  m_Cell[nX][nY].pGetSoil()->DoSiltSplashOffEdge(dSiltToDepositOpposite);
                  m_Cell[nX][nY].pGetSoil()->DoSandSplashOffEdge(dSandToDepositOpposite);

                  // dTotClayOffEdge += dClayToDepositOpposite;
                  // dTotSiltOffEdge += dSiltToDepositOpposite;
                  // dTotSandOffEdge += dSandToDepositOpposite;

                  // And half is either deposited or moved to sediment load on this side
                  bool bToSedLoad = false;
                  m_Cell[nXAdjOneSide][nYAdjOneSide].pGetSoil()->DoSplashToSedLoadOrDeposit(dClayToDepositThis, dSiltToDepositThis, dSandToDepositThis, bToSedLoad);

                  // And add to totals
                  if (bToSedLoad)
                  {
                     // dTotClayToSedLoad += dClayToDepositThis;
                     // dTotSiltToSedLoad += dSiltToDepositThis;
                     // dTotSandToSedLoad += dSandToDepositThis;
                  }
                  else
                  {
                     // dTotClayDeposit += dClayToDepositThis;
                     // dTotSiltDeposit += dSiltToDepositThis;
                     // dTotSandDeposit += dSandToDepositThis;
                  }
               }
               else
               {
                  // This is not an edge cell. Sort out this and opposite downslope and upslope gradients. Poesen (1985) found the downslope share of splashed sediment to be 1 - 0.5 * e ^ (-2.2 * tan(beta)), where beta is slope angle
                  double dShareThis = 0;
                  double dShareOpposite = 0;

                  if (bDownSlopeThis && bDownSlopeOpposite)
                  {
                     // On a peak, downslope on both this and opposite sides. Just assume that half goes to each side
                     dShareThis = 0.5;
                     dShareOpposite = 0.5;
                  }
                  else if (bDownSlopeThis && (! bDownSlopeOpposite))
                  {
                     // Downslope only on this side, upslope (or offedge) on opposite side
                     dShareThis = exp(m_dPoesenSplashConstant * dTanBetaThis);
                     dShareOpposite = 1 - dShareThis;
                  }
                  else if ((! bDownSlopeThis) && bDownSlopeOpposite)
                  {
                     // Downslope only on opposite side, upslope (or offedge) on this side
                     dShareOpposite = exp(m_dPoesenSplashConstant * dTanBetaOpposite);
                     dShareThis = 1 - dShareOpposite;
                  }
                  else if ((! bDownSlopeThis) && (! bDownSlopeOpposite))
                  {
                     // In a hollow, upslope on both this and opposite sides. Just assume that half goes to each side
                     dShareThis = 0.5;
                     dShareOpposite = 0.5;
                  }

                  dClayToDepositThis = dClayToDepositBothSides * dShareThis;
                  dClayToDepositOpposite = dClayToDepositBothSides * dShareOpposite;
                  dSiltToDepositThis = dSiltToDepositBothSides * dShareThis;
                  dSiltToDepositOpposite = dSiltToDepositBothSides * dShareOpposite;
                  dSandToDepositThis = dSandToDepositBothSides * dShareThis;
                  dSandToDepositOpposite = dSandToDepositBothSides * dShareOpposite;

                  bool bToSedLoad = false;
                  m_Cell[nXAdjOneSide][nYAdjOneSide].pGetSoil()->DoSplashToSedLoadOrDeposit(dClayToDepositThis, dSiltToDepositThis, dSandToDepositThis, bToSedLoad);

                  // And add to totals
                  if (bToSedLoad)
                  {
                     // dTotClayToSedLoad += dClayToDepositThis;
                     // dTotSiltToSedLoad += dSiltToDepositThis;
                     // dTotSandToSedLoad += dSandToDepositThis;
                  }
                  else
                  {
                     // dTotClayDeposit += dClayToDepositThis;
                     // dTotSiltDeposit += dSiltToDepositThis;
                     // dTotSandDeposit += dSandToDepositThis;
                  }

                  bToSedLoad = false;
                  m_Cell[nXAdjOtherSide][nYAdjOtherSide].pGetSoil()->DoSplashToSedLoadOrDeposit(dClayToDepositOpposite, dSiltToDepositOpposite, dSandToDepositOpposite, bToSedLoad);

                  // And add to totals
                  if (bToSedLoad)
                  {
                     // dTotClayToSedLoad += dClayToDepositOpposite;
                     // dTotSiltToSedLoad += dSiltToDepositOpposite;
                     // dTotSandToSedLoad += dSandToDepositOpposite;
                  }
                  else
                  {
                     // dTotClayDeposit += dClayToDepositOpposite;
                     // dTotSiltDeposit += dSiltToDepositOpposite;
                     // dTotSandDeposit += dSandToDepositOpposite;
                  }
               }
            }
         }
      }

      // // TEST
      // m_ofsLog << std::fixed << setprecision(4);
      // m_ofsLog << "dTotClayDetach = " << dTotClayDetach << " dTotSiltDetach = " << dTotSiltDetach << " dTotSandDetach = " << dTotSandDetach << endl;
      // m_ofsLog << "dTotClayDeposit = " << dTotClayDeposit << " dTotSiltDeposit = " << dTotSiltDeposit << " dTotSandDeposit = " << dTotSandDeposit << endl;
      // m_ofsLog << "dTotClayToSedLoad = " << dTotClayToSedLoad << " dTotSiltToSedLoad = " << dTotSiltToSedLoad << " dTotSandToSedLoad = " << dTotSandToSedLoad << endl;
      // m_ofsLog << "dTotClayOffEdge = " << dTotClayOffEdge << " dTotSiltOffEdge = " << dTotSiltOffEdge << " dTotSandOffEdge = " << dTotSandOffEdge << endl;
      // m_ofsLog << dTotClayDetach - (dTotClayDeposit + dTotClayToSedLoad + dTotClayOffEdge) << "\t" << dTotSiltDetach - (dTotSiltDeposit + dTotSiltToSedLoad + dTotSiltOffEdge) << "\t" << dTotSandDetach - (dTotSandDeposit + dTotSandToSedLoad + dTotSandOffEdge) << "\t" << endl;
   }

   if (m_bPlanchonSplashEqn)
   {
      // Using the Planchon et al. approach: modified from Planchon O., Esteves M., Silvera N. and Lapetite J.M. (2000). Raindrop erosion of tillage induced microrelief. Possible use of the diffusion equation. Soil and Tillage Research 56(3-4), 131-144. First calculate the Laplacian for all cells in the grid, also zero each cell's temporary splash deposition value
      if (m_bSplashForward)
      {
         for (int nX = 0; nX < m_nXGridMax; nX++)
         {
            for (int nY = 0; nY < m_nYGridMax; nY++)
            {
               if (m_Cell[nX][nY].bIsMissingValue())
                  continue;

               m_Cell[nX][nY].pGetSoil()->SetLaplacian(dCalcLaplacian(nX, nY));
            }
         }
      }
      else
      {
         for (int nX = m_nXGridMax-1; nX >= 0; nX--)
         {
            for (int nY = m_nYGridMax-1; nY >= 0; nY--)
            {
               if (m_Cell[nX][nY].bIsMissingValue())
                  continue;

               m_Cell[nX][nY].pGetSoil()->SetLaplacian(dCalcLaplacian(nX, nY));
            }
         }
      }

      // DEBUG_SEDLOAD("just before splash");

      // Change directon for next time
      m_bSplashForward = ! m_bSplashForward;

      // Now calculate the splash detachment for cells which have just received some rain. change in elevation due to splash redistribution for each cell. A problem with this approach is that the totals for detached and deposited sediment are not identical i.e. mass is not conserved. So this has to be corrected
      int nRainCell = 0;
      double dTotClayDetach = 0;
      double dTotSiltDetach = 0;
      double dTotSandDetach = 0;
      double dTmpSplashTotAllDeposit = 0;

      for (int nX = 0; nX < m_nXGridMax; nX++)
      {
         for (int nY = 0; nY < m_nYGridMax; nY++)
         {
            if (m_Cell[nX][nY].bIsMissingValue())
               continue;

            // Get the depth of rain which fell during this iteration. TODO CHECK Note that this assumes that splash calcs are run EVERY iteration when there is rain
            double dRain = m_Cell[nX][nY].pGetRainAndRunon()->dGetRain();
            if (dRain > 0)
            {
               // Some rain has fallen on this cell
               nRainCell++;

               // Calculate the kinetic energy of the rain = 0.5 m v**2
               double dKE = m_dPartKE * dRain;
               m_dEndOfIterKE += dKE;

               // Now calculate the amount of splash detachment or deposition resulting from this KE
               double dL = m_Cell[nX][nY].pGetSoil()->dGetLaplacian();
               double dToChange = dKE * m_dSplashConstantNormalized * dL;
               // if (bFpEQ(dToChange, 0.0, TOLERANCE))
               //    continue;
               // else if (dToChange > 0)
               if (dToChange > 0)
               {
                  // We have splash deposition: save the dToChange value for this cell for the moment, to be corrected later
                  m_Cell[nX][nY].pGetSoil()->SetSplashDepositTemp(dToChange);

                  dTmpSplashTotAllDeposit += dToChange;
               }
               else
               {
                  // We have splash detachment. First attenuate the dToChange depending on the depth of surface water
                  dToChange *= dCalcSplashCubicSpline(m_Cell[nX][nY].pGetSurfaceWater()->dGetSurfaceWaterDepth());

                  // Now do the detachment
                  double dClayDetach = 0;
                  double dSiltDetach = 0;
                  double dSandDetach = 0;

                  m_Cell[nX][nY].pGetSoil()->DoSplashDetach(-dToChange, dClayDetach, dSiltDetach, dSandDetach);

                  // And add to totals detached
                  dTotClayDetach += dClayDetach;
                  dTotSiltDetach += dSiltDetach;
                  dTotSandDetach += dSandDetach;
               }
            }
         }
      }

      // DEBUG_SEDLOAD("middle splash");

      double dTotAllDetach = dTotClayDetach + dTotSiltDetach + dTotSandDetach;

      // Safety check
      if (bFpEQ(dTotAllDetach, 0.0, TOLERANCE))
         return;

      double dFracClay = dTotClayDetach / dTotAllDetach;
      double dFracSilt = dTotSiltDetach / dTotAllDetach;
      double dFracSand = dTotSandDetach / dTotAllDetach;

      double dTotCorrectionPerCell = (dTotAllDetach - dTmpSplashTotAllDeposit) / nRainCell;

      // int nCHECKcells = 0;
      // double dCHECKSplashDepositClay = 0;
      // double dCHECKSplashDepositSilt = 0;
      // double dCHECKSplashDepositSand = 0;
      // double dCHECKSplashSedLoadClay = 0;
      // double dCHECKSplashSedLoadSilt = 0;
      // double dCHECKSplashSedLoadSand = 0;

      // Now go through all cells again, to correct for mass conservation
      if (dTmpSplashTotAllDeposit > 0)
      {
         for (int nX = 0; nX < m_nXGridMax; nX++)
         {
            for (int nY = 0; nY < m_nYGridMax; nY++)
            {
               if (m_Cell[nX][nY].bIsMissingValue())
                  continue;

               // First get the temporary (incorrect) value for splash deposition on this cell
               double dTmpSplashDeposit = m_Cell[nX][nY].pGetSoil()->dGetSplashDepositTemp();

               double dRain = m_Cell[nX][nY].pGetRainAndRunon()->dGetRain();
               if (dRain > 0)
               {
                  // nCHECKcells++;
                  // Now correct the value for splash deposition
                  double dThisCellAllTot = dTmpSplashDeposit + dTotCorrectionPerCell;
                  // assert(dThisCellAllTot > 0);

                  // And partition it into the three size classes
                  double dThisCellClayDeposit = dThisCellAllTot * dFracClay;
                  double dThisCellSiltDeposit = dThisCellAllTot * dFracSilt;
                  double dThisCellSandDeposit = dThisCellAllTot * dFracSand;

                  bool bToSedLoad;
                  m_Cell[nX][nY].pGetSoil()->DoSplashToSedLoadOrDeposit(dThisCellClayDeposit, dThisCellSiltDeposit, dThisCellSandDeposit, bToSedLoad);

                  // if (bToSedLoad)
                  // {
                  //    // CHECK
                  //    dCHECKSplashSedLoadClay += dThisCellClayDeposit;
                  //    dCHECKSplashSedLoadSilt += dThisCellSiltDeposit;
                  //    dCHECKSplashSedLoadSand += dThisCellSandDeposit;
                  // }
                  // else
                  // {
                  //    // CHECK
                  //    dCHECKSplashDepositClay += dThisCellClayDeposit;
                  //    dCHECKSplashDepositSilt += dThisCellSiltDeposit;
                  //    dCHECKSplashDepositSand += dThisCellSandDeposit;
                  // }
               }
            }
         }
      }

      // DEBUG_SEDLOAD("middle splash 2");

      // assert (nCHECKcells == nRainCell);
      // TEST
      // double dTmpSplashDetachTotClay = 0;
      // double dTmpSplashDetachTotSilt = 0;
      // double dTmpSplashDetachTotSand = 0;
      // double dTmpSplashDepositTotClay = 0;
      // double dTmpSplashDepositTotSilt = 0;
      // double dTmpSplashDepositTotSand = 0;
      // double dTmpSplashSedLoadTotClay = 0;
      // double dTmpSplashSedLoadTotSilt = 0;
      // double dTmpSplashSedLoadTotSand = 0;
   /*
      for (int nX = 0; nX < m_nXGridMax; nX++)
      {
         for (int nY = 0; nY < m_nYGridMax; nY++)
         {
            if (m_Cell[nX][nY].bIsMissingValue())
               continue;

            dTmpSplashDepositTotClay += m_Cell[nX][nY].pGetSoil()->dGetClaySplashDeposit();
            dTmpSplashDepositTotSilt += m_Cell[nX][nY].pGetSoil()->dGetSiltSplashDeposit();
            dTmpSplashDepositTotSand += m_Cell[nX][nY].pGetSoil()->dGetSandSplashDeposit();

            dTmpSplashDetachTotClay += m_Cell[nX][nY].pGetSoil()->dGetClaySplashDetach();
            dTmpSplashDetachTotSilt += m_Cell[nX][nY].pGetSoil()->dGetSiltSplashDetach();
            dTmpSplashDetachTotSand += m_Cell[nX][nY].pGetSoil()->dGetSandSplashDetach();

            dTmpSplashSedLoadTotClay += m_Cell[nX][nY].pGetSedLoad()->dGetClaySplashSedLoad();
            dTmpSplashSedLoadTotSilt += m_Cell[nX][nY].pGetSedLoad()->dGetSiltSplashSedLoad();
            dTmpSplashSedLoadTotSand += m_Cell[nX][nY].pGetSedLoad()->dGetSandSplashSedLoad();
         }
      }

      m_ofsLog << "After splash" << endl;
      m_ofsLog << std::fixed << setprecision(0);

      m_ofsLog << "dTmpSplashDetachTotClay = " << dTmpSplashDetachTotClay * m_dCellSquare << " dTmpSplashDepositTotClay = " << dTmpSplashDepositTotClay * m_dCellSquare << " dTmpSplashSedLoadTotClay = " << dTmpSplashSedLoadTotClay * m_dCellSquare << " dTmpSplashDetachTotClay - dTmpSplashDepositTotClay - dTmpSplashSedLoadTotClay = " << (dTmpSplashDetachTotClay - dTmpSplashDepositTotClay - dTmpSplashSedLoadTotClay) * m_dCellSquare << endl;
      // m_ofsLog << "dTotClayDetach = " << dTotClayDetach * m_dCellSquare << " dCHECKSplashDepositClay = " << dCHECKSplashDepositClay * m_dCellSquare << " dCHECKSplashSedLoadClay = " << dCHECKSplashSedLoadClay * m_dCellSquare << endl;

      m_ofsLog << "dTmpSplashDetachTotSilt = " << dTmpSplashDetachTotSilt * m_dCellSquare << " dTmpSplashDepositTotSilt = " << dTmpSplashDepositTotSilt * m_dCellSquare << " dTmpSplashSedLoadTotSilt = " << dTmpSplashSedLoadTotSilt * m_dCellSquare << " dTmpSplashDetachTotSilt - dTmpSplashDepositTotSilt - dTmpSplashSedLoadTotSilt = " << (dTmpSplashDetachTotSilt - dTmpSplashDepositTotSilt - dTmpSplashSedLoadTotSilt) * m_dCellSquare << endl;
      // m_ofsLog << "dTotSiltDetach = " << dTotSiltDetach * m_dCellSquare << " dCHECKSplashDepositSilt = " << dCHECKSplashDepositSilt * m_dCellSquare << " dCHECKSplashSedLoadSilt = " << dCHECKSplashSedLoadSilt * m_dCellSquare << endl;

      m_ofsLog << "dTmpSplashDetachTotSand = " << dTmpSplashDetachTotSand * m_dCellSquare << " dTmpSplashDepositTotSand = " << dTmpSplashDepositTotSand * m_dCellSquare << " dTmpSplashSedLoadTotSand = " << dTmpSplashSedLoadTotSand * m_dCellSquare << " dTmpSplashDetachTotSand - dTmpSplashDepositTotSand - dTmpSplashSedLoadTotSand = " << (dTmpSplashDetachTotSand - dTmpSplashDepositTotSand - dTmpSplashSedLoadTotSand) * m_dCellSquare << endl << endl;
      // m_ofsLog << "dTotSandDetach = " << dTotSandDetach * m_dCellSquare << " dCHECKSplashDepositSand = " << dCHECKSplashDepositSand * m_dCellSquare << " dCHECKSplashSedLoadSand = " << dCHECKSplashSedLoadSand * m_dCellSquare << endl << endl;*/
   }

   // Now merge the changes to the temporary fields
   // TODO
}

//=========================================================================================================================================
//! This member function of CSimulation initializes the water depth-splash attenuation (efficiency) relationship using an adaptation of the cubic spline routine (spline.c) in: Press, W.H., Teukolsky, S.A., Vetterling, W.T. and Flannery, B.P. (1992). Numerical Recipes in C (Second Edition), Cambridge University Press, Cambridge. 994 pp.
//=========================================================================================================================================
void CSimulation::InitSplashAttenuation(void)
{
   int nLen = static_cast<int>(m_VdSplashDepth.size());
   vector<double> VdU(nLen, 0);

   // First stab at calculating m_VdSplashEffCoeff[0]
   if (m_VdSplashEff[0] > 0.99e30)
      m_VdSplashEffCoeff[0] = VdU[0] = 0;
   else
   {
      m_VdSplashEffCoeff[0] = 100;                  // original value was -0.5;
      VdU[0] = (3 / (m_VdSplashDepth[1] - m_VdSplashDepth[0])) * ((m_VdSplashEff[1] - m_VdSplashEff[0]) / (m_VdSplashDepth[1] - m_VdSplashDepth[0]) - m_VdSplashEff[0]);
   }

   // Now similarly calculate m_VdSplashEffCoeff[1] to m_VdSplashEffCoeff[nLen-2]
   for (int i = 1; i < nLen-1; i++)
   {
      double dSig = (m_VdSplashDepth[i] - m_VdSplashDepth[i-1]) / (m_VdSplashDepth[i+1] - m_VdSplashDepth[i-1]);

      double dP = dSig * m_VdSplashEffCoeff[i-1] + 2;
      m_VdSplashEffCoeff[i] = (dSig - 1) / dP;

      VdU[i] = (m_VdSplashEff[i+1] - m_VdSplashEff[i]) / (m_VdSplashDepth[i+1] - m_VdSplashDepth[i]) - (m_VdSplashEff[i] - m_VdSplashEff[i-1]) / (m_VdSplashDepth[i] - m_VdSplashDepth[i-1]);

      VdU[i] = (6 * VdU[i] / (m_VdSplashDepth[i+1] - m_VdSplashDepth[i-1]) - dSig * VdU[i-1]) / dP;
   }

   double dQn, dUn;
   if (m_VdSplashEff[nLen-1] > 0.99e30)
      dQn = dUn = 0;
   else
   {
      dQn = 0.5;
      dUn = (3 / (m_VdSplashDepth[nLen-1] - m_VdSplashDepth[nLen-2])) * (m_VdSplashEff[nLen-1] - (m_VdSplashEff[nLen-1] - m_VdSplashEff[nLen-2]) / (m_VdSplashDepth[nLen-1] - m_VdSplashDepth[nLen-2]));
   }

   // Similarly calculate m_VdSplashEffCoeff[nLen-1]
   m_VdSplashEffCoeff[nLen-1] = (dUn - dQn * VdU[nLen-2])/ (dQn * m_VdSplashEffCoeff[nLen-2] + 1);

   // Finally recalculate values of m_VdSplashEffCoeff[nLen-2] to m_VdSplashEffCoeff[0]
   for (int j = nLen-2; j >= 0; j--)
      m_VdSplashEffCoeff[j] = m_VdSplashEffCoeff[j] * m_VdSplashEffCoeff[j+1] + VdU[j];
}

//=========================================================================================================================================
//! This member function of CSimulation interpolates a value of splash efficiency using a constrained cubic spline. It is a modification of splint.c in: Press, W.H., Teukolsky, S.A., Vetterling, W.T. and Flannery, B.P. (1992). Numerical Recipes in C (Second Edition), Cambridge University  Press, Cambridge. 994 pp.
//=========================================================================================================================================
double CSimulation::dCalcSplashCubicSpline(double dDepth) const
{
   // Don't bother to do the calculation if there is no surface water
   if (bFpEQ(dDepth, 0.0, TOLERANCE))
      return (1);

   int nLen = static_cast<int>(m_VdSplashDepth.size());

   // If water depth is greater than maximum stored for the splash efficiency-water depth relationship, then use the maximum value (prevents possible spurious values being returned when the spline relationship is used to extrapolate values much higher than those used to define it)
   if (dDepth > m_VdSplashDepth[nLen-1])
      return (0);

   // OK start to calculate the cubic spline value
   int
      nLo = 0,
      nHi = nLen-1;

   while ((nHi - nLo)  > 1)
   {
      int n = (nHi + nLo) >> 1;
      if (m_VdSplashDepth[n] > dDepth)
         nHi = n;
      else
         nLo = n;
   }
   double fH = m_VdSplashDepth[nHi] - m_VdSplashDepth[nLo];

/*
   double fA = (m_VdSplashDepth[khi] - dDepth)/fH;
   double fB = (dDepth - m_VdSplashDepth[nLo])/fH;
   double dEff = (fA * m_VdSplashEff[nLo] + fB * m_VdSplashEff[nHi] + ((fA*fA*fA - fA) * m_VdSplashEffCoeff[nLo] + (fB*fB*fB - fB) * m_VdSplashEffCoeff[nHi]) * (fH*fH)/6);
*/
   // Calculate the normalized splash efficiency. This version is algebraically equivalent to the above, but more efficient:
   // from Prince, T. 'Curve fitting by Chebyshef and other methods', C/C++ Users' Journal
   double dB = (dDepth - m_VdSplashDepth[nLo]);
   double dA = (m_VdSplashDepth[nHi] - dDepth);
   double dEff = ((m_VdSplashEff[nLo] - m_VdSplashEffCoeff[nLo] / 6 * dB * (dA + fH)) * dA + (m_VdSplashEff[nHi] - m_VdSplashEffCoeff[nHi] / 6 * dA * (dB + fH)) * dB) / fH;

   return tMax(0.0, dEff);
}

//=========================================================================================================================================
//! This member function of CSimulation calculates the Laplacian for 4 cells with a centred scheme and ordinary axes (x-x, y-y). It is Eq (20) in the paper. The original was written by Olivier Planchon, 2 May 2001
//=========================================================================================================================================
double CSimulation::dCalcLaplacian(int const nX, int const nY)
{
/*
   // This is the original code
   double dThisElev = m_Cell[nX][nY].dGetSoilSurfaceElevation();
   return (m_dPlanchonCellSizeKC * (Deriv2(m_Cell[nX-1][nY].dGetSoilSurfaceElevation(), dThisElev, m_Cell[nX+1][nY].dGetSoilSurfaceElevation(), m_dCellSquare) + Deriv2(m_Cell[nX][nY-1].dGetSoilSurfaceElevation(), dThisElev, m_Cell[nX][nY+1].dGetSoilSurfaceElevation(), m_dCellSquare)));

   // This is mathematically equivalent to the code above
   return (m_dPlanchonCellSizeKC * (m_Cell[nX-1][nY].dGetSoilSurfaceElevation() + m_Cell[nX+1][nY].dGetSoilSurfaceElevation() + m_Cell[nX][nY-1].dGetSoilSurfaceElevation() + m_Cell[nX][nY+1].dGetSoilSurfaceElevation() - (4 * m_Cell[nX][nY].dGetSoilSurfaceElevation())));
*/
   // Modification of the code above to also work with edge cells (which have less than 4 adjacent cells)
   int nAdj = 0;
   int nXTmp, nYTmp;
   double dLaplacian = 0;

   nXTmp = nX-1;
   nYTmp = nY;
   if ((nXTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()))
   {
      nAdj++;
      dLaplacian += m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation();
   }

   nXTmp = nX+1;
   nYTmp = nY;
   if ((nXTmp < m_nXGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()))
   {
      nAdj++;
      dLaplacian += m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation();
   }

   nXTmp = nX;
   nYTmp = nY-1;
   if ((nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()))
   {
      nAdj++;
      dLaplacian += m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation();
   }

   nXTmp = nX;
   nYTmp = nY+1;
   if ((nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()))
   {
      nAdj++;
      dLaplacian += m_Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation();
   }

   dLaplacian -= (nAdj * m_Cell[nX][nY].pGetSoil()->dGetSoilSurfaceElevation());
   dLaplacian *= m_dPlanchonCellSizeKC;

   return (dLaplacian);
}

//=========================================================================================================================================
//! This member function of CSimulation outputs splash efficiency (1 - attenuation) calculated as a constrained cubic spline, for checking purposes
//=========================================================================================================================================
void CSimulation::CheckSplashAttenuation(void) const
{
   // Put together file name for CSV splash attenuation check file
   string strFilePathName = m_strOutputPath;
   strFilePathName.append(SPLASH_ATTENUATION_CHECK);
   strFilePathName.append(CSV_EXT);

   // Create an ofstream object then try to open file for output
   ofstream SplashAttenuationStream;
   SplashAttenuationStream.open(strFilePathName, ios::out | ios::trunc);

   // Did it open OK?
   if (! SplashAttenuationStream.is_open())
   {
      // Error: cannot open splash check file for output
      cerr << ERR << "cannot open " << strFilePathName << " for output" << endl;
      return;
   }

   SplashAttenuationStream << std::fixed << setprecision(6);

//   SplashAttenuationStream << "Depth\t,\tEfficiency\t,\tAttenuation" << endl;
   double const dDelta = 0.1;
   for (double d = 0; d <= (static_cast<int>(m_VdSplashDepth.size()) + 10); d += dDelta)
      SplashAttenuationStream << d << ",\t" << dCalcSplashCubicSpline(d) << ",\t" << 1 - dCalcSplashCubicSpline(d) << endl;

   // Close file
   SplashAttenuationStream.close();
}

//=========================================================================================================================================
//! This member function of CSimulation reads in the parameters for the splash attenuation-water depth relationship.
//=========================================================================================================================================
bool CSimulation::bReadSplashAttenuationData(void)
{
   // Put together file name
   string strFilePathName = m_strSplashAttenuationFile;

   // Create ifstream object
   ifstream InStream;

   // Try to open splash efficiency file for input
   InStream.open(strFilePathName, ios::in);
   if (! InStream.is_open())
   {
      // Error: cannot open splash efficiency file for input
      cerr << ERR << "cannot open " << strFilePathName << " for input" << endl;
      return (false);
   }

   string strRec, strErr;
   int nLine = 0;
   while (getline(InStream, strRec))
   {
      // Trim off leading and trailing whitespace
      strRec = strTrim(&strRec);

      nLine++;

      // If it is a blank line or a comment then ignore it
      if ((! strRec.empty()) && (strRec[0] != QUOTE1) && (strRec[0] != QUOTE2))
      {
         // Now remove any trailing comments, starting with a QUOTE1 character
         // TODO what about QUOTE2?
         vector<string> VstrLeftRight = VstrSplit(&strRec, QUOTE1);
         if (VstrLeftRight.size() > 0)
         {
            // Assume this is a depth/efficiency pair, split by space
            vector<string> VstrDepthEfficiency = VstrSplit(&VstrLeftRight[0], SPACE);
            if (VstrDepthEfficiency.size() < 1)
            {
               strErr = "reading splash efficiency: line ";
               strErr.append(to_string(nLine));
               strErr.append(" is invalid: '");
               strErr.append(strRec);
               strErr.append("'");

               break;
            }

            // OK we have a pair, so first read depth, multiplied by drop diameter
            double dDpth = stod(VstrDepthEfficiency[0]) * m_dDropDiameter;      // in mm

            if (dDpth < 0)
            {
               strErr = "reading splash efficiency: line ";
               strErr.append(to_string(nLine));
               strErr.append(" has invalid water depth: '");
               strErr.append(strRec);
               strErr.append("'");

               break;
            }

            // Now read in splash efficiency
            double dSplashEff = stod(VstrDepthEfficiency[1]);

            if (dSplashEff < 0)
            {
               strErr = "reading splash efficiency: line ";
               strErr.append(to_string(nLine));
               strErr.append(" has invalid splash efficiency: '");
               strErr.append(strRec);
               strErr.append("'");

               break;
            }

            // All OK so store values
            m_VdSplashDepth.push_back(dDpth);      // mm
            m_VdSplashEff.push_back(dSplashEff);
            m_VdSplashEffCoeff.push_back(0);
         }
      }

      // Did an error occur?
      if (! strErr.empty())
      {
         // Error in input to splash efficiency file
         cerr << ERR << strErr << endl;
         InStream.close();
         return (false);
      }
   }

   // Now go through all m_VdSplashDepth values, should increase in sequence
   for (unsigned int i = 1; i < m_VdSplashDepth.size(); i++)
   {
      if (m_VdSplashDepth[i] <= m_VdSplashDepth[i-1])
      {
         // Error
         strErr = "reading splash efficiency: line ";
         strErr.append(to_string(nLine));
         strErr.append(" has water depth <= previous water depth: '");
         strErr.append(strRec);
         strErr.append("'");

         // Error in input to splash efficiency file
         cerr << ERR << strErr << endl;
         InStream.close();
         return (false);
      }
   }

   InStream.close();

   return (true);
}

