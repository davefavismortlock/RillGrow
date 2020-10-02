/*=========================================================================================================================================

This is cell_subsurface_water.cpp: implementations of the RillGrow class used to represent suboverland flow

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
#include "cell_subsurface_water.h"


CSoilWater::CSoilWater(void)
:
   m_dInfiltWaterLost(0),
   m_dCumulInfiltWaterLost(0)
{
   m_pCell = NULL;
}

CSoilWater::~CSoilWater(void)
{
}


void CSoilWater::SetParent(CCell* const pParent)
{
   m_pCell = pParent;
}


// Loses overland flow to infiltration
void CSoilWater::Infiltrate(double const dInfilt)
{
   m_pCell->pGetSurfaceWater()->ChangeSurfaceWater(-dInfilt);

   // Is the cell now dry?
   if (! m_pCell->pGetSurfaceWater()->bIsWet())
      m_pCell->m_pSim->DecrNumWetCells();

   // Subtract from the this-iteration total depth of overland flow
   m_pCell->m_pSim->AddSurfaceWater(-dInfilt);

   // Get a pointer to the top layer
   CLayer* pLayer = m_pCell->pGetSoil()->pLayerGetLayer(0);

   // And add water to this layer
   pLayer->ChangeSoilWater(dInfilt);

   m_dInfiltWaterLost    += dInfilt;
   m_dCumulInfiltWaterLost += dInfilt;
}

// Loses all this cell's overland flow to infiltration, and increases the cell's elevation with any sediment that was being transported
void CSoilWater::InfiltrateAndMakeDry(void)
{
   double
      dWaterDepth = m_pCell->pGetSurfaceWater()->dGetSurfaceWaterDepth(),
      dClaySediment = m_pCell->pGetSediment()->dGetClaySedimentLoad(),
      dSiltSediment = m_pCell->pGetSediment()->dGetSiltSedimentLoad(),
      dSandSediment = m_pCell->pGetSediment()->dGetSandSedimentLoad();

   // First remove the surface water (also decrements the surface water total, and count of wet cells)
   m_pCell->pGetSurfaceWater()->SetSurfaceWaterZero();

   // Get a pointer to the top layer
   CLayer* pLayer = m_pCell->pGetSoil()->pLayerGetLayer(0);

   // And add water to this layer
   pLayer->ChangeSoilWater(dWaterDepth);

   m_dInfiltWaterLost      += dWaterDepth;
   m_dCumulInfiltWaterLost += dWaterDepth;

   // And then sort out the sediment
   m_pCell->pGetSoil()->DoInfiltrationDeposit(dClaySediment, dSiltSediment, dSandSediment);
}

// Zeros the values (they are kept over several iterations) values for water lost to infiltration and sediment deposited due to infiltration
void CSoilWater::SetZeroThisIterInfiltration(void)
{
   m_dInfiltWaterLost = 0;
   m_pCell->pGetSoil()->SetInfiltrationDepositionZero();
}

// Returns the depth of water lost by infiltration
double CSoilWater::dGetInfiltration(void) const
{
   return m_dInfiltWaterLost;
}

// Returns the cumulative depth of water lost by infiltration
double CSoilWater::dGetCumulInfiltration(void) const
{
   return m_dCumulInfiltWaterLost;
}


// Returns the depth of soil water in the top soil layer
double CSoilWater::dGetTopLayerSoilWater(void)
{
   // Get a pointer to the top layer
   CLayer* pLayer = m_pCell->pGetSoil()->pLayerGetLayer(0);

   return pLayer->dGetSoilWater();
}
