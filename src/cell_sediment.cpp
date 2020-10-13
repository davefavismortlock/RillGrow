/*=========================================================================================================================================

This is cell_sediment.cpp: implementations of the RillGrow class used to represent sediment in a cell's surface water

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
#include "cell.h"
#include "cell_sediment.h"


CCellSedimentLoad::CCellSedimentLoad(void)
:
   m_dClaySedLoad(0),
   m_dSiltSedLoad(0),
   m_dSandSedLoad(0),
   m_dCumulClaySedLoad(0),
   m_dCumulSiltSedLoad(0),
   m_dCumulSandSedLoad(0)
   {
   m_pCell = NULL;
}

CCellSedimentLoad::~CCellSedimentLoad(void)
{
}


void CCellSedimentLoad::SetParent(CCell* const pParent)
{
   m_pCell = pParent;
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


// Adds to this cell's sediment load, and the this-iteration total sediment load
void CCellSedimentLoad::AddToSedLoad(const double dAddClayDepth, const double dAddSiltDepth, const double dAddSandDepth)
{
   assert(dAddClayDepth > 0);
   assert(dAddSiltDepth > 0);
   assert(dAddSandDepth > 0);

   // OK, add to this cell's sediment load
   m_dClaySedLoad += dAddClayDepth;
   m_dSiltSedLoad += dAddSiltDepth;
   m_dSandSedLoad += dAddSandDepth;

   // Add to this cell's cumulative sediment load
   m_dCumulClaySedLoad += dAddClayDepth;
   m_dCumulSiltSedLoad += dAddSiltDepth;
   m_dCumulSandSedLoad += dAddSandDepth;

   // Add to the this-iteration total sediment load
   m_pSim->ChangeThisIterClaySedLoad(dAddClayDepth);
   m_pSim->ChangeThisIterSiltSedLoad(dAddSiltDepth);
   m_pSim->ChangeThisIterSandSedLoad(dAddSandDepth);
}

// Removes from this cell's sediment load, and the this-iteration total sediment load; the three paramters are set to the values actually removed
void CCellSedimentLoad::RemoveFromSedLoad(double& dAddClayDepth, double& dAddSiltDepth, double& dAddSandDepth)
{
   assert(dAddClayDepth > 0);
   assert(dAddSiltDepth > 0);
   assert(dAddSandDepth > 0);

   // Is there enough clay sediment in this cell's sediment load? If not, constrain
   dAddClayDepth = tMin(m_dClaySedLoad, dAddClayDepth);

   // Is there enough silt sediment in this cell's sediment load? If not, constrain
   dAddSiltDepth = tMin(m_dSiltSedLoad, dAddSiltDepth);

   // Is there enough sand sediment in this cell's sediment load? If not, constrain
   dAddSandDepth = tMin(m_dSandSedLoad, dAddSandDepth);

   // OK, remove from this cell's sediment load
   m_dClaySedLoad -= dAddClayDepth;
   m_dSiltSedLoad -= dAddSiltDepth;
   m_dSandSedLoad -= dAddSandDepth;

   assert(m_dClaySedLoad >= 0);
   assert(m_dSiltSedLoad >= 0);
   assert(m_dSandSedLoad >= 0);

   // Remove from this cell's cumulative sediment load
   m_dCumulClaySedLoad -= dAddClayDepth;
   m_dCumulSiltSedLoad -= dAddSiltDepth;
   m_dCumulSandSedLoad -= dAddSandDepth;

   // Remove from the this-iteration total sediment load
   m_pSim->ChangeThisIterClaySedLoad(-dAddClayDepth);
   m_pSim->ChangeThisIterSiltSedLoad(-dAddSiltDepth);
   m_pSim->ChangeThisIterSandSedLoad(-dAddSandDepth);
}

// Removes from this cell's clay-sized sediment load
void CCellSedimentLoad::RemoveFromClaySedLoad(double& dDepthToRemove)
{
   assert(dDepthToRemove > 0);

   dDepthToRemove = tMin(m_dClaySedLoad, dDepthToRemove);
   m_dClaySedLoad -= dDepthToRemove;
   assert(m_dClaySedLoad >= 0);

   m_dCumulClaySedLoad -= dDepthToRemove;

   m_pSim->ChangeThisIterClaySedLoad(-dDepthToRemove);
}

double CCellSedimentLoad::dGetClaySedLoad(void) const
{
   return m_dClaySedLoad;
}

// Sets this cell's clay-sized sediment load to zero and returns the (+ve) depth that was subtracted
double CCellSedimentLoad::dSetClaySedLoadZero(void)
{
   double dSedimentLoad = m_dClaySedLoad;
   m_dClaySedLoad = 0;

   // Change cumulative value
   m_dCumulClaySedLoad -= dSedimentLoad;

   m_pSim->ChangeThisIterClaySedLoad(-dSedimentLoad);

   return dSedimentLoad;
}


// Removes from this cell's silt-sized sediment load
void CCellSedimentLoad::RemoveFromSiltSedLoad(double& dDepthToRemove)
{
   assert(dDepthToRemove > 0);

   dDepthToRemove = tMin(m_dSiltSedLoad, dDepthToRemove);
   m_dSiltSedLoad -= dDepthToRemove;
   assert(m_dSiltSedLoad >= 0);

   m_dCumulSiltSedLoad -= dDepthToRemove;

   m_pSim->ChangeThisIterSiltSedLoad(-dDepthToRemove);
}

double CCellSedimentLoad::dGetSiltSedLoad(void) const
{
   return m_dSiltSedLoad;
}

// Sets this cell's silt-sized sediment load to zero and returns the (+ve) depth that was subtracted
double CCellSedimentLoad::dSetSiltSedLoadZero(void)
{
   double dSedimentLoad = m_dSiltSedLoad;
   m_dSiltSedLoad = 0;

   // Change cumulative value
   m_dCumulSiltSedLoad -= dSedimentLoad;

   m_pSim->ChangeThisIterSiltSedLoad(-dSedimentLoad);

   return dSedimentLoad;
}


// Removes from this cell's sand-sized sediment load
void CCellSedimentLoad::RemoveFromSandSedLoad(double& dDepthToRemove)
{
   assert(dDepthToRemove > 0);

   dDepthToRemove = tMin(m_dSandSedLoad, dDepthToRemove);
   m_dSandSedLoad -= dDepthToRemove;
   assert(m_dSandSedLoad >= 0);

   m_dCumulSandSedLoad -= dDepthToRemove;

   m_pSim->ChangeThisIterSandSedLoad(-dDepthToRemove);
}

double CCellSedimentLoad::dGetSandSedLoad(void) const
{
   return m_dSandSedLoad;
}

// Sets this cell's sand-sized sediment load to zero and returns the (+ve) depth that was subtracted
double CCellSedimentLoad::dSetSandSedLoadZero(void)
{
   double dSedimentLoad = m_dSandSedLoad;
   m_dSandSedLoad = 0;

   // Change cumulative value
   m_dCumulSandSedLoad -= dSedimentLoad;

   m_pSim->ChangeThisIterSandSedLoad(-dSedimentLoad);

   return dSedimentLoad;
}

// Returns this-iteration percentage sediment concentration (all sediment size classes)
double CCellSedimentLoad::dGetAllSizeSedConc(void)
{
   double dWaterDepth = m_pCell->pGetSurfaceWater()->dGetSurfaceWaterDepth();

   if (dWaterDepth == 0)
      return 0;

   return 100 * (m_dClaySedLoad + m_dSiltSedLoad + m_dSandSedLoad) / dWaterDepth;
}
