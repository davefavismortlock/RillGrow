/*=========================================================================================================================================

This is cell_sediment.cpp: implementations of the RillGrow class used to represent sediment in a cell's surface water

 Copyright (C) 2018 David Favis-Mortlock

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
   m_dSandSedimentLoad(0)
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


double CSediment::dGetAllSizeSedimentLoad(void) const
{
   return (m_dClaySedimentLoad + m_dSiltSedimentLoad + m_dSandSedimentLoad);
}


// Adds to this cell's clay-sized sediment load and returns the updated clay-sized sediment load (the return value is not used if we are adding sediment)
double CSediment::dChangeClaySedimentLoad(double const dDepth)
{
   m_dClaySedimentLoad += dDepth;   
   m_pSim->AddThisIterClaySedimentLoad(dDepth);
   
   return m_dClaySedimentLoad;
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
   m_pSim->AddThisIterClaySedimentLoad(-dSedimentLoad);
   
   return dSedimentLoad;
}


// Adds to this cell's silt-sized sediment load and returns the updated silt-sized sediment load (the return value is not used if we are adding sediment)
double CSediment::dChangeSiltSedimentLoad(double const dDepth)
{
   m_dSiltSedimentLoad += dDepth;
   m_pSim->AddThisIterSiltSedimentLoad(dDepth);
   
   return m_dSiltSedimentLoad;
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
   m_pSim->AddThisIterSiltSedimentLoad(-dSedimentLoad);
   
   return dSedimentLoad;
}


// Adds to this cell's sand-sized sediment load and returns the updated sand-sized sediment load (the return value is not used if we are adding sediment)
double CSediment::dChangeSandSedimentLoad(double const dDepth)
{
   m_dSandSedimentLoad += dDepth;
   m_pSim->AddThisIterSandSedimentLoad(dDepth);
   
   return m_dSandSedimentLoad;
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
   m_pSim->AddThisIterSandSedimentLoad(-dSedimentLoad);
   
   return dSedimentLoad;
}

// Returns percentage sediment concentration (all sediment size classes)
double CSediment::dGetAllSizeSedimentConcentration(void)
{
   double dWaterDepth = m_pCell->pGetSurfaceWater()->dGetSurfaceWater();
   
   if (dWaterDepth == 0)
      return 0;
   
   return 100 * (m_dClaySedimentLoad + m_dSiltSedimentLoad + m_dSandSedimentLoad) / dWaterDepth;  
}
