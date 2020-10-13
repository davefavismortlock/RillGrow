#ifndef __SUBSURFACE_WATER_H__
   #define __SUBSURFACE_WATER_H__
/*=========================================================================================================================================

 This is cell_subsurface_water.h: declarations for the RillGrow class used to represent subsurface water

 Copyright (C) 2020 David Favis-Mortlock

 ==========================================================================================================================================

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation,
 Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
class CCell;                                 // Forward declaration

#include "simulation.h"


class CCellSubsurfaceWater
{
private:
   double
      m_dInfiltWaterLost,                    // Overland flow lost to infilt, as a depth (mm)
      m_dCumulInfiltWaterLost;               // Cumulative surface water lost to infilt, as a depth (mm)

   CCell* m_pCell;

public:
   CCellSubsurfaceWater(void);
   ~CCellSubsurfaceWater(void);

   void SetParent(CCell* const);

   void DoInfiltration(double&);
   void InfiltrateAndMakeDry(void);
   void SetZeroThisIterInfiltration(void);
   double dGetInfiltration(void) const;
   double dGetCumulInfiltration(void) const;

   double dGetTopLayerSoilWater(void);
};

#endif         // __SUBSURFACE_WATER_H__
