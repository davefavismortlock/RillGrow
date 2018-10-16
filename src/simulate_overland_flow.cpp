/*=========================================================================================================================================

 This is do_overland_flow.cpp: it handles overland flow

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

 This routes flow from all wet cells during one timestep, calculates per-iteration totals, also adds to running totals for this iteration.
 It also calls the infiltration, splash, and slumping routines (if these are enabled by the user)

=========================================================================================================================================*/
void CSimulation::DoAllFlowRouting(void)
{
   // Go through all cells in the Cell array, and calculate the outflow from each cell. Write the results additively to the delta fields in the Cell array
   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      for (int nY = 0; nY < m_nYGridMax; nY++)
      {
         if (Cell[nX][nY].bIsMissingValue())
            continue;
         
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
                     // This is not closed, so see if we can do some off-edge flow. If so, move the water and maybe do some erosion or deposition
                     TryEdgeCellOutFlow(nX, nY, DIRECTION_TOP);
               }
               
               else if (nEdge == DIRECTION_RIGHT)
               {
                  // Right edge
                  if (m_bClosedThisEdge[EDGE_RIGHT])
                     // This edge is closed, so see if there is an adjacent cell in any of the non-edge directions to which some or all of its water can flow. If there is, move the water and maybe do some erosion or deposition
                     TryCellOutFlow(nX, nY);
                  else
                     // This is not closed, so see if we can do some off-edge flow. If so, move the water and maybe do some erosion or deposition
                     TryEdgeCellOutFlow(nX, nY, DIRECTION_RIGHT);
               }
               
               else if (nEdge == DIRECTION_BOTTOM)
               {
                  // Bottom edge
                  if (m_bClosedThisEdge[EDGE_BOTTOM])
                     // This edge is closed, so see if there is an adjacent cell in any of the non-edge directions to which some or all of its water can flow. If there is, move the water and maybe do some erosion or deposition
                     TryCellOutFlow(nX, nY);
                  else
                     // This is not closed, so see if we can do some off-edge flow. If so, move the water and maybe do some erosion or deposition
                     TryEdgeCellOutFlow(nX, nY, DIRECTION_BOTTOM);
               }
               
               else if (nEdge == DIRECTION_LEFT)
               {
                  // Left edge
                  if (m_bClosedThisEdge[EDGE_LEFT])
                     // This edge is closed, so see if there is an adjacent cell in any of the non-edge directions to which some or all of its water can flow. If there is, move the water and maybe do some erosion or deposition
                     TryCellOutFlow(nX, nY);
                  else
                     // This is not closed, so see if we can do some off-edge flow. If so, move the water and maybe do some erosion or deposition
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

 This routine looks at the Cell array and moves water downhill, out from a single cell, if possible. If water is moved then the transport capacity routine is called, which in turn may call the erosion or deposition routines. Results are written, additively, to the Cell array

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

   // Look on the Cell array and find the adjacent cell with the steepest energy slope i.e. the steepest downhill top-surface gradient from the water surface of this wet cell to the top surface (which could be either water or soil) of an adjacent cell. This elevation difference is the head
   int nDir = nFindSteepestEnergySlope(nX, nY, Cell[nX][nY].dGetTopElevation(), nLowX, nLowY, dHead, dTopSlope, dHLen);
   Cell[nX][nY].pGetSurfaceWater()->SetFlowDirection(nDir);
   if (DIRECTION_NONE == nDir)
   {
      // No adjacent cells have a downhill top surface, so no outflow here. Set flow velocity and depth-weighted flow velocity to zero and return
      Cell[nX][nY].pGetSurfaceWater()->InitializeAllFlowVelocity();
      
      return;
   }

   // The top surface of an adjacent cell is lower, so water could flow from this cell to the lower cell: to equalize water surfaces, we need to move half the head
   dHead /= 2;

   // If this is the biggest head during this iteration, save it (will be used in off-edge flow calcs)
   m_dThisIterMaxHead = tMax(dHead, m_dThisIterMaxHead);
   
   // Ditto for the smallest head
   m_dThisIterMinHead = tMin(dHead, m_dThisIterMinHead);

   double dThisDepth = Cell[nX][nY].pGetSurfaceWater()->dGetSurfaceWater();
   if (dThisDepth <= dHead)
   {
      // Even assuming that enough time has passed (see below) there isn't enough water on this cell to level both water surfaces: so just move (at most) what is there
      // TODO how does this affect dOutFlowTime? Is currently ignored
      dHead = dThisDepth;
   }

   if (dHead < FLOW_MIN_CONSIDERED)
   {
      // If head is very tiny, don't bother calculating outflow. Set flow velocity and depth-weighted flow velocity to zero and return
      Cell[nX][nY].pGetSurfaceWater()->InitializeAllFlowVelocity();

      return;
   }

   // Now use the Darcy-Weisbach equation to calculate flowspeed (which may be constrained) and hence the time taken for water to flow from the centroid of this cell to the next
   double dFlowSpeed = 0;                                   // value to be calculated in dTimeToCrossCell
   C2DVec Vec2DFlowVelocity;                                // ditto
   double dOutFlowTime = dTimeToCrossCell(nX, nY, nDir, dTopSlope, dThisDepth, dHLen, Vec2DFlowVelocity, dFlowSpeed);
   if (dOutFlowTime == FOREVER)
   {
      // Flow speed is effectively zero, because the depth is nearly zero, so no outflow. Set flow velocity and depth-weighted flow velocity to zero and return
      Cell[nX][nY].pGetSurfaceWater()->InitializeAllFlowVelocity();

      return;
   }

   // If the timestep is less than this outflow time, there will not have been enough time for the whole of the head to move from the centroid of one cell to the centroid of the next. Assume a linear relationship, so that depth_to_move = (head * timestep) / outflowtime
   double 
      dFractionOfTimestep = tMin(m_dTimeStep / dOutFlowTime, 1.0),
      dWDpthToMove = dHead * dFractionOfTimestep;

   // Set the flow velocity and the depth-averaged velocity. This is lower than the normal velocity if only a part of the depth is moved; same as the normal velocity if the the whole depth is moved
   double dFractionToMove = tMin(dWDpthToMove / dThisDepth, 1.0);
   Cell[nX][nY].pGetSurfaceWater()->SetFlowVelocity(Vec2DFlowVelocity);
   Cell[nX][nY].pGetSurfaceWater()->SetDepthWeightedFlowVelocity(Vec2DFlowVelocity * dFractionToMove);

   // Move water (and maybe sediment) from this cell to the adjacent cell
   CellMoveWater(nX, nY, nLowX, nLowY, dThisDepth, dWDpthToMove);

   // This outflow may or may not be erosive: it depends on whether or not it has exceeded its transport capacity. So check transport capacity for this cell, and either erode it or deposit some sediment here
   if (m_bFlowErosion)
      CalcTransportCapacity(nX, nY, nLowX, nLowY, dThisDepth, dHead, dTopSlope, dHLen, dFlowSpeed, dWDpthToMove);
}


/*=========================================================================================================================================

 This moves water off the edge from a single cell, if possible

=========================================================================================================================================*/
void CSimulation::TryEdgeCellOutFlow(int const nX, int const nY, int const nDir)
{
   // We need a value for off-edge head: different approaches seem to work best in different situations
   double dHead = 0;
   if (m_dGradient > 0)
   {
      // The user has imposed a gradient, so this is e.g. a flume. Use the maximum on-grid head during the previous iteration
      dHead = m_dLastIterMaxHead;      
   }
   else
   {
      // No gradient imposed, so this is probably a real soil-surface DEM. TODO experiment
      dHead = (m_dLastIterMaxHead + m_dLastIterMaxHead) / 2;
   }   

   double dThisDepth = Cell[nX][nY].pGetSurfaceWater()->dGetSurfaceWater();
   if (dThisDepth < dHead)
   {
      // There isn't enough water on this cell to level both water surfaces, so just move what's there
      // TODO how does this affect fOutFlowTime? Is currently ignored
      dHead = dThisDepth;
   }

   if (dHead < FLOW_MIN_CONSIDERED)
   {
      // Elevation difference not great enough, no flow off this edge yet
      // TODO Check this
      //      Cell[nX][nY].InitializeAllFlowVelocity();

      return;
   }

   // OK, there is flow off this edge, so set the flow direction
   Cell[nX][nY].pGetSurfaceWater()->SetFlowDirection(nDir);

   double 
      dTopSlope = dHead * m_dInvCellSide,
      dFlowSpeed = 0;                                                      // Value to be calculated in dTimeToCrossCell
   C2DVec Vec2DFlowVelocity;                                               // Ditto

   // Now use the Darcy-Weisbach equation to calculate flowspeed (which may be constrained) and hence the time taken for water to flow from the centroid of this cell to the next
   double dOutFlowTime = dTimeToCrossCell(nX, nY, nDir, dTopSlope, dThisDepth, m_dCellSide, Vec2DFlowVelocity, dFlowSpeed);
   if (dOutFlowTime == FOREVER)
   {
      // TODO check this
      //      Cell[nX][nY].InitializeAllFlowVelocity();
      
      return;
   }

   // If the timestep is less than this outflow time, there will not have been enough time for the whole of the head to move from the centroid of one cell to the centroid of the next. Assume a linear relationship, so that depth_to_move = (head * timestep) / outflowtime
   double 
      dFractionOfTimestep = tMin(m_dTimeStep / dOutFlowTime, 1.0),
      dWDpthToMove = dHead * dFractionOfTimestep,
      dFractionToMove = tMin(dWDpthToMove / dThisDepth, 1.0);

   // Set the flow velocity and the depth-averaged velocity. This is lower than the normal velocity if only a part of the depth is moved; same as the normal velocity if the the whole depth is moved TODO is this OK?
   Cell[nX][nY].pGetSurfaceWater()->SetFlowVelocity(Vec2DFlowVelocity);
   Cell[nX][nY].pGetSurfaceWater()->SetDepthWeightedFlowVelocity(Vec2DFlowVelocity * dFractionToMove);

   // Move water from this cell, off the edge
   Cell[nX][nY].pGetSurfaceWater()->ChangeSurfaceWater(-dWDpthToMove);
   m_dThisIterWaterLost += dWDpthToMove;
   
   // Now move the sediment that was being transported in this depth of water (assumes that sediment is well mixed in the water column)
   double 
      dClaySedimentToRemove = Cell[nX][nY].pGetSediment()->dGetClaySedimentLoad() * dFractionToMove,
      dSiltSedimentToRemove = Cell[nX][nY].pGetSediment()->dGetSiltSedimentLoad() * dFractionToMove,
      dSandSedimentToRemove = Cell[nX][nY].pGetSediment()->dGetSandSedimentLoad() * dFractionToMove;
      
   if (dClaySedimentToRemove > 0)
   {
      // Remove some clay-sized sediment
      Cell[nX][nY].pGetSediment()->dChangeClaySedimentLoad(-dClaySedimentToRemove);       
      m_dThisIterClaySedLost += dClaySedimentToRemove;
   }

   if (dSiltSedimentToRemove > 0)
   {
      // Remove some silt-sized sediment
      Cell[nX][nY].pGetSediment()->dChangeSiltSedimentLoad(-dSiltSedimentToRemove);       
      m_dThisIterSiltSedLost += dSiltSedimentToRemove;
   }
   
   if (dSandSedimentToRemove > 0)
   {
      // Remove some sand-sized sediment
      Cell[nX][nY].pGetSediment()->dChangeSandSedimentLoad(-dSandSedimentToRemove);       
      m_dThisIterSandSedLost += dSandSedimentToRemove;
   }
   
   #if defined _DEBUG
   // Add this amount to the display-only total of surface water lost from the edge for this cell
   Cell[nX][nY].pGetSurfaceWater()->AddSurfaceWaterLost(dWDpthToMove);
#endif

   // Finally, off-edge outflow from this edge cell may be capable of eroding the edge cell, or deposition may occur on the edge cell. So check transport capacity for this cell, and either erode it or deposit some sediment here. TODO Note that we use the original (i.e. pre-outflow) values here, is this OK?
   if (m_bFlowErosion)
      CalcTransportCapacity(nX, nY, -1, -1, dThisDepth, dHead, dTopSlope, m_dCellSide, dFlowSpeed, dWDpthToMove);
}


/*=========================================================================================================================================

 Use the Darcy-Weisbach equation to calculate flowspeed and hence the time needed to flow from the centroid of this cell (the wet one) to
 the centroid of the next. If flowspeed is too high, it may be constrained

=========================================================================================================================================*/
double CSimulation::dTimeToCrossCell(int const nX, int const nY, int const nDir, double const dTopSlope, double dThisDepth, double const dHLen, C2DVec& Vec2DFlowVelocity, double& dFlowSpeed)
{
   double const 
      DARCY_WEISBACH_CONST = 0.008,
      CONV_CONST = 1000;

   // Calculate flow speed using the Darcy-Weisbach equation, with its friction factor determined by means of nondimensional boundary roughness (lambda = depth / epsilon), see: Lawrence, D.S.L. (1997). Macroscale surface roughness and frictional resistance in overland flow. Earth Surface Processes and Landforms 22, 365-382. Some changes made to equation because of units
   dFlowSpeed = CONV_CONST * dCalcFrictionFactor(nX, nY, dThisDepth / m_dEpsilon, false) * sqrt(DARCY_WEISBACH_CONST * m_dG * dCalcHydraulicRadius(nX, nY, nDir, dThisDepth) * dTopSlope);

   // Sometimes depth can be so small that a flow speed of zero gets calculated
   if (dFlowSpeed <= 0)
      return FOREVER;

   // If it is high enough, remember this flow speed when setting the value of m_dTimeStep in the next iteration
   m_dPossibleMaxSpeed = tMax(dFlowSpeed, m_dPossibleMaxSpeed);
   if (dFlowSpeed > m_dMaxSpeed)
      // Flowspeed is too high for this value of m_dTimeStep (due to Courant condition) so constrain it
      dFlowSpeed = m_dMaxSpeed;

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

 Calculates a cell's hydraulic radius

=========================================================================================================================================*/
double CSimulation::dCalcHydraulicRadius(int const nX, int const nY, int const nDir, double const dDepth)
{
   int dN = 0;

   // Check the two cells which are orthogonal to the direction of flow
   int nXAdj, nYAdj;
   switch (nDir)
   {
   case DIRECTION_BOTTOM:
   case DIRECTION_TOP:
      nXAdj = nX-1;
      nYAdj = nY;
      if ((nXAdj >= 0) && Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         dN++;

      nXAdj = nX+1;
      nYAdj = nY;
      if ((nXAdj < m_nXGridMax) && Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         dN++;

      break;

   case DIRECTION_BOTTOM_RIGHT:
   case DIRECTION_TOP_LEFT:
      nXAdj = nX+1;
      nYAdj = nY-1;
      if ((nXAdj < m_nXGridMax) && (nYAdj > 0) && Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         dN++;

      nXAdj = nX-1;
      nYAdj = nY+1;
      if ((nXAdj >= 0) && (nYAdj < m_nYGridMax) && Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         dN++;

      break;

   case DIRECTION_BOTTOM_LEFT:
   case DIRECTION_TOP_RIGHT:
      nXAdj = nX-1;
      nYAdj = nY-1;
      if ((nXAdj >= 0) && (nYAdj >= 0) && Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         dN++;

      nXAdj = nX+1;
      nYAdj = nY+1;
      if ((nXAdj < m_nXGridMax) && (nYAdj < m_nYGridMax) && Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         dN++;

      break;

   case DIRECTION_LEFT:
   case DIRECTION_RIGHT:
      nXAdj = nX;
      nYAdj = nY-1;
      if ((nYAdj >= 0) && Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         dN++;

      nXAdj = nX;
      nYAdj = nY+1;
      if ((nYAdj < m_nYGridMax) && Cell[nXAdj][nYAdj].pGetSurfaceWater()->bIsWet())
         dN++;

      break;
   }

   if (2 == dN)
   {
      // Both adjacent cells are wet, so R = (w * d / w) = d
      return (dDepth);
   }
   else if (1 == dN)
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

 Calculates the friction factor, using the notion of relative boundary roughness. Actually returns 1 / sqrt(friction factor)

=========================================================================================================================================*/
double CSimulation::dCalcFrictionFactor(int const nX, int const nY, double const dLambda, bool const bJustCheckEquation)
{
   double const 
      LAMBDA_SHALLOW_FLOW_THRESHOLD                = 1,
      LAMBDA_MARGINALLY_INUNDATED_FLOW_THRESHOLD   = 10;

   double const 
      MARGINAL_FLOW_CONST = 10,
      DEEP_FLOW_CONST_A = 1.64,
      DEEP_FLOW_CONST_B = 0.803;

   double dF;
   if (dLambda <= LAMBDA_SHALLOW_FLOW_THRESHOLD)
   {
      // Shallow flow
      dF = 1 / sqrt((8/PI) * m_dPr * m_dCd * tMin(PI/4, dLambda));

      if (! bJustCheckEquation)
         Cell[nX][nY].pGetSurfaceWater()->SetInundation(SHALLOW_FLOW);
   }
   else if (dLambda <= LAMBDA_MARGINALLY_INUNDATED_FLOW_THRESHOLD)
   {
      // Marginally-inundated flow
      dF = dLambda / sqrt(MARGINAL_FLOW_CONST);

      if (! bJustCheckEquation)
         Cell[nX][nY].pGetSurfaceWater()->SetInundation(MARGINAL_FLOW);
   }
   else
   {
      // Well-inundated flow
      dF = DEEP_FLOW_CONST_A + (DEEP_FLOW_CONST_B * log(dLambda));

      if (! bJustCheckEquation)
         Cell[nX][nY].pGetSurfaceWater()->SetInundation(DEEP_FLOW);
   }

   if (! bJustCheckEquation)
      // Save the 'real' value of the friction factor (not just 1 / sqrt(friction factor), which is what this function returns)
      Cell[nX][nY].pGetSurfaceWater()->SetFrictionFactor(1 / (dF * dF));

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
void CSimulation::CellMoveWater(int const nXFrom, int const nYFrom, int const nXTo, int const nYTo, double const& dThisDepth, double const& dWDpthToMove)
{
   // First remove the water from the source cell
   Cell[nXFrom][nYFrom].pGetSurfaceWater()->ChangeSurfaceWater(-dWDpthToMove);

   // And add the water to the destination cell
   Cell[nXTo][nYTo].pGetSurfaceWater()->ChangeSurfaceWater(dWDpthToMove);

   // Now calculate how much sediment to move
   double
      dFrac = dWDpthToMove / dThisDepth,
      dClaySedToMove = Cell[nXFrom][nYFrom].pGetSediment()->dGetClaySedimentLoad() * dFrac,
      dSiltSedToMove = Cell[nXFrom][nYFrom].pGetSediment()->dGetSiltSedimentLoad() * dFrac,
      dSandSedToMove = Cell[nXFrom][nYFrom].pGetSediment()->dGetSandSedimentLoad() * dFrac;
      
   if (dClaySedToMove > 0)
   {
      // Remove this depth of clay-sized sediment from the source cell
      Cell[nXFrom][nYFrom].pGetSediment()->dChangeClaySedimentLoad(-dClaySedToMove);
      
      // And add it to the destination cell
      Cell[nXTo][nYTo].pGetSediment()->dChangeClaySedimentLoad(dClaySedToMove);
   }
   
   if (dSiltSedToMove > 0)
   {
      // Remove this depth of silt-sized sediment from the source cell
      Cell[nXFrom][nYFrom].pGetSediment()->dChangeSiltSedimentLoad(-dSiltSedToMove);
      
      // And add it to the destination cell
      Cell[nXTo][nYTo].pGetSediment()->dChangeSiltSedimentLoad(dSiltSedToMove);
   }
   
   if (dSandSedToMove > 0)
   {
      // Remove this depth of sand-sized sediment from the source cell
      Cell[nXFrom][nYFrom].pGetSediment()->dChangeSandSedimentLoad(-dSandSedToMove);
      
      // And add it to the destination cell
      Cell[nXTo][nYTo].pGetSediment()->dChangeSandSedimentLoad(dSandSedToMove);
   }
}


/*=========================================================================================================================================
 
 Adds to this this-iteration value of surface water depth
 
=========================================================================================================================================*/
void CSimulation::AddThisIterSurfaceWater(double const dDepth)
{
   m_dThisIterSurfaceWaterStored += dDepth;
}
