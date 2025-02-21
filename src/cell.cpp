/*!
\file cell.cpp
\brief Implementation of the RillGrow class used to represent each soil cell object
\details TODO
\author David Favis-Mortlock
\date 2025
\copyright GNU General Public License
*/

/*=========================================================================================================================================
This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
=========================================================================================================================================*/
#include "rg.h"
#include "simulation.h"
#include "cell.h"
#include "2d_vec.h"

//! Constructor with initialization list
CCell::CCell(void)
:
   m_bHadHeadcutRetreat(false),
   m_nEdgeCell(DIRECTION_NONE),
   m_dBasementElev(NODATA),
   m_dInitialSoilSurfaceElev(NODATA)
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

//! Destructor
CCell::~CCell(void)
{
}

//! Set this cell's edge cell value
void CCell::SetEdgeCell(int const nDirection)
{
   m_nEdgeCell = nDirection;
}

//! Return the edge cell value for this cell
int CCell::nGetEdge(void) const
{
   return m_nEdgeCell;
}

//! Return true if this is an edge cell
bool CCell::bIsEdgeCell(void) const
{
   return (m_nEdgeCell != DIRECTION_NONE);
}

//! Return true if this cell's elevation is a missing value. This is needed for irregularly-shaped DEMs
bool CCell::bIsMissingValue(void) const
{
   if (bFpEQ(m_dInitialSoilSurfaceElev, m_pSim->dGetMissingValue(), TOLERANCE))
      return true;

   return false;
}

// //! Return true if this cell's elevation is a missing value or if this is an edge cell
// bool CCell::bIsMissingOrEdge(void) const
// {
//    if ((bFpEQ(m_dInitialSoilSurfaceElev, m_pSim->dGetMissingValue(), TOLERANCE)) || (m_nEdgeCell != DIRECTION_NONE))
//       return true;
//
//    return false;
// }

//! Sets this cell's basement elevation
void CCell::SetBasementElevation(double const dElev)
{
   m_dBasementElev = dElev;
}

//! Returns this cell's basement elevation
double CCell::dGetBasementElevation(void) const
{
   return m_dBasementElev;
}

//! Sets this cell's initial soil surface elevation. Must be called only at the start of the simulation
void CCell::SetInitialSoilSurfaceElevation(double const dNewElev)
{
   m_dInitialSoilSurfaceElev = dNewElev;
}

//! Returns this cell's initial elevation (could be a missing value)
double CCell::dGetInitialSoilSurfaceElevation(void) const
{
   return m_dInitialSoilSurfaceElev;
}

//! Returns the elevation of this cell's top surface, which could be the water surface if the cell is wet, or the soil surface if dry
double CCell::dGetTopElevation(void) const
{
   return (m_Soil.dGetSoilSurfaceElevation() + m_SurfaceWater.dGetSurfaceWaterDepth());
}

// //! Sets this cell's retreat for one of the eight directions
// double CCell::dGetStoredRetreat(int const nDirection) const
// {
//    // Note does not check for valid nDirection
//    return m_dStoredRetreat[nDirection];
// }

//! Sets this cell's stored headcut retreat
void CCell::SetStoredRetreat(int const nDirection, double const dRet)
{
   // Note does not check for valid nDirection
   m_dStoredRetreat[nDirection] = dRet;
}

//! Adds to this cell's stored headcut retreat
void CCell::AddToStoredRetreat(int const nDirection, double const dInc)
{
   // Note does not check for valid nDirection
   m_dStoredRetreat[nDirection] += dInc;
}

//! Returns true if this cell is ready to move headcut retreat to the next upstream cell
bool CCell::bIsReadyForHeadcutRetreat(int const nDirection, double const dSize) const
{
   if (m_dStoredRetreat[nDirection] >= dSize)
      return true;

   return false;
}

//! Sets the "has had headcut retreat" flag to true
void CCell::SetHasHadHeadcutRetreat(void)
{
   m_bHadHeadcutRetreat = true;
}

//! Returns the value of the "has had headcut retreat" flag
bool CCell::bHasHadHeadcutRetreat(void) const
{
   return m_bHadHeadcutRetreat;
}

//! Returns a pointer to the CCellSoil object
CCellSoil* CCell::pGetSoil(void)
{
   return &m_Soil;
}

//! Returns a pointer to the CCellRainAndRunon object
CCellRainAndRunon* CCell::pGetRainAndRunon(void)
{
   return &m_RainAndRunOn;
}

//! Returns a pointer to the CCellSurfaceWater object
CCellSurfaceWater* CCell::pGetSurfaceWater(void)
{
   return &m_SurfaceWater;
}

//! Returns a pointer to the CCellSedimentLoad object
CCellSedimentLoad* CCell::pGetSedLoad(void)
{
   return &m_SedLoad;
}

//! Returns a pointer to the CCellSubsurfaceWater object
CCellSubsurfaceWater* CCell::pGetSoilWater(void)
{
   return &m_SoilWater;
}

//! Adds this cell's values foe elevation and soil water content to the to the whole-grid pre-simulation values
void CCell::AddToPreSimulationValues(void) const
{
   m_pSim->GetPreSimulationValues(m_Soil.dGetSoilSurfaceElevation(), m_SoilWater.dGetAllSoilWater());
}

//! Gets this cell's values for end-of-iteration totals
void CCell::GetEndOfIterValues(void) const
{
   // Add to these whole-grid values for water
   m_pSim->AddToEndOfIterRain(m_RainAndRunOn.dGetRain());
   m_pSim->AddToEndOfIterRunon(m_RainAndRunOn.dGetRunOn());

   m_pSim->AddToEndOfIterStoredSoilWater(m_SoilWater.dGetAllSoilWater());
   m_pSim->AddToEndOfIterInfiltration(m_SoilWater.dGetThisIterInfiltration());
   m_pSim->AddToEndOfIterExfiltration(m_SoilWater.dGetExfiltration());
   m_pSim->AddToEndOfIterSurfaceWaterLost(m_SurfaceWater.dGetSurfaceWaterLost());

   if (m_SurfaceWater.bIsWet())
   {
      // Increment the whole-grid count of wet cells
      m_pSim->IncrNumWetCells();

      // Add to the whole-grid stored surface water value
      double dWaterDepth = m_SurfaceWater.dGetSurfaceWaterDepth();
      m_pSim->AddToEndOfIterStoredSurfaceWater(dWaterDepth);

      // Not sure why we have to cast away constness here...
      const_cast<CCell*>(this)->pGetSurfaceWater()->AddToCumulSurfaceWater(dWaterDepth);

      // Add to the whole-grid this-iteration sediment load values
      m_pSim->AddToEndOfIterClaySedLoad(m_SedLoad.dGetLastIterClaySedLoad() + m_SedLoad.dGetClaySplashSedLoad() + m_SedLoad.dGetClayFlowSedLoad() + m_SedLoad.dGetClaySlumpSedLoad() + m_SedLoad.dGetClayToppleSedLoad() + m_SedLoad.dGetClayHeadcutRetreatSedLoad() - m_SedLoad.dGetClaySedLoadRemoved());

      m_pSim->AddToEndOfIterSiltSedLoad(m_SedLoad.dGetLastIterSiltSedLoad() + m_SedLoad.dGetSiltSplashSedLoad() + m_SedLoad.dGetSiltFlowSedLoad() + m_SedLoad.dGetSiltSlumpSedLoad() + m_SedLoad.dGetSiltToppleSedLoad() + m_SedLoad.dGetSiltHeadcutRetreatSedLoad() - m_SedLoad.dGetSiltSedLoadRemoved());

      m_pSim->AddToEndOfIterSandSedLoad(m_SedLoad.dGetLastIterSandSedLoad() + m_SedLoad.dGetSandSplashSedLoad() + m_SedLoad.dGetSandFlowSedLoad() + m_SedLoad.dGetSandSlumpSedLoad() + m_SedLoad.dGetSandToppleSedLoad() + m_SedLoad.dGetSandHeadcutRetreatSedLoad() - m_SedLoad.dGetSandSedLoadRemoved());
   }

   // Add to this whole-grid value for elevation
   m_pSim->AddToEndOfIterTotalElev(m_Soil.dGetSoilSurfaceElevation());

   // Add to these whole-grid values for sediment detachment
   m_pSim->AddToEndOfIterClayFlowDetach(m_Soil.dGetClayFlowDetach());
   m_pSim->AddToEndOfIterSiltFlowDetach(m_Soil.dGetSiltFlowDetach());
   m_pSim->AddToEndOfIterSandFlowDetach(m_Soil.dGetSandFlowDetach());

   m_pSim->AddToEndOfIterClaySplashDetach(m_Soil.dGetClaySplashDetach());
   m_pSim->AddToEndOfIterSiltSplashDetach(m_Soil.dGetSiltSplashDetach());
   m_pSim->AddToEndOfIterSandSplashDetach(m_Soil.dGetSandSplashDetach());

   m_pSim->AddToEndOfIterClaySlumpDetach(m_Soil.dGetClaySlumpDetach());
   m_pSim->AddToEndOfIterSiltSlumpDetach(m_Soil.dGetSiltSlumpDetach());
   m_pSim->AddToEndOfIterSandSlumpDetach(m_Soil.dGetSandSlumpDetach());

   m_pSim->AddToEndOfIterClayToppleDetach(m_Soil.dGetClayToppleDetach());
   m_pSim->AddToEndOfIterSiltToppleDetach(m_Soil.dGetSiltToppleDetach());
   m_pSim->AddToEndOfIterSandToppleDetach(m_Soil.dGetSandToppleDetach());

   // Add to these whole-grid values for sediment deposition
   m_pSim->AddToEndOfIterClayFlowDeposit(m_Soil.dGetClayFlowDeposit());
   m_pSim->AddToEndOfIterSiltFlowDeposit(m_Soil.dGetSiltFlowDeposit());
   m_pSim->AddToEndOfIterSandFlowDeposit(m_Soil.dGetSandFlowDeposit());

   m_pSim->AddToEndOfIterClaySplashDeposit(m_Soil.dGetClaySplashDeposit());
   m_pSim->AddToEndOfIterSiltSplashDeposit(m_Soil.dGetSiltSplashDeposit());
   m_pSim->AddToEndOfIterSandSplashDeposit(m_Soil.dGetSandSplashDeposit());

   m_pSim->AddToEndOfIterClaySlumpDeposit(m_Soil.dGetClaySlumpDeposit());
   m_pSim->AddToEndOfIterSiltSlumpDeposit(m_Soil.dGetSiltSlumpDeposit());
   m_pSim->AddToEndOfIterSandSlumpDeposit(m_Soil.dGetSandSlumpDeposit());

   m_pSim->AddToEndOfIterClayToppleDeposit(m_Soil.dGetClayToppleDeposit());
   m_pSim->AddToEndOfIterSiltToppleDeposit(m_Soil.dGetSiltToppleDeposit());
   m_pSim->AddToEndOfIterSandToppleDeposit(m_Soil.dGetSandToppleDeposit());

   m_pSim->AddToEndOfIterClayInfiltDeposit(m_Soil.dGetClayInfiltDeposit());
   m_pSim->AddToEndOfIterSiltInfiltDeposit(m_Soil.dGetSiltInfiltDeposit());
   m_pSim->AddToEndOfIterSandInfiltDeposit(m_Soil.dGetSandInfiltDeposit());

   m_pSim->AddToEndOfIterSedimentOffEdge(m_SedLoad.dGetClaySedOffEdge(), m_SedLoad.dGetSiltSedOffEdge(), m_SedLoad.dGetSandSedOffEdge());
   m_pSim->AddToEndOfIterClaySplashOffEdge(m_Soil.dGetClaySplashOffEdge());
   m_pSim->AddToEndOfIterSiltSplashOffEdge(m_Soil.dGetSiltSplashOffEdge());
   m_pSim->AddToEndOfIterSandSplashOffEdge(m_Soil.dGetSandSplashOffEdge());

   // Calculate detachment due to all processes, for each size class, for this cell
   double dTotClayDetach = m_Soil.dGetClayFlowDetach() + m_Soil.dGetClaySplashDetach() + m_Soil.dGetClaySlumpDetach() + m_Soil.dGetClayToppleDetach() + m_Soil.dGetClayHeadcutRetreatDetach();
   double dTotSiltDetach = m_Soil.dGetSiltFlowDetach() + m_Soil.dGetSiltSplashDetach() + m_Soil.dGetSiltSlumpDetach() + m_Soil.dGetSiltToppleDetach() + m_Soil.dGetSiltHeadcutRetreatDetach();
   double dTotSandDetach = m_Soil.dGetSandFlowDetach() + m_Soil.dGetSandSplashDetach() + m_Soil.dGetSandSlumpDetach() + m_Soil.dGetSandToppleDetach() + m_Soil.dGetSandHeadcutRetreatDetach();

   // Calculate deposition due to all processes, for each size class, for this cell
   double dTotClayDeposit = m_Soil.dGetClayFlowDeposit() + m_Soil.dGetClaySplashDeposit() + m_Soil.dGetClaySlumpDeposit() + m_Soil.dGetClayToppleDeposit() + m_Soil.dGetClayHeadcutRetreatDeposit() + m_Soil.dGetClayInfiltDeposit();
   double dTotSiltDeposit = m_Soil.dGetSiltFlowDeposit() + m_Soil.dGetSiltSplashDeposit() + m_Soil.dGetSiltSlumpDeposit() + m_Soil.dGetSiltToppleDeposit() + m_Soil.dGetSiltHeadcutRetreatDeposit() + m_Soil.dGetSiltInfiltDeposit();
   double dTotSandDeposit = m_Soil.dGetSandFlowDeposit() + m_Soil.dGetSandSplashDeposit() + m_Soil.dGetSandSlumpDeposit() + m_Soil.dGetSandToppleDeposit() + m_Soil.dGetSandHeadcutRetreatDeposit() + m_Soil.dGetSandInfiltDeposit();

   // Now calculate net detachment (i.e. detachment - deposition) for this cell
   double dNetClayDetach = dTotClayDetach - dTotClayDeposit;
   double dNetSiltDetach = dTotSiltDetach - dTotSiltDeposit;
   double dNetSandDetach = dTotSandDetach - dTotSandDeposit;

   m_pSim->AddToEndOfIterNetClayDetach(dNetClayDetach);
   m_pSim->AddToEndOfIterNetSiltDetach(dNetSiltDetach);
   m_pSim->AddToEndOfIterNetSandDetach(dNetSandDetach);
}

