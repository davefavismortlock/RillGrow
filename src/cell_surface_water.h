#ifndef __SURFACE_WATER_H__
   #define __SURFACE_WATER_H__
/*=========================================================================================================================================

 This is m_Cell_surface_water.h: declarations for the RillGrow class used to represent a m_Cell's surface water

 Copyright (C) 2023 David Favis-Mortlock

 ==========================================================================================================================================

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation,
 Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
class CCell;                                       // Forward declaration
class CSimulation;                                 // Ditto

#include "2d_vec.h"

class CCellSurfaceWater
{
public:
   static CSimulation* m_pSim;

private:
   bool
      m_bFlowThisIter;                             // Switch to prevent processing twice in the same iteration
   int
      m_nFlowDirection,
      m_nInundationClass;                          // For Lawrence fricton factor calcs: 0 = dry, 1 = shallow flow, 2 = marginally inundated, 3 = well inundated

   double
      m_dSurfaceWaterDepth,                        // Water on soil surface, as a depth (mm)
      m_dCumulSurfaceWaterDepth,                   // Cumulative depth of water on soil surface (mm)
      m_dSurfaceWaterDepthLost,                    // This-iteration depth lost via edge m_Cell (mm)
      m_dCumulSurfaceWaterDepthLost,               // Cumulative depth lost via edge m_Cell (mm)
      m_dStreamPower,
      m_dTransportCapacity,
      m_dFrictionFactor;

   C2DVec
      m_vFlowVelocity,                             // Flow velocity
      m_vCumulFlowVelocity,                        // Cumulative time-weighted flow velocity
      m_vDWFlowVelocity,                           // Depth-weighted flow velocity
      m_vCumulDWFlowVelocity;                      // Cumulative time-weighted depth-weighted flow velocity

   CCell* pCell;

public:
   CCellSurfaceWater(void);
   ~CCellSurfaceWater(void);

   void SetParent(CCell* const);

   void InitializeFlow(void);
   void SetFlowThisIter(void);
   bool bFlowThisIter(void) const;

   void SetInundation(int const);
   int nGetInundation(void) const;

   void SetFlowDirection(int const);
   int nGetFlowDirection(void) const;

   void AddSurfaceWater(double const);
   void RemoveSurfaceWater(double&);
   void SetSurfaceWaterZero(void);
   double dGetSurfaceWaterDepth(void) const;
   bool bIsWet(void) const;
   double dGetCumulSurfaceWater(void) const;

   void AddSurfaceWaterLost(double const);
   double dGetSurfaceWaterLost(void) const;
   double dGetCumulSurfaceWaterLost(void) const;

   void InitializeAllFlowVelocity(void);
   void ZeroAllFlowVelocity(void);
   void SetFlowVelocity(const C2DVec&);
   void SetFlowVelocity(const C2DVec*);
   void SetFlowVelocity(double const, double const);
   C2DVec* vecGetFlowVel(void);
   double dGetFlowSpd(void) const;
   double dCumulFlowSpeed(void) const;
   void SetDepthWeightedFlowVelocity(const C2DVec&);
   void SetDepthWeightedFlowVelocity(const C2DVec*);
   void SetDepthWeightedFlowVelocity(double const, double const);
   C2DVec* vecGetDWFlowVel(void);
   double dGetDWFlowVelX(void) const;
   double dGetDWFlowVelY(void) const;
   double dGetDWFlowSpd(void) const;
   double dGetCumulDWFlowSpd(void) const;

   void SetStreamPower(double const);
   double dGetStreamPower(void) const;

   void SetTransportCapacity(double const);
   double dGetTransportCapacity(void) const;

   void SetFrictionFactor(double const);
   double dGetFrictionFactor(void) const;

   double dGetFroude(double const) const;
};
#endif         // __SURFACE_WATER_H__
