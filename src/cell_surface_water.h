#ifndef __SURFACE_WATER_H__
   #define __SURFACE_WATER_H__
/*=========================================================================================================================================

This is cell_surface_water.h: declarations for the RillGrow class used to represent a cell's surface water

Copyright (C) 2025 David Favis-Mortlock

==========================================================================================================================================

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
class CCell;                                       // Forward declaration
class CSimulation;                                 // Ditto

#include "2d_vec.h"

class CCellSurfaceWater
{
public:
   //! Pointer to the parent cell object
   static CSimulation* m_pSim;

private:
   //! Flow direction
   int m_nFlowDirection;
   
   //! For Lawrence friction factor approach: 0 = dry, 1 = shallow flow, 2 = marginally inundated, 3 = well inundated
   int m_nInundationClass;                          

   //! Water on soil surface as a depth (mm)
   double m_dSurfaceWaterDepth;    
   
   //! Temporary depth of water (mm) on soil surface, is a transaction field used during rill erosion calculations
   double m_dTmpSurfaceWaterDepth;

   //! Cumulative depth of water on soil surface (mm)
   double m_dCumulSurfaceWaterDepth;                 
   
   // This-iteration depth lost via edge cell (mm)
   double m_dSurfaceWaterDepthLost;    
   
   //! Cumulative depth lost via edge cell (mm)
   double m_dCumulSurfaceWaterDepthLost;    
   
   //! Stream power of overland flow on this cell
   double m_dStreamPower;
   
   //! Transport capacity of overland flow on this cell
   double m_dTransportCapacity;
   
   //! Friction factor for overland flow on this cell
   double m_dFrictionFactor;

   //! Vector flow velocity
   C2DVec m_vFlowVelocity; 
   
   //! Vector cumulative time-weighted flow velocity
   C2DVec m_vCumulFlowVelocity;    
   
   //! Vector depth-weighted flow velocity
   C2DVec m_vDWFlowVelocity;        
   
   //! Vector cumulative time-weighted depth-weighted flow velocity
   C2DVec m_vCumulDWFlowVelocity;     
   
   //! Pointer to the parent cell object
   CCell* pCell;

public:
   CCellSurfaceWater(void);
   ~CCellSurfaceWater(void);

   void SetParent(CCell* const);

   void InitializeFlow(void);
   void SetFlowThisIter(void);
   // bool bFlowThisIter(void) const;

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
   void AddToCumulSurfaceWater(double const);

   void InitTmpSurfaceWater(void);
   void AddTmpSurfaceWater(double const);
   void RemoveTmpSurfaceWater(double&);
   // void SetTmpSurfaceWaterZero(void);
   void FinishTmpSurfaceWater(void);

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
