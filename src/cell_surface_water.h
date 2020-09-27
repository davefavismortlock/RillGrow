#ifndef __SURFACE_WATER_H__
   #define __SURFACE_WATER_H__
/*=========================================================================================================================================

 This is cell_surface_water.h: declarations for the RillGrow class used to represent a cell's overland flow

 Copyright (C) 2020 David Favis-Mortlock

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


class CSurfaceWater
{
public:
   static CSimulation* m_pSim;

private:
   int
      m_nFlowDirection,
      m_nInundationClass;                          // 0 = dry, 1 = shallow flow, 2 = marginally inundated, 3 = well inundated

   double
      m_dSurfaceWaterDepth,                        // Water on soil surface, as a depth (mm)
      m_dCumulSurfaceWaterDepth,                   // Cumulative deoth of water on soil surface (mm)
      m_dStreamPower,
      m_dTransCap,
      m_dFrictionFactor;

#if defined _DEBUG
   double
      m_dCumulSurfaceWaterDepthLost;               // Total lost from grid via this edge cell, as a depth (mm)
#endif

   C2DVec
      m_vFlowVelocity,                             // Flow velocity
      m_vCumulFlowVelocity,                        // Cumulative time-weighted flow velocity
      m_vDWFlowVelocity,                           // Depth-weighted flow velocity
      m_vCumulDWFlowVelocity;                      // Cumulative time-weighted depth-weighted flow velocity

   CCell* m_pCell;

public:
   CSurfaceWater(void);
   ~CSurfaceWater(void);

   void SetParent(CCell* const);

   void InitializeFlow(void);

   void SetInundation(int const);
   int nGetInundation(void) const;

   void SetFlowDirection(int const);
   int nGetFlowDirection(void) const;

   void ChangeSurfaceWater(double const);
   void SetSurfaceWaterZero(void);
   double dGetSurfaceWaterDepth(void) const;
   bool bIsWet(void) const;
   double dGetCumulSurfaceWater(void) const;

#if defined _DEBUG
   void AddSurfaceWaterLost(double const);
   double dGetCumulSurfaceWaterLost(void) const;
#endif

   void InitializeAllFlowVelocity(void);
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
