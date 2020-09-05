#ifndef __CELL_H__
   #define __CELL_H__
/*=========================================================================================================================================

 This is cell.h: the declaration of the class used to represent each soil cell object

 Copyright (C) 2020 David Favis-Mortlock

 ==========================================================================================================================================

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation,
 Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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
   static CSimulation* m_pSim;

private:
   bool m_bHasHadHeadcutRetreat;
   int m_nEdgeCell;
   double
      m_dBasementElevationation,                            // Elevation of bottom of lowest soil layer, in mm
      m_dInitialSoilSurfaceElevation,                       // Initial elevation of soil surface, in mm
      m_dStoredRetreat[8];                                  // Stored (i.e. within-cell) headcut retreat along directions 1 to 8

   CSoil m_Soil;
   CRainAndRunon m_RainAndRunon;
   CSurfaceWater m_SurfaceWater;
   CSediment m_Sediment;
   CSoilWater m_SoilWater;

public:
   // ============================================================ Constructor ============================================================
   CCell(void);
   ~CCell(void);

   void SetEdgeCell(int const);
   int nGetEdge(void) const;
   bool bIsEdgeCell(void) const;
   bool bIsMissingValue(void) const;
   bool bIsMissingOrEdge(void) const;

   void SetBasementElevation(double const);
   double dGetBasementElevation(void) const;

   void SetInitialSoilSurfaceElevation(double const);
   double dGetInitialSoilSurfaceElevation(void) const;

   double dGetTopElevation(void);
   void SetSoilSurfaceElevation(double const);
   double dGetSoilSurfaceElevation(void);

   double dGetStoredRetreat(int const);
   void SetStoredRetreat(int const, double const);
   void AddToStoredRetreat(int const, double const);
   bool bIsReadyForHeadcutRetreat(int const, double const);
   void SetHasHadHeadcutRetreat(void);
   bool bHasHadHeadcutRetreat(void);

   CSoil* pGetSoil(void);
   CRainAndRunon* pGetRainAndRunon(void);
   CSurfaceWater* pGetSurfaceWater(void);
   CSediment* pGetSediment(void);
   CSoilWater* pGetSoilWater(void);

   void InitializeAtStartOfIteration(bool const);
};
#endif         // __CELL_H__
