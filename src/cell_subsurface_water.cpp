/*=========================================================================================================================================

This is m_Cell_subsurface_water.cpp: implementations of the RillGrow class used to represent subsurface water

 Copyright (C) 2023 David Favis-Mortlock

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
   m_dInfiltWater(0),
   m_dCumulInfiltWater(0),
   m_dExfiltWater(0),
   m_dCumulExfiltWater(0)
{
   pCell = NULL;
}

CCellSubsurfaceWater::~CCellSubsurfaceWater(void)
{
}


void CCellSubsurfaceWater::SetParent(CCell* const pParent)
{
   pCell = pParent;
}


// Loses surface water to infiltration
void CCellSubsurfaceWater::DoInfiltration(double& dInfilt)
{
   // If there is insufficient surface water depth to remove the whole of dInfilt, dInfilt gets reduced
   pCell->pGetSurfaceWater()->RemoveSurfaceWater(dInfilt);

   // Get a pointer to the top layer
   CCellSoilLayer* pLayer = pCell->pGetSoil()->pLayerGetLayer(0);

   // And add water to this layer
   pLayer->ChangeSoilWater(dInfilt);

   m_dInfiltWater += dInfilt;
   m_dCumulInfiltWater += dInfilt;
}

// Loses all this cell's surface water to infilt, and increases the cell's elevation with any sediment that was being transported
void CCellSubsurfaceWater::InfiltrateAndMakeDry(double& dClaySediment, double& dSiltSediment, double& dSandSediment)
{
   double dWaterDepth = pCell->pGetSurfaceWater()->dGetSurfaceWaterDepth();

   dClaySediment = pCell->pGetSedLoad()->dGetClaySedLoad(),
   dSiltSediment = pCell->pGetSedLoad()->dGetSiltSedLoad(),
   dSandSediment = pCell->pGetSedLoad()->dGetSandSedLoad();

   // First remove the surface water (also decrements the surface water total, and count of wet m_Cells)
   pCell->pGetSurfaceWater()->SetSurfaceWaterZero();

   // Get a pointer to the top layer
   CCellSoilLayer* pLayer = pCell->pGetSoil()->pLayerGetLayer(0);

   // And add water to this layer
   pLayer->ChangeSoilWater(dWaterDepth);

   m_dInfiltWater      += dWaterDepth;
   m_dCumulInfiltWater += dWaterDepth;

   // And then sort out the sediment
   pCell->pGetSoil()->DoInfiltrationDeposit(dClaySediment, dSiltSediment, dSandSediment);
}

// Zeros the values (they are kept over several iterations) values for water lost to infilt and sediment deposited due to infilt
void CCellSubsurfaceWater::InitializeInfiltration(void)
{
   m_dInfiltWater =
   m_dExfiltWater = 0;
   pCell->pGetSoil()->SetInfiltrationDepositionZero();
}

// Returns the depth of water lost by infilt
double CCellSubsurfaceWater::dGetInfiltration(void) const
{
   return m_dInfiltWater;
}

// Returns the cumulative depth of water lost by infilt
double CCellSubsurfaceWater::dGetCumulInfiltration(void) const
{
   return m_dCumulInfiltWater;
}

// Sends soil water to overland flow
void CCellSubsurfaceWater::DoExfiltration(double const dExcess)
{
   // Get a pointer to the top layer
   CCellSoilLayer* pLayer = pCell->pGetSoil()->pLayerGetLayer(0);

   pLayer->ChangeSoilWater(-dExcess);
   pCell->pGetSurfaceWater()->AddSurfaceWater(dExcess);
}

// Returns the depth of exfiltrated water
double CCellSubsurfaceWater::dGetExfiltration(void) const
{
   return m_dExfiltWater;
}

// Returns the cumulative depth of exfiltrated water
double CCellSubsurfaceWater::dGetCumulExfiltration(void) const
{
   return m_dCumulExfiltWater;
}



// Returns the depth of soil water in the top soil layer
double CCellSubsurfaceWater::dGetTopLayerSoilWater(void)
{
   // Get a pointer to the top layer
   CCellSoilLayer* pLayer = pCell->pGetSoil()->pLayerGetLayer(0);

   return pLayer->dGetSoilWater();
}
