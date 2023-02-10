/*=========================================================================================================================================

This is m_Cell_sediment.cpp: implementations of the RillGrow class used to represent sediment in a m_Cell's surface water

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
#include "cell.h"
#include "cell_sediment.h"


CCellSedimentLoad::CCellSedimentLoad(void)
:
   m_dClaySedLoad(0),
   m_dSiltSedLoad(0),
   m_dSandSedLoad(0),
   m_dCumulClaySedLoad(0),
   m_dCumulSiltSedLoad(0),
   m_dCumulSandSedLoad(0),
   m_dFromSplashClaySedLoad(0),
   m_dFromSplashSiltSedLoad(0),
   m_dFromSplashSandSedLoad(0),
   m_dFromSlumpClaySedLoad(0),
   m_dFromSlumpSiltSedLoad(0),
   m_dFromSlumpSandSedLoad(0),
   m_dFromToppleClaySedLoad(0),
   m_dFromToppleSiltSedLoad(0),
   m_dFromToppleSandSedLoad(0),
   m_dFromHeadcutRetreatClaySedLoad(0),
   m_dFromHeadcutRetreatSiltSedLoad(0),
   m_dFromHeadcutRetreatSandSedLoad(0)
   {
   m_pm_Cell = NULL;
}

CCellSedimentLoad::~CCellSedimentLoad(void)
{
}


void CCellSedimentLoad::SetParent(CCell* const pParent)
{
   m_pm_Cell = pParent;
}


void CCellSedimentLoad::InitializeAllSizeSedLoad(void)
{
   m_dClaySedLoad =
   m_dSiltSedLoad =
   m_dSandSedLoad = 0;
}


double CCellSedimentLoad::dGetCumulAllSizeSedLoad(void) const
{
   return (m_dCumulClaySedLoad + m_dCumulSiltSedLoad + m_dCumulSandSedLoad);
}


double CCellSedimentLoad::dGetAllSizeSedLoad(void) const
{
   return (m_dClaySedLoad + m_dSiltSedLoad + m_dSandSedLoad);
}


// Adds to this m_Cell's sediment load
void CCellSedimentLoad::AddToSedLoad(const double dAddClayDepth, const double dAddSiltDepth, const double dAddSandDepth)
{
   if (dAddClayDepth > 0)
   {
      // OK, add to this m_Cell's sediment load
      m_dClaySedLoad += dAddClayDepth;

      // Add to this m_Cell's cumulative sediment load
      m_dCumulClaySedLoad += dAddClayDepth;
   }

   if (dAddSiltDepth > 0)
   {
      // OK, add to this m_Cell's sediment load
      m_dSiltSedLoad += dAddSiltDepth;

      // Add to this m_Cell's cumulative sediment load
      m_dCumulSiltSedLoad += dAddSiltDepth;
   }

   if (dAddSandDepth > 0)
   {
      // OK, add to this m_Cell's sediment load
      m_dSandSedLoad += dAddSandDepth;

      // Add to this m_Cell's cumulative sediment load
      m_dCumulSandSedLoad += dAddSandDepth;
   }
}

// Removes from this m_Cell's sediment load; the three paramters are set to the values actually removed
void CCellSedimentLoad::RemoveFromSedLoad(double& dAddClayDepth, double& dAddSiltDepth, double& dAddSandDepth)
{
   if (dAddClayDepth > 0)
   {
      // Is there enough clay sediment in this m_Cell's sediment load? If not, constrain
      dAddClayDepth = tMin(m_dClaySedLoad, dAddClayDepth);

      // OK, remove from this m_Cell's sediment load
      m_dClaySedLoad -= dAddClayDepth;

      // Remove from this m_Cell's cumulative sediment load
      m_dCumulClaySedLoad -= dAddClayDepth;
   }

   if (dAddSiltDepth > 0)
   {
      // Is there enough silt sediment in this m_Cell's sediment load? If not, constrain
      dAddSiltDepth = tMin(m_dSiltSedLoad, dAddSiltDepth);

      // OK, remove from this m_Cell's sediment load
      m_dSiltSedLoad -= dAddSiltDepth;

      // Remove from this m_Cell's cumulative sediment load
      m_dCumulSiltSedLoad -= dAddSiltDepth;
   }

   if (dAddSandDepth > 0)
   {
      // Is there enough sand sediment in this m_Cell's sediment load? If not, constrain
      dAddSandDepth = tMin(m_dSandSedLoad, dAddSandDepth);

      // OK, remove from this m_Cell's sediment load
      m_dSandSedLoad -= dAddSandDepth;

      // Remove from this m_Cell's cumulative sediment load
      m_dCumulSandSedLoad -= dAddSandDepth;
   }
}

// Removes from this m_Cell's clay-sized sediment load
void CCellSedimentLoad::RemoveFromClaySedLoad(double& dDepthToRemove)
{
   dDepthToRemove = tMin(m_dClaySedLoad, dDepthToRemove);
   m_dClaySedLoad -= dDepthToRemove;

   m_dCumulClaySedLoad -= dDepthToRemove;
}

double CCellSedimentLoad::dGetClaySedLoad(void) const
{
   return m_dClaySedLoad;
}

// Sets this m_Cell's clay-sized sediment load to zero and returns the (+ve) depth that was subtracted
double CCellSedimentLoad::dSetClaySedLoadZero(void)
{
   double dSedimentLoad = m_dClaySedLoad;
   m_dClaySedLoad = 0;

   // Change cumulative value
   m_dCumulClaySedLoad -= dSedimentLoad;

   return dSedimentLoad;
}


// Removes from this m_Cell's silt-sized sediment load
void CCellSedimentLoad::RemoveFromSiltSedLoad(double& dDepthToRemove)
{
   dDepthToRemove = tMin(m_dSiltSedLoad, dDepthToRemove);
   m_dSiltSedLoad -= dDepthToRemove;

   m_dCumulSiltSedLoad -= dDepthToRemove;
}

double CCellSedimentLoad::dGetSiltSedLoad(void) const
{
   return m_dSiltSedLoad;
}

// Sets this m_Cell's silt-sized sediment load to zero and returns the (+ve) depth that was subtracted
double CCellSedimentLoad::dSetSiltSedLoadZero(void)
{
   double dSedimentLoad = m_dSiltSedLoad;
   m_dSiltSedLoad = 0;

   // Change cumulative value
   m_dCumulSiltSedLoad -= dSedimentLoad;

   return dSedimentLoad;
}


// Removes from this m_Cell's sand-sized sediment load
void CCellSedimentLoad::RemoveFromSandSedLoad(double& dDepthToRemove)
{
   dDepthToRemove = tMin(m_dSandSedLoad, dDepthToRemove);
   m_dSandSedLoad -= dDepthToRemove;

   m_dCumulSandSedLoad -= dDepthToRemove;
}

double CCellSedimentLoad::dGetSandSedLoad(void) const
{
   return m_dSandSedLoad;
}

// Sets this m_Cell's sand-sized sediment load to zero and returns the (+ve) depth that was subtracted
double CCellSedimentLoad::dSetSandSedLoadZero(void)
{
   double dSedimentLoad = m_dSandSedLoad;
   m_dSandSedLoad = 0;

   // Change cumulative value
   m_dCumulSandSedLoad -= dSedimentLoad;

   return dSedimentLoad;
}

// Returns this-iteration percentage sediment concentration (all sediment size classes)
double CCellSedimentLoad::dGetAllSizeSedConc(void)
{
   double dWaterDepth = m_pm_Cell->pGetSurfaceWater()->dGetSurfaceWaterDepth();

   if (dWaterDepth == 0)
      return 0;

   return 100 * (m_dClaySedLoad + m_dSiltSedLoad + m_dSandSedLoad) / dWaterDepth;
}

// Add to the m_Cell's values for this-iteration sediment load derived from splash
void CCellSedimentLoad::AddToSplashSedLoad(const double dClay, const double dSilt, const double dSand)
{
   m_dFromSplashClaySedLoad += dClay;
   m_dFromSplashSiltSedLoad += dSilt;
   m_dFromSplashSandSedLoad += dSand;
}

// Returns the m_Cell's this-iteration value for clay sediment load derived from splash
double CCellSedimentLoad::dGetClaySplashSedLoad(void) const
{
   return m_dFromSplashClaySedLoad;
}

// Returns the m_Cell's this-iteration value for silt sediment load derived from splash
double CCellSedimentLoad::dGetSiltSplashSedLoad(void) const
{
   return m_dFromSplashSiltSedLoad;
}

// Returns the m_Cell's this-iteration value for sand sediment load derived from splash
double CCellSedimentLoad::dGetSandSplashSedLoad(void) const
{
   return m_dFromSplashSandSedLoad;
}

// Add to the m_Cell's values for this-iteration (actually, several iterations) sediment load derived from slumping
void CCellSedimentLoad::AddToSlumpSedLoad(const double dClay, const double dSilt, const double dSand)
{
   m_dFromSlumpClaySedLoad += dClay;
   m_dFromSlumpSiltSedLoad += dSilt;
   m_dFromSlumpSandSedLoad += dSand;
}

// Returns the m_Cell's this-iteration (actually, several iterations) value for clay sediment load derived from slumping
double CCellSedimentLoad::dGetClaySlumpSedLoad(void) const
{
   return m_dFromSlumpClaySedLoad;
}

// Returns the m_Cell's this-iteration value (actually, several iterations) for silt sediment load derived from slumping
double CCellSedimentLoad::dGetSiltSlumpSedLoad(void) const
{
   return m_dFromSlumpSiltSedLoad;
}

// Returns the m_Cell's this-iteration value (actually, several iterations) for sand sediment load derived from slumping
double CCellSedimentLoad::dGetSandSlumpSedLoad(void) const
{
   return m_dFromSlumpSandSedLoad;
}



// Add to the m_Cell's values for this-iteration (actually, several iterations) sediment load derived from toppling
void CCellSedimentLoad::AddToToppleSedLoad(const double dClay, const double dSilt, const double dSand)
{
   m_dFromToppleClaySedLoad += dClay;
   m_dFromToppleSiltSedLoad += dSilt;
   m_dFromToppleSandSedLoad += dSand;
}

// Returns the m_Cell's this-iteration (actually, several iterations) value for clay sediment load derived from toppling
double CCellSedimentLoad::dGetClayToppleSedLoad(void) const
{
   return m_dFromToppleClaySedLoad;
}

// Returns the m_Cell's this-iteration value (actually, several iterations) for silt sediment load derived from toppling
double CCellSedimentLoad::dGetSiltToppleSedLoad(void) const
{
   return m_dFromToppleSiltSedLoad;
}

// Returns the m_Cell's this-iteration value (actually, several iterations) for sand sediment load derived from toppling
double CCellSedimentLoad::dGetSandToppleSedLoad(void) const
{
   return m_dFromToppleSandSedLoad;
}

void CCellSedimentLoad::AddToHeadcutRetreatSedLoad(const double dClay, const double dSilt, const double dSand)
{
   m_dFromHeadcutRetreatClaySedLoad += dClay;
   m_dFromHeadcutRetreatSiltSedLoad += dSilt;
   m_dFromHeadcutRetreatSandSedLoad += dSand;
}

double CCellSedimentLoad::dGetClayHeadcutRetreatSedLoad(void) const
{
   return m_dFromHeadcutRetreatClaySedLoad;
}

double CCellSedimentLoad::dGetSiltHeadcutRetreatSedLoad(void) const
{
   return m_dFromHeadcutRetreatSiltSedLoad;
}

double CCellSedimentLoad::dGetSandHeadcutRetreatSedLoad(void) const
{
   return m_dFromHeadcutRetreatSandSedLoad;
}

void CCellSedimentLoad::InitializeSplashSedLoads(void)
{
   m_dFromSplashClaySedLoad =
   m_dFromSplashSiltSedLoad =
   m_dFromSplashSandSedLoad = 0;
}

void CCellSedimentLoad::InitializeSlumpAndToppleSedLoads(void)
{
   m_dFromSplashClaySedLoad =
   m_dFromSplashSiltSedLoad =
   m_dFromSplashSandSedLoad =
   m_dFromSlumpClaySedLoad  =
   m_dFromSlumpSiltSedLoad  =
   m_dFromSlumpSandSedLoad  =
   m_dFromToppleClaySedLoad =
   m_dFromToppleSiltSedLoad =
   m_dFromToppleSandSedLoad = 0;
}

void CCellSedimentLoad::InitializeHeadcutRetreatSedLoads(void)
{
   m_dFromHeadcutRetreatClaySedLoad =
   m_dFromHeadcutRetreatSiltSedLoad =
   m_dFromHeadcutRetreatSandSedLoad = 0;
}
