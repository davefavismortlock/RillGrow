/*=========================================================================================================================================

 This is simulate_overland_flow.cpp: it handles surface water

 Copyright (C) 2020 David Favis-Mortlock

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

 This routes flow from all wet cells during one timestep, calculates per-iteration totals, also adds to running totals for this iteration.
 It also calls the infilt, splash, and slumping routines (if these are enabled by the user)

=========================================================================================================================================*/
void CSimulation::DoAllFlowRouting(void)
{
   // Go through all cells in the Cell array, and calculate the outflow from each cell. Write the results additively to the delta fields in the Cell array
   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      for (int nY = 0; nY < m_nYGridMax; nY++)
      {
         if (Cell[nX][nY].bIsMissingValue())
            // Don't do cells outside the valid part of the grid
            continue;

//          if (Cell[nX][nY].pGetSurfaceWater()->bFlowThisIter())
//             // Don't process flow on moved-to cells a second time this iteration
//             continue;

         if (Cell[nX][nY].pGetSurfaceWater()->bIsWet())
         {
            // This is a wet cell, is an edge cell?
            if (Cell[nX][nY].bIsEdgeCell())
            {
               // It is an edge cell, which edge?
               int nEdge = Cell[nX][nY].nGetEdge();

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
}


/*=========================================================================================================================================

 This routine moves water downhill, out from a single cell, if possible. If water is moved then the transport capacity routine is called, which in turn may call the erosion or deposition routines. Results are written, additively, to the Cell array

=========================================================================================================================================*/
void CSimulation::TryCellOutFlow(int const nX, int const nY)
{
   int
      nLowX = 0,
      nLowY = 0;

   double
      dHead = 0,
      dTopSlope = 0,
      dHLen = 0;

   // Look at the Cell array and find the adjacent cell with the steepest energy slope i.e. the steepest downhill top-surface gradient from the water surface of this wet cell to the top surface (which could be either water or soil) of an adjacent cell. This elevation difference is the head
   int nDir = nFindSteepestEnergySlope(nX, nY, Cell[nX][nY].dGetTopElevation(), nLowX, nLowY, dHead, dTopSlope, dHLen);
   Cell[nX][nY].pGetSurfaceWater()->SetFlowDirection(nDir);
   if (DIRECTION_NONE == nDir)
   {
      // No adjacent cells have a downhill top surface, so no outflow here. Initialize flow velocity and depth-weighted flow velocity and return
      Cell[nX][nY].pGetSurfaceWater()->InitializeAllFlowVelocity();
      return;
   }

   // The top surface of an adjacent cell is lower, so water could flow from this cell to the lower cell: to equalize water surfaces, we need to move half the head
   dHead /= 2;

   double dThisDepth = Cell[nX][nY].pGetSurfaceWater()->dGetSurfaceWaterDepth();
   if (dThisDepth <= dHead)
   {
      // There isn't enough water on this cell to level both water surfaces: so just move (at most) what is there
      dHead = dThisDepth;
   }

   // Add this head to the this-iteration total, and increment the count of this-iteration heads (this is used in off-edge flow calcs)
   m_dThisIterTotHead += dHead;
   m_ulNumHead++;

   // Now use either a Manning-type equation, or the Darcy-Weisbach equation, to calculate flow speed (which may be subsequently constrained) and hence the time taken for water to flow from the centroid of this cell to the centroid of the next cell
   double dFlowSpeed = 0;                                   // Value to be calculated in dTimeToCrossCell, is in mm/sec
   C2DVec Vec2DFlowVelocity;                                // ditto
   double dOutFlowTime = dTimeToCrossCell(nX, nY, nDir, dTopSlope, dThisDepth, dHLen, Vec2DFlowVelocity, dFlowSpeed);
   if (dOutFlowTime == FOREVER)
   {
      // Flow speed is effectively zero, because the depth is nearly zero, so no outflow. Initialize flow velocity and depth-weighted flow velocity and return
      Cell[nX][nY].pGetSurfaceWater()->InitializeAllFlowVelocity();
      return;
   }

   // If the outflow time is greater than the timestep, there will not have been enough time for the whole of the head to move from the centroid of one cell to the centroid of the next. Assume a linear relationship, so that depth_to_move = head * (timestep) / outflowtime)
   double dDepthToMove = dHead;
   if (dOutFlowTime > m_dTimeStep)
      dDepthToMove = dHead * (m_dTimeStep / dOutFlowTime);

   // Save the flow velocity
   Cell[nX][nY].pGetSurfaceWater()->SetFlowVelocity(Vec2DFlowVelocity);

   // Set the depth-averaged velocity. This is less than the normal velocity if only a part of the depth is moved; same as the normal velocity if the the whole depth is moved
   double dFractionToMove = tMin(dDepthToMove / dThisDepth, 1.0);
   Cell[nX][nY].pGetSurfaceWater()->SetDepthWeightedFlowVelocity(Vec2DFlowVelocity * dFractionToMove);

   // Don't move tiny depths
   if (dDepthToMove < WATER_TOLERANCE)
      return;

   // Move water (and maybe sediment) from this cell to the adjacent cell
   CellMoveWater(nX, nY, nLowX, nLowY, dThisDepth, dDepthToMove);

   if (m_bFlowErosion)
      // This outflow may or may not be erosive: it depends on whether or not it has exceeded its transport capacity. So check transport capacity for this cell, and either erode it or deposit some sediment here
      CalcTransportCapacity(nX, nY, nLowX, nLowY, nDir, dThisDepth, dHead, dTopSlope, dHLen, dFlowSpeed, dDepthToMove);
}


/*=========================================================================================================================================

 This moves water off the edge from a single cell, if possible

=========================================================================================================================================*/
void CSimulation::TryEdgeCellOutFlow(int const nX, int const nY, int const nDir)
{
   // We need a value for off-edge head: use the average on-grid head during the previous iteration, multiplied by a constant
   double
      dHead = m_dLastIterAvgHead * m_dOffEdgeConst,
      dThisDepth = Cell[nX][nY].pGetSurfaceWater()->dGetSurfaceWaterDepth();

   // Can we move dHead depth off the grid?
   if (dThisDepth < dHead)
      // We don't have dHead depth of water on the cell, so move only dThisDepth
      dHead = dThisDepth;

   double
      dTopSlope = dHead * m_dInvCellSide,
      dFlowSpeed = 0;                                                      // Value to be calculated in dTimeToCrossCell(), is in mm/sec
   C2DVec Vec2DFlowVelocity;                                               // Ditto

   // Now use either a Manning-type equation, or the Darcy-Weisbach equation, to calculate flowspeed (which may be constrained) and hence the time taken for water to flow from the centroid of this cell to the next
   double dOutFlowTime = dTimeToCrossCell(nX, nY, nDir, dTopSlope, dThisDepth, m_dCellSide, Vec2DFlowVelocity, dFlowSpeed);
   if (dOutFlowTime == FOREVER)
   {
      // Flow speed is effectively zero, because the depth is nearly zero, so no flow off the grid. Initialize flow velocity and depth-weighted flow velocity and return
      // TEST
//       Cell[nX][nY].pGetSurfaceWater()->InitializeAllFlowVelocity();
//       Cell[nX][nY].pGetSurfaceWater()->SetFlowDirection(DIRECTION_NONE);
      return;
   }

   // If the timestep is less than this outflow time, there will not have been enough time for the whole of the head to move from the centroid of one cell to the centroid of the next. Assume a linear relationship, so that depth_to_move = (head * timestep) / outflowtime) and the flow direction
   double
      dFractionOfTimestep = tMin(m_dTimeStep / dOutFlowTime, 1.0),
      dDepthToMove = dHead * dFractionOfTimestep,
      dFractionToMove = tMin(dDepthToMove / dThisDepth, 1.0);

   // Set the flow velocity and the depth-averaged velocity (this is lower than the normal velocity if only a part of the depth is moved; same as the normal velocity if the the whole depth is moved TODO is this OK?
   Cell[nX][nY].pGetSurfaceWater()->SetFlowVelocity(Vec2DFlowVelocity);
   Cell[nX][nY].pGetSurfaceWater()->SetDepthWeightedFlowVelocity(Vec2DFlowVelocity * dFractionToMove);
   Cell[nX][nY].pGetSurfaceWater()->SetFlowDirection(nDir);

   // Move water from this cell, off the edge. Note that if there is insufficient surface water, dDepthToMove gets reduced
   Cell[nX][nY].pGetSurfaceWater()->RemoveSurfaceWater(dDepthToMove);

   // Add this amount to the display-only total of surface water lost from the edge for this cell
   Cell[nX][nY].pGetSurfaceWater()->AddSurfaceWaterLost(dDepthToMove);

   // Now deal with the sediment: move the sediment that was being transported in this depth of water off the edge of the grid. We assume here that all transported sedimentsis well mixed in the water column. First, are we assuming that all transported sediment falls off the edge) as in a flume), or only a fraction of the transported sediment leaves the edge?
   if (m_bFlumeTypeSim)
      dFractionToMove = 1;

   double
      dClaySedimentToRemove = Cell[nX][nY].pGetSedLoad()->dGetClaySedLoad() * dFractionToMove,
      dSiltSedimentToRemove = Cell[nX][nY].pGetSedLoad()->dGetSiltSedLoad() * dFractionToMove,
      dSandSedimentToRemove = Cell[nX][nY].pGetSedLoad()->dGetSandSedLoad() * dFractionToMove;

   // Remove some clay-sized sediment from the cell (and from the total of clay-sized sediment moving as sediment load). Note that dClaySedimentToRemove gets changed if there is insufficient clay-sized sediment on the cell
   if (dClaySedimentToRemove > 0)
   {
      Cell[nX][nY].pGetSedLoad()->RemoveFromClaySedLoad(dClaySedimentToRemove);

      // Add to this iteration's total clay lost
      m_dThisIterClaySedLost += dClaySedimentToRemove;
   }

   // Remove some silt-sized sediment from the cell (and from the total of silt-sized sediment moving as sediment load). Note that dSiltSedimentToRemove gets changed if there is insufficient silt-sized sediment on the cell
   if (dSiltSedimentToRemove > 0)
   {
      Cell[nX][nY].pGetSedLoad()->RemoveFromSiltSedLoad(dSiltSedimentToRemove);

      // Add to this iteration's total silt lost
      m_dThisIterSiltSedLost += dSiltSedimentToRemove;
   }

   // Remove some sand-sized sediment from the cell (and from the total of sand-sized sediment moving as sediment load). Note that dSandSedimentToRemove gets changed if there is insufficient sand-sized sediment on the cell
   if (dSandSedimentToRemove > 0)
   {
      Cell[nX][nY].pGetSedLoad()->RemoveFromSandSedLoad(dSandSedimentToRemove);

      // Add to this iteration's total sand lost
      m_dThisIterSandSedLost += dSandSedimentToRemove;
   }

   // Finally, the off-edge outflow from this edge cell may have been capable of eroding the edge cell, or deposition may have occurred on the edge cell. So check transport capacity for this cell, and either erode it or deposit some sediment. Note that we use the original (i.e. pre-outflow) values here
   if (m_bFlowErosion)
      CalcTransportCapacity(nX, nY, -1, -1, nDir, dThisDepth, dHead, dTopSlope, m_dCellSide, dFlowSpeed, dDepthToMove);
}


/*=========================================================================================================================================

 Use either a Manning-type equation or the Darcy-Weisbach equation to calculate flowspeed and hence the time needed to flow from the centroid of this cell (the wet one) to
 the centroid of the next. If flowspeed is too high, it may be constrained

=========================================================================================================================================*/
double CSimulation::dTimeToCrossCell(int const nX, int const nY, int const nDir, double const dTopSlope, double dThisDepth, double const dHLen, C2DVec& Vec2DFlowVelocity, double& dFlowSpeed)
{
   if (m_bManningEqn)
   {
      // Using a Manning-type equation

      // TODO


   }

   if (m_bDarcyWeisbachEqn)
   {
      // Using the Darcy-Weisbach equation: flow speed = sqrt((8 G R S) / ff) in SI units. Note that hydraulic radius should be in m, but we calculate it in mm so need to divide R by 1000. Thus the D-W constant 8 becomes 0.008. Also flow speed is calculated by D-W in m/sec, but we need it in mm/sec, so need to multiply flow speed by 1000
      double const DARCY_WEISBACH_CONST = 0.008;
      double const FLOW_SPEED_CONVERSION_CONST = 1000;

      if (m_bFrictionFactorConstant)
      {
         // Calculate flow speed using the Darcy-Weisbach equation, with its friction factor set to a constant value
         dFlowSpeed = FLOW_SPEED_CONVERSION_CONST * sqrt((DARCY_WEISBACH_CONST * m_dG * dCalcHydraulicRadius(nX, nY) * dTopSlope) / m_dFFConstant);

         // Save the value of the friction factor
         Cell[nX][nY].pGetSurfaceWater()->SetFrictionFactor(m_dFFConstant);
      }

      if (m_bFrictionFactorReynolds)
      {
         // Calculate flow speed using the Darcy-Weisbach equation, with its friction factor related to Reynolds' number by this empirical relationship: ff = A * Re^B. Note that the Reynolds' number is calculated using the previous iteration's flow speed
         double
            dRe = dGetReynolds(nX, nY),
            dFF = m_dFFReynoldsParamA * pow(dRe, m_dFFReynoldsParamB);

         dFlowSpeed = FLOW_SPEED_CONVERSION_CONST * sqrt((DARCY_WEISBACH_CONST * m_dG * dCalcHydraulicRadius(nX, nY) * dTopSlope) / dFF);

         // Save the value of the friction factor
         Cell[nX][nY].pGetSurfaceWater()->SetFrictionFactor(dFF);
      }

      if (m_bFrictionFactorLawrence)
      {
         // Calculate flow speed using the Darcy-Weisbach equation, with its friction factor determined by means of nondimensional boundary roughness (lambda = depth / epsilon), see: Lawrence, D.S.L. (1997). Macroscale surface roughness and frictional resistance in surface water. Earth Surface Processes and Landforms 22, 365-382. Some changes made to equation because of units TODO CHECK UNITS SINCE HYDRAULIC RADIUS IS IN MM
         double
            dLambda = dThisDepth / m_dFFLawrenceEpsilon,
            dFF = dCalcLawrenceFrictionFactor(nX, nY, dLambda, false);

         dFlowSpeed = FLOW_SPEED_CONVERSION_CONST * sqrt((DARCY_WEISBACH_CONST * m_dG * dCalcHydraulicRadius(nX, nY) * dTopSlope) / dFF);

         // Save the value of the friction factor
         Cell[nX][nY].pGetSurfaceWater()->SetFrictionFactor(dFF);
      }
   }

   // Sometimes depth can be so small that a flow speed of zero gets calculated
   if (dFlowSpeed <= 0)
      return FOREVER;

   // If this flow speed is high enough, save it in m_dPossMaxSpeedNextIter and later use this to set the value of m_dTimeStep for the next iteration
   m_dPossMaxSpeedNextIter = tMax(dFlowSpeed, m_dPossMaxSpeedNextIter);

   // Constrain the flow speed: this is necessary TODO SAY MORE
   if (dFlowSpeed > m_dMaxFlowSpeed)
   {
      Vec2DFlowVelocity = Vec2DFlowVelocity * (m_dMaxFlowSpeed / dFlowSpeed);     // Note can't use *=, get const conversion error
      dFlowSpeed = m_dMaxFlowSpeed;
   }

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


/*=========================================================================================================================================

 Calculates a cell's hydraulic radius (in mm)

=========================================================================================================================================*/
double CSimulation::dCalcHydraulicRadius(int const nX, int const nY)
{
   int
      nN = 0,
      nDir = Cell[nX][nY].pGetSurfaceWater()->nGetFlowDirection();
   double
      dDepth = Cell[nX][nY].pGetSurfaceWater()->dGetSurfaceWaterDepth();

   // Check the two cells which are orthogonal to the direction of flow
   int nXAdj, nYAdj;
   switch (nDir)
   {
   case DIRECTION_BOTTOM:
   case DIRECTION_TOP:
      nXAdj = nX-1;
      nYAdj = nY;
      if ((nXAdj >= 0) && Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         nN++;

      nXAdj = nX+1;
      nYAdj = nY;
      if ((nXAdj < m_nXGridMax) && Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         nN++;

      break;

   case DIRECTION_BOTTOM_RIGHT:
   case DIRECTION_TOP_LEFT:
      nXAdj = nX+1;
      nYAdj = nY-1;
      if ((nXAdj < m_nXGridMax) && (nYAdj > 0) && Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         nN++;

      nXAdj = nX-1;
      nYAdj = nY+1;
      if ((nXAdj >= 0) && (nYAdj < m_nYGridMax) && Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         nN++;

      break;

   case DIRECTION_BOTTOM_LEFT:
   case DIRECTION_TOP_RIGHT:
      nXAdj = nX-1;
      nYAdj = nY-1;
      if ((nXAdj >= 0) && (nYAdj >= 0) && Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         nN++;

      nXAdj = nX+1;
      nYAdj = nY+1;
      if ((nXAdj < m_nXGridMax) && (nYAdj < m_nYGridMax) && Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         nN++;

      break;

   case DIRECTION_LEFT:
   case DIRECTION_RIGHT:
      nXAdj = nX;
      nYAdj = nY-1;
      if ((nYAdj >= 0) && Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         nN++;

      nXAdj = nX;
      nYAdj = nY+1;
      if ((nYAdj < m_nYGridMax) && Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         nN++;

      break;
   }

   if (2 == nN)
   {
      // Both adjacent cells are wet, so R = (w * d / w) = d
      return (dDepth);
   }
   else if (1 == nN)
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


/*=========================================================================================================================================

 Calculates the Lawrence (1997) friction factor for the Darcy-Weisbach equation, using the notion of relative boundary roughness

=========================================================================================================================================*/
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
         Cell[nX][nY].pGetSurfaceWater()->SetInundation(SHALLOW_FLOW);
   }
   else if (dLambda <= LAMBDA_MARGINALLY_INUNDATED_FLOW_THRESHOLD)
   {
      // Marginally-inundated flow, equation 15 in Lawrence (1997)
      dF = MARGINAL_FLOW_CONST / (dLambda * dLambda);

      if (! bJustCheckEquation)
         Cell[nX][nY].pGetSurfaceWater()->SetInundation(MARGINAL_FLOW);
   }
   else
   {
      // Well-inundated flow, equation 12 in Lawrence (1997)
      dF = pow(DEEP_FLOW_CONST_A + (DEEP_FLOW_CONST_B * log(dLambda)), 2);

      if (! bJustCheckEquation)
         Cell[nX][nY].pGetSurfaceWater()->SetInundation(DEEP_FLOW);
   }

   return dF;
}


/*=========================================================================================================================================

 Identifies the adjacent cell which has the steepest downhill energy slope (i.e. top-surface gradient)

=========================================================================================================================================*/
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
   if ((nYTmp < m_nYGridMax) && (! Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dTmpDiff = dThisTop - Cell[nXTmp][nYTmp].dGetTopElevation()) > 0))
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
   if ((nXTmp < m_nXGridMax) && (nYTmp < m_nYGridMax) && (! Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dTmpDiff = dThisTop - Cell[nXTmp][nYTmp].dGetTopElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellDiag) > dTopSlope))
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
   if ((nXTmp >= 0) && (nYTmp < m_nYGridMax) && (! Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dTmpDiff = dThisTop - Cell[nXTmp][nYTmp].dGetTopElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellDiag) > dTopSlope))
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
   if ((nXTmp < m_nXGridMax) && (! Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dTmpDiff = dThisTop - Cell[nXTmp][nYTmp].dGetTopElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellSide) > dTopSlope))
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
   if ((nXTmp >= 0) && (! Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dTmpDiff = dThisTop - Cell[nXTmp][nYTmp].dGetTopElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellSide) > dTopSlope))
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
   if ((nXTmp < m_nXGridMax) && (nYTmp >= 0) && (! Cell[nXTmp][nYTmp].bIsMissingValue())  && ((dTmpDiff = dThisTop - Cell[nXTmp][nYTmp].dGetTopElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellDiag) > dTopSlope))
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
   if ((nXTmp >= 0) && (nYTmp >= 0) && (! Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dTmpDiff = dThisTop - Cell[nXTmp][nYTmp].dGetTopElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellDiag) > dTopSlope))
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
   if ((nYTmp >= 0) && (! Cell[nXTmp][nYTmp].bIsMissingValue()) && ((dTmpDiff = dThisTop - Cell[nXTmp][nYTmp].dGetTopElevation()) > 0) && ((dTanX = dTmpDiff * m_dInvCellSide) > dTopSlope))
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


/*=========================================================================================================================================

 This function moves water from one cell to the adjacent cell, also moves some or all of the sediment load

=========================================================================================================================================*/
void CSimulation::CellMoveWater(int const nXFrom, int const nYFrom, int const nXTo, int const nYTo, double const& dThisDepth, double& dDepthToMove)
{
   // First remove the water from the source cell
   Cell[nXFrom][nYFrom].pGetSurfaceWater()->RemoveSurfaceWater(dDepthToMove);

   // Add the water to the destination cell
   Cell[nXTo][nYTo].pGetSurfaceWater()->AddSurfaceWater(dDepthToMove);

   // Is there any sediment load on the source cell?
   if (Cell[nXFrom][nYFrom].pGetSedLoad()->dGetAllSizeSedLoad() > 0)
   {
      // There is, so calculate how much sediment to move
      double
         dFrac = dDepthToMove / dThisDepth,
         dClaySedToMove = Cell[nXFrom][nYFrom].pGetSedLoad()->dGetClaySedLoad() * dFrac,
         dSiltSedToMove = Cell[nXFrom][nYFrom].pGetSedLoad()->dGetSiltSedLoad() * dFrac,
         dSandSedToMove = Cell[nXFrom][nYFrom].pGetSedLoad()->dGetSandSedLoad() * dFrac;

      assert(dClaySedToMove >= 0);
      assert(dSiltSedToMove >= 0);
      assert(dSandSedToMove >= 0);

      if ((dClaySedToMove + dSiltSedToMove + dSandSedToMove) > 0)
      {
         // OK, remove this depth of sediment from the source cell TODO What if source cell is now dry, but still has some sediment left on it?
         Cell[nXFrom][nYFrom].pGetSedLoad()->RemoveFromSedLoad(dClaySedToMove, dSiltSedToMove, dSandSedToMove);

         // And add it to the destination cell
         Cell[nXTo][nYTo].pGetSedLoad()->AddToSedLoad(dClaySedToMove, dSiltSedToMove, dSandSedToMove);
      }
   }
}


/*=========================================================================================================================================

 Calculates the Reynolds number for a cell

=========================================================================================================================================*/
double CSimulation::dGetReynolds(int const nX, int const nY)
{
   double
      dFlowSpeed = Cell[nX][nY].pGetSurfaceWater()->dGetFlowSpd(),
      dR = dCalcHydraulicRadius(nX, nY);

   // Must divide by 1e6 because velocity and hydraulic radius are in mm/sec and mm, the equation has them in m/sec and m
   double dRe = (1e-6 * dFlowSpeed * dR) / m_dNu;

   return dRe;
}


