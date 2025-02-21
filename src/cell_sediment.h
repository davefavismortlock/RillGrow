#ifndef __SEDIMENT_H__
   #define __SEDIMENT_H__
/*=========================================================================================================================================

This is cell_sediment.h: declarations for the RillGrow class used to represent sediment in a cell's surface water

Copyright (C) 2025 David Favis-Mortlock

==========================================================================================================================================

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
class CCell;                                 // Forward declaration


class CCellSedimentLoad
{
public:
   //! Pointer to the simulation object
   static CSimulation* m_pSim;

private:
   //! This-iteration clay sediment load (mm depth)
   double m_dLastIterClaySedLoad;

   //! This-iteration silt sediment load (mm depth)
   double m_dLastIterSiltSedLoad;

   //! This-iteration sand sediment load (mm depth)
   double m_dLastIterSandSedLoad;

   //! Cumulative clay sediment load (mm depth, used to calculate average)
   double m_dCumulClaySedLoad;

   //! Cumulative silt sediment load (mm depth, used to calculate average)
   double m_dCumulSiltSedLoad;

   //! Cumulative sand sediment load (mm depth, used to calculate average)
   double m_dCumulSandSedLoad;

   //! The clay sediment load derived from flow erosion during this iteration (mm depth)
   double m_dThisIterFlowClaySedLoad;

   //! The silt sediment load derived from flow erosion during this iteration (mm depth)
   double m_dThisIterFlowSiltSedLoad;

   //! The sand sediment load derived from flow erosion during this iteration (mm depth)
   double m_dThisIterFlowSandSedLoad;

   //! This-iteration clay sediment load (mm depth) derived from splash redistribution
   double m_dThisIterSplashClaySedLoad;

   //! This-iteration silt sediment load (mm depth) derived from splash redistribution
   double m_dThisIterSplashSiltSedLoad;

   //! This-iteration sand sediment load (mm depth) derived from splash redistribution
   double m_dThisIterSplashSandSedLoad;

   //! This-iteration clay sediment load (mm depth) derived from slumping
   double m_dThisIterSlumpClaySedLoad;

   //! This-iteration silt sediment load (mm depth) derived from slumping
   double m_dThisIterSlumpSiltSedLoad;

   //! This-iteration sand sediment load (mm depth) derived from slumping
   double m_dThisIterSlumpSandSedLoad;

   //! This-iteration clay sediment load (mm depth) derived from toppling
   double m_dThisIterTopplingClaySedLoad;

   //! This-iteration silt sediment load (mm depth) derived from toppling
   double m_dThisIterTopplingSiltSedLoad;

   //! This-iteration sand sediment load (mm depth) derived from toppling
   double m_dThisIterTopplingSandSedLoad;

   //! This-iteration clay sediment load (mm depth) derived from headcut retreat
   double m_dThisIterHeadcutRetreatClaySedLoad;

   //! This-iteration silt sediment load (mm depth) derived from headcut retreat
   double m_dThisIterHeadcutRetreatSiltSedLoad;

   //! This-iteration sand sediment load (mm depth) derived from headcut retreat
   double m_dThisIterHeadcutRetreatSandSedLoad;

   //! This-iteration clay sediment removed (mm depth)
   double m_dThisIterClaySedRemoved;

   //! This-iteration silt sediment removed (mm depth)
   double m_dThisIterSiltSedRemoved;

   //! This-iteration sand sediment removed (mm depth)
   double m_dThisIterSandSedRemoved;

   //! This-iteration clay sediment lost from edge (only meaningful for edge cells, and not explicitly considered in end-of-iteration mass balance calcs)
   double m_dClaySplashOffEdge;

   //! This-iteration silt sediment lost from edge (only meaningful for edge cells, and not explicitly considered in end-of-iteration mass balance calcs)
   double m_dSiltSplashOffEdge;

   //! This-iteration sand sediment lost from edge (only meaningful for edge cells, and not explicitly considered in end-of-iteration mass balance calcs)
   double m_dSandSplashOffEdge;

   //! Pointer to the parent cell object
   CCell* pCell;

public:
   CCellSedimentLoad(void);
   ~CCellSedimentLoad(void);

   void SetParent(CCell* const);

   void InitializeAllSizeSedLoad(void);
   void ResetSedLoad(void);

   void AddToClayFlowSedLoad(const double);
   void AddToSiltFlowSedLoad(const double);
   void AddToSandFlowSedLoad(const double);

   void AddToClaySedLoadRemoved(double&);
   void AddToSiltSedLoadRemoved(double&);
   void AddToSandSedLoadRemoved(double&);

   double dGetLastIterAllSizeSedLoad(void) const;
   double dGetThisIterAllSizeSedLoad(void) const;
   double dGetCumulAllSizeSedLoad(void) const;

   double dGetLastIterClaySedLoad(void) const;
   double dGetLastIterSiltSedLoad(void) const;
   double dGetLastIterSandSedLoad(void) const;

   double dGetAllSizeSedConcentration(void);

   void InitSplashOffEdge(void);
   void AddToSplashSedLoad(const double, const double, const double);
   double dGetClaySplashSedLoad(void) const;
   double dGetSiltSplashSedLoad(void) const;
   double dGetSandSplashSedLoad(void) const;

   double dGetClayFlowSedLoad(void) const;
   double dGetSiltFlowSedLoad(void) const;
   double dGetSandFlowSedLoad(void) const;

   void InitSlumpAndToppleSedLoads(void);
   void AddToSlumpSedLoad(const double, const double, const double);
   double dGetClaySlumpSedLoad(void) const;
   double dGetSiltSlumpSedLoad(void) const;
   double dGetSandSlumpSedLoad(void) const;

   void AddToToppleSedLoad(const double, const double, const double);
   double dGetClayToppleSedLoad(void) const;
   double dGetSiltToppleSedLoad(void) const;
   double dGetSandToppleSedLoad(void) const;

   void InitializeHeadcutRetreatSedLoads(void);
   void AddToHeadcutRetreatSedLoad(const double, const double, const double);
   double dGetClayHeadcutRetreatSedLoad(void) const;
   double dGetSiltHeadcutRetreatSedLoad(void) const;
   double dGetSandHeadcutRetreatSedLoad(void) const;

   double dGetClaySedLoadRemoved(void) const;
   double dGetSiltSedLoadRemoved(void) const;
   double dGetSandSedLoadRemoved(void) const;

   // Note that off-edge values are not explicitly included in the end-of-iteration mass balance calculations
   void AddToClaySedOffEdge(const double);
   void AddToSiltSedOffEdge(const double);
   void AddToSandSedOffEdge(const double);
   double dGetClaySedOffEdge(void) const;
   double dGetSiltSedOffEdge(void) const;
   double dGetSandSedOffEdge(void) const;
};
#endif         // __SEDIMENT_H__
