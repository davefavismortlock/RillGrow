/*=========================================================================================================================================

 This is cell.cpp: the implementation of the class used to represent each soil cell object

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
#include "2d_vec.h"


CCell::CCell(void)
:
   m_bHadHeadcutRetreat(false),
   m_nEdgeCell(DIRECTION_NONE),
   m_dBasementElevationation(0),
   m_dInitialSoilSurfaceElevation(0)
{
   m_Soil.SetParent(this);
   m_SurfaceWater.SetParent(this);
   m_SoilWater.SetParent(this);
   m_RainAndRunOn.SetParent(this);
   m_SedLoad.SetParent(this);

   for (int n = 0; n < 8; n++)
   {
      m_dStoredRetreat[n] = 0;
   }
}

CCell::~CCell(void)
{
}


void CCell::SetEdgeCell(int const nDirection)
{
   m_nEdgeCell = nDirection;
}

int CCell::nGetEdge(void) const
{
   return m_nEdgeCell;
}

bool CCell::bIsEdgeCell(void) const
{
   return (m_nEdgeCell != DIRECTION_NONE);
}

// Returns true if this cell's elevation is a missing value, is needed for irregularly-shaped DEMs
bool CCell::bIsMissingValue(void) const
{
   if (m_dInitialSoilSurfaceElevation == m_pSim->dGetMissingValue())
      return true;

   return false;
}

bool CCell::bIsMissingOrEdge(void) const
{
   if ((m_dInitialSoilSurfaceElevation == m_pSim->dGetMissingValue()) || (m_nEdgeCell != DIRECTION_NONE))
      return true;

   return false;
}


// Set's this cell's basement elevation
void CCell::SetBasementElevation(double const dElev)
{
   m_dBasementElevationation = dElev;
}

// Returns this cell's basement elevation
double CCell::dGetBasementElevation(void) const
{
   return m_dBasementElevationation;
}


// Sets this cell's initial soil surface elevation. Note that it must be called only at the start of the simulation (or when adjusting the edge cells during the simulation), because it doesn't calculate either erosion or deposition
void CCell::SetInitialSoilSurfaceElevation(double const dNewElev)
{
   m_dInitialSoilSurfaceElevation = dNewElev;
}

// Returns this cell's initial elevation
double CCell::dGetInitialSoilSurfaceElevation(void) const
{
   return m_dInitialSoilSurfaceElevation;
}

// Returns the elevation of this cell's top surface, which could be the water surface if the cell is wet, or the soil surface if dry
double CCell::dGetTopElevation(void)
{
   return (m_Soil.dGetSoilSurfaceElevation() + m_SurfaceWater.dGetSurfaceWaterDepth());
}

// This sets a new value for the soil surface elevation. Since it destroys mass balance, it must only be called during the ugly-but-necessary AdjustUnboundedEdges() routine
void CCell::SetSoilSurfaceElevation(double const dNewElev)
{
   double
      dCurrentElev = m_Soil.dGetSoilSurfaceElevation(),
      dDiff = dNewElev - dCurrentElev;

   m_Soil.ChangeTopLayerThickness(dDiff);
}

// Returns the elevation of this cell's soil surface
double CCell::dGetSoilSurfaceElevation(void)
{
   return (m_Soil.dGetSoilSurfaceElevation());
}


// Sets this cell's retreat for one of the eight directions
double CCell::dGetStoredRetreat(int const nDirection) const
{
   // Note does not check for valid nDirection
   return m_dStoredRetreat[nDirection];
}

void CCell::SetStoredRetreat(int const nDirection, double const dRet)
{
   // Note does not check for valid nDirection
   m_dStoredRetreat[nDirection] = dRet;
}

void CCell::AddToStoredRetreat(int const nDirection, double const dInc)
{
   // Note does not check for valid nDirection
   m_dStoredRetreat[nDirection] += dInc;
}

bool CCell::bIsReadyForHeadcutRetreat(int const nDirection, double const dSize) const
{
   if (m_dStoredRetreat[nDirection] >= dSize)
      return true;

   return false;
}

void CCell::SetHasHadHeadcutRetreat(void)
{
   m_bHadHeadcutRetreat = true;
}

bool CCell::bHasHadHeadcutRetreat(void) const
{
   return m_bHadHeadcutRetreat;
}


// Returns a pointer to the CCellSoil object
CCellSoil* CCell::pGetSoil(void)
{
   return &m_Soil;
}

// Returns a pointer to the CCellRainAndRunon object
CCellRainAndRunon* CCell::pGetRainAndRunon(void)
{
   return &m_RainAndRunOn;
}

CCellSurfaceWater* CCell::pGetSurfaceWater(void)
{
   return &m_SurfaceWater;
}

// Returns a pointer to the CellSediment object
CCellSedimentLoad* CCell::pGetSedLoad(void)
{
   return &m_SedLoad;
}

// Returns a pointer to the CCellSubsurfaceWater object
CCellSubsurfaceWater* CCell::pGetSoilWater(void)
{
   return &m_SoilWater;
}


// Gets values for end-of-iteration totals, and then initializes per-operation values ready for the next iteration's flow routing, splash redistribution, and slumping/toppling
void CCell::CalcIterTotalsAndInit(bool& bIsWet, double& dRain, double& dRunOn, double& dSurfaceWaterDepth, double& dSurfaceWaterLost, double& dClaySedLoad, double& dSiltSedLoad, double& dSandSedLoad, double& dClayFlowDetach, double& dSiltFlowDetach, double& dSandFlowDetach, double& dClayFlowDeposit, double& dSiltFlowDeposit, double& dSandFlowDeposit, double& dClaySplashDetach, double& dSiltSplashDetach, double& dSandSplashDetach, double& dClaySplashDeposit, double& dSiltSplashDeposit, double& dSandSplashDeposit, double& dClaySplashToSedLoad, double& dSiltSplashToSedLoad, double& dSandSplashToSedLoad, double& dInfiltration, double& dExfiltration, double& dClayInfiltDeposit, double& dSiltInfiltDeposit, double& dSandInfiltDeposit, bool const bSlump)
{
   dRain = m_RainAndRunOn.dGetRain();
   dRunOn = m_RainAndRunOn.dGetRunOn();

   if (m_SurfaceWater.bIsWet())
   {
      bIsWet = true;
      dSurfaceWaterDepth = m_SurfaceWater.dGetSurfaceWaterDepth();

      dClaySedLoad = m_SedLoad.dGetClaySedLoad();
      dSiltSedLoad = m_SedLoad.dGetSiltSedLoad();
      dSandSedLoad = m_SedLoad.dGetSandSedLoad();
   }

   dSurfaceWaterLost = m_SurfaceWater.dGetSurfaceWaterLost();

   dClayFlowDetach = m_Soil.dGetClayFlowDetach();
   dSiltFlowDetach = m_Soil.dGetSiltFlowDetach();
   dSandFlowDetach = m_Soil.dGetSandFlowDetach();

   dClayFlowDeposit = m_Soil.dGetClayFlowDeposit();
   dSiltFlowDeposit = m_Soil.dGetSiltFlowDeposit();
   dSandFlowDeposit = m_Soil.dGetSandFlowDeposit();

   dClaySplashDetach = m_Soil.dGetClaySplashDetach();
   dSiltSplashDetach = m_Soil.dGetSiltSplashDetach();
   dSandSplashDetach = m_Soil.dGetSandSplashDetach();

   dClaySplashDeposit = m_Soil.dGetClaySplashDeposit();
   dSiltSplashDeposit = m_Soil.dGetSiltSplashDeposit();
   dSandSplashDeposit = m_Soil.dGetSandSplashDeposit();

   dClaySplashToSedLoad = m_SedLoad.dGetClaySplashSedLoad();
   dSiltSplashToSedLoad = m_SedLoad.dGetSiltSplashSedLoad();
   dSandSplashToSedLoad = m_SedLoad.dGetSandSplashSedLoad();

   dInfiltration = m_SoilWater.dGetInfiltration();
   dExfiltration = m_SoilWater.dGetExfiltration();

   dClayInfiltDeposit = m_Soil.dGetClayInfiltDeposit();
   dSiltInfiltDeposit = m_Soil.dGetSiltInfiltDeposit();
   dSandInfiltDeposit = m_Soil.dGetSandInfiltDeposit();

   // Now initialize, ready for the next iteration
   m_RainAndRunOn.InitializeRainAndRunon();
   m_SurfaceWater.InitializeFlow();
   m_SoilWater.InitializeInfiltration();
   m_Soil.InitializeDetachAndDeposit(bSlump);
}

