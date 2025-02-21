/*=========================================================================================================================================

This is cell_surface_water.cpp: implementations of the RillGrow class used to represent a cell's surface water

Copyright (C) 2025 David Favis-Mortlock

==========================================================================================================================================

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
#include "rg.h"
#include "cell.h"
#include "cell_surface_water.h"

//! Constructor with initialization list
CCellSurfaceWater::CCellSurfaceWater(void)
:
   m_nFlowDirection(DIRECTION_NONE),
   m_nInundationClass(NO_FLOW),
   m_dSurfaceWaterDepth(0),
   m_dTmpSurfaceWaterDepth(0),
   m_dCumulSurfaceWaterDepth(0),
   m_dSurfaceWaterDepthLost(0),
   m_dCumulSurfaceWaterDepthLost(0),
   m_dStreamPower(0),
   m_dTransportCapacity(0),
   m_dFrictionFactor(0)
{
   pCell = NULL;
}

//! Destructor
CCellSurfaceWater::~CCellSurfaceWater(void)
{
}

//! Sets the pointer to the parent cell object
void CCellSurfaceWater::SetParent(CCell* const pParent)
{
   pCell = pParent;
}

//! Initializes overland flow values
void CCellSurfaceWater::InitializeFlow(void)
{
   // m_bFlowThisIter = false;

   m_nFlowDirection = DIRECTION_NONE;
   m_nInundationClass = NO_FLOW;

   m_dSurfaceWaterDepthLost =
   m_dStreamPower =
   m_dTransportCapacity = 0;
}

//! Sets the inundation class for the Lawrence flow speed approach
void CCellSurfaceWater::SetInundation(int const nNewInundation)
{
   m_nInundationClass = nNewInundation;
}

//! Returns the inundation class for the Lawrence flow speed approach
int CCellSurfaceWater::nGetInundation(void) const
{
   return m_nInundationClass;
}

//! Sets the surface water direction
void CCellSurfaceWater::SetFlowDirection(int nNewFlowDir)
{
   m_nFlowDirection = nNewFlowDir;
}

//! Returns the surface water direction
int CCellSurfaceWater::nGetFlowDirection(void) const
{
   return m_nFlowDirection;
}

//! Adds to this cell's surface water depth
void CCellSurfaceWater::AddSurfaceWater(double const dAddDepth)
{
   m_dSurfaceWaterDepth += dAddDepth;
}

//! Removes from this cell's surface water depth; if there is insufficient water, then the function returns false and sets the parameter to the depth actually removed
void CCellSurfaceWater::RemoveSurfaceWater(double& dRemoveDepth)
{
   if (dRemoveDepth > m_dSurfaceWaterDepth)
   {
      dRemoveDepth = m_dSurfaceWaterDepth;
      this->SetSurfaceWaterZero();
   }
   else
   {
      m_dSurfaceWaterDepth -= dRemoveDepth;
   }
}

//! Sets this cell's surface water depth to zero, also zeros flow velocities
void CCellSurfaceWater::SetSurfaceWaterZero(void)
{
   m_dSurfaceWaterDepth = 0;
   this->ZeroAllFlowVelocity();
}

//! Returns the depth of surface water (in mm) on this cell
double CCellSurfaceWater::dGetSurfaceWaterDepth(void) const
{
   return m_dSurfaceWaterDepth;
}

//! Returns true if surface water depth > 0, false otherwise
bool CCellSurfaceWater::bIsWet(void) const
{
   return ((m_dSurfaceWaterDepth > 0) ? true : false);
}

//! Gets the cumulative surface water depth
double CCellSurfaceWater::dGetCumulSurfaceWater(void) const
{
   return m_dCumulSurfaceWaterDepth;
}

//! Adds to the cell's cumulative surface water depth
void CCellSurfaceWater::AddToCumulSurfaceWater(double const dWaterDepth)
{
   m_dCumulSurfaceWaterDepth += dWaterDepth;
}

//! Initializes the temporary surface water depth value
void CCellSurfaceWater::InitTmpSurfaceWater(void)
{
   m_dTmpSurfaceWaterDepth = m_dSurfaceWaterDepth;
}

//! Adds to the temporary surface water depth value
void CCellSurfaceWater::AddTmpSurfaceWater(double const dAddDepth)
{
   m_dTmpSurfaceWaterDepth += dAddDepth;
}

//! Removes from this cell's temporary surface water depth; if there is insufficient water, then the parameter is set to the depth actually removed
void CCellSurfaceWater::RemoveTmpSurfaceWater(double& dRemoveDepth)
{
   if (dRemoveDepth > m_dSurfaceWaterDepth)
      dRemoveDepth = m_dSurfaceWaterDepth;      // Since always, m_dTmpSurfaceWaterDepth >= m_dSurfaceWaterDepth here

   m_dTmpSurfaceWaterDepth -= dRemoveDepth;
}

//! Copies the from the temporary surface water depth value
void CCellSurfaceWater::FinishTmpSurfaceWater(void)
{
   m_dSurfaceWaterDepth = m_dTmpSurfaceWaterDepth;
}

//! Increments the total depth of water lost from the grid via this cell (only meaningful for edge cells)
void CCellSurfaceWater::AddSurfaceWaterLost(double const dAddWater)
{
   m_dSurfaceWaterDepthLost += dAddWater;
   m_dCumulSurfaceWaterDepthLost += dAddWater;
}

//! Returns the this-iteration depth of water lost from the grid via this cell (only meaningful for edge cells)
double CCellSurfaceWater::dGetSurfaceWaterLost(void) const
{
   return m_dSurfaceWaterDepthLost;
}

//! Returns the cumulative depth of water lost from the grid via this cell (only meaningful for edge cells)
double CCellSurfaceWater::dGetCumulSurfaceWaterLost(void) const
{
   return m_dCumulSurfaceWaterDepthLost;
}

//! Initializes overland flow velocities
void CCellSurfaceWater::InitializeAllFlowVelocity(void)
{
   // Need to make this small +ve or -ve random to seed Reynolds' number calculations if using Darcy-Weisbach/Reynolds' flow speed calcs. Could be zero for other approaches, but does no harm
   m_vFlowVelocity.x = m_pSim->dGetRandGaussian() * INIT_MAX_SPEED_GUESS;
   m_vFlowVelocity.y = m_pSim->dGetRandGaussian() * INIT_MAX_SPEED_GUESS;

   // Set these to zero
   m_vDWFlowVelocity.x =
   m_vDWFlowVelocity.y = 0;
}

//! Zeroes all overland flow velocities
void CCellSurfaceWater::ZeroAllFlowVelocity(void)
{
   m_vFlowVelocity.x =
   m_vFlowVelocity.y =
   m_vDWFlowVelocity.x =
   m_vDWFlowVelocity.y = 0;
}

//! Sets the vector flow velocity for this cell
void CCellSurfaceWater::SetFlowVelocity(const C2DVec& vNewVel)
{
   m_vFlowVelocity.x = vNewVel.x;
   m_vFlowVelocity.y = vNewVel.y;

   double dTimeStep = m_pSim->dGetTimeStep();
   m_vCumulFlowVelocity.x += (vNewVel.x * dTimeStep);
   m_vCumulFlowVelocity.y += (vNewVel.y * dTimeStep);
}

//! Sets the vector flow velocity for this cell (overloaded version with pointer to C2DVec as parameter)
void CCellSurfaceWater::SetFlowVelocity(const C2DVec* pvNewVel)
{
   m_vFlowVelocity.x = pvNewVel->x;
   m_vFlowVelocity.y = pvNewVel->y;

   double dTimeStep = m_pSim->dGetTimeStep();
   m_vCumulFlowVelocity.x += (pvNewVel->x * dTimeStep);
   m_vCumulFlowVelocity.y += (pvNewVel->y * dTimeStep);
}

//! Sets the vector flow velocity for this cell (overloaded version used when inputting two numbers)
void CCellSurfaceWater::SetFlowVelocity(double const dInx, double const dIny)
{
   m_vFlowVelocity.x = dInx;
   m_vFlowVelocity.y = dIny;

   double dTimeStep = m_pSim->dGetTimeStep();
   m_vCumulFlowVelocity.x += (dInx * dTimeStep);
   m_vCumulFlowVelocity.y += (dIny * dTimeStep);
}

//! Returns the surface water velocity for this cell as a pointer to an existing vector
C2DVec* CCellSurfaceWater::vecGetFlowVel(void)
{
   return &m_vFlowVelocity;
}

//! Returns the surface water velocity for this cell as a scalar
double CCellSurfaceWater::dGetFlowSpd(void) const
{
   return m_vFlowVelocity.dToScalar();
}

//! Returns the cumulative total of surface water scalar velocity
double CCellSurfaceWater::dCumulFlowSpeed(void) const
{
   return m_vCumulFlowVelocity.dToScalar();
}

//! Sets the vector which stores the depth-weighted surface water velocity for this cell
void CCellSurfaceWater::SetDepthWeightedFlowVelocity(const C2DVec& vNewVel)
{
   m_vDWFlowVelocity.x = vNewVel.x;
   m_vDWFlowVelocity.y = vNewVel.y;

   double dTimeStep = m_pSim->dGetTimeStep();
   m_vCumulFlowVelocity.x += (vNewVel.x * dTimeStep);
   m_vCumulFlowVelocity.y += (vNewVel.y * dTimeStep);
}

//! Sets the vector which stores the depth-weighted surface water velocity for this cell (overloaded version with pointer to C2DVec as parameter)
void CCellSurfaceWater::SetDepthWeightedFlowVelocity(const C2DVec* pvNewVel)
{
   m_vDWFlowVelocity.x = pvNewVel->x;
   m_vDWFlowVelocity.y = pvNewVel->y;

   double dTimeStep = m_pSim->dGetTimeStep();
   m_vCumulFlowVelocity.x += (pvNewVel->x * dTimeStep);
   m_vCumulFlowVelocity.y += (pvNewVel->y * dTimeStep);
}

//! Sets the vector which stores the depth-weighted surface water velocity for this cell (overloaded version used when inputting two numbers)
void CCellSurfaceWater::SetDepthWeightedFlowVelocity(double const dInX, double const dInY)
{
   m_vDWFlowVelocity.x = dInX;
   m_vDWFlowVelocity.y = dInY;

   double dTimeStep = m_pSim->dGetTimeStep();
   m_vCumulFlowVelocity.x += (dInX * dTimeStep);
   m_vCumulFlowVelocity.y += (dInY * dTimeStep);
}

//! Returns the depth-weighted surface water velocity for this cell as a pointer to an existing vector
C2DVec* CCellSurfaceWater::vecGetDWFlowVel(void)
{
   return &m_vDWFlowVelocity;
}

//! Returns the x component of depth-weighted surface water velocity for this cell
double CCellSurfaceWater::dGetDWFlowVelX(void) const
{
   return m_vDWFlowVelocity.x;
}

//! Returns the y component of depth-weighted surface water velocity for this cell
double CCellSurfaceWater::dGetDWFlowVelY(void) const
{
   return m_vDWFlowVelocity.y;
}

//! Returns the depth-weighted surface water velocity for this cell as a scalar
double CCellSurfaceWater::dGetDWFlowSpd(void) const
{
   return m_vDWFlowVelocity.dToScalar();
}

//! Returns the cumulative total of depth-weighted surface water velocity as a scalar
double CCellSurfaceWater::dGetCumulDWFlowSpd(void) const
{
   return m_vCumulDWFlowVelocity.dToScalar();
}

// Sets this cell's surface water streampower
void CCellSurfaceWater::SetStreamPower(double const dNewStreamPower)
{
   m_dStreamPower = dNewStreamPower;
}

//! Returns the surface water streampower value for this cell
double CCellSurfaceWater::dGetStreamPower(void) const
{
   return m_dStreamPower;
}

//! Sets this cell's transport capacity (is a depth equivalent i.e. what depth of sediment could be transported)
void CCellSurfaceWater::SetTransportCapacity(double const dNewTC)
{
   m_dTransportCapacity = dNewTC;
}

//! Returns the transport capacity for this cell
double CCellSurfaceWater::dGetTransportCapacity(void) const
{
   return m_dTransportCapacity;
}

//! Sets this cell's friction factor
void CCellSurfaceWater::SetFrictionFactor(double const dNewFrictionFactor)
{
   m_dFrictionFactor = dNewFrictionFactor;
}

//! Returns the friction factor for this cell
double CCellSurfaceWater::dGetFrictionFactor(void) const
{
   return (m_dFrictionFactor);
}

//! Calculates and returns the Froude number
double CCellSurfaceWater::dGetFroude(double const dG) const
{
   // Divide by 1e3 in each case because in mm/sec and mm, need them in m/sec and m
   return ((m_dSurfaceWaterDepth > 0) ? m_vFlowVelocity.dToScalar() * 1e-3 / sqrt(m_dSurfaceWaterDepth * dG * 1e-3) : 0);
}
