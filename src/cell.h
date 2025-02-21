#ifndef __CELL_H__
   #define __CELL_H__
/*=========================================================================================================================================

This is cell.h: the declaration of the RillGrow class used to represent each soil cell object

Copyright (C) 2025 David Favis-Mortlock

==========================================================================================================================================

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
class CSimulation;                                 // Forward declaration

#include "cell_soil.h"
#include "cell_rain_and_runon.h"
#include "cell_surface_water.h"
#include "cell_sediment.h"
#include "cell_subsurface_water.h"

class CCell
{
public:
   //! Pointer to the simulation object
   static CSimulation* m_pSim;

private:
   //! Switch to show if this cell had headcut retreat this iteration
   bool m_bHadHeadcutRetreat;
   
   //! Edge cell code
   int m_nEdgeCell;
   
   //! Elevation of bottom of lowest soil layer, in mm. Only unerodible material below this
   double m_dBasementElev;
   
   //! Initial elevation of soil surface in mm, or flagged as missing value if outside area of measured elevations
   double m_dInitialSoilSurfaceElev;
   
   //! Stored (i.e. within-cell) headcut retreat along directions 1 to 8
   double m_dStoredRetreat[8];                                  

   //! Cell soil object
   CCellSoil m_Soil;
   
   //! Cell rain-and-runoff object
   CCellRainAndRunon m_RainAndRunOn;
   
   //! Cell surface water object
   CCellSurfaceWater m_SurfaceWater;
   
   //! Cell sediment load object
   CCellSedimentLoad m_SedLoad;
   
   //! Cell soil subsurface water object
   CCellSubsurfaceWater m_SoilWater;

public:
   // ============================================================ Constructor ============================================================
   CCell(void);
   ~CCell(void);

   void SetEdgeCell(int const);
   int nGetEdge(void) const;
   bool bIsEdgeCell(void) const;
   bool bIsMissingValue(void) const;
   // bool bIsMissingOrEdge(void) const;

   void SetBasementElevation(double const);
   double dGetBasementElevation(void) const;

   void SetInitialSoilSurfaceElevation(double const);
   double dGetInitialSoilSurfaceElevation(void) const;
   double dGetTopElevation(void) const;

   void AddToPreSimulationValues(void) const;

   // double dGetStoredRetreat(int const) const;
   void SetStoredRetreat(int const, double const);
   void AddToStoredRetreat(int const, double const);
   bool bIsReadyForHeadcutRetreat(int const, double const) const;
   void SetHasHadHeadcutRetreat(void);
   bool bHasHadHeadcutRetreat(void) const;

   CCellSoil* pGetSoil(void);
   CCellRainAndRunon* pGetRainAndRunon(void);
   CCellSurfaceWater* pGetSurfaceWater(void);
   CCellSedimentLoad* pGetSedLoad(void);
   CCellSubsurfaceWater* pGetSoilWater(void);

   void GetEndOfIterValues(void) const;
};
#endif         // __CELL_H__
