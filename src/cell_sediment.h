#ifndef __SEDIMENT_H__
   #define __SEDIMENT_H__
/*=========================================================================================================================================

 This is cell_sediment.h: declarations for the RillGrow class used to represent sediment in a cell's overland flow

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


class CSediment
{
public:
   static CSimulation* m_pSim;

private:
   double
      m_dClaySedimentLoad,
      m_dSiltSedimentLoad,
      m_dSandSedimentLoad,
      m_dCumulClaySedimentLoad,
      m_dCumulSiltSedimentLoad,
      m_dCumulSandSedimentLoad;

   CCell* m_pCell;

public:
   CSediment(void);
   ~CSediment(void);

   void SetParent(CCell* const);

   void InitializeAllSizeSedimentLoad(void);

   double dGetAllSizeSedimentLoad(void) const;
   double dGetCumulAllSizeSedimentLoad(void) const;

   void ChangeSedimentLoad(double const, double const, double const);

   void DoChangeClaySedimentLoad(double const);
   double dGetClaySedimentLoad(void) const;
   double dSetClaySedLoadZero(void);

   void DoChangeSiltSedimentLoad(double const);
   double dGetSiltSedimentLoad(void) const;
   double dSetSiltSedLoadZero(void);

   void DoChangeSandSedimentLoad(double const);
   double dGetSandSedimentLoad(void) const;
   double dSetSandSedimentLoadZero(void);

   double dGetAllSizeSedimentConcentration(void);
};

#endif         // __SEDIMENT_H__
