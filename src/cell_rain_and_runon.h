#ifndef __RAIN_AND_RUNON_H__
   #define __RAIN_AND_RUNON_H__
/*=========================================================================================================================================

 This is cell_rain_and_runon.h: declarations for the RillGrow class used to represent incident water i.e. rain and runon

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


class CRainAndRunon
{
public:
   static CSimulation* m_pSim;

private:
   double
      m_dRain,                               // This-iteration rainfall, as a depth (mm)
      m_dCumulRain,                          // Cumulative rain, as a depth (mm)
      m_dRunOn,                              // This-iteration runon, as a depth (mm)
      m_dCumulRunOn,                         // Cumulative runon, as a depth (mm)
      m_dRainVarM;                           // Constant multiplier, default = 1

   CCell* m_pCell;

public:
   CRainAndRunon(void);
   ~CRainAndRunon(void);

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
