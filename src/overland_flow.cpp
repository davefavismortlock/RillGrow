/*=========================================================================================================================================

This is overland_flow.cpp: it handles surface water movement for RillGrow

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
//! This routes flow from all wet cells during one timestep
//=========================================================================================================================================
void CSimulation::DoAllFlowRouting(void)
{
   // First copy the surface water and (if we are considering flow erosion) sediment load values TODO IS THIS CORRECT? for every cell to the temporary values
   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      for (int nY = 0; nY < m_nYGridMax; nY++)
      {
         m_Cell[nX][nY].pGetSoil()->InitTmpLayerThicknesses();
         m_Cell[nX][nY].pGetSurfaceWater()->InitTmpSurfaceWater();
      }
   }

   // DEBUG_SEDLOAD("in flow routing 1");

   // Go through all cells in the cell array, and calculate the outflow from each cell. Write the results to the temporary fields in the cell objects
   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      for (int nY = 0; nY < m_nYGridMax; nY++)
      {
         if (m_Cell[nX][nY].bIsMissingValue())
            // Don't do cells outside the valid part of the grid
            continue;

         if (m_Cell[nX][nY].pGetSurfaceWater()->bIsWet())
         {
            // This is a wet cell, is an edge cell?
            if (m_Cell[nX][nY].bIsEdgeCell())
            {
               // It is an edge cell, which edge?
               int nEdge = m_Cell[nX][nY].nGetEdge();

               if (nEdge == DIRECTION_TOP)
               {
                  // Top edge
                  if (m_bClosedThisEdge[EDGE_TOP])
                     // This edge is closed, so see if there is an adjacent cell in any of the non-edge directions to which some or all of its water can flow. If there is, move the water and maybe do some erosion or deposition
                     TryCellOutFlow(nX, nY);
                  else
                     // This edge is not closed, so see if we can do some off-edge flow. If so, move the water and maybe do some erosion or deposition
                     TryEdgeCellOutFlow(nX, nY, DIRECTION_TOP);
               }

               else if (nEdge == DIRECTION_RIGHT)
               {
                  // Right edge
                  if (m_bClosedThisEdge[EDGE_RIGHT])
                     // This edge is closed, so see if there is an adjacent cell in any of the non-edge directions to which some or all of its water can flow. If there is, move the water and maybe do some erosion or deposition
                     TryCellOutFlow(nX, nY);
                  else
                     // This edge is not closed, so see if we can do some off-edge flow. If so, move the water and maybe do some erosion or deposition
                     TryEdgeCellOutFlow(nX, nY, DIRECTION_RIGHT);
               }

               else if (nEdge == DIRECTION_BOTTOM)
               {
                  // Bottom edge
                  if (m_bClosedThisEdge[EDGE_BOTTOM])
                     // This edge is closed, so see if there is an adjacent cell in any of the non-edge directions to which some or all of its water can flow. If there is, move the water and maybe do some erosion or deposition
                     TryCellOutFlow(nX, nY);
                  else
                     // This edge is not closed, so see if we can do some off-edge flow. If so, move the water and maybe do some erosion or deposition
                     TryEdgeCellOutFlow(nX, nY, DIRECTION_BOTTOM);
               }

               else if (nEdge == DIRECTION_LEFT)
               {
                  // Left edge
                  if (m_bClosedThisEdge[EDGE_LEFT])
                     // This edge is closed, so see if there is an adjacent cell in any of the non-edge directions to which some or all of its water can flow. If there is, move the water and maybe do some erosion or deposition
                     TryCellOutFlow(nX, nY);
                  else
                     // This edge is not closed, so see if we can do some off-edge flow. If so, move the water and maybe do some erosion or deposition
                     TryEdgeCellOutFlow(nX, nY, DIRECTION_LEFT);
               }
            }
            else
               // It isn't an edge cell. See if there is an adjacent cell to which some or all of its water can flow. If there is, move the water and maybe do some erosion or deposition
               TryCellOutFlow(nX, nY);
         }
      }
   }

   // DEBUG_SEDLOAD("in flow routing 2");

   // And finally copy from the temporary values to the surface water and (if considering flow erosion) sediment load values for each cell
   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      for (int nY = 0; nY < m_nYGridMax; nY++)
      {
         m_Cell[nX][nY].pGetSoil()->FinishTmpLayerThicknesses();
         m_Cell[nX][nY].pGetSurfaceWater()->FinishTmpSurfaceWater();

         if (! m_Cell[nX][nY].pGetSurfaceWater()->bIsWet())
            m_Cell[nX][nY].pGetSurfaceWater()->ZeroAllFlowVelocity();
      }
   }

   // DEBUG_SEDLOAD("in flow routing 3");
}

//=========================================================================================================================================
//! This routine moves water downhill, out from a single cell, if possible. If water is moved then (if we are considering flow erosion) the transport capacity routine is called, which in turn may call the erosion or deposition routines. Results are written, additively, to the temporary fields of the cell array
//=========================================================================================================================================
void CSimulation::TryCellOutFlow(int const nX, int const nY)
{
   int
      nLowX = 0,
      nLowY = 0;

   double
      dHead = 0,
      dTopSlope = 0,
      dHLen = 0;

   // Look at the cell array and find the adjacent cell with the steepest energy slope i.e. the steepest downhill top-surface gradient from the water surface of this wet cell to the top surface (which could be either water or soil) of an adjacent cell. This elevation difference is the head
   int nDir = nFindSteepestEnergySlope(nX, nY, m_Cell[nX][nY].dGetTopElevation(), nLowX, nLowY, dHead, dTopSlope, dHLen);
   m_Cell[nX][nY].pGetSurfaceWater()->SetFlowDirection(nDir);
   if (nDir == DIRECTION_NONE)
   {
      // No adjacent cells have a downhill top surface, so no outflow here. Initialize flow velocity and depth-weighted flow velocity and return
      m_Cell[nX][nY].pGetSurfaceWater()->InitializeAllFlowVelocity();
      return;
   }

   // The top surface of an adjacent cell is lower, so water could flow from this cell to the lower cell: to equalize water surfaces, we need to move half the head
   dHead /= 2;

   double dThisDepth = m_Cell[nX][nY].pGetSurfaceWater()->dGetSurfaceWaterDepth();
   if (dThisDepth <= dHead)
   {
      // There isn't enough water on this cell to level both water surfaces: so just move (at most) what is there
      dHead = dThisDepth;
   }

   // Add this head to the this-iteration total, and increment the count of this-iteration heads (this is used in off-edge flow calcs)
   m_dEndOfIterTotHead += dHead;
   m_ulNumHead++;

   // Now use either a Manning-type equation, or the Darcy-Weisbach equation, to calculate flow speed (which may be subsequently constrained) and hence the time taken for water to flow from the centroid of this cell to the centroid of the next cell
   double dFlowSpeed = 0;                                   // Value to be calculated in dTimeToCrossCell, is in mm/sec
   C2DVec Vec2DFlowVelocity;                                // ditto
   double dOutFlowTime = dTimeToCrossCell(nX, nY, nDir, dTopSlope, dThisDepth, dHLen, Vec2DFlowVelocity, dFlowSpeed);
   if (bFpEQ(dOutFlowTime, FOREVER, TOLERANCE))
   {
      // Flow speed is effectively zero, because the depth is nearly zero, so no outflow. Initialize flow velocity and depth-weighted flow velocity and return
      m_Cell[nX][nY].pGetSurfaceWater()->InitializeAllFlowVelocity();
      return;
   }

   // If the outflow time is greater than the timestep, there will not have been enough time for the whole of the head to move from the centroid of one cell to the centroid of the next. Assume that depth_to_move = head * (1 - ((outflowtime - timestep)**2 / (ouflowtime - timestep)**2))
   double dDepthToMove = dHead;
   double dFractionToMove = 1;
   if (dOutFlowTime > m_dTimeStep)
   {
      dFractionToMove = (1 - (pow((dOutFlowTime - m_dTimeStep), 2) / pow((dOutFlowTime + m_dTimeStep), 2)));
      dDepthToMove *= dFractionToMove;
   }

   if (dDepthToMove < WATER_TOLERANCE)
   {
      // Flow speed is effectively zero, because the depth is nearly zero, so no outflow. Initialize flow velocity and depth-weighted flow velocity and return
      m_Cell[nX][nY].pGetSurfaceWater()->InitializeAllFlowVelocity();
      return;
   }

   // We have non-zero water depth, so save the flow velocity
   m_Cell[nX][nY].pGetSurfaceWater()->SetFlowVelocity(Vec2DFlowVelocity);

   // Set the depth-averaged velocity, for display only. This is less than the normal velocity if only a part of the depth is moved; is the same as the normal velocity if the the whole depth is moved
   m_Cell[nX][nY].pGetSurfaceWater()->SetDepthWeightedFlowVelocity(Vec2DFlowVelocity * dFractionToMove);

   // Move water (and maybe sediment) from this cell to the adjacent cell
   CellMoveWaterAndSediment(nX, nY, nLowX, nLowY, dThisDepth, dDepthToMove);

   if (m_bFlowErosion || m_bSplash || m_bSlumping)
      // This outflow may or may not be erosive: it depends on whether or not it has exceeded its transport capacity. So check transport capacity for this cell, and either erode it or deposit some sediment here
      CalcTransportCapacity(nX, nY, nLowX, nLowY, nDir, dThisDepth, dHead, dTopSlope, dHLen, dFlowSpeed, dDepthToMove);
}

//=========================================================================================================================================
//! This moves water off the edge from a single cell, if possible
//=========================================================================================================================================
void CSimulation::TryEdgeCellOutFlow(int const nX, int const nY, int const nDir)
{
   // We need a value for off-edge head: use the average on-grid head during the previous iteration, multiplied by a constant: m_dOffEdgeParamA * (m_dGradient**m_dOffEdgeParamB) with m_dGradient is in %
   double dHead = m_dLastIterAvgHead * m_dOffEdgeHeadConst;
   double dThisDepth = m_Cell[nX][nY].pGetSurfaceWater()->dGetSurfaceWaterDepth();

   // Can we move dHead depth off the grid?
  if (dThisDepth < dHead)
      // We don't have dHead depth of water on the cell, so move only dThisDepth
      dHead = dThisDepth;

   double dTopSlope = dHead * m_dInvCellSide;
   double dFlowSpeed = 0;                                                  // Value to be calculated in dTimeToCrossCell(), is in mm/sec
   C2DVec Vec2DFlowVelocity;                                               // Ditto

   // Now use either a Manning-type equation, or the Darcy-Weisbach equation, to calculate flow speed (which may be constrained) and hence the time taken for water to flow from the centroid of this cell to the centroid of the next
   double dOutFlowTime = dTimeToCrossCell(nX, nY, nDir, dTopSlope, dThisDepth, m_dCellSide, Vec2DFlowVelocity, dFlowSpeed);
   if (bFpEQ(dOutFlowTime, FOREVER, TOLERANCE))
   {
      // Flow speed is effectively zero, because the depth is nearly zero, so no flow off the grid. Initialize flow velocity and depth-weighted flow velocity and return
      m_Cell[nX][nY].pGetSurfaceWater()->InitializeAllFlowVelocity();
      m_Cell[nX][nY].pGetSurfaceWater()->SetFlowDirection(DIRECTION_NONE);
      return;
   }

   // If the outflow time is greater than the timestep, there will not have been enough time for the whole of the head to move from the centroid of one cell to the centroid of the next. Assume that depth_to_move = head * (1 - ((outflowtime - timestep)**2 / (ouflowtime - timestep)**2))
   double dDepthToMove = dHead;
   double dFractionToMove = 1;
   if (dOutFlowTime > m_dTimeStep)
   {
      dFractionToMove = (1 - (pow((dOutFlowTime - m_dTimeStep), 2) / pow((dOutFlowTime + m_dTimeStep), 2)));
      dDepthToMove *= dFractionToMove;
   }

   if (dDepthToMove < WATER_TOLERANCE)
   {
      // Flow speed is effectively zero, because the depth is nearly zero, so no outflow. Initialize flow velocity and depth-weighted flow velocity and return
      m_Cell[nX][nY].pGetSurfaceWater()->InitializeAllFlowVelocity();
      return;
   }

   // We have non-zero water depth, so set the flow velocity and the depth-averaged velocity (this is lower than the normal velocity if only a part of the depth is moved; same as the normal velocity if the the whole depth is moved
   m_Cell[nX][nY].pGetSurfaceWater()->SetFlowVelocity(Vec2DFlowVelocity);
   m_Cell[nX][nY].pGetSurfaceWater()->SetDepthWeightedFlowVelocity(Vec2DFlowVelocity * dFractionToMove);
   m_Cell[nX][nY].pGetSurfaceWater()->SetFlowDirection(nDir);

   // Move water from this cell, off the edge. Note that if there is insufficient surface water, dDepthToMove gets reduced
   m_Cell[nX][nY].pGetSurfaceWater()->RemoveTmpSurfaceWater(dDepthToMove);

   // Add this amount to the total of surface water lost from the edge for this cell
   m_Cell[nX][nY].pGetSurfaceWater()->AddSurfaceWaterLost(dDepthToMove);

   if (m_bFlowErosion || m_bSplash || m_bSlumping)
   {
      // Now deal with the sediment: move the sediment that was being transported in this depth of water off the edge of the grid. We assume here that all transported sediments is well mixed in the water column
      double dClaySedToRemove = m_Cell[nX][nY].pGetSedLoad()->dGetLastIterClaySedLoad() * dFractionToMove;
      double dSiltSedToRemove = m_Cell[nX][nY].pGetSedLoad()->dGetLastIterSiltSedLoad() * dFractionToMove;
      double dSandSedToRemove = m_Cell[nX][nY].pGetSedLoad()->dGetLastIterSandSedLoad() * dFractionToMove;

      if (dClaySedToRemove > 0)
      {
         // OK, remove clay sediment from the sed load of the source cell. Note that dClaySedToRemove is changed if there is insufficient sediment on the cell
         m_Cell[nX][nY].pGetSedLoad()->AddToClaySedLoadRemoved(dClaySedToRemove);

         // Add to the cell's values for sediment lost
         m_Cell[nX][nY].pGetSedLoad()->AddToClaySedOffEdge(dClaySedToRemove);
      }

      if (dSiltSedToRemove > 0)
      {
         // OK, remove silt sediment from the flow sed load of the source cell. Note that dSiltSedToRemove is changed if there is insufficient sediment on the cell
         m_Cell[nX][nY].pGetSedLoad()->AddToSiltSedLoadRemoved(dSiltSedToRemove);

         // Add to the cell's values for sediment lost
         m_Cell[nX][nY].pGetSedLoad()->AddToSiltSedOffEdge(dSiltSedToRemove);
      }

      if (dSandSedToRemove > 0)
      {
         // OK, remove sand sediment from the flow sed load of the source cell. Note that dSandSedToRemove is changed if there is insufficient sediment on the cell
         m_Cell[nX][nY].pGetSedLoad()->AddToSandSedLoadRemoved(dSandSedToRemove);

         // Add to the cell's values for sediment lost
         m_Cell[nX][nY].pGetSedLoad()->AddToSandSedOffEdge(dSandSedToRemove);
      }

      // Finally, the off-edge outflow from this edge cell may also be capable of eroding the edge cell, or deposition may have occurred on the edge cell. So check transport capacity for this cell, and either erode it or deposit some sediment. Note that we use the original (i.e. pre-outflow) values here
      CalcTransportCapacity(nX, nY, -1, -1, nDir, dThisDepth, dHead, dTopSlope, m_dCellSide, dFlowSpeed, dDepthToMove);
   }
}

//=========================================================================================================================================
//! Use either a Manning-type equation or the Darcy-Weisbach equation to calculate flowspeed and hence the time needed to flow from the centroid of this cell (the wet one) to the centroid of the next cell. If flow speed is too high, it is constrained
//=========================================================================================================================================
double CSimulation::dTimeToCrossCell(int const nX, int const nY, int const nDir, double const dTopSlope, double dThisDepth, double const dHLen, C2DVec& Vec2DFlowVelocity, double& dFlowSpeed)
{
   if (m_bManningFlowSpeedEqn)
      CalcFlowSpeedManning(dFlowSpeed);

   if (m_bDarcyWeisbachFlowSpeedEqn)
      CalcFlowSpeedDarcyWeisbach(nX, nY, dTopSlope, dThisDepth, dFlowSpeed);

   // Sometimes depth can be so small that a flow speed of zero gets calculated
   if (dFlowSpeed <= 0)
      return FOREVER;

   // If this flow speed is high enough, save it in m_dPossMaxSpeedNextIter and later use this to set the value of m_dTimeStep for the next iteration
   m_dPossMaxSpeedNextIter = tMax(dFlowSpeed, m_dPossMaxSpeedNextIter);

   // Must now apply this scalar flow speed in the correct flow direction i.e. must create a flow velocity vector (note that the origin is at top left here). Note also that diagonal flow is faster than orthogonal flow: this is incorrect from a strict vector perspective, but is a necessary artefact of using an eight-way flow: to keep flow equally probable in each of the eight directions (all else being equal), flow must be faster on the longer diagonals. Unpleasant but apparently unavoidable unless e.g. a hexagonal grid is used
   switch (nDir)
   {
   case DIRECTION_BOTTOM:
      // +ve in y direction
      Vec2DFlowVelocity.x = 0;
      Vec2DFlowVelocity.y = dFlowSpeed;
      break;

   case DIRECTION_BOTTOM_RIGHT:
      // +ve in x direction, +ve in y direction
      Vec2DFlowVelocity.x = dFlowSpeed;
      Vec2DFlowVelocity.y = dFlowSpeed;
      dFlowSpeed *= m_dInvCos45;              // see above
      break;

   case DIRECTION_BOTTOM_LEFT:
      // -ve in x direction, +ve in y direction
      Vec2DFlowVelocity.x = -dFlowSpeed;
      Vec2DFlowVelocity.y = dFlowSpeed;
      dFlowSpeed *= m_dInvCos45;              // see above
      break;

   case DIRECTION_RIGHT:
      // +ve in x direction
      Vec2DFlowVelocity.x = dFlowSpeed;
      Vec2DFlowVelocity.y = 0;
      break;

   case DIRECTION_TOP_RIGHT:
      // +ve in x direction, -ve in y direction
      Vec2DFlowVelocity.x = dFlowSpeed;
      Vec2DFlowVelocity.y = -dFlowSpeed;
      dFlowSpeed *= m_dInvCos45;              // see above
      break;

   case DIRECTION_TOP_LEFT:
      // -ve in x direction, -ve in y direction
      Vec2DFlowVelocity.x = -dFlowSpeed;
      Vec2DFlowVelocity.y = -dFlowSpeed;
      dFlowSpeed *= m_dInvCos45;              // see above
      break;

   case DIRECTION_LEFT:
      // -ve in x direction
      Vec2DFlowVelocity.x = -dFlowSpeed;
      Vec2DFlowVelocity.y = 0;
      break;

   case DIRECTION_TOP:
      // -ve in y direction
      Vec2DFlowVelocity.x = 0;
      Vec2DFlowVelocity.y = -dFlowSpeed;
      break;
   }

   // Finally, calculate the time needed for flow to cross this cell
   return (dHLen / dFlowSpeed);
}

//=========================================================================================================================================
//! Calculates a cell's hydraulic radius (in mm).
//=========================================================================================================================================
double CSimulation::dCalcHydraulicRadius(int const nX, int const nY)
{
   int
      nN = 0,
      nDir = m_Cell[nX][nY].pGetSurfaceWater()->nGetFlowDirection();
   double
      dDepth = m_Cell[nX][nY].pGetSurfaceWater()->dGetSurfaceWaterDepth();

   // Check the two cells which are orthogonal to the direction of flow
   int nXAdj, nYAdj;
   switch (nDir)
   {
   case DIRECTION_BOTTOM:
   case DIRECTION_TOP:
      nXAdj = nX-1;
      nYAdj = nY;
      if ((nXAdj >= 0) && m_Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         nN++;

      nXAdj = nX+1;
      nYAdj = nY;
      if ((nXAdj < m_nXGridMax) && m_Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         nN++;

      break;

   case DIRECTION_BOTTOM_RIGHT:
   case DIRECTION_TOP_LEFT:
      nXAdj = nX+1;
      nYAdj = nY-1;
      if ((nXAdj < m_nXGridMax) && (nYAdj > 0) && m_Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         nN++;

      nXAdj = nX-1;
      nYAdj = nY+1;
      if ((nXAdj >= 0) && (nYAdj < m_nYGridMax) && m_Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         nN++;

      break;

   case DIRECTION_BOTTOM_LEFT:
   case DIRECTION_TOP_RIGHT:
      nXAdj = nX-1;
      nYAdj = nY-1;
      if ((nXAdj >= 0) && (nYAdj >= 0) && m_Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         nN++;

      nXAdj = nX+1;
      nYAdj = nY+1;
      if ((nXAdj < m_nXGridMax) && (nYAdj < m_nYGridMax) && m_Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         nN++;

      break;

   case DIRECTION_LEFT:
   case DIRECTION_RIGHT:
      nXAdj = nX;
      nYAdj = nY-1;
      if ((nYAdj >= 0) && m_Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         nN++;

      nXAdj = nX;
      nYAdj = nY+1;
      if ((nYAdj < m_nYGridMax) && m_Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         nN++;

      break;
   }

   if (nN == 2)
   {
      // Both adjacent cells are wet, so R = (w * d / w) = d
      return (dDepth);
   }
   else if (nN == 1)
   {
      // Only one adjacent cell is wet, so R = w * d / (w + d)
      return (m_dCellSide * dDepth / (m_dCellSide + dDepth));
   }
   else
   {
      // No adjacent cells are wet, so R = w * d / (w + 2d)
      return (m_dCellSide * dDepth / (m_dCellSide + (2 * dDepth)));
   }
}

//=========================================================================================================================================
//! Calculates the Lawrence (1997) friction factor for the Darcy-Weisbach equation, using the notion of relative boundary roughness
//=========================================================================================================================================
double CSimulation::dCalcLawrenceFrictionFactor(int const nX, int const nY, double const dLambda, bool const bJustCheckEquation)
{
   double const
      LAMBDA_SHALLOW_FLOW_THRESHOLD              = 1,
      LAMBDA_MARGINALLY_INUNDATED_FLOW_THRESHOLD = 10;

   double const
      SHALLOW_FLOW_CONST = 8,
      MARGINAL_FLOW_CONST = 10,
      DEEP_FLOW_CONST_A = 1.64,
      DEEP_FLOW_CONST_B = 0.803;

   double dF;
   if (dLambda <= LAMBDA_SHALLOW_FLOW_THRESHOLD)
   {
      // Shallow flow, equation 19 in Lawrence (1997)
      dF = (SHALLOW_FLOW_CONST / PI) * m_dFFLawrencePr * m_dFFLawrenceCd * tMin(PI/4, dLambda);

      if (! bJustCheckEquation)
         m_Cell[nX][nY].pGetSurfaceWater()->SetInundation(SHALLOW_FLOW);
   }
   else if (dLambda <= LAMBDA_MARGINALLY_INUNDATED_FLOW_THRESHOLD)
   {
      // Marginally-inundated flow, equation 15 in Lawrence (1997)
      dF = MARGINAL_FLOW_CONST / (dLambda * dLambda);

      if (! bJustCheckEquation)
         m_Cell[nX][nY].pGetSurfaceWater()->SetInundation(MARGINAL_FLOW);
   }
   else
   {
      // Well-inundated flow, equation 12 in Lawrence (1997)
      dF = pow(DEEP_FLOW_CONST_A + (DEEP_FLOW_CONST_B * log(dLambda)), 2);

      if (! bJustCheckEquation)
         m_Cell[nX][nY].pGetSurfaceWater()->SetInundation(DEEP_FLOW);
   }

   return dF;
}

//=========================================================================================================================================
//! Identifies the adjacent cell which has the steepest downhill energy slope (i.e. top-surface gradient)
//=========================================================================================================================================
int CSimulation::nFindSteepestEnergySlope(int const nX, int const nY, double const dThisTop, int& nLowX, int& nLowY, double& dTopDiff, double& dTopSlope, double& dHLen)
{
   int
      nXTmp,
      nYTmp,
      nDir = DIRECTION_NONE;
   double
      dTmpDiff,
      dTanX;

   // Planview bottom
   nXTmp = nX;
   nYTmp = nY+1;
   if ((nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dTmpDiff = dThisTop - m_Cell[nXTmp][nYTmp].dGetTopElevation()) > 0))
   {
      // It's downhill, and being the first one checked it must be the steepest so far
      dTopSlope = dTmpDiff * m_dInvCellSide;                                                       // is tan(top slope)
      nLowX = nXTmp;
      nLowY = nYTmp;
      dTopDiff = dTmpDiff;
      dHLen = m_dCellSide;
      nDir = DIRECTION_BOTTOM;
   }

   // Planview bottom right
   nXTmp = nX+1;
   nYTmp = nY+1;
   if ((nXTmp < m_nXGridMax) && (nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dTmpDiff = dThisTop - m_Cell[nXTmp][nYTmp].dGetTopElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellDiag) > dTopSlope))
   {
      // It's the steepest so far
      dTopSlope = dTanX;                                                                           // is tan(top slope)
      nLowX = nXTmp;
      nLowY = nYTmp;
      dTopDiff = dTmpDiff;
      dHLen = m_dCellDiag;
      nDir = DIRECTION_BOTTOM_RIGHT;
   }

   // Planview bottom left
   nXTmp = nX-1;
   nYTmp = nY+1;
   if ((nXTmp >= 0) && (nYTmp < m_nYGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dTmpDiff = dThisTop - m_Cell[nXTmp][nYTmp].dGetTopElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellDiag) > dTopSlope))
   {
      // It's the steepest so far
      dTopSlope = dTanX;                                                                           // is tan(top slope)
      nLowX = nXTmp;
      nLowY = nYTmp;
      dTopDiff = dTmpDiff;
      dHLen = m_dCellDiag;
      nDir = DIRECTION_BOTTOM_LEFT;
   }

   // Planview right
   nXTmp = nX+1;
   nYTmp = nY;
   if ((nXTmp < m_nXGridMax) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dTmpDiff = dThisTop - m_Cell[nXTmp][nYTmp].dGetTopElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellSide) > dTopSlope))
   {
      // It's the steepest so far
      dTopSlope = dTanX;                                                                           // is tan(top slope)
      nLowX = nXTmp;
      nLowY = nYTmp;
      dTopDiff = dTmpDiff;
      dHLen = m_dCellSide;
      nDir = DIRECTION_RIGHT;
   }

   // Planview left
   nXTmp = nX-1;
   nYTmp = nY;
   if ((nXTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dTmpDiff = dThisTop - m_Cell[nXTmp][nYTmp].dGetTopElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellSide) > dTopSlope))
   {
      // It's the steepest so far
      dTopSlope = dTanX;                                                                           // is tan(top slope)
      nLowX = nXTmp;
      nLowY = nYTmp;
      dTopDiff = dTmpDiff;
      dHLen = m_dCellSide;
      nDir = DIRECTION_LEFT;
   }

   // Planview top right
   nXTmp = nX+1;
   nYTmp = nY-1;
   if ((nXTmp < m_nXGridMax) && (nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue())  && ((dTmpDiff = dThisTop - m_Cell[nXTmp][nYTmp].dGetTopElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellDiag) > dTopSlope))
   {
      // It's the steepest so far
      dTopSlope = dTanX;                                                                           // is tan(top slope)
      nLowX = nXTmp;
      nLowY = nYTmp;
      dTopDiff = dTmpDiff;
      dHLen = m_dCellDiag;
      nDir = DIRECTION_TOP_RIGHT;
   }

   // Planview top left
   nXTmp = nX-1;
   nYTmp = nY-1;
   if ((nXTmp >= 0) && (nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dTmpDiff = dThisTop - m_Cell[nXTmp][nYTmp].dGetTopElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellDiag) > dTopSlope))
   {
      // It's the steepest so far
      dTopSlope = dTanX;                                                                           // is tan(top slope)
      nLowX = nXTmp;
      nLowY = nYTmp;
      dTopDiff = dTmpDiff;
      dHLen = m_dCellDiag;
      nDir = DIRECTION_TOP_LEFT;
   }

   // Planview top
   nXTmp = nX;
   nYTmp = nY-1;
   if ((nYTmp >= 0) && (! m_Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dTmpDiff = dThisTop - m_Cell[nXTmp][nYTmp].dGetTopElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellSide) > dTopSlope))
   {
      // It's the steepest so far
      dTopSlope = dTanX;                                                                           // is tan(top slope)
      nLowX = nXTmp;
      nLowY = nYTmp;
      dTopDiff = dTmpDiff;
      dHLen = m_dCellSide;
      nDir = DIRECTION_TOP;
   }

   return (nDir);
}

//=========================================================================================================================================
//! This function moves surface water from one cell to the adjacent cell, it also moves some or all of the sediment load
//=========================================================================================================================================
void CSimulation::CellMoveWaterAndSediment(int const nXFrom, int const nYFrom, int const nXTo, int const nYTo, double const& dThisDepth, double& dDepthToMove)
{
   // First remove the water from the temporary field of the source cell
   m_Cell[nXFrom][nYFrom].pGetSurfaceWater()->RemoveTmpSurfaceWater(dDepthToMove);

   // Then add the water to the temporary field of the destination cell
   m_Cell[nXTo][nYTo].pGetSurfaceWater()->AddTmpSurfaceWater(dDepthToMove);

   if (m_bFlowErosion || m_bSplash || m_bSlumping)
   {
      // Is there any sediment load on the source cell?
      if (m_Cell[nXFrom][nYFrom].pGetSedLoad()->dGetLastIterAllSizeSedLoad() > 0)
      {
         // There is, so calculate how much sediment to move
         double dFrac = dDepthToMove / dThisDepth;
         double dClaySedToMove = m_Cell[nXFrom][nYFrom].pGetSedLoad()->dGetLastIterClaySedLoad() * dFrac;
         double dSiltSedToMove = m_Cell[nXFrom][nYFrom].pGetSedLoad()->dGetLastIterSiltSedLoad() * dFrac;
         double dSandSedToMove = m_Cell[nXFrom][nYFrom].pGetSedLoad()->dGetLastIterSandSedLoad() * dFrac;

         if (dClaySedToMove > 0)
         {
            // OK, remove this depth of sediment from the clay sediment load of the source cell. Note that dClaySedToMove is changed to the depth actually removed
            m_Cell[nXFrom][nYFrom].pGetSedLoad()->AddToClaySedLoadRemoved(dClaySedToMove);

            // And add this depth to the sediment load of the destination cell
            m_Cell[nXTo][nYTo].pGetSedLoad()->AddToClayFlowSedLoad(dClaySedToMove);
         }

         if (dSiltSedToMove > 0)
         {
            // OK, remove this depth of sediment from the silt sediment load of the source cell. Note that dSiltSedToMove is changed to the depth actually removed
            m_Cell[nXFrom][nYFrom].pGetSedLoad()->AddToSiltSedLoadRemoved(dSiltSedToMove);

            // And add this depth to the sediment load of the destination cell
            m_Cell[nXTo][nYTo].pGetSedLoad()->AddToSiltFlowSedLoad(dSiltSedToMove);
         }

         if (dSandSedToMove > 0)
         {
            // OK, remove this depth of sediment from the sand sediment load of the source cell. Note that dSandSedToMove is changed to the depth actually removed
            m_Cell[nXFrom][nYFrom].pGetSedLoad()->AddToSandSedLoadRemoved(dSandSedToMove);

            // And add this depth to the sediment load of the destination cell
            m_Cell[nXTo][nYTo].pGetSedLoad()->AddToSandFlowSedLoad(dSandSedToMove);
         }
      }
   }
}

//=========================================================================================================================================
//! Calculates the Reynolds number for flow in a cell
//=========================================================================================================================================
double CSimulation::dGetReynolds(int const nX, int const nY)
{
   double
      dFlowSpeed = m_Cell[nX][nY].pGetSurfaceWater()->dGetFlowSpd(),
      dR = dCalcHydraulicRadius(nX, nY);

   // Must divide by 1e6 because velocity and hydraulic radius are in mm/sec and mm, the equation has them in m/sec and m
   double dRe = (1e-6 * dFlowSpeed * dR) / m_dNu;

   return dRe;
}

//=========================================================================================================================================
//! Calculates flow speed using the Manning equation
//=========================================================================================================================================
void CSimulation::CalcFlowSpeedManning(double& dFlowSpeed)
{
   // Using a Manning-type equation TODO
   dFlowSpeed = 0;
}

//=========================================================================================================================================
//! Calculates flow speed using the Darcy-Weisbach equation
//=========================================================================================================================================
void CSimulation::CalcFlowSpeedDarcyWeisbach(int const nX, int const nY, double const dTopSlope, double const dThisDepth, double& dFlowSpeed)
{
   // Note that the Reynolds' number is calculated using the previous iteration's flow speed
   double dRe = dGetReynolds(nX, nY);

   // Safety check
   if (bFpEQ(dRe, 0.0, TOLERANCE))
   {
      dFlowSpeed = 0;
      return;
   }

   double dFF = 0;

   if (m_bFrictionFactorConstant)
   {
      // Set the friction factor set to a constant value
      dFF = m_dFFConstant;
   }

   if (m_bFrictionFactorReynolds)
   {
      // Assume that the friction factor is related to Reynolds' number by this empirical relationship: ff = A * Re^B
      dFF = m_dFFReynoldsParamA * pow(dRe, m_dFFReynoldsParamB);
   }

   if (m_bFrictionFactorLawrence)
   {
      // In this approach, the friction factor determined by means of nondimensional boundary roughness (lambda = depth / epsilon), see: Lawrence, D.S.L. (1997). Macroscale surface roughness and frictional resistance in surface water. Earth Surface Processes and Landforms 22, 365-382. Some changes made to equation because of units
      double dLambda = dThisDepth / m_dFFLawrenceEpsilon;         // TODO check units, both are in mm, is this correct?

      dFF = dCalcLawrenceFrictionFactor(nX, nY, dLambda, false);
   }

   if (m_bFrictionFactorCheng)
   {
      // From Cheng, Nian-Sheng (September 2008). "Formulas for Friction Factor in Transitional Regimes". Journal of Hydraulic Engineering. 134 (9): 1357–1362. doi:10.1061/(asce)0733-9429(2008)134:9(1357). hdl:10220/7647. ISSN 0733-9429.

      // See also https://en.wikipedia.org/wiki/Talk%3ADarcy_friction_factor_formulae " I feel like it's overstating that the equation from Bellos et al., 2018 is the only one formula, among others in this wiki article, works for open-channel flow. The most naive way to work with open-channel flow is to use the hydraulic diameter of an open channel in formulas of closed pipe flows. As no validation (i.e., against experimental data) is available, no one can really say the most naive approach is not working, In this sense, it may be too much to say Bellos et al., 2018 is the only one working with open channels."

      double dEps = m_dChengRoughnessHeight / 1000;      // Effective roughness height in mm, convert to m

      double dD = 4 * dCalcHydraulicRadius(nX, nY);      // Hydraulic diameter in mm. See https://en.wikipedia.org/wiki/Hydraulic_diameter
      dD /= 1000;                                        // Convert to m

      double dParamA = 1 / (1 + pow((dRe / 2720), 9));
      double dParamB = 1 / (1 + pow((dRe / (160 * (dD / dEps) )  ), 2));

      double dTerm1 = pow((dRe / 64), dParamA);
      double dTerm2 = pow((1.8 * log(dRe / 6.8)), (2 * (1 - dParamA) * dParamB));
      double dTerm3 = pow((2.0 * log((3.7 * dD) / dEps)), (2 * (1 - dParamA) * (1 - dParamB)));

      // double dTmp1 = (2.0 * log((3.7 * dD) / dEps));
      // double dTmp2 = (2 * (1 - dParamA) * (1 - dParamB));

      // Safety check
      if (isnan(dTerm3))
      {
         // This is the most common value of dTerm3 TODO improve this
         dTerm3 = 1;
      }

      dFF = 1 / (dTerm1 * dTerm2 * dTerm3);
   }

   // Safety check
   if (isnan(dFF))
   {
      dFlowSpeed = 0;
      return;
   }

   // OK, we have the friction factor, so use the Darcy-Weisbach equation: flow speed = sqrt((8 G R S) / ff) in SI units. Note that hydraulic radius should be in m, but we calculate it in mm so need to divide R by 1000. Thus the D-W constant 8 becomes 0.008. Also flow speed is calculated by D-W in m/sec, but we need it in mm/sec, so need to multiply flow speed by 1000
   double const DARCY_WEISBACH_CONST = 0.008;
   double const FLOW_SPEED_CONVERSION_CONST = 1000;
   dFlowSpeed = FLOW_SPEED_CONVERSION_CONST * sqrt((DARCY_WEISBACH_CONST * m_dG * dCalcHydraulicRadius(nX, nY) * dTopSlope) / dFF);

   // Safety check: have to constrain flow speed if it gets too high. Unpleasant but practically necessary. Note that this is the maximum only of the x and y components of flow speed: resultant flow speed can he higher i.e. sqrt(2 * m_dMaxFlowSpeed^2)
   if (dFlowSpeed > m_dMaxFlowSpeed)
   {
      dFlowSpeed = m_dMaxFlowSpeed;
      dFF = m_dMissingValue;
   }

   // Also constrain DFF, since can get very big numbers here (GDAL writes as a float, not a double)
   // if (dFF > FLT_MAX) dFF = FLT_MAX;

   // Save the value of the friction factor for this cell for later display
   m_Cell[nX][nY].pGetSurfaceWater()->SetFrictionFactor(dFF);
}

