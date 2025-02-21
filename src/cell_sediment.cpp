/*=========================================================================================================================================

This is cell_sediment.cpp: implementations of the RillGrow class used to represent sediment in cell's surface water

Copyright (C) 2025 David Favis-Mortlock

==========================================================================================================================================

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
#include "rg.h"
#include "cell.h"
#include "cell_sediment.h"

//! Constructor with initialization list
CCellSedimentLoad::CCellSedimentLoad(void)
:
   m_dLastIterClaySedLoad(0),
   m_dLastIterSiltSedLoad(0),
   m_dLastIterSandSedLoad(0),
   m_dCumulClaySedLoad(0),
   m_dCumulSiltSedLoad(0),
   m_dCumulSandSedLoad(0),
   m_dThisIterFlowClaySedLoad(0),
   m_dThisIterFlowSiltSedLoad(0),
   m_dThisIterFlowSandSedLoad(0),
   m_dThisIterSplashClaySedLoad(0),
   m_dThisIterSplashSiltSedLoad(0),
   m_dThisIterSplashSandSedLoad(0),
   m_dThisIterSlumpClaySedLoad(0),
   m_dThisIterSlumpSiltSedLoad(0),
   m_dThisIterSlumpSandSedLoad(0),
   m_dThisIterTopplingClaySedLoad(0),
   m_dThisIterTopplingSiltSedLoad(0),
   m_dThisIterTopplingSandSedLoad(0),
   m_dThisIterHeadcutRetreatClaySedLoad(0),
   m_dThisIterHeadcutRetreatSiltSedLoad(0),
   m_dThisIterHeadcutRetreatSandSedLoad(0),
   m_dThisIterClaySedRemoved(0),
   m_dThisIterSiltSedRemoved(0),
   m_dThisIterSandSedRemoved(0),
   m_dClaySplashOffEdge(0),
   m_dSiltSplashOffEdge(0),
   m_dSandSplashOffEdge(0)
   {
   pCell = NULL;
}

//! Destructor
CCellSedimentLoad::~CCellSedimentLoad(void)
{
}

//! Sets the pointer to the parent cell object
void CCellSedimentLoad::SetParent(CCell* const pParent)
{
   pCell = pParent;
}

//! Initializes all sediment load size classes
void CCellSedimentLoad::InitializeAllSizeSedLoad(void)
{
   m_dLastIterClaySedLoad = 0;
   m_dLastIterSiltSedLoad = 0;
   m_dLastIterSandSedLoad = 0;
   m_dThisIterFlowClaySedLoad = 0;
   m_dThisIterFlowSiltSedLoad = 0;
   m_dThisIterFlowSandSedLoad = 0;
   m_dThisIterSplashClaySedLoad = 0;
   m_dThisIterSplashSiltSedLoad = 0;
   m_dThisIterSplashSandSedLoad = 0;
   m_dThisIterSlumpClaySedLoad = 0;
   m_dThisIterSlumpSiltSedLoad = 0;
   m_dThisIterSlumpSandSedLoad = 0;
   m_dThisIterTopplingClaySedLoad = 0;
   m_dThisIterTopplingSiltSedLoad = 0;
   m_dThisIterTopplingSandSedLoad = 0;
   m_dThisIterHeadcutRetreatClaySedLoad = 0;
   m_dThisIterHeadcutRetreatSiltSedLoad = 0;
   m_dThisIterHeadcutRetreatSandSedLoad = 0;
   m_dThisIterClaySedRemoved = 0;
   m_dThisIterSiltSedRemoved = 0;
   m_dThisIterSandSedRemoved = 0;
}

//! Resets all sediment load size classes
void CCellSedimentLoad::ResetSedLoad(void)
{
   // assert(m_dThisIterFlowClaySedLoad >= 0);
   // assert(m_dThisIterSplashClaySedLoad >= 0);
   // assert(m_dThisIterSlumpClaySedLoad >= 0);
   // assert(m_dThisIterTopplingClaySedLoad >= 0);
   // assert(m_dThisIterHeadcutRetreatClaySedLoad >= 0);
   // assert(m_dThisIterClaySedRemoved >= 0);

   // TEST
   // cout << std::fixed << setprecision(20);
   // if (m_dThisIterClaySedRemoved > m_dLastIterClaySedLoad)
   //    cout << "ERROR: m_dThisIterClaySedRemoved = " << m_dThisIterClaySedRemoved << " m_dLastIterClaySedLoad = " << m_dLastIterClaySedLoad << endl;

   m_dLastIterClaySedLoad += (m_dThisIterFlowClaySedLoad + m_dThisIterSplashClaySedLoad + m_dThisIterSlumpClaySedLoad + m_dThisIterTopplingClaySedLoad + m_dThisIterHeadcutRetreatClaySedLoad - m_dThisIterClaySedRemoved);

   // TEST
   if (m_dLastIterClaySedLoad < 0)
      m_dLastIterClaySedLoad = 0;

   // assert(m_dLastIterClaySedLoad >= 0);

   // assert(m_dThisIterFlowSiltSedLoad >= 0);
   // assert(m_dThisIterSplashSiltSedLoad >= 0);
   // assert(m_dThisIterSlumpSiltSedLoad >= 0);
   // assert(m_dThisIterTopplingSiltSedLoad >= 0);
   // assert(m_dThisIterHeadcutRetreatSiltSedLoad >= 0);
   // assert(m_dThisIterSiltSedRemoved >= 0);

   // TEST
   // if (m_dThisIterSiltSedRemoved > m_dLastIterSiltSedLoad)
   //    cout << "ERROR: m_dThisIterSiltSedRemoved = " << m_dThisIterSiltSedRemoved << " m_dLastIterSiltSedLoad = " << m_dLastIterSiltSedLoad << endl;

   m_dLastIterSiltSedLoad += (m_dThisIterFlowSiltSedLoad + m_dThisIterSplashSiltSedLoad + m_dThisIterSlumpSiltSedLoad + m_dThisIterTopplingSiltSedLoad + m_dThisIterHeadcutRetreatSiltSedLoad - m_dThisIterSiltSedRemoved);

   // TEST
   if (m_dLastIterSiltSedLoad < 0)
      m_dLastIterSiltSedLoad = 0;

   // assert(m_dLastIterSiltSedLoad >= 0);

   // assert(m_dThisIterFlowSandSedLoad >= 0);
   // assert(m_dThisIterSplashSandSedLoad >= 0);
   // assert(m_dThisIterSlumpSandSedLoad >= 0);
   // assert(m_dThisIterTopplingSandSedLoad >= 0);
   // assert(m_dThisIterHeadcutRetreatSandSedLoad >= 0);
   // assert(m_dThisIterSandSedRemoved >= 0);

   // TEST
   // if (m_dThisIterSandSedRemoved > m_dLastIterSandSedLoad)
   //    cout << "ERROR: m_dThisIterSandSedRemoved = " << m_dThisIterSandSedRemoved << " m_dLastIterSandSedLoad = " << m_dLastIterSandSedLoad << endl;

   m_dLastIterSandSedLoad += (m_dThisIterFlowSandSedLoad + m_dThisIterSplashSandSedLoad + m_dThisIterSlumpSandSedLoad + m_dThisIterTopplingSandSedLoad + m_dThisIterHeadcutRetreatSandSedLoad - m_dThisIterSandSedRemoved);

   // TEST
   if (m_dLastIterSandSedLoad < 0)
      m_dLastIterSandSedLoad = 0;

   // assert(m_dLastIterSandSedLoad >= 0);

   m_dThisIterFlowClaySedLoad = 0;
   m_dThisIterFlowSiltSedLoad = 0;
   m_dThisIterFlowSandSedLoad = 0;
   m_dThisIterSplashClaySedLoad = 0;
   m_dThisIterSplashSiltSedLoad = 0;
   m_dThisIterSplashSandSedLoad = 0;
   m_dThisIterSlumpClaySedLoad = 0;
   m_dThisIterSlumpSiltSedLoad = 0;
   m_dThisIterSlumpSandSedLoad = 0;
   m_dThisIterTopplingClaySedLoad = 0;
   m_dThisIterTopplingSiltSedLoad = 0;
   m_dThisIterTopplingSandSedLoad = 0;
   m_dThisIterHeadcutRetreatClaySedLoad = 0;
   m_dThisIterHeadcutRetreatSiltSedLoad = 0;
   m_dThisIterHeadcutRetreatSandSedLoad = 0;
   m_dThisIterClaySedRemoved = 0;
   m_dThisIterSiltSedRemoved = 0;
   m_dThisIterSandSedRemoved = 0;
}

//! Adds to this cell's flow sediment load for clay, also add to cumulative values
void CCellSedimentLoad::AddToClayFlowSedLoad(const double dAddClayDepth)
{
   // assert(dAddClayDepth >= 0);

   m_dThisIterFlowClaySedLoad += dAddClayDepth;

   // Also add to this cell's cumulative sediment load
   m_dCumulClaySedLoad += dAddClayDepth;
}

//! Adds to this cell's flow sediment load for silt, also add to cumulative values
void CCellSedimentLoad::AddToSiltFlowSedLoad(const double dAddSiltDepth)
{
   // assert(dAddSiltDepth >= 0);

   m_dThisIterFlowSiltSedLoad += dAddSiltDepth;

   // Also add to this cell's cumulative sediment load
   m_dCumulSiltSedLoad += dAddSiltDepth;
}

//! Adds to this cell's flow sediment load for sand, also add to cumulative values
void CCellSedimentLoad::AddToSandFlowSedLoad(const double dAddSandDepth)
{
   // assert(dAddSandDepth >= 0);

   m_dThisIterFlowSandSedLoad += dAddSandDepth;

   // Also add to this cell's cumulative sediment load
   m_dCumulSandSedLoad += dAddSandDepth;
}

//! Adds to this cell's total of clay-sized sediment removed, considering supply limitation. The parameter is set to the value actually removed
void CCellSedimentLoad::AddToClaySedLoadRemoved(double& dRemoveDepth)
{
   dRemoveDepth = tMin(m_dLastIterClaySedLoad - m_dThisIterClaySedRemoved, dRemoveDepth);

   m_dThisIterClaySedRemoved += dRemoveDepth;
}

//! Adds to this cell's total of silt-sized sediment removed, considering supply limitation. The parameter is set to the value actually removed
void CCellSedimentLoad::AddToSiltSedLoadRemoved(double& dRemoveDepth)
{
   dRemoveDepth = tMin(m_dLastIterSiltSedLoad - m_dThisIterSiltSedRemoved, dRemoveDepth);

   m_dThisIterSiltSedRemoved += dRemoveDepth;
}

//! Adds to this cell's total of sand-sized sediment removed, considering supply limitation. The parameter is set to the value actually removed
void CCellSedimentLoad::AddToSandSedLoadRemoved(double& dRemoveDepth)
{
   dRemoveDepth = tMin(m_dLastIterSandSedLoad - m_dThisIterSandSedRemoved, dRemoveDepth);

   m_dThisIterSandSedRemoved += dRemoveDepth;
}

//! Returns cumulative sediment load (total for all size classes)
double CCellSedimentLoad::dGetCumulAllSizeSedLoad(void) const
{
   return (m_dCumulClaySedLoad + m_dCumulSiltSedLoad + m_dCumulSandSedLoad);
}

//! Returns the clay sediment load due to flow erosion during this iteration
double CCellSedimentLoad::dGetClayFlowSedLoad(void) const
{
   return m_dThisIterFlowClaySedLoad;
}

//! Returns the silt sediment load due to flow erosion during this iteration
double CCellSedimentLoad::dGetSiltFlowSedLoad(void) const
{
   return m_dThisIterFlowSiltSedLoad;
}

//! Returns the sand sediment load due to flow erosion during this iteration
double CCellSedimentLoad::dGetSandFlowSedLoad(void) const
{
   return m_dThisIterFlowSandSedLoad;
}

//! Returns last-iteration sediment load for this cell (total for all size classes)
double CCellSedimentLoad::dGetLastIterAllSizeSedLoad(void) const
{
   return (m_dLastIterClaySedLoad + m_dLastIterSiltSedLoad + m_dLastIterSandSedLoad);
}

//! Returns this-iteration sediment load for this cell (total for all size classes)
double CCellSedimentLoad::dGetThisIterAllSizeSedLoad(void) const
{
   double dClayTot = m_dThisIterFlowClaySedLoad + m_dThisIterSplashClaySedLoad + m_dThisIterSlumpClaySedLoad + m_dThisIterTopplingClaySedLoad + m_dThisIterHeadcutRetreatClaySedLoad - m_dThisIterClaySedRemoved;

   double dSiltTot = m_dThisIterFlowSiltSedLoad + m_dThisIterSplashSiltSedLoad + m_dThisIterSlumpSiltSedLoad + m_dThisIterTopplingSiltSedLoad + m_dThisIterHeadcutRetreatSiltSedLoad - m_dThisIterSiltSedRemoved;

   double dSandTot = m_dThisIterFlowSandSedLoad + m_dThisIterSplashSandSedLoad + m_dThisIterSlumpSandSedLoad + m_dThisIterTopplingSandSedLoad + m_dThisIterHeadcutRetreatSandSedLoad - m_dThisIterSandSedRemoved;

   return (dClayTot + dSiltTot + dSandTot);
}

//! Gets the last-iteration clay sediment load for this cell
double CCellSedimentLoad::dGetLastIterClaySedLoad(void) const
{
   return m_dLastIterClaySedLoad;
}

//! Gets the last-iteration silt sediment load for this cell
double CCellSedimentLoad::dGetLastIterSiltSedLoad(void) const
{
   return m_dLastIterSiltSedLoad;
}

//! Gets the last-iteration sand sediment load for this cell
double CCellSedimentLoad::dGetLastIterSandSedLoad(void) const
{
   return m_dLastIterSandSedLoad;
}

//! Returns this-iteration percentage sediment concentration (all sediment size classes)
double CCellSedimentLoad::dGetAllSizeSedConcentration(void)
{
   double dWaterDepth = pCell->pGetSurfaceWater()->dGetSurfaceWaterDepth();

   if (bFpEQ(dWaterDepth, 0.0, TOLERANCE))
      return 0;

   double dClayTot = m_dThisIterFlowClaySedLoad + m_dThisIterSplashClaySedLoad + m_dThisIterSlumpClaySedLoad + m_dThisIterTopplingClaySedLoad + m_dThisIterHeadcutRetreatClaySedLoad;

   double dSiltTot = m_dThisIterFlowSiltSedLoad + m_dThisIterSplashSiltSedLoad + m_dThisIterSlumpSiltSedLoad + m_dThisIterTopplingSiltSedLoad + m_dThisIterHeadcutRetreatSiltSedLoad;

   double dSandTot = m_dThisIterFlowSandSedLoad + m_dThisIterSplashSandSedLoad + m_dThisIterSlumpSandSedLoad + m_dThisIterTopplingSandSedLoad + m_dThisIterHeadcutRetreatSandSedLoad;

   return 100 * (dClayTot + dSiltTot + dSandTot) / dWaterDepth;
}

//! Add to the cell's values for this-iteration sediment load derived from splash, also adds to the cumulative sediment load cvalues
void CCellSedimentLoad::AddToSplashSedLoad(const double dClay, const double dSilt, const double dSand)
{
   m_dThisIterSplashClaySedLoad += dClay;
   m_dThisIterSplashSiltSedLoad += dSilt;
   m_dThisIterSplashSandSedLoad += dSand;

   // Also add to this cell's cumulative sediment load
   m_dCumulClaySedLoad += dClay;
   m_dCumulSiltSedLoad += dSilt;
   m_dCumulSandSedLoad += dSand;
}

//! Returns the cell's this-iteration value for clay sediment load derived from splash
double CCellSedimentLoad::dGetClaySplashSedLoad(void) const
{
   return m_dThisIterSplashClaySedLoad;
}

//! Returns the cell's this-iteration value for silt sediment load derived from splash
double CCellSedimentLoad::dGetSiltSplashSedLoad(void) const
{
   return m_dThisIterSplashSiltSedLoad;
}

//! Returns the cell's this-iteration value for sand sediment load derived from splash
double CCellSedimentLoad::dGetSandSplashSedLoad(void) const
{
   return m_dThisIterSplashSandSedLoad;
}

//! Add to the cell's values for this-iteration (actually, several iterations) sediment load derived from slumping, also adds to the cumulative sediment load values
void CCellSedimentLoad::AddToSlumpSedLoad(const double dClay, const double dSilt, const double dSand)
{
   m_dThisIterSlumpClaySedLoad += dClay;
   m_dThisIterSlumpSiltSedLoad += dSilt;
   m_dThisIterSlumpSandSedLoad += dSand;

   // Also add to this cell's cumulative sediment load
   m_dCumulClaySedLoad += dClay;
   m_dCumulSiltSedLoad += dSilt;
   m_dCumulSandSedLoad += dSand;

}

//! Returns the cell's this-iteration (actually, several iterations) value for clay sediment load derived from slumping
double CCellSedimentLoad::dGetClaySlumpSedLoad(void) const
{
   return m_dThisIterSlumpClaySedLoad;
}

//! Returns the cell's this-iteration value (actually, several iterations) for silt sediment load derived from slumping
double CCellSedimentLoad::dGetSiltSlumpSedLoad(void) const
{
   return m_dThisIterSlumpSiltSedLoad;
}

//! Returns the cell's this-iteration value (actually, several iterations) for sand sediment load derived from slumping
double CCellSedimentLoad::dGetSandSlumpSedLoad(void) const
{
   return m_dThisIterSlumpSandSedLoad;
}

//! Adds to the cell's values for this-iteration (actually, several iterations) sediment load derived from toppling. Also ads to the cumulative sediment load values
void CCellSedimentLoad::AddToToppleSedLoad(const double dClay, const double dSilt, const double dSand)
{
   m_dThisIterTopplingClaySedLoad += dClay;
   m_dThisIterTopplingSiltSedLoad += dSilt;
   m_dThisIterTopplingSandSedLoad += dSand;

   // Also add to this cell's cumulative sediment load
   m_dCumulClaySedLoad += dClay;
   m_dCumulSiltSedLoad += dSilt;
   m_dCumulSandSedLoad += dSand;
}

//! Returns the cell's this-iteration (actually, several iterations) value for clay sediment load derived from toppling
double CCellSedimentLoad::dGetClayToppleSedLoad(void) const
{
   return m_dThisIterTopplingClaySedLoad;
}

//! Returns the cell's this-iteration value (actually, several iterations) for silt sediment load derived from toppling
double CCellSedimentLoad::dGetSiltToppleSedLoad(void) const
{
   return m_dThisIterTopplingSiltSedLoad;
}

//! Returns the cell's this-iteration value (actually, several iterations) for sand sediment load derived from toppling
double CCellSedimentLoad::dGetSandToppleSedLoad(void) const
{
   return m_dThisIterTopplingSandSedLoad;
}

//! Adds to the this-iteration sediment load derived from headcut retreat. Also adds to the cumulative sediment load values
void CCellSedimentLoad::AddToHeadcutRetreatSedLoad(const double dClay, const double dSilt, const double dSand)
{
   m_dThisIterHeadcutRetreatClaySedLoad += dClay;
   m_dThisIterHeadcutRetreatSiltSedLoad += dSilt;
   m_dThisIterHeadcutRetreatSandSedLoad += dSand;

   // Also add to this cell's cumulative sediment load
   m_dCumulClaySedLoad += dClay;
   m_dCumulSiltSedLoad += dSilt;
   m_dCumulSandSedLoad += dSand;

}

//! Gets the this-iteration clay sediment load derived from headcut retreat
double CCellSedimentLoad::dGetClayHeadcutRetreatSedLoad(void) const
{
   return m_dThisIterHeadcutRetreatClaySedLoad;
}

//! Gets the this-iteration silt sediment load derived from headcut retreat
double CCellSedimentLoad::dGetSiltHeadcutRetreatSedLoad(void) const
{
   return m_dThisIterHeadcutRetreatSiltSedLoad;
}

//! Gets the this-iteration sand sediment load derived from headcut retreat
double CCellSedimentLoad::dGetSandHeadcutRetreatSedLoad(void) const
{
   return m_dThisIterHeadcutRetreatSandSedLoad;
}

//! Initializes this-iteration sediment load (all size classes) derived from slumping, and toppling
void CCellSedimentLoad::InitSlumpAndToppleSedLoads(void)
{
   m_dThisIterSlumpClaySedLoad  =
   m_dThisIterSlumpSiltSedLoad  =
   m_dThisIterSlumpSandSedLoad  =
   m_dThisIterTopplingClaySedLoad =
   m_dThisIterTopplingSiltSedLoad =
   m_dThisIterTopplingSandSedLoad = 0;
}

//! Initializes this-iteration off-edge splash
void CCellSedimentLoad::InitSplashOffEdge(void)
{
   m_dClaySplashOffEdge =
   m_dSiltSplashOffEdge =
   m_dSandSplashOffEdge = 0;
}

//! Initializes this-iteration sediment load (all size classes) derived from headcut retreat
void CCellSedimentLoad::InitializeHeadcutRetreatSedLoads(void)
{
   m_dThisIterHeadcutRetreatClaySedLoad =
   m_dThisIterHeadcutRetreatSiltSedLoad =
   m_dThisIterHeadcutRetreatSandSedLoad = 0;
}

//! Returns this-iteration clay sediment load removed
double CCellSedimentLoad::dGetClaySedLoadRemoved(void) const
{
   return m_dThisIterClaySedRemoved;
}

//! Returns this-iteration silt sediment load removed
double CCellSedimentLoad::dGetSiltSedLoadRemoved(void) const
{
   return m_dThisIterSiltSedRemoved;
}

//! Returns this-iteration sand sediment load removed
double CCellSedimentLoad::dGetSandSedLoadRemoved(void) const
{
   return m_dThisIterSandSedRemoved;
}

//! Adds to the this-iteration clay sediment lost from the grid edge. Only meaningful for edge cells
void CCellSedimentLoad::AddToClaySedOffEdge(const double dClay)
{
   m_dClaySplashOffEdge += dClay;
}

//! Adds to the this-iteration silt sediment lost from the grid edge. Only meaningful for edge cells
void CCellSedimentLoad::AddToSiltSedOffEdge(const double dSilt)
{
   m_dSiltSplashOffEdge += dSilt;
}

//! Adds to the this-iteration sand sediment lost from the grid edge. Only meaningful for edge cells
void CCellSedimentLoad::AddToSandSedOffEdge(const double dSand)
{
   m_dSandSplashOffEdge += dSand;
}

//! Returns the this-iteration value for clay sediment lost from the grid edge. Only meaningful for edge cells
double CCellSedimentLoad::dGetClaySedOffEdge(void) const
{
   return m_dClaySplashOffEdge;
}

//! Returns the this-iteration value for silt sediment lost from the grid edge. Only meaningful for edge cells
double CCellSedimentLoad::dGetSiltSedOffEdge(void) const
{
   return m_dSiltSplashOffEdge;
}

//! Returns the this-iteration value for sand sediment lost from the grid edge. Only meaningful for edge cells
double CCellSedimentLoad::dGetSandSedOffEdge(void) const
{
   return m_dSandSplashOffEdge;
}
