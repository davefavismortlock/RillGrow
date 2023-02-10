#ifndef __SEDIMENT_H__
   #define __SEDIMENT_H__
/*=========================================================================================================================================

 This is m_Cell_sediment.h: declarations for the RillGrow class used to represent sediment in a m_Cell's surface water

 Copyright (C) 2023 David Favis-Mortlock

 ==========================================================================================================================================

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation,
 Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
class CCell;                                 // Forward declaration


class CCellSedimentLoad
{
public:
   static CSimulation* m_pSim;

private:
   double
      m_dClaySedLoad,
      m_dSiltSedLoad,
      m_dSandSedLoad,
      m_dCumulClaySedLoad,
      m_dCumulSiltSedLoad,
      m_dCumulSandSedLoad,
      m_dFromSplashClaySedLoad,
      m_dFromSplashSiltSedLoad,
      m_dFromSplashSandSedLoad,
      m_dFromSlumpClaySedLoad,
      m_dFromSlumpSiltSedLoad,
      m_dFromSlumpSandSedLoad,
      m_dFromToppleClaySedLoad,
      m_dFromToppleSiltSedLoad,
      m_dFromToppleSandSedLoad,
      m_dFromHeadcutRetreatClaySedLoad,
      m_dFromHeadcutRetreatSiltSedLoad,
      m_dFromHeadcutRetreatSandSedLoad;

   CCell* pCell;

public:
   CCellSedimentLoad(void);
   ~CCellSedimentLoad(void);

   void SetParent(CCell* const);

   void InitializeAllSizeSedLoad(void);

   double dGetAllSizeSedLoad(void) const;
   double dGetCumulAllSizeSedLoad(void) const;

   void AddToSedLoad(const double, const double, const double);
   void RemoveFromSedLoad(double&, double&, double&);

   void RemoveFromClaySedLoad(double&);
   double dGetClaySedLoad(void) const;
   double dSetClaySedLoadZero(void);

   void RemoveFromSiltSedLoad(double&);
   double dGetSiltSedLoad(void) const;
   double dSetSiltSedLoadZero(void);

   void RemoveFromSandSedLoad(double&);
   double dGetSandSedLoad(void) const;
   double dSetSandSedLoadZero(void);

   double dGetAllSizeSedConc(void);

   void AddToSplashSedLoad(const double, const double, const double);
   double dGetClaySplashSedLoad(void) const;
   double dGetSiltSplashSedLoad(void) const;
   double dGetSandSplashSedLoad(void) const;

   void AddToSlumpSedLoad(const double, const double, const double);
   double dGetClaySlumpSedLoad(void) const;
   double dGetSiltSlumpSedLoad(void) const;
   double dGetSandSlumpSedLoad(void) const;

   void AddToToppleSedLoad(const double, const double, const double);
   double dGetClayToppleSedLoad(void) const;
   double dGetSiltToppleSedLoad(void) const;
   double dGetSandToppleSedLoad(void) const;

   void AddToHeadcutRetreatSedLoad(const double, const double, const double);
   double dGetClayHeadcutRetreatSedLoad(void) const;
   double dGetSiltHeadcutRetreatSedLoad(void) const;
   double dGetSandHeadcutRetreatSedLoad(void) const;

   void InitializeSplashSedLoads(void);
   void InitializeSlumpAndToppleSedLoads(void);
   void InitializeHeadcutRetreatSedLoads(void);
};

#endif         // __SEDIMENT_H__
