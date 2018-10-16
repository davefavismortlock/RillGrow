/*=========================================================================================================================================

 This is cell_rain_and_runon.cpp: implementations of the RillGrow class used to represent incident water i.e. rain and runon

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
#include "cell_rain_and_runon.h"


CRainAndRunon::CRainAndRunon(void)
:
   m_dRain(0),
   m_dTotRain(0),
   m_dRunOn(0),
   m_dTotRunOn(0),
   m_dRainVarM(1)
{   
   m_pCell = NULL;
}

CRainAndRunon::~CRainAndRunon(void)
{
}


void CRainAndRunon::SetParent(CCell* const pParent)
{
   m_pCell = pParent;
}


// Sets this-iteration rainfall and runon to zero for this cell
void CRainAndRunon::InitializeRainAndRunon(void)
{
   m_dRain = 
   m_dRunOn = 0;
}

// Increments rainfall for this cell, also adds to the surface water in the cell
void CRainAndRunon::dAddRain(double const dRain)
{
   // Add rain to this cell
   m_dRain    += dRain;
   m_dTotRain += dRain;
   
   // Was the cell previously dry?
   if (! m_pCell->pGetSurfaceWater()->bIsWet())
   {
      // It was, so increment the number of wet cells
      m_pSim->IncrThisIterNumWetCells();
      
      // And initialize flow velocity on this cell
      m_pCell->pGetSurfaceWater()->InitializeAllFlowVelocity();
      
      // Initialize sediment load
      m_pCell->pGetSediment()->InitializeAllSizeSedimentLoad();
   }
   
   // Increase the depth of surface water
   m_pCell->pGetSurfaceWater()->ChangeSurfaceWater(dRain);
}

// Returns the this-timestep rainfall on this cell
double CRainAndRunon::dGetRain(void) const
{
   return m_dRain;
}


// Returns the total rainfall on this cell
double CRainAndRunon::dGetTotRain(void) const
{
   return m_dTotRain;
}


// Sets this cell's rainfall variation multiplier
void CRainAndRunon::SetRainVarM(double const dNewVar)
{
   m_dRainVarM = dNewVar;
}

// Returns this cell's rainfall variation multiplier
double CRainAndRunon::dGetRainVarM(void) const
{
   return m_dRainVarM;
}


// Increments this cell's runon, also adds to the surface water on the cell
void CRainAndRunon::AddRunOn(double const dRunOn)
{
   m_dRunOn += dRunOn;
   m_dTotRunOn += dRunOn;
   
   // Was the cell previously dry?
   if (! m_pCell->pGetSurfaceWater()->bIsWet())
   {   
      // It was, so increment this this-iteration count of wet cells
      m_pSim->IncrThisIterNumWetCells();
      
      // And initialize flow velocity for this cell
      m_pCell->pGetSurfaceWater()->InitializeAllFlowVelocity();
      
      // Initialize sediment load
      m_pCell->pGetSediment()->InitializeAllSizeSedimentLoad();
   }
   
   // Increase the depth of surface water
   m_pCell->pGetSurfaceWater()->ChangeSurfaceWater(dRunOn);
}

// Returns the this-timestep run-on on this cell
double CRainAndRunon::dGetRunOn(void) const
{
   return m_dRunOn;
}

// Returns the total run-on on this cell
double CRainAndRunon::dGetTotRunOn(void) const
{
   return m_dTotRunOn;
}


