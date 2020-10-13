/*=========================================================================================================================================

This is cell_subsurface_water.cpp: implementations of the RillGrow class used to represent subsurface water

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


CCellSubsurfaceWater::CCellSubsurfaceWater(void)
:
   m_dInfiltWaterLost(0),
   m_dCumulInfiltWaterLost(0)
{
   m_pCell = NULL;
}

CCellSubsurfaceWater::~CCellSubsurfaceWater(void)
{
}


void CCellSubsurfaceWater::SetParent(CCell* const pParent)
{
   m_pCell = pParent;
}


// Loses surface water to infiltration
void CCellSubsurfaceWater::DoInfiltration(double& dInfilt)
{
   // If there is insufficient surface water depth to remove the whole of dInfilt, dInfilt gets reduced
   m_pCell->pGetSurfaceWater()->RemoveSurfaceWater(dInfilt);

   // Get a pointer to the top layer
   CCellSoilLayer* pLayer = m_pCell->pGetSoil()->pLayerGetLayer(0);

   // And add water to this layer
   pLayer->ChangeSoilWater(dInfilt);

   m_dInfiltWaterLost += dInfilt;
   m_dCumulInfiltWaterLost += dInfilt;
}

// Loses all this cell's surface water to infilt, and increases the cell's elevation with any sediment that was being transported
void CCellSubsurfaceWater::InfiltrateAndMakeDry(void)
{
   double
      dWaterDepth = m_pCell->pGetSurfaceWater()->dGetSurfaceWaterDepth(),
      dClaySediment = m_pCell->pGetSediment()->dGetClaySedLoad(),
      dSiltSediment = m_pCell->pGetSediment()->dGetSiltSedLoad(),
      dSandSediment = m_pCell->pGetSediment()->dGetSandSedLoad();

   // First remove the surface water (also decrements the surface water total, and count of wet cells)
   m_pCell->pGetSurfaceWater()->SetSurfaceWaterZero();

   // Get a pointer to the top layer
   CCellSoilLayer* pLayer = m_pCell->pGetSoil()->pLayerGetLayer(0);

   // And add water to this layer
   pLayer->ChangeSoilWater(dWaterDepth);

   m_dInfiltWaterLost      += dWaterDepth;
   m_dCumulInfiltWaterLost += dWaterDepth;

   // And then sort out the sediment
   m_pCell->pGetSoil()->DoInfiltrationDeposit(dClaySediment, dSiltSediment, dSandSediment);
}

// Zeros the values (they are kept over several iterations) values for water lost to infilt and sediment deposited due to infilt
void CCellSubsurfaceWater::SetZeroThisIterInfiltration(void)
{
   m_dInfiltWaterLost = 0;
   m_pCell->pGetSoil()->SetInfiltrationDepositionZero();
}

// Returns the depth of water lost by infilt
double CCellSubsurfaceWater::dGetInfiltration(void) const
{
   return m_dInfiltWaterLost;
}

// Returns the cumulative depth of water lost by infilt
double CCellSubsurfaceWater::dGetCumulInfiltration(void) const
{
   return m_dCumulInfiltWaterLost;
}


// Returns the depth of soil water in the top soil layer
double CCellSubsurfaceWater::dGetTopLayerSoilWater(void)
{
   // Get a pointer to the top layer
   CCellSoilLayer* pLayer = m_pCell->pGetSoil()->pLayerGetLayer(0);

   return pLayer->dGetSoilWater();
}
