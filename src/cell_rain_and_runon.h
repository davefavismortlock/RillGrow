#ifndef __RAIN_AND_RUNON_H__
   #define __RAIN_AND_RUNON_H__
/*=========================================================================================================================================

This is cell_rain_and_runon.h: declarations for the RillGrow class used to represent incident water (rain and runon)

Copyright (C) 2025 David Favis-Mortlock

==========================================================================================================================================

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
class CCell;                                 // Forward declaration

class CCellRainAndRunon
{
public:
   //! Pointer to the simulation object
   static CSimulation* m_pSim;

private:
   //! This-iteration rainfall, as a depth (mm)
   double m_dRain;          
   
   //! Cumulative rain, as a depth (mm)
   double m_dCumulRain;         
   
   //! This-iteration runon, as a depth (mm)
   double m_dRunOn;        
   
   //! Cumulative runon, as a depth (mm)
   double m_dCumulRunOn;        
   
   //! Constant multiplier, default = 1
   double m_dRainVarM;                           

   //! Pointer to the parent cell object
   CCell* pCell;

public:
   CCellRainAndRunon(void);
   ~CCellRainAndRunon(void);

   void SetParent(CCell* const);

   void InitializeRainAndRunon(void);

   void AddRain(double const);
   double dGetRain(void) const;
   double dGetCumulRain(void) const;
   void SetRainVarM(double const);
   double dGetRainVarM(void) const;

   void AddRunOn(double const);
   double dGetRunOn(void) const;
   double dGetCumulRunOn(void) const;
};

#endif         // __RAIN_AND_RUNON_H__
