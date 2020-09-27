/*=========================================================================================================================================

This is cell_sediment.cpp: implementations of the RillGrow class used to represent sediment in a cell's overland flow

 Copyright (C) 2020 David Favis-Mortlock

 ==========================================================================================================================================

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation,
 Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
#include "rg.h"
#include "cell.h"
#include "cell_sediment.h"


CSediment::CSediment(void)
:
   m_dClaySedimentLoad(0),
   m_dSiltSedimentLoad(0),
   m_dSandSedimentLoad(0),
   m_dCumulClaySedimentLoad(0),
   m_dCumulSiltSedimentLoad(0),
   m_dCumulSandSedimentLoad(0)
   {
   m_pCell = NULL;
}

CSediment::~CSediment(void)
{
}


void CSediment::SetParent(CCell* const pParent)
{
   m_pCell = pParent;
}


void CSediment::InitializeAllSizeSedimentLoad(void)
{
   m_dClaySedimentLoad =
   m_dSiltSedimentLoad =
   m_dSandSedimentLoad = 0;
}


double CSediment::dGetCumulAllSizeSedimentLoad(void) const
{
   return (m_dCumulClaySedimentLoad + m_dCumulSiltSedimentLoad + m_dCumulSandSedimentLoad);
}


double CSediment::dGetAllSizeSedimentLoad(void) const
{
   return (m_dClaySedimentLoad + m_dSiltSedimentLoad + m_dSandSedimentLoad);
}


// Adds to this cell's sediment load
void CSediment::ChangeSedimentLoad(double const dClayDepth, double const dSiltDepth, double const dSandDepth)
{
   m_dClaySedimentLoad += dClayDepth;
   m_dSiltSedimentLoad += dSiltDepth;
   m_dSandSedimentLoad += dSandDepth;

   m_dCumulClaySedimentLoad += dClayDepth;
   m_dCumulSiltSedimentLoad += dSiltDepth;
   m_dCumulSandSedimentLoad += dSandDepth;

   m_pSim->AddThisIterClaySedimentLoad(dClayDepth);
   m_pSim->AddThisIterSiltSedimentLoad(dSiltDepth);
   m_pSim->AddThisIterSandSedimentLoad(dSandDepth);
}

// Adds to this cell's clay-sized sediment load
void CSediment::DoChangeClaySedimentLoad(double const dDepth)
{
   m_dClaySedimentLoad += dDepth;
   m_dCumulClaySedimentLoad += dDepth;

   m_pSim->AddThisIterClaySedimentLoad(dDepth);
}

double CSediment::dGetClaySedimentLoad(void) const
{
   return m_dClaySedimentLoad;
}

// Sets this cell's clay-sized sediment load to zero and returns the (+ve) depth that was subtracted
double CSediment::dSetClaySedLoadZero(void)
{
   double dSedimentLoad = m_dClaySedimentLoad;
   m_dClaySedimentLoad = 0;

   // Note do not change cumulative value
   m_pSim->AddThisIterClaySedimentLoad(-dSedimentLoad);

   return dSedimentLoad;
}


// Adds to this cell's silt-sized sediment load
void CSediment::DoChangeSiltSedimentLoad(double const dDepth)
{
   m_dSiltSedimentLoad += dDepth;
   m_dCumulSiltSedimentLoad += dDepth;

   m_pSim->AddThisIterSiltSedimentLoad(dDepth);
}

double CSediment::dGetSiltSedimentLoad(void) const
{
   return m_dSiltSedimentLoad;
}

// Sets this cell's silt-sized sediment load to zero and returns the (+ve) depth that was subtracted
double CSediment::dSetSiltSedLoadZero(void)
{
   double dSedimentLoad = m_dSiltSedimentLoad;
   m_dSiltSedimentLoad = 0;

   // Note do not change cumulative value
   m_pSim->AddThisIterSiltSedimentLoad(-dSedimentLoad);

   return dSedimentLoad;
}


// Adds to this cell's sand-sized sediment load
void CSediment::DoChangeSandSedimentLoad(double const dDepth)
{
   m_dSandSedimentLoad += dDepth;
   m_dCumulSandSedimentLoad += dDepth;

   m_pSim->AddThisIterSandSedimentLoad(dDepth);
}

double CSediment::dGetSandSedimentLoad(void) const
{
   return m_dSandSedimentLoad;
}

// Sets this cell's sand-sized sediment load to zero and returns the (+ve) depth that was subtracted
double CSediment::dSetSandSedimentLoadZero(void)
{
   double dSedimentLoad = m_dSandSedimentLoad;
   m_dSandSedimentLoad = 0;

   // Note do not change cumulative value
   m_pSim->AddThisIterSandSedimentLoad(-dSedimentLoad);

   return dSedimentLoad;
}

// Returns this-iteration percentage sediment concentration (all sediment size classes)
double CSediment::dGetAllSizeSedimentConcentration(void)
{
   double dWaterDepth = m_pCell->pGetSurfaceWater()->dGetSurfaceWaterDepth();

   if (dWaterDepth == 0)
      return 0;

   return 100 * (m_dClaySedimentLoad + m_dSiltSedimentLoad + m_dSandSedimentLoad) / dWaterDepth;
}
