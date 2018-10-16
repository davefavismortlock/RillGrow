/*=========================================================================================================================================

 This is cell_surface_water.cpp: implementations of the RillGrow class used to represent a cell's surface water

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
#include "cell.h"
#include "cell_surface_water.h"


CSurfaceWater::CSurfaceWater(void)
:
   m_nFlowDirection(DIRECTION_NONE),
   m_nInundationClass(NO_FLOW),
   m_dSurfaceWaterDepth(0),
   m_dCumulativeSurfaceWaterDepth(0),
   m_dStreamPower(0),
   m_dTransCap(0),
   m_dFrictionFactor(0)
{   
#if defined _DEBUG
   m_dCumulativeSurfaceWaterDepthLost = 0;
#endif
   
   m_pCell = NULL;
}

CSurfaceWater::~CSurfaceWater(void)
{
}


void CSurfaceWater::SetParent(CCell* const pParent)
{
   m_pCell = pParent;
}


void CSurfaceWater::InitializeFlow(void)
{
   m_nFlowDirection = DIRECTION_NONE;
   m_nInundationClass = NO_FLOW;

   m_dStreamPower =
   m_dTransCap = 0;   
}

// Sets the inundation class
void CSurfaceWater::SetInundation(int const nNewInundation)
{
   m_nInundationClass = nNewInundation;
}

// Returns the inundation class
int CSurfaceWater::nGetInundation(void) const
{
   return m_nInundationClass;
}


// Sets the surface water flow direction
void CSurfaceWater::SetFlowDirection(int nNewFlowDir)
{
   m_nFlowDirection = nNewFlowDir;
}

// Returns the surface water flow direction
int CSurfaceWater::nGetFlowDirection(void) const
{
   return m_nFlowDirection;
}


// Changes this cell's surface water depth. Assumes that cell is already wet (or that chahes to number of wet cells etc. have already been taken care of.) Note no checks here to see if new depth is sensible (e.g. non-negative)
void CSurfaceWater::ChangeSurfaceWater(double const dNewWater)
{
   m_pSim->AddThisIterSurfaceWater(dNewWater);

   m_dSurfaceWaterDepth += dNewWater;   
   m_dCumulativeSurfaceWaterDepth += dNewWater;   
}

// Sets this cell's surface water depth to zero
void CSurfaceWater::SetSurfaceWaterZero(void)
{   
   m_dCumulativeSurfaceWaterDepth -= m_dSurfaceWaterDepth;
   m_dSurfaceWaterDepth = 0;
   
   this->InitializeAllFlowVelocity();
   
   // Subtract from the this-iteration total depth of surface water
   m_pSim->AddThisIterSurfaceWater(-m_dSurfaceWaterDepth);
   
   // Decrement the this-iteration total of wet cells
   m_pSim->DecrThisIterNumWetCells();   
}

// Returns the depth of surface water on this cell
double CSurfaceWater::dGetSurfaceWater(void) const
{
   return m_dSurfaceWaterDepth;
}

// Returns true if water depth > 0, false otherwise
bool CSurfaceWater::bIsWet(void) const
{
   return ((m_dSurfaceWaterDepth > 0) ? true : false);
}


// Gets the running total for water depth weighted by time (need to divide by elapsed time to get average depth)
double CSurfaceWater::dGetCumulativeSurfaceWater(void) const
{
   return m_dCumulativeSurfaceWaterDepth;
}


#if defined _DEBUG
// Increments the total depth of water lost from the grid via this cell (only meaningful for edge cells)
void CSurfaceWater::AddSurfaceWaterLost(double const dAddWater)
{
   m_dCumulativeSurfaceWaterDepthLost += dAddWater;
}

// Returns the total depth of water lost from the grid via this cell (only meaningful for edge cells)
double CSurfaceWater::dGetCumulativeSurfaceWaterLost(void) const
{
   return m_dCumulativeSurfaceWaterDepthLost;
}
#endif

// Initialize flow velocities
void CSurfaceWater::InitializeAllFlowVelocity(void)
{
   m_vFlowVelocity.x =
   m_vFlowVelocity.y =
   m_vDWFlowVelocity.x =
   m_vDWFlowVelocity.y = 0;   
}

// Sets the vector which stores the surface water flow velocity for a cell
void CSurfaceWater::SetFlowVelocity(const C2DVec& vNewVel)
{
   m_vFlowVelocity.x = vNewVel.x;
   m_vFlowVelocity.y = vNewVel.y;
   
   double dTimeStep = m_pSim->dGetTimeStep();
   m_vTotFlowVelocity.x += (vNewVel.x * dTimeStep);
   m_vTotFlowVelocity.y += (vNewVel.y * dTimeStep);
}

// Overloaded version with pointer to C2DVec as param
void CSurfaceWater::SetFlowVelocity(const C2DVec* pvNewVel)
{
   m_vFlowVelocity.x = pvNewVel->x;
   m_vFlowVelocity.y = pvNewVel->y;
   
   double dTimeStep = m_pSim->dGetTimeStep();
   m_vTotFlowVelocity.x += (pvNewVel->x * dTimeStep);
   m_vTotFlowVelocity.y += (pvNewVel->y * dTimeStep);
}

// Overloaded version used when inputting two numbers
void CSurfaceWater::SetFlowVelocity(double const dInx, double const dIny)
{
   m_vFlowVelocity.x = dInx;
   m_vFlowVelocity.y = dIny;
   
   double dTimeStep = m_pSim->dGetTimeStep();
   m_vTotFlowVelocity.x += (dInx * dTimeStep);
   m_vTotFlowVelocity.y += (dIny * dTimeStep);
}

// Returns the surface water flow velocity for a cell as a pointer to an existing vector
C2DVec* CSurfaceWater::vecGetFlowVel(void)
{
   return &m_vFlowVelocity;
}

// Returns the surface water flow velocity for a cell as a scalar
double CSurfaceWater::dGetFlowSpd(void) const
{
   return m_vFlowVelocity.dToScalar();
}


// Returns the scalar running total of surface water flow velocity
double CSurfaceWater::dCumulativeFlowSpeed(void) const
{
   return m_vTotFlowVelocity.dToScalar();
}


// Sets the vector which stores the depth-weighted surface water flow velocity for a cell
void CSurfaceWater::SetDepthWeightedFlowVelocity(const C2DVec& vNewVel)
{
   m_vDWFlowVelocity.x = vNewVel.x;
   m_vDWFlowVelocity.y = vNewVel.y;
   
   double dTimeStep = m_pSim->dGetTimeStep();
   m_vTotFlowVelocity.x += (vNewVel.x * dTimeStep);
   m_vTotFlowVelocity.y += (vNewVel.y * dTimeStep);
}

// Overloaded version with pointer to C2DVec as param
void CSurfaceWater::SetDepthWeightedFlowVelocity(const C2DVec* pvNewVel)
{
   m_vDWFlowVelocity.x = pvNewVel->x;
   m_vDWFlowVelocity.y = pvNewVel->y;
   
   double dTimeStep = m_pSim->dGetTimeStep();
   m_vTotFlowVelocity.x += (pvNewVel->x * dTimeStep);
   m_vTotFlowVelocity.y += (pvNewVel->y * dTimeStep);
}

// Overloaded version used when inputting two numbers
void CSurfaceWater::SetDepthWeightedFlowVelocity(double const dInX, double const dInY)
{
   m_vDWFlowVelocity.x = dInX;
   m_vDWFlowVelocity.y = dInY;
   
   double dTimeStep = m_pSim->dGetTimeStep();
   m_vTotFlowVelocity.x += (dInX * dTimeStep);
   m_vTotFlowVelocity.y += (dInY * dTimeStep);
}

// Returns the depth-weighted surface water flow velocity for a cell as a pointer to an existing vector
C2DVec* CSurfaceWater::vecGetDWFlowVel(void)
{
   return &m_vDWFlowVelocity;
}

// Returns the x component of depth-weighted surface water flow velocity for a cell as a double
double CSurfaceWater::dGetDWFlowVelX(void) const
{
   return m_vDWFlowVelocity.x;
}

// Returns the y component of depth-weighted surface water flow velocity for a cell as a double
double CSurfaceWater::dGetDWFlowVelY(void) const
{
   return m_vDWFlowVelocity.y;
}

// Returns the depth-weighted surface water flow velocity for a cell as a scalar
double CSurfaceWater::dGetDWFlowSpd(void) const
{
   return m_vDWFlowVelocity.dToScalar();
}


// Returns the scalar running total of depth-weighted surface water flow velocity
double CSurfaceWater::dGetTotDWFlowSpd(void) const
{
   return m_vTotDWFlowVelocity.dToScalar();
}


// Sets a cell's surface water streampower
void CSurfaceWater::SetStreamPower(double const dNewStreamPower)
{
   m_dStreamPower = dNewStreamPower;
}

// Returns the surface water streampower value for this cell
double CSurfaceWater::dGetStreamPower(void) const
{
   return m_dStreamPower;
}


// Sets this cell's transport capacity: as with all others, it's a depth equivalent i.e. what depth of sediment could be transported
void CSurfaceWater::SetTransportCapacity(double const dNewTC)
{
   m_dTransCap = dNewTC;
}

// Returns the transport capacity for this cell
double CSurfaceWater::dGetTransportCapacity(void) const
{
   return m_dTransCap;
}


// Sets this cell's friction factor
void CSurfaceWater::SetFrictionFactor(double const dNewFrictionFactor)
{
   m_dFrictionFactor = dNewFrictionFactor;
}

// Returns the friction factor for this cell
double CSurfaceWater::dGetFrictionFactor(void) const
{
   return (m_dFrictionFactor);
}


// Calculates and returns the Reynolds number
double CSurfaceWater::dGetReynolds(double const dNu) const
{
   // Divide by 1e6 because velocity and depth in mm/sec and mm, need them in m/sec and m
   return (m_vFlowVelocity.dToScalar() * m_dSurfaceWaterDepth * 1e-6 / dNu);
}


// Calculates and returns the Froude number
double CSurfaceWater::dGetFroude(double const dG) const
{
   // Divide by 1e3 in each case because in mm/sec and mm, need in m/sec and m
   return ((m_dSurfaceWaterDepth > 0) ? m_vFlowVelocity.dToScalar() * 1e-3 / sqrt(m_dSurfaceWaterDepth * dG * 1e-3) : 0);
}

