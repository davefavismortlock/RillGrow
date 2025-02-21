/*=========================================================================================================================================

This is cell_rain_and_runon.cpp: implementations of the RillGrow class used to represent incident water (rain and runon)

Copyright (C) 2025 David Favis-Mortlock

==========================================================================================================================================

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
#include "rg.h"
#include "cell.h"
#include "cell_rain_and_runon.h"

//! Constructor with intialization list
CCellRainAndRunon::CCellRainAndRunon(void)
:
   m_dRain(0),
   m_dCumulRain(0),
   m_dRunOn(0),
   m_dCumulRunOn(0),
   m_dRainVarM(1)
{
   pCell = NULL;
}

//! Destructor
CCellRainAndRunon::~CCellRainAndRunon(void)
{
}

//! Sets the pointer to the parent cell object
void CCellRainAndRunon::SetParent(CCell* const pParent)
{
   pCell = pParent;
}


//! Sets this-iteration rainfall and runon to zero for this cell
void CCellRainAndRunon::InitializeRainAndRunon(void)
{
   m_dRain =
   m_dRunOn = 0;
}

//! Adds to this-iteration rainfall on this cell, also adds to the surface water on the cell
void CCellRainAndRunon::AddRain(double const dRain)
{
   // Add rain to this cell
   m_dRain      += dRain;
   m_dCumulRain += dRain;

   // Was the cell previously dry?
   if (! pCell->pGetSurfaceWater()->bIsWet())
   {
      // It was, so initialize flow velocities on this cell
      pCell->pGetSurfaceWater()->InitializeAllFlowVelocity();

      // Initialize sediment load
      pCell->pGetSedLoad()->InitializeAllSizeSedLoad();
   }

   // Increase the depth of surface water on the cell
   pCell->pGetSurfaceWater()->AddSurfaceWater(dRain);
}

//! Returns the this-timestep rainfall on this cell
double CCellRainAndRunon::dGetRain(void) const
{
   return m_dRain;
}

//! Returns the cumulative rainfall on this cell
double CCellRainAndRunon::dGetCumulRain(void) const
{
   return m_dCumulRain;
}

//! Sets this cell's rainfall variation multiplier
void CCellRainAndRunon::SetRainVarM(double const dNewVar)
{
   m_dRainVarM = dNewVar;
}

//! Returns this cell's rainfall variation multiplier
double CCellRainAndRunon::dGetRainVarM(void) const
{
   return m_dRainVarM;
}

//! Increments this cell's runon, also adds to the surface water on the cell
void CCellRainAndRunon::AddRunOn(double const dRunOn)
{
   m_dRunOn += dRunOn;
   m_dCumulRunOn += dRunOn;

   // Was the cell previously dry?
   if (! pCell->pGetSurfaceWater()->bIsWet())
   {
      // It was, so initialize flow velocities for this cell
      pCell->pGetSurfaceWater()->InitializeAllFlowVelocity();

      // Initialize sediment load
      pCell->pGetSedLoad()->InitializeAllSizeSedLoad();
   }

   // Increase the depth of surface water
   pCell->pGetSurfaceWater()->AddSurfaceWater(dRunOn);
}

//! Returns the this-timestep run-on on this cell
double CCellRainAndRunon::dGetRunOn(void) const
{
   return m_dRunOn;
}

//! Returns the cumulative run-on on this cell
double CCellRainAndRunon::dGetCumulRunOn(void) const
{
   return m_dCumulRunOn;
}


