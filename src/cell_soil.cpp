/*=========================================================================================================================================

 This is cell_soil.cpp: implementations of the RillGrow class used to represent the soil column

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
#include "cell_soil.h"


CCellSoil::CCellSoil(void)
:
   m_dClayFlowDetach(0),
   m_dSiltFlowDetach(0),
   m_dSandFlowDetach(0),
   m_dCumulClayFlowDetach(0),
   m_dCumulSiltFlowDetach(0),
   m_dCumulSandFlowDetach(0),
   m_dClayFlowDeposit(0),
   m_dSiltFlowDeposit(0),
   m_dSandFlowDeposit(0),
   m_dCumulClayFlowDeposit(0),
   m_dCumulSiltFlowDeposit(0),
   m_dCumulSandFlowDeposit(0),
   m_dClaySplashDetach(0),
   m_dSiltSplashDetach(0),
   m_dSandSplashDetach(0),
   m_dCumulClaySplashDetach(0),
   m_dCumulSiltSplashDetach(0),
   m_dCumulSandSplashDetach(0),
   m_dTemporarySplashDeposit(0),
   m_dClaySplashDeposit(0),
   m_dSiltSplashDeposit(0),
   m_dSandSplashDeposit(0),
   m_dCumulClaySplashDeposit(0),
   m_dCumulSiltSplashDeposit(0),
   m_dCumulSandSplashDeposit(0),
   m_dClaySlumpDetach(0),
   m_dSiltSlumpDetach(0),
   m_dSandSlumpDetach(0),
   m_dCumulClaySlumpDetach(0),
   m_dCumulSiltSlumpDetach(0),
   m_dCumulSandSlumpDetach(0),
   m_dClaySlumpDeposit(0),
   m_dSiltSlumpDeposit(0),
   m_dSandSlumpDeposit(0),
   m_dCumulClaySlumpDeposit(0),
   m_dCumulSiltSlumpDeposit(0),
   m_dCumulSandSlumpDeposit(0),
   m_dClayToppleDetach(0),
   m_dSiltToppleDetach(0),
   m_dSandToppleDetach(0),
   m_dCumulClayToppleDetach(0),
   m_dCumulSiltToppleDetach(0),
   m_dCumulSandToppleDetach(0),
   m_dClayToppleDeposit(0),
   m_dSiltToppleDeposit(0),
   m_dSandToppleDeposit(0),
   m_dCumulClayToppleDeposit(0),
   m_dCumulSiltToppleDeposit(0),
   m_dCumulSandToppleDeposit(0),
   m_dClayInfiltDeposit(0),
   m_dSiltInfiltDeposit(0),
   m_dSandInfiltDeposit(0),
   m_dCumulClayInfiltDeposit(0),
   m_dCumulSiltInfiltDeposit(0),
   m_dCumulSandInfiltDeposit(0),
   m_dShearStress(0),
   m_dCumulShearStress(0),
   m_dLaplacian(0),
   m_dClayHeadcutRetreatDetach(0),
   m_dSiltHeadcutRetreatDetach(0),
   m_dSandHeadcutRetreatDetach(0),
   m_dCumulClayHeadcutRetreatDetach(0),
   m_dCumulSiltHeadcutRetreatDetach(0),
   m_dCumulSandHeadcutRetreatDetach(0),
   m_dClayHeadcutRetreatDeposit(0),
   m_dSiltHeadcutRetreatDeposit(0),
   m_dSandHeadcutRetreatDeposit(0),
   m_dCumulClayHeadcutRetreatDeposit(0),
   m_dCumulSiltHeadcutRetreatDeposit(0),
   m_dCumulSandHeadcutRetreatDeposit(0)

{
   m_pCell = NULL;
}

CCellSoil::~CCellSoil(void)
{
}


void CCellSoil::SetParent(CCell* const pParent)
{
   m_pCell = pParent;
}

CCellSoilLayer* CCellSoil::pLayerGetLayer(int const nLayer)
{
   return &m_VLayer[nLayer];
}

// Sets up the soil layers
void CCellSoil::SetSoilLayers(double const dTotalDepth, int const nLayers, vector<string> const* pVstrInputSoilLayerName, vector<double> const* pVdInputSoilLayerThickness, vector<double> const* pVdInputSoilLayerPerCentClay, vector<double> const* pVdInputSoilLayerPerCentSilt, vector<double> const* pVdInputSoilLayerPerCentSand, vector<double> const* pVdInputSoilLayerBulkDensity, vector<double> const* pVdInputSoilLayerClayFlowErodibility, vector<double> const* pVdInputSoilLayerSiltFlowErodibility, vector<double> const* pVdInputSoilLayerSandFlowErodibility, vector<double> const* pVdInputSoilLayerClaySplashErodibility, vector<double> const* pVdInputSoilLayerSiltSplashErodibility, vector<double> const* pVdInputSoilLayerSandSplashErodibility, vector<double> const* pVdInputSoilLayerClaySlumpErodibility, vector<double> const* pVdInputSoilLayerSiltSlumpErodibility, vector<double> const* pVdInputSoilLayerSandSlumpErodibility, vector<double>* pVdInfiltCPHWF, vector<double>* pVdInfiltChiPart)
{
   double dDepthRemaining = dTotalDepth;

   for (int nLay = nLayers-1; nLay >= 0; nLay--)
   {
      CCellSoilLayer layerNew;
      m_VLayer.push_back(layerNew);

      m_VLayer.back().SetName(&pVstrInputSoilLayerName->at(nLay));

      double
         dLayerThickness = pVdInputSoilLayerThickness->at(nLay),
         dClayThickness,
         dSiltThickness,
         dSandThickness;

      if (nLay > 0)
      {
         // Soil layers below the top layer
         dClayThickness = dLayerThickness * pVdInputSoilLayerPerCentClay->at(nLay) / 100,
         dSiltThickness = dLayerThickness * pVdInputSoilLayerPerCentSilt->at(nLay) / 100,
         dSandThickness = dLayerThickness * pVdInputSoilLayerPerCentSand->at(nLay) / 100;
      }
      else
      {
         // The top layer of soil
         dClayThickness = dDepthRemaining * pVdInputSoilLayerPerCentClay->at(nLay) / 100,
         dSiltThickness = dDepthRemaining * pVdInputSoilLayerPerCentSilt->at(nLay) / 100,
         dSandThickness = dDepthRemaining * pVdInputSoilLayerPerCentSand->at(nLay) / 100;
      }

      m_VLayer.back().SetClayThickness(dClayThickness);
      m_VLayer.back().SetSiltThickness(dSiltThickness);
      m_VLayer.back().SetSandThickness(dSandThickness);

      dDepthRemaining -= (dClayThickness + dSiltThickness + dSandThickness);

      m_VLayer.back().SetBulkDensity(pVdInputSoilLayerBulkDensity->at(nLay));             // Note is in kg/m**3

      m_VLayer.back().SetClayFlowErodibility(pVdInputSoilLayerClayFlowErodibility->at(nLay));
      m_VLayer.back().SetSiltFlowErodibility(pVdInputSoilLayerSiltFlowErodibility->at(nLay));
      m_VLayer.back().SetSandFlowErodibility(pVdInputSoilLayerSandFlowErodibility->at(nLay));

      m_VLayer.back().SetClaySplashErodibility(pVdInputSoilLayerClaySplashErodibility->at(nLay));
      m_VLayer.back().SetSiltSplashErodibility(pVdInputSoilLayerSiltSplashErodibility->at(nLay));
      m_VLayer.back().SetSandSplashErodibility(pVdInputSoilLayerSandSplashErodibility->at(nLay));

      m_VLayer.back().SetClaySlumpErodibility(pVdInputSoilLayerClaySlumpErodibility->at(nLay));
      m_VLayer.back().SetSiltSlumpErodibility(pVdInputSoilLayerSiltSlumpErodibility->at(nLay));
      m_VLayer.back().SetSandSlumpErodibility(pVdInputSoilLayerSandSlumpErodibility->at(nLay));

      // Also create these all-cell infilt values
      pVdInfiltCPHWF->push_back(0);
      pVdInfiltChiPart->push_back(0);
   }
}

double CCellSoil::dGetSoilSurfaceElevation(void)
{
   double dSoilSurfaceTop = m_pCell->dGetBasementElevation();

   for (unsigned int nLay = 0; nLay < m_VLayer.size(); nLay++)
      dSoilSurfaceTop += m_VLayer[nLay].dGetLayerThickness();

   return dSoilSurfaceTop;
}


// Returns the bulk density (in kg/m**3) of the topmost layer with non-zero thickness. If there are no layers with non-zero thickness (i.e. we are down to unerodible basement) returns -1
double CCellSoil::dGetBulkDensityOfTopNonZeroLayer(void)
{
   for (unsigned int nLay = 0; nLay < m_VLayer.size(); nLay++)
   {
      if (m_VLayer[nLay].dGetLayerThickness() > 0)
      {
         return m_VLayer[nLay].dGetBulkDensity();
      }
   }
   return -1;
}


// Changes the thickness of the topmost soil layer
void CCellSoil::ChangeTopLayerThickness(double const dChange)
{
   m_VLayer[0].ChangeThickness(dChange);
}


// Do supply-limited detachment from surface water on this cell
void CCellSoil::DoFlowDetach(double const dDepthToErode)
{
   double
      dTotClayEroded = 0,
      dTotSiltDetached = 0,
      dTotSandDetached = 0;

   for (unsigned int nLayer = 0; nLayer < m_VLayer.size(); nLayer++)
   {
      CCellSoilLayer* pLayer = pLayerGetLayer(nLayer);

      double
         dClayDetached = 0,
         dSiltDetached = 0,
         dSandDetached = 0;

      pLayer->DoLayerFlowErosion(dDepthToErode, dClayDetached, dSiltDetached, dSandDetached);

      dTotClayEroded += dClayDetached;
      dTotSiltDetached += dSiltDetached;
      dTotSandDetached += dSandDetached;

      if (bFPIsEqual(dClayDetached + dSiltDetached + dSandDetached, dDepthToErode, SEDIMENT_TOLERANCE))
      {
         // We have eroded enough
         break;
      }
   }

   // Add to the cell's sediment load
   m_pCell->pGetSedLoad()->AddToSedLoad(dTotClayEroded, dTotSiltDetached, dTotSandDetached);

   // Add to the cell's flow detachment totals
   m_dClayFlowDetach += dTotClayEroded;
   m_dSiltFlowDetach += dTotSiltDetached;
   m_dSandFlowDetach += dTotSandDetached;
   m_dCumulClayFlowDetach += dTotClayEroded;
   m_dCumulSiltFlowDetach += dTotSiltDetached;
   m_dCumulSandFlowDetach += dTotSandDetached;
}

// Do deposition from surface water on this cell, constraining if the full amount cannot be deposited
void CCellSoil::DoSedLoadDeposit(double const dClayFraction, double const dSiltFraction, double const dSandFraction)
{
   double
      dClaySedLoad = m_pCell->pGetSedLoad()->dGetClaySedLoad(),
      dSiltSedLoad = m_pCell->pGetSedLoad()->dGetSiltSedLoad(),
      dSandSedLoad = m_pCell->pGetSedLoad()->dGetSandSedLoad(),
      dClayToDeposit = dClayFraction * dClaySedLoad,
      dSiltToDeposit = dSiltFraction * dSiltSedLoad,
      dSandToDeposit = dSandFraction * dSandSedLoad;

   // Do the deposition TODO Should we always deposit to the top layer? What about if top layer has been eroded to zero thickness?
   m_VLayer.back().DoLayerDeposition(dClayToDeposit, dSiltToDeposit, dSandToDeposit);

   if (dClayToDeposit > 0)
   {
      // Add to the this-cell totals
      m_dClayFlowDeposit += dClayToDeposit;
      m_dCumulClayFlowDeposit += dClayToDeposit;

      // Remove from the cell's surface water sediment load, also from the this-iteration sediment load
      m_pCell->pGetSedLoad()->RemoveFromClaySedLoad(dClayToDeposit);
   }

   if (dSiltToDeposit > 0)
   {
      // Add to the this-cell totals
      m_dSiltFlowDeposit += dSiltToDeposit;
      m_dCumulSiltFlowDeposit += dSiltToDeposit;

      // Remove from the cell's surface water sediment load, also from the this-iteration sediment load
      m_pCell->pGetSedLoad()->RemoveFromSiltSedLoad(dSiltToDeposit);
   }

   if (dSandToDeposit > 0)
   {
      // Add to the this-cell totals
      m_dSandFlowDeposit += dSandToDeposit;
      m_dCumulSandFlowDeposit += dSandToDeposit;

      // Remove from the cell's surface water sediment load, also from the this-iteration sediment load
      m_pCell->pGetSedLoad()->RemoveFromSandSedLoad(dSandToDeposit);
   }
}

// Returns the this-iteration clay-sized surface water detachment for this cell
double CCellSoil::dGetClayFlowDetach(void) const
{
   return m_dClayFlowDetach;
}

// Returns the this-iteration silt-sized surface water detachment for this cell
double CCellSoil::dGetSiltFlowDetach(void) const
{
   return m_dSiltFlowDetach;
}

// Returns the this-iteration sand-sized surface water detachment for this cell
double CCellSoil::dGetSandFlowDetach(void) const
{
   return m_dSandFlowDetach;
}

// Returns the this-iteration surface water detachment (all sediment sizes) for this cell
double CCellSoil::dGetTotFlowDetach(void) const
{
   return m_dClayFlowDetach + m_dSiltFlowDetach + m_dSandFlowDetach;
}

// Returns the cumulative clay-sized surface water detachment for this cell
double CCellSoil::dGetCumulClayFlowDetach(void) const
{
   return m_dCumulClayFlowDetach;
}

// Returns the cumulative silt-sized surface water detachment for this cell
double CCellSoil::dGetCumulSiltFlowDetach(void) const
{
   return m_dCumulSiltFlowDetach;
}

// Returns the cumulative sand-sized surface water detachment for this cell
double CCellSoil::dGetCumulSandFlowDetach(void) const
{
   return m_dCumulSandFlowDetach;
}

// Returns the cumulative surface water detachment (all sediment sizes) for this cell
double CCellSoil::dGetCumulAllSizeFlowDetach(void) const
{
   return m_dCumulClayFlowDetach + m_dCumulSiltFlowDetach + m_dCumulSandFlowDetach;
}

// Returns the this-iteration clay-sized surface water deposition on this cell
double CCellSoil::dGetClayFlowDeposit(void) const
{
   return m_dClayFlowDeposit;
}

// Returns the this-iteration silt-sized surface water deposition on this cell
double CCellSoil::dGetSiltFlowDeposit(void) const
{
   return m_dSiltFlowDeposit;
}

// Returns the this-iteration sand-sized surface water deposition on this cell
double CCellSoil::dGetSandFlowDeposit(void) const
{
   return m_dSandFlowDeposit;
}

// Returns the this-iteration depth of surface water deposition (all size classes) for this cell
double CCellSoil::dGetAllSizeFlowDeposit(void) const
{
   return (m_dClayFlowDeposit + m_dSiltFlowDeposit + m_dSandFlowDeposit);
}

// Returns the cumulative clay-sized surface water deposition on this cell
double CCellSoil::dGetCumulClayFlowDeposit(void) const
{
   return m_dCumulClayFlowDeposit;
}

// Returns the cumulative silt-sized surface water deposition on this cell
double CCellSoil::dGetCumulSiltFlowDeposit(void) const
{
   return m_dCumulSiltFlowDeposit;
}

// Returns the cumulative sand-sized surface water deposition on this cell
double CCellSoil::dGetCumulSandFlowDeposit(void) const
{
   return m_dCumulSandFlowDeposit;
}

// Returns the cumulative depth of surface water deposition (all size classes) for this cell
double CCellSoil::dGetCumulAllSizeFlowDeposit(void) const
{
   return (m_dCumulClayFlowDeposit + m_dCumulSiltFlowDeposit + m_dCumulSandFlowDeposit);
}


// Decreases this cell's elevation as a result of splash detachment
void CCellSoil::DoSplashDetach(double const dThickness, double& dTotClayEroded, double& dTotSiltEroded,  double& dTotSandEroded)
{
   dTotClayEroded =
   dTotSiltEroded =
   dTotSandEroded = 0;

   for (unsigned int nLayer = 0; nLayer < m_VLayer.size(); nLayer++)
   {
      CCellSoilLayer* pLayer = pLayerGetLayer(nLayer);

      double
         dClayDetached = 0,
         dSiltDetached = 0,
         dSandDetached = 0;

      pLayer->DoLayerSplashErosion(dThickness, dClayDetached, dSiltDetached, dSandDetached);

      dTotClayEroded += dClayDetached;
      dTotSiltEroded += dSiltDetached;
      dTotSandEroded += dSandDetached;

      if (bFPIsEqual(dClayDetached + dSiltDetached + dSandDetached, dThickness, SEDIMENT_TOLERANCE))
      {
         // We have eroded enough
         break;
      }
   }

   // Add to the this-cell totals
   m_dClaySplashDetach    += dTotClayEroded;
   m_dSiltSplashDetach    += dTotSiltEroded;
   m_dSandSplashDetach    += dTotSandEroded;
   m_dCumulClaySplashDetach += dTotClayEroded;
   m_dCumulSiltSplashDetach += dTotSiltEroded;
   m_dCumulSandSplashDetach += dTotSandEroded;
}

// Set the temporary splash deposition field for this cell
void CCellSoil::SetSplashDepositTemp(double const dTemp)
{
   m_dTemporarySplashDeposit = dTemp;
}

// Returns the value of the splash deposition temporary field
double CCellSoil::dGetSplashDepositTemp(void) const
{
   return m_dTemporarySplashDeposit;
}

// If the cell is dry, increases this cell's elevation as a result of splash deposition; or if it is wet, add to this cell's sediment load
void CCellSoil::DoSplashToSedLoadOrDeposit(double const dClayChangeElev, double const dSiltChangeElev, double const dSandChangeElev)
{

   if (m_pCell->pGetSurfaceWater()->bIsWet())
   {
      // Cell is wet, so add to its sediment load
      m_pCell->pGetSedLoad()->AddToSedLoad(dClayChangeElev, dSiltChangeElev, dSandChangeElev);
      m_pCell->pGetSedLoad()->AddToSplashSedLoad(dClayChangeElev, dSiltChangeElev, dSandChangeElev);
   }
   else
   {
      // Cell is dry, so do deposition
      // TODO Should we always deposit to the top layer? What about if top layer has been eroded to zero thickness?
      m_VLayer.back().DoLayerDeposition(dClayChangeElev, dSiltChangeElev, dSandChangeElev);

      // Update this-cell totals for splash deposition
      m_dClaySplashDeposit    += dClayChangeElev;
      m_dCumulClaySplashDeposit += dClayChangeElev;
      m_dSiltSplashDeposit    += dSiltChangeElev;
      m_dCumulSiltSplashDeposit += dSiltChangeElev;
      m_dSandSplashDeposit    += dSandChangeElev;
      m_dCumulSandSplashDeposit += dSandChangeElev;
   }
}

// Returns the depth of clay-sized splash detachment for this iteration on this cell
double CCellSoil::dGetClaySplashDetach(void) const
{
   return m_dClaySplashDetach;
}

// Returns the depth of silt-sized splash detachment for this iteration on this cell
double CCellSoil::dGetSiltSplashDetach(void) const
{
   return m_dSiltSplashDetach;
}

// Returns the depth of sand-sized splash detachment for this iteration on this cell
double CCellSoil::dGetSandSplashDetach(void) const
{
   return m_dSandSplashDetach;
}

// Returns the depth of splash detachment (all sediment size classes) for this iteration on this cell
double CCellSoil::dGetAllSizeSplashDetach(void) const
{
   return (m_dClaySplashDetach + m_dSiltSplashDetach + m_dSandSplashDetach);
}


// Returns the depth of clay-sized splash deposition for for this iteration on this cell
double CCellSoil::dGetClaySplashDeposit(void) const
{
   return m_dClaySplashDeposit;
}

// Returns the depth of silt-sized splash deposition for for this iteration on this cell
double CCellSoil::dGetSiltSplashDeposit(void) const
{
   return m_dSiltSplashDeposit;
}

// Returns the depth of sand-sized splash deposition for for this iteration on this cell
double CCellSoil::dGetSandSplashDeposit(void) const
{
   return m_dSandSplashDeposit;
}

// Returns the depth of splash deposition (all sediment size classes) for for this iteration on this cell
double CCellSoil::dGetAllSizeSplashDeposit(void) const
{
   return (m_dClaySplashDeposit + m_dSiltSplashDeposit + m_dSandSplashDeposit);
}


// Returns the cumulative depth of clay-sized splash detachment for this cell
double CCellSoil::dGetCumulClaySplashDetach(void) const
{
   return m_dCumulClaySplashDetach;
}

// Returns the cumulative depth of silt-sized splash detachment for this cell
double CCellSoil::dGetCumulSiltSplashDetach(void) const
{
   return m_dCumulSiltSplashDetach;
}

// Returns the cumulative depth of sand-sized splash detachment for this cell
double CCellSoil::dGetCumulSandSplashDetach(void) const
{
   return m_dCumulSandSplashDetach;
}

// Returns the cumulative depth of splash detachment (all sediment size classes) for this cell
double CCellSoil::dGetCumulAllSizeSplashDetach(void) const
{
   return (m_dCumulClaySplashDetach + m_dCumulSiltSplashDetach + m_dCumulSandSplashDetach);
}


// Returns the cumulative depth of clay-sized splash deposition for this cell
double CCellSoil::dGetCumulClaySplashDeposit(void) const
{
   return m_dCumulClaySplashDeposit;
}

// Returns the cumulative depth of silt-sized splash deposition for this cell
double CCellSoil::dGetCumulSiltSplashDeposit(void) const
{
   return m_dCumulSiltSplashDeposit;
}

// Returns the cumulative depth of sand-sized splash deposition for this cell
double CCellSoil::dGetCumulSandSplashDeposit(void) const
{
   return m_dCumulSandSplashDeposit;
}

// Returns the cumulative depth of splash deposition (all sediment size classes) for this cell
double CCellSoil::dGetCumulAllSizeSplashDeposit(void) const
{
   return (m_dCumulClaySplashDeposit + m_dCumulSiltSplashDeposit + m_dCumulSandSplashDeposit);
}


// Decreases this cell's elevation as a result of slumping
void CCellSoil::DoSlumpDetach(double const dThickness, double& dTotClayDetached, double& dTotSiltDetached, double& dTotSandDetached)
{
   for (unsigned int nLayer = 0; nLayer < m_VLayer.size(); nLayer++)
   {
      CCellSoilLayer* pLayer = pLayerGetLayer(nLayer);

      double
         dClayDetached = 0,
         dSiltDetached = 0,
         dSandDetached = 0;

      pLayer->DoLayerSlumpErosion(dThickness, dClayDetached, dSiltDetached, dSandDetached);

      dTotClayDetached += dClayDetached;
      dTotSiltDetached += dSiltDetached;
      dTotSandDetached += dSandDetached;

      if (bFPIsEqual(dClayDetached + dSiltDetached + dSandDetached, dThickness, SEDIMENT_TOLERANCE))
      {
         // We have eroded enough
         break;
      }
   }

   // Add to this-cell totals
   m_dClaySlumpDetach += dTotClayDetached;
   m_dSiltSlumpDetach += dTotSiltDetached;
   m_dSandSlumpDetach += dTotSandDetached;
   m_dCumulClaySlumpDetach += dTotClayDetached;
   m_dCumulSiltSlumpDetach += dTotSiltDetached;
   m_dCumulSandSlumpDetach += dTotSandDetached;
}


// Adds sediment to this cell as a result of slump: to the top layer of the soil if tdry, or to the sediment load if wet
void CCellSoil::DoSlumpDepositOrToSedLoad(double const dClayChngElev, double const dSiltChngElev, double const dSandChngElev, double& dClayDeposit, double& dSiltDeposit, double& dSandDeposit, double& dClayToSedLoad, double& dSiltToSedLoad, double& dSandToSedLoad)
{
   if (m_pCell->pGetSurfaceWater()->bIsWet())
   {
      // Cell is wet, so add to its sediment load
      m_pCell->pGetSedLoad()->AddToSedLoad(dClayChngElev, dSiltChngElev, dSandChngElev);
      m_pCell->pGetSedLoad()->AddToSlumpSedLoad(dClayChngElev, dSiltChngElev, dSandChngElev);

      dClayToSedLoad = dClayChngElev;
      dSiltToSedLoad = dSiltChngElev;
      dSandToSedLoad = dSandChngElev;
   }
   else
   {
      // Do the deposition
      // TODO Should we always deposit to the top layer? What about if top layer has been eroded to zero thickness?
      m_VLayer.back().DoLayerDeposition(dClayChngElev, dSiltChngElev, dSandChngElev);

      // Add to this-cell totals
      m_dClaySlumpDeposit += dClayChngElev;
      m_dSiltSlumpDeposit += dSiltChngElev;
      m_dSandSlumpDeposit += dSandChngElev;
      m_dCumulClaySlumpDeposit += dClayChngElev;
      m_dCumulSiltSlumpDeposit += dSiltChngElev;
      m_dCumulSandSlumpDeposit += dSandChngElev;

      dClayDeposit = dClayChngElev;
      dSiltDeposit = dSiltChngElev;
      dSandDeposit = dSandChngElev;
   }
}

// Returns the this-iteration depth of clay-sized slump detachment for this cell
double CCellSoil::dGetClaySlumpDetach(void) const
{
   return m_dClaySlumpDetach;
}

// Returns the this-iteration depth of silt-sized slump detachment for this cell
double CCellSoil::dGetSiltSlumpDetach(void) const
{
   return m_dSiltSlumpDetach;
}

// Returns the this-iteration depth of sand-sized slump detachment for this cell
double CCellSoil::dGetSandSlumpDetach(void) const
{
   return m_dSandSlumpDetach;
}

// Returns the this-iteration depth of slump detachment (all size classes) for this cell
double CCellSoil::dGetAllSizeSlumpDetach(void) const
{
   return (m_dClaySlumpDetach + m_dSiltSlumpDetach + m_dSandSlumpDetach);
}


// Returns the cumulative depth of clay-sized slump detachment for this cell
double CCellSoil::dGetCumulClaySlumpDetach(void) const
{
   return m_dCumulClaySlumpDetach;
}

// Returns the cumulative depth of silt-sized slump detachment for this cell
double CCellSoil::dGetCumulSiltSlumpDetach(void) const
{
   return m_dCumulSiltSlumpDetach;
}

// Returns the cumulative depth of sand-sized slump detachment for this cell
double CCellSoil::dGetCumulSandSlumpDetach(void) const
{
   return m_dCumulSandSlumpDetach;
}

// Returns the cumulative depth of slump detachment (all size classes) for this cell
double CCellSoil::dGetCumulAllSizeSlumpDetach(void) const
{
   return (m_dCumulClaySlumpDetach + m_dCumulSiltSlumpDetach + m_dCumulSandSlumpDetach);
}

// Returns the this-iteration depth of clay-sized slump deposition for this cell
double CCellSoil::dGetClaySlumpDeposit(void) const
{
   return m_dClaySlumpDeposit;
}

// Returns the this-iteration depth of silt-sized slump deposition for this cell
double CCellSoil::dGetSiltSlumpDeposit(void) const
{
   return m_dSiltSlumpDeposit;
}

// Returns the this-iteration depth of sand-sized slump deposition for this cell
double CCellSoil::dGetSandSlumpDeposit(void) const
{
   return m_dSandSlumpDeposit;
}

// Returns the cumulative depth of clay-sized slump deposition for this cell
double CCellSoil::dGetCumulClaySlumpDeposit(void) const
{
   return m_dCumulClaySlumpDeposit;
}

// Returns the cumulative depth of silt-sized slump deposition for this cell
double CCellSoil::dGetCumulSiltSlumpDeposit(void) const
{
   return m_dCumulSiltSlumpDeposit;
}

// Returns the cumulative depth of sand-sized slump deposition for this cell
double CCellSoil::dGetCumulSandSlumpDeposit(void) const
{
   return m_dCumulSandSlumpDeposit;
}

// Returns the cumulative depth of slump deposition (all size classes) for this cell
double CCellSoil::dGetCumulAllSizeSlumpDeposit(void) const
{
   return (m_dCumulClaySlumpDeposit + m_dCumulSiltSlumpDeposit + m_dCumulSandSlumpDeposit);
}


// Decreases this cell's elevation as a result of toppling
void CCellSoil::DoToppleDetach(double const dThickness, double& dTotClayDetached, double& dTotSiltDetached, double& dTotSandDetached)
{
   for (unsigned int nLayer = 0; nLayer < m_VLayer.size(); nLayer++)
   {
      CCellSoilLayer* pLayer = pLayerGetLayer(nLayer);

      double
         dClayDetached = 0,
         dSiltDetached = 0,
         dSandDetached = 0;

      pLayer->DoLayerToppleErosion(dThickness, dClayDetached, dSiltDetached, dSandDetached);

      dTotClayDetached += dClayDetached;
      dTotSiltDetached += dSiltDetached;
      dTotSandDetached += dSandDetached;

      if (bFPIsEqual(dClayDetached + dSiltDetached + dSandDetached, dThickness, SEDIMENT_TOLERANCE))
      {
         // We have eroded enough
         break;
      }
   }

   // Add to this-cell totals
   m_dClayToppleDetach += dTotClayDetached;
   m_dSiltToppleDetach += dTotSiltDetached;
   m_dSandToppleDetach += dTotSandDetached;
   m_dCumulClayToppleDetach += dTotClayDetached;
   m_dCumulSiltToppleDetach += dTotSiltDetached;
   m_dCumulSandToppleDetach += dTotSandDetached;
}

// Adds sediment to this cell as a result of toppling. If the cell is wet, the sediment goes to the cell's sediment load
void CCellSoil::DoToppleDepositOrToSedLoad(double const dClayChngElev, double const dSiltChngElev,double const dSandChngElev, double& dClayDeposited, double& dSiltDeposited, double& dSandDeposited, double& dClayToSedLoad, double& dSiltToSedLoad, double& dSandToSedLoad)
{
   if (m_pCell->pGetSurfaceWater()->bIsWet())
   {
      // Cell is wet, so add to its sediment load
      m_pCell->pGetSedLoad()->AddToSedLoad(dClayChngElev, dSiltChngElev, dSandChngElev);
      m_pCell->pGetSedLoad()->AddToToppleSedLoad(dClayChngElev, dSiltChngElev, dSandChngElev);

      dClayToSedLoad = dClayChngElev;
      dSiltToSedLoad = dSiltChngElev;
      dSandToSedLoad = dSandChngElev;
   }
   else
   {
      // Do the deposition
      // TODO Should we always deposit to the top layer? What about if top layer has been eroded to zero thickness?
      m_VLayer.back().DoLayerDeposition(dClayChngElev, dSiltChngElev, dSandChngElev);

      // Add to this-cell totals
      m_dClayToppleDeposit    += dClayChngElev;
      m_dCumulClayToppleDeposit += dClayChngElev;
      m_dSiltToppleDeposit    += dSiltChngElev;
      m_dCumulSiltToppleDeposit += dSiltChngElev;
      m_dSandToppleDeposit    += dSandChngElev;
      m_dCumulSandToppleDeposit += dSandChngElev;

      dClayDeposited = dClayChngElev;
      dSiltDeposited = dSiltChngElev;
      dSandDeposited = dSandChngElev;
   }
}

// Returns the depth of clay-sized toppling detachment for this cell
double CCellSoil::dGetClayToppleDetach(void) const
{
   return m_dClayToppleDetach;
}

// Returns the depth of silt-sized toppling detachment for this cell
double CCellSoil::dGetSiltToppleDetach(void) const
{
   return m_dSiltToppleDetach;
}

// Returns the depth of sand-sized toppling detachment for this cell
double CCellSoil::dGetSandToppleDetach(void) const
{
   return m_dSandToppleDetach;
}

// Returns the depth of toppling detachment (all sediment size classes) for this cell
double CCellSoil::dGetAllSizeToppleDetach(void) const
{
   return m_dClayToppleDetach + m_dSiltToppleDetach + m_dSandToppleDetach;
}

// Returns the cumulative depth of clay-sized toppling detachment for this cell
double CCellSoil::dGetCumulClayToppleDetach(void) const
{
   return m_dCumulClayToppleDetach;
}

// Returns the cumulative depth of silt-sized toppling detachment for this cell
double CCellSoil::dGetCumulSiltToppleDetach(void) const
{
   return m_dCumulSiltToppleDetach;
}

// Returns the cumulative depth of sand-sized toppling detachment for this cell
double CCellSoil::dGetCumulSandToppleDetach(void) const
{
   return m_dCumulSandToppleDetach;
}

// Returns the cumulative depth of toppling detachment (all sediment size classes) for this cell
double CCellSoil::dGetCumulAllSizeToppleDetach(void) const
{
   return m_dCumulClayToppleDetach + m_dCumulSiltToppleDetach + m_dCumulSandToppleDetach;
}


// Returns the depth of clay-sized toppling deposition for this cell
double CCellSoil::dGetClayToppleDeposit(void) const
{
   return m_dClayToppleDeposit;
}

// Returns the depth of silt-sized toppling deposition for this cell
double CCellSoil::dGetSiltToppleDeposit(void) const
{
   return m_dSiltToppleDeposit;
}

// Returns the depth of sand-sized toppling deposition for this cell
double CCellSoil::dGetSandToppleDeposit(void) const
{
   return m_dSandToppleDeposit;
}

// Returns the depth of toppling deposition (all size classes) for this cell
double CCellSoil::dGetAllSizeToppleDeposit(void) const
{
   return m_dClayToppleDeposit + m_dSiltToppleDeposit + m_dSandToppleDeposit;
}

// Returns the cumulative depth of clay-sized toppling deposition for this cell
double CCellSoil::dGetCumulClayToppleDeposit(void) const
{
   return m_dCumulClayToppleDeposit;
}

// Returns the cumulative depth of silt-sized toppling deposition for this cell
double CCellSoil::dGetCumulSiltToppleDeposit(void) const
{
   return m_dCumulSiltToppleDeposit;
}

// Returns the cumulative depth of sand-sized toppling deposition for this cell
double CCellSoil::dGetCumulSandToppleDeposit(void) const
{
   return m_dCumulSandToppleDeposit;
}

// Returns the cumulative depth of toppling deposition (all size classes) for this cell
double CCellSoil::dGetCumulAllSizeToppleDeposit(void) const
{
   return m_dCumulClayToppleDeposit + m_dCumulSiltToppleDeposit + m_dCumulSandToppleDeposit;
}

// Zeros the this-iteration (actually they are kept over several iterations) values for slumping, toppling and shear stress
void CCellSoil::ZeroThisOperationSlump(void)
{
   m_dClaySlumpDetach   =
   m_dSiltSlumpDetach   =
   m_dSandSlumpDetach   =
   m_dClayToppleDetach  =
   m_dSiltToppleDetach  =
   m_dSandToppleDetach  =
   m_dClayToppleDeposit =
   m_dSiltToppleDeposit =
   m_dSandToppleDeposit = 0;
}

void CCellSoil::DoInfiltrationDeposit(double const dClayDeposit, double const dSiltDeposit, double const dSandDeposit)
{
   // Cell is dry, so do the deposition
   // TODO Should we always deposit to the top layer? What about if top layer has been eroded to zero thickness?
   m_VLayer.back().DoLayerDeposition(dClayDeposit, dSiltDeposit, dSandDeposit);

   // Add to the cell's totals
   m_dClayInfiltDeposit += dClayDeposit;
   m_dSiltInfiltDeposit += dSiltDeposit;
   m_dSandInfiltDeposit += dSandDeposit;
}

void CCellSoil::SetInfiltrationDepositionZero(void)
{
   m_dClayInfiltDeposit =
   m_dSiltInfiltDeposit =
   m_dSandInfiltDeposit = 0;
}

// Returns the clay-sized sediment that was deposited when all water was lost to infilt
double CCellSoil::dGetClayInfiltDeposit(void) const
{
   return m_dClayInfiltDeposit;
}

// Returns the silt-sized sediment that was deposited when all water was lost to infilt
double CCellSoil::dGetSiltInfiltDeposit(void) const
{
   return m_dSiltInfiltDeposit;
}

// Returns the sand-sized sediment that was deposited when all water was lost to infilt
double CCellSoil::dGetSandInfiltDeposit(void) const
{
   return m_dSandInfiltDeposit;
}

// Returns the sediment (all size classes)  that was deposited when all water was lost to infilt
double CCellSoil::dGetTotInfiltDeposit(void) const
{
   return (m_dClayInfiltDeposit + m_dSiltInfiltDeposit + m_dSandInfiltDeposit);
}

// Returns the cumulative total of clay-sized sediment that was deposited when all water was lost to infilt
double CCellSoil::dGetCumulClayInfiltDeposit(void) const
{
   return m_dCumulClayInfiltDeposit;
}

// Returns the cumulative total of silt-sized sediment that was deposited when all water was lost to infilt
double CCellSoil::dGetCumulSiltInfiltDeposit(void) const
{
   return m_dCumulSiltInfiltDeposit;
}

// Returns the cumulative total of sand-sized sediment that was deposited when all water was lost to infilt
double CCellSoil::dGetCumulSandInfiltDeposit(void) const
{
   return m_dCumulSandInfiltDeposit;
}

// Returns the cumulative total of sediment (all size classes)  that was deposited when all water was lost to infilt
double CCellSoil::dGetCumulTotInfiltDeposit(void) const
{
   return (m_dCumulClayInfiltDeposit + m_dCumulSiltInfiltDeposit + m_dCumulSandInfiltDeposit);
}


// Increments the shear stress and the cumulative shear stress for this cell
void CCellSoil::IncShearStress(double const dNewShearStress)
{
   m_dShearStress += dNewShearStress;
   m_dCumulShearStress += dNewShearStress;
}

// Returns the shear stress for this cell
double CCellSoil::dGetShearStress(void) const
{
   return (m_dShearStress);
}

// Returns the cumulative shear stress for this cell
double CCellSoil::dGetCumulShearStress(void) const
{
   return (m_dCumulShearStress);
}


// Calculates and returns cumulative net soil loss from flow erosion, splash redistribution, slumping, and infilt deposition
double CCellSoil::dGetCumulAllSizeLowering(void) const
{
   double
      dTotFlowDetach = m_dCumulClayFlowDetach + m_dCumulSiltFlowDetach + m_dCumulSandFlowDetach,
      dTotFlowDeposit = m_dCumulClayFlowDeposit + m_dCumulSiltFlowDeposit + m_dCumulSandFlowDeposit,
      dTotSplashDetach = m_dCumulClaySplashDetach + m_dCumulSiltSplashDetach + m_dCumulSandSplashDetach,
      dTotSplashDeposit = m_dCumulClaySplashDeposit + m_dCumulSiltSplashDeposit + m_dCumulSandSplashDeposit,
      dTotSlumpDetach = m_dCumulClaySlumpDetach + m_dCumulSiltSlumpDetach + m_dCumulSandSlumpDetach,
      dTotInfiltDeposit = m_dCumulClayInfiltDeposit + m_dCumulSiltInfiltDeposit + m_dCumulSandInfiltDeposit;

   return (dTotFlowDetach - dTotFlowDeposit + dTotSplashDetach - dTotSplashDeposit + dTotSlumpDetach - dTotInfiltDeposit);
}

// Removes soil (i.e. decreases elevation) from this cell during headcut retreat
void CCellSoil::DoHeadcutRetreatDetach(double const dThickness, double& dTotClayDetached, double& dTotSiltDetached, double& dTotSandDetached)
{
   for (unsigned int nLayer = 0; nLayer < m_VLayer.size(); nLayer++)
   {
      CCellSoilLayer* pLayer = pLayerGetLayer(nLayer);

      double
         dClayDetached = 0,
         dSiltDetached = 0,
         dSandDetached = 0;

      pLayer->DoLayerHeadcutRetreatErosion(dThickness, dClayDetached, dSiltDetached, dSandDetached);

      dTotClayDetached += dClayDetached;
      dTotSiltDetached += dSiltDetached;
      dTotSandDetached += dSandDetached;

      if (bFPIsEqual(dClayDetached + dSiltDetached + dSandDetached, dThickness, SEDIMENT_TOLERANCE))
      {
         // We have eroded enough
         break;
      }
   }

   // Add to this-cell totals
   m_dClayHeadcutRetreatDetach += dTotClayDetached;
   m_dSiltHeadcutRetreatDetach += dTotSiltDetached;
   m_dSandHeadcutRetreatDetach += dTotSandDetached;
   m_dCumulClayHeadcutRetreatDetach += dTotClayDetached;
   m_dCumulSiltHeadcutRetreatDetach += dTotSiltDetached;
   m_dCumulSandHeadcutRetreatDetach += dTotSandDetached;
}

// Adds sediment to this cell as a result of headcut retreat. If the cell is wet, the sediment is added to sediment load
void CCellSoil::DoHeadcutRetreatDepositOrToSedLoad(double const dClayChngElev, double const dSiltChngElev, double const dSandChngElev, double& dClayDeposited, double& dSiltDeposited, double& dSandDeposited, double& dClayToSedLoad, double& dSiltToSedLoad, double& dSandToSedLoad)
{
   if (m_pCell->pGetSurfaceWater()->bIsWet())
   {
      // Cell is wet, so add to its sediment load
      m_pCell->pGetSedLoad()->AddToSedLoad(dClayChngElev, dSiltChngElev, dSandChngElev);
      m_pCell->pGetSedLoad()->AddToHeadcutRetreatSedLoad(dClayChngElev, dSiltChngElev, dSandChngElev);

      dClayToSedLoad = dClayChngElev;
      dSiltToSedLoad = dSiltChngElev;
      dSandToSedLoad = dSandChngElev;
   }
   else
   {
      // Do the deposition
      // TODO Should we always deposit to the top layer? What about if top layer has been eroded to zero thickness?
      m_VLayer.back().DoLayerDeposition(dClayChngElev, dSiltChngElev, dSandChngElev);

      // Add to this-cell totals
      m_dClayHeadcutRetreatDeposit += dClayChngElev;
      m_dSiltHeadcutRetreatDeposit += dSiltChngElev;
      m_dSandHeadcutRetreatDeposit += dSandChngElev;
      m_dCumulClayHeadcutRetreatDeposit += dClayChngElev;
      m_dCumulSiltHeadcutRetreatDeposit += dSiltChngElev;
      m_dCumulSandHeadcutRetreatDeposit += dSandChngElev;

      dClayDeposited = dClayChngElev;
      dSiltDeposited = dSiltChngElev;
      dSandDeposited = dSandChngElev;
   }
}

// Returns this cell's depth of clay soil lost (mm) by headcut retreat
double CCellSoil::dGetClayHeadcutRetreatDetach(void) const
{
   return m_dClayHeadcutRetreatDetach;
}

// Returns this cell's depth of silt soil lost (mm) by headcut retreat
double CCellSoil::dGetSiltHeadcutRetreatDetach(void) const
{
   return m_dSiltHeadcutRetreatDetach;
}

// Returns this cell's depth of sand soil lost (mm) by headcut retreat
double CCellSoil::dGetSandHeadcutRetreatDetach(void) const
{
   return m_dSandHeadcutRetreatDetach;
}

// Returns this cell's cumulative depth of clay soil lost (mm) by headcut retreat
double CCellSoil::dGetCumulClayHeadcutRetreatDetach(void) const
{
   return m_dCumulClayHeadcutRetreatDetach;
}

// Returns this cell's cumulative depth of silt soil lost (mm) by headcut retreat
double CCellSoil::dGetCumulSiltHeadcutRetreatDetach(void) const
{
   return m_dCumulSiltHeadcutRetreatDetach;
}

// Returns this cell's cumulative depth of sand soil lost (mm) by headcut retreat
double CCellSoil::dGetCumulSandHeadcutRetreatDetach(void) const
{
   return m_dCumulSandHeadcutRetreatDetach;
}

// Returns this cell's depth of clay soil gained (mm) by headcut retreat
double CCellSoil::dGetClayHeadcutRetreatDeposit(void) const
{
   return m_dClayHeadcutRetreatDeposit;
}

// Returns this cell's depth of silt soil gained (mm) by headcut retreat
double CCellSoil::dGetSiltHeadcutRetreatDeposit(void) const
{
   return m_dSiltHeadcutRetreatDeposit;
}

// Returns this cell's depth of sand soil gained (mm) by headcut retreat
double CCellSoil::dGetSandHeadcutRetreatDeposit(void) const
{
   return m_dSandHeadcutRetreatDeposit;
}

// Returns this cell's cumulative depth of clay soil gained (mm) by headcut retreat
double CCellSoil::dGetCumulClayHeadcutRetreatDeposit(void) const
{
   return m_dCumulClayHeadcutRetreatDeposit;
}

// Returns this cell's cumulative depth of silt soil gained (mm) by headcut retreat
double CCellSoil::dGetCumulSiltHeadcutRetreatDeposit(void) const
{
   return m_dCumulSiltHeadcutRetreatDeposit;
}

// Returns this cell's cumulative depth of sand soil gained (mm) by headcut retreat
double CCellSoil::dGetCumulSandHeadcutRetreatDeposit(void) const
{
   return m_dCumulSandHeadcutRetreatDeposit;
}


// ============================================================== m_dLaplacian =========================================================
// Sets the Laplacian value for this cell
void CCellSoil::SetLaplacian(double const dNewLapl)
{
   m_dLaplacian = dNewLapl;

   // Also initialize the splash deposition temporary field
   m_dTemporarySplashDeposit = 0;
}

// Returns the Laplacian value for this cell
double CCellSoil::dGetLaplacian(void) const
{
   return (m_dLaplacian);
};


// Sets several this-iteration values to zero
void CCellSoil::InitializeDetachAndDeposit(bool const bSlump)
{
   m_dClayFlowDetach    =
   m_dSiltFlowDetach    =
   m_dSandFlowDetach    =
   m_dClayFlowDeposit   =
   m_dSiltFlowDeposit   =
   m_dSandFlowDeposit   =
   m_dClaySplashDetach  =
   m_dSiltSplashDetach  =
   m_dSandSplashDetach  =
   m_dClaySplashDeposit =
   m_dSiltSplashDeposit =
   m_dSandSplashDeposit =
   m_dLaplacian         = 0;

   m_pCell->pGetSedLoad()->InitializeSplashSedLoads();

   if (bSlump)
   {
      m_dShearStress =
      m_dClaySlumpDetach   =
      m_dSiltSlumpDetach   =
      m_dSandSlumpDetach   =
      m_dClaySlumpDeposit  =
      m_dSiltSlumpDeposit  =
      m_dSandSlumpDeposit  =
      m_dClayToppleDetach  =
      m_dSiltToppleDetach  =
      m_dSandToppleDetach  =
      m_dClayToppleDeposit =
      m_dSiltToppleDeposit =
      m_dSandToppleDeposit = 0;

      m_pCell->pGetSedLoad()->InitializeSlumpAndToppleSedLoads();
   }
}


