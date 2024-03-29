#ifndef __m_Cell_H__
   #define __m_Cell_H__
/*=========================================================================================================================================

 This is m_Cell.h: the declaration of the class used to represent each soil m_Cell object

 Copyright (C) 2023 David Favis-Mortlock

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
   bool m_bHadHeadcutRetreat;
   int m_nEdgem_Cell;
   double
      m_dBasementElevationation,                            // Elevation of bottom of lowest soil layer, in mm
      m_dInitialSoilSurfaceElevation,                       // Initial elevation of soil surface, in mm
      m_dStoredRetreat[8];                                  // Stored (i.e. within-m_Cell) headcut retreat along directions 1 to 8

   CCellSoil m_Soil;
   CCellRainAndRunon m_RainAndRunOn;
   CCellSurfaceWater m_SurfaceWater;
   CCellSedimentLoad m_SedLoad;
   CCellSubsurfaceWater m_SoilWater;

public:
   // ============================================================ Constructor ============================================================
   CCell(void);
   ~CCell(void);

   void SetEdgem_Cell(int const);
   int nGetEdge(void) const;
   bool bIsEdgem_Cell(void) const;
   bool bIsMissingValue(void) const;
   bool bIsMissingOrEdge(void) const;

   void SetBasementElevation(double const);
   double dGetBasementElevation(void) const;

   void SetInitialSoilSurfaceElevation(double const);
   double dGetInitialSoilSurfaceElevation(void) const;

   double dGetTopElevation(void);
   void SetSoilSurfaceElevation(double const);
   double dGetSoilSurfaceElevation(void);

   double dGetStoredRetreat(int const) const;
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

   void CalcIterTotalsAndInit(bool&, double&, double&, double&, double&, double&, double&, double&, double&, double&, double&, double&, double&, double&, double&, double&, double&, double&, double&, double&, double&, double&, double&, bool const);
};
#endif         // __m_Cell_H__
