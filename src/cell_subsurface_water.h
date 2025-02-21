#ifndef __SUBSURFACE_WATER_H__
   #define __SUBSURFACE_WATER_H__
/*=========================================================================================================================================

This is cell_subsurface_water.h: declarations for the RillGrow class used to represent subsurface water

Copyright (C) 2025 David Favis-Mortlock

==========================================================================================================================================

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
class CCell;                                 // Forward declaration

#include "simulation.h"

class CCellSubsurfaceWater
{
private:
   //! This-iteration infiltrated soil water for this cell, as a depth (mm)
   double m_dEndOfIterInfiltWater;
   
   //! Cumulative infiltrated soil water for this cell, as a depth (mm)
   double m_dCumulInfiltWater;     
   
   //! This-iteration exfiltrated soil water for this cell, as a depth (mm)
   double m_dEndOfIterExfiltWater;
   
   //! Cumulative exfiltrated soil water for this cell, as a depth (mm)
   double m_dCumulExfiltWater;                   

   //! Pointer to the parent cell object
   CCell* m_pCell;

public:
   CCellSubsurfaceWater(void);
   ~CCellSubsurfaceWater(void);

   void SetParent(CCell* const);

   void DoInfiltration(double&);
   void InfiltrateAndMakeDry(double&, double&, double&);
   void InitializeInfiltration(void);
   double dGetThisIterInfiltration(void) const;
   double dGetCumulInfiltration(void) const;

   void DoExfiltration(double const);
   double dGetExfiltration(void) const;
   double dGetCumulExfiltration(void) const;

   double dGetTopLayerSoilWater(void);
   double dGetAllSoilWater(void) const;
};
#endif         // __SUBSURFACE_WATER_H__
