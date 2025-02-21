/*=========================================================================================================================================

This is cell_subsurface_water.cpp: implementations of the RillGrow class used to represent subsurface water

Copyright (C) 2025 David Favis-Mortlock

==========================================================================================================================================

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
#include "rg.h"
#include "cell.h"
#include "cell_subsurface_water.h"

//! Constructor with initialization list
CCellSubsurfaceWater::CCellSubsurfaceWater(void)
:
   m_dEndOfIterInfiltWater(0),
   m_dCumulInfiltWater(0),
   m_dEndOfIterExfiltWater(0),
   m_dCumulExfiltWater(0)
{
   m_pCell = NULL;
}

//! Destructor
CCellSubsurfaceWater::~CCellSubsurfaceWater(void)
{
}

//! Sets the pointer to the parent cell object
void CCellSubsurfaceWater::SetParent(CCell* const pParent)
{
   m_pCell = pParent;
}

//! Loses surface water to infiltration into the top soil layer. If there is insufficient surface water depth to remove the depth specified by the parameter, the value of the parameter is reduced
void CCellSubsurfaceWater::DoInfiltration(double& dInfilt)
{
   m_pCell->pGetSurfaceWater()->RemoveSurfaceWater(dInfilt);

   // Get a pointer to the top layer
   CCellSoilLayer* pLayer = m_pCell->pGetSoil()->pLayerGetLayer(0);

   // And add water to this layer
   pLayer->ChangeSoilWater(dInfilt);

   m_dEndOfIterInfiltWater += dInfilt;
   m_dCumulInfiltWater += dInfilt;
}

//! Loses all this cell's surface water to infiltration into the top soil layer, and increases the cell's elevation with any sediment that was being transported
void CCellSubsurfaceWater::InfiltrateAndMakeDry(double& dClaySediment, double& dSiltSediment, double& dSandSediment)
{
   double dWaterDepth = m_pCell->pGetSurfaceWater()->dGetSurfaceWaterDepth();

   dClaySediment = m_pCell->pGetSedLoad()->dGetLastIterClaySedLoad(),
   dSiltSediment = m_pCell->pGetSedLoad()->dGetLastIterSiltSedLoad(),
   dSandSediment = m_pCell->pGetSedLoad()->dGetLastIterSandSedLoad();

   // First remove the surface water (also decrements the surface water total, and count of wet cells)
   m_pCell->pGetSurfaceWater()->SetSurfaceWaterZero();

   // Get a pointer to the top layer
   CCellSoilLayer* pLayer = m_pCell->pGetSoil()->pLayerGetLayer(0);

   // And add water to this layer
   pLayer->ChangeSoilWater(dWaterDepth);

   m_dEndOfIterInfiltWater += dWaterDepth;
   m_dCumulInfiltWater += dWaterDepth;

   // And then sort out the sediment
   m_pCell->pGetSoil()->DoInfiltrationDeposit(dClaySediment, dSiltSediment, dSandSediment);
}

//! Zeros the values for water lost to infiltration and sediment deposited due to infiltration
void CCellSubsurfaceWater::InitializeInfiltration(void)
{
   m_dEndOfIterInfiltWater =
   m_dEndOfIterExfiltWater = 0;
   m_pCell->pGetSoil()->SetInfiltrationDepositionZero();
}

//! Returns the this-iteration depth of water lost by infiltration
double CCellSubsurfaceWater::dGetThisIterInfiltration(void) const
{
   return m_dEndOfIterInfiltWater;
}

//! Returns the cumulative depth of water lost by infiltration
double CCellSubsurfaceWater::dGetCumulInfiltration(void) const
{
   return m_dCumulInfiltWater;
}

//! Exfiltrates i.e. sends soil water out of the top soil layer, to become overland flow
void CCellSubsurfaceWater::DoExfiltration(double const dExcess)
{
   // Get a pointer to the top layer
   CCellSoilLayer* pLayer = m_pCell->pGetSoil()->pLayerGetLayer(0);

   pLayer->ChangeSoilWater(-dExcess);
   m_pCell->pGetSurfaceWater()->AddSurfaceWater(dExcess);
}

//! Returns the depth of exfiltrated water
double CCellSubsurfaceWater::dGetExfiltration(void) const
{
   return m_dEndOfIterExfiltWater;
}

//! Returns the cumulative depth of exfiltrated water
double CCellSubsurfaceWater::dGetCumulExfiltration(void) const
{
   return m_dCumulExfiltWater;
}

//! Returns the depth of soil water in the top soil layer
double CCellSubsurfaceWater::dGetTopLayerSoilWater(void)
{
   // Get a pointer to the top layer
   CCellSoilLayer const* pLayer = m_pCell->pGetSoil()->pLayerGetLayer(0);

   return pLayer->dGetSoilWater();
}

//! Returns the total depth of water in all soil layers
double CCellSubsurfaceWater::dGetAllSoilWater(void) const
{
   int nLayer = m_pCell->pGetSoil()->nGetNumLayers();

   double dTotSoilWater = 0;
   for (int n = 0; n < nLayer; n++)
   {
      // Get a pointer to this layer
      CCellSoilLayer const* pLayer = m_pCell->pGetSoil()->pLayerGetLayer(n);

      dTotSoilWater += pLayer->dGetSoilWater();
   }

   return dTotSoilWater;
}
