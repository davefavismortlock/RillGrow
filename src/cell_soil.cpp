/*=========================================================================================================================================

This is cell_soil.cpp: implementations of the RillGrow class used to represent the soil column

Copyright (C) 2025 David Favis-Mortlock

==========================================================================================================================================

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
#include "rg.h"
#include "cell.h"
#include "cell_soil.h"

//! Constructor with initialization list
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
   m_dTempSplashDeposit(0),
   m_dClaySplashDeposit(0),
   m_dSiltSplashDeposit(0),
   m_dSandSplashDeposit(0),
   m_dCumulClaySplashDeposit(0),
   m_dCumulSiltSplashDeposit(0),
   m_dCumulSandSplashDeposit(0),
   m_dClaySplashOffEdge(0),
   m_dSiltSplashOffEdge(0),
   m_dSandSplashOffEdge(0),
   m_dCumulClaySplashOffEdge(0),
   m_dCumulSiltSplashOffEdge(0),
   m_dCumulSandSplashOffEdge(0),
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
   pCell = NULL;
}

//! Destructor
CCellSoil::~CCellSoil(void)
{
}

//! Sets the pointer to the parent cell object
void CCellSoil::SetParent(CCell* const pParent)
{
   pCell = pParent;
}

//! Gets a pointer to a soil layer
CCellSoilLayer* CCellSoil::pLayerGetLayer(int const nLayer)
{
   return &m_VLayer[nLayer];
}

//! Returns the number of soil layers
int CCellSoil::nGetNumLayers(void) const
{
   return static_cast<int>(m_VLayer.size());
}

//! Sets up the soil layers
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

      // Also create these all-cell infiltration values
      pVdInfiltCPHWF->push_back(0);
      pVdInfiltChiPart->push_back(0);
   }
}

//! Copy every soil layer's thickness to that layer's temporary thickness, for all size classes
void CCellSoil::InitTmpLayerThicknesses(void)
{
   for (int nLayer = 0; nLayer < static_cast<int>(m_VLayer.size()); nLayer++)
   {
      CCellSoilLayer* pLayer = pLayerGetLayer(nLayer);
      pLayer->SetTmpClayThickness(pLayer->dGetClayThickness());
      pLayer->SetTmpSiltThickness(pLayer->dGetSiltThickness());
      pLayer->SetTmpSandThickness(pLayer->dGetSandThickness());
   }
}

//! Copy every soil layer's temporary thickness to that layer's thickness, for all size classes
void CCellSoil::FinishTmpLayerThicknesses(void)
{
   for (int nLayer = 0; nLayer < static_cast<int>(m_VLayer.size()); nLayer++)
   {
      CCellSoilLayer* pLayer = pLayerGetLayer(nLayer);
      pLayer->SetClayThickness(pLayer->dGetTmpClayThickness());
      pLayer->SetSiltThickness(pLayer->dGetTmpSiltThickness());
      pLayer->SetSandThickness(pLayer->dGetTmpSandThickness());
   }
}

double CCellSoil::dGetSoilSurfaceElevation(void) const
{
   double dSoilSurfaceTop = pCell->dGetBasementElevation();

   for (unsigned int nLay = 0; nLay < m_VLayer.size(); nLay++)
      dSoilSurfaceTop += m_VLayer[nLay].dGetLayerThickness();

   return dSoilSurfaceTop;
}

//! Returns the bulk density (in kg/m**3) of the topmost layer with non-zero thickness. If there are no layers with non-zero thickness (i.e. we are down to unerodible basement) returns -1
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

// //! Changes the thickness of the topmost soil layer
// void CCellSoil::ChangeTopLayerThickness(double const dChange)
// {
//    m_VLayer[0].ChangeThickness(dChange);
// }

//! Do supply-limited flow detachment for all size classes on this cell, removing from the soil layer's temporary thickness fields
void CCellSoil::DoFlowDetach(double const dDepthToErode, bool const bIsEdgeCell)
{
   double dTotClayDetached = 0;
   double dTotSiltDetached = 0;
   double dTotSandDetached = 0;

   for (int nLayer = 0; nLayer < static_cast<int>(m_VLayer.size()); nLayer++)
   {
      CCellSoilLayer* pLayer = pLayerGetLayer(nLayer);

      double dClayDetached = 0;
      double dSiltDetached = 0;
      double dSandDetached = 0;

      pLayer->DoLayerFlowErosion(dDepthToErode, dClayDetached, dSiltDetached, dSandDetached);
      //assert(dDepthToErode == dClayDetached + dSiltDetached + dSandDetached);

      dTotClayDetached += dClayDetached;
      dTotSiltDetached += dSiltDetached;
      dTotSandDetached += dSandDetached;

      double dTotDetached = dClayDetached + dSiltDetached + dSandDetached;

      if (bFpEQ(dTotDetached, dDepthToErode, SEDIMENT_TOLERANCE))
      {
         // We have eroded enough
         break;
      }
   }

   // assert(dTotClayDetached >= 0);
   // assert(dTotSiltDetached >= 0);
   // assert(dTotSandDetached >= 0);

   if (bIsEdgeCell)
   {
      // This is an edge cell, so add to the cell's values for sediment lost
      if (dTotClayDetached > 0)
         pCell->pGetSedLoad()->AddToClaySedOffEdge(dTotClayDetached);
      if (dTotSiltDetached > 0)
         pCell->pGetSedLoad()->AddToSiltSedOffEdge(dTotSiltDetached);
      if (dTotSandDetached > 0)
         pCell->pGetSedLoad()->AddToSandSedOffEdge(dTotSandDetached);
   }
   else
   {
      // This isn't an edge cell, so add to the cell's flow sediment load
      if (dTotClayDetached > 0)
         pCell->pGetSedLoad()->AddToClayFlowSedLoad(dTotClayDetached);
      if (dTotSiltDetached > 0)
         pCell->pGetSedLoad()->AddToSiltFlowSedLoad(dTotSiltDetached);
      if (dTotSandDetached > 0)
         pCell->pGetSedLoad()->AddToSandFlowSedLoad(dTotSandDetached);
   }

   // Add to the cell's flow detachment totals
   m_dClayFlowDetach += dTotClayDetached;
   m_dSiltFlowDetach += dTotSiltDetached;
   m_dSandFlowDetach += dTotSandDetached;
   m_dCumulClayFlowDetach += dTotClayDetached;
   m_dCumulSiltFlowDetach += dTotSiltDetached;
   m_dCumulSandFlowDetach += dTotSandDetached;
}

//! Do deposition from surface water on to the temporary thickness fields of this cell, constraining if the full amount cannot be deposited
void CCellSoil::DoSedLoadDeposit(double const dClayFraction, double const dSiltFraction, double const dSandFraction)
{
   double
      dClaySedLoad = pCell->pGetSedLoad()->dGetLastIterClaySedLoad(),
      dSiltSedLoad = pCell->pGetSedLoad()->dGetLastIterSiltSedLoad(),
      dSandSedLoad = pCell->pGetSedLoad()->dGetLastIterSandSedLoad(),
      dClayToDeposit = dClayFraction * dClaySedLoad,
      dSiltToDeposit = dSiltFraction * dSiltSedLoad,
      dSandToDeposit = dSandFraction * dSandSedLoad;

   if (dClayToDeposit > 0)
   {
      // Add to the this cell's this-iteration depth of clay removed, note that dClayToDeposit is set to the depth actually removed
      pCell->pGetSedLoad()->AddToClaySedLoadRemoved(dClayToDeposit);

      // Add to the this-cell deposition totals
      m_dClayFlowDeposit += dClayToDeposit;
      m_dCumulClayFlowDeposit += dClayToDeposit;
   }

   if (dSiltToDeposit > 0)
   {
      // Add to the this cell's this-iteration depth of silt removed, note that dSiltToDeposit is set to the depth actually removed
      pCell->pGetSedLoad()->AddToSiltSedLoadRemoved(dSiltToDeposit);

      // Add to the this-cell deposition totals
      m_dSiltFlowDeposit += dSiltToDeposit;
      m_dCumulSiltFlowDeposit += dSiltToDeposit;
   }

   if (dSandToDeposit > 0)
   {
      // Add to the this cell's this-iteration depth of sand removed, note that dSandToDeposit is set to the depth actually removed
      pCell->pGetSedLoad()->AddToSandSedLoadRemoved(dSandToDeposit);

      // Add to the this-cell deposition totals
      m_dSandFlowDeposit += dSandToDeposit;
      m_dCumulSandFlowDeposit += dSandToDeposit;
   }

   // Finally do the deposition onto the temporary thickness fields: always deposit to the top layer, even if the top layer has previously been eroded to zero thickness
   m_VLayer.front().DoTmpLayerDeposition(dClayToDeposit, dSiltToDeposit, dSandToDeposit);
}

//! Returns the this-iteration clay-sized surface water detachment for this cell
double CCellSoil::dGetClayFlowDetach(void) const
{
   return m_dClayFlowDetach;
}

//! Returns the this-iteration silt-sized surface water detachment for this cell
double CCellSoil::dGetSiltFlowDetach(void) const
{
   return m_dSiltFlowDetach;
}

//! Returns the this-iteration sand-sized surface water detachment for this cell
double CCellSoil::dGetSandFlowDetach(void) const
{
   return m_dSandFlowDetach;
}

//! Returns the this-iteration surface water detachment (all sediment sizes) for this cell
double CCellSoil::dGetAllSizeFlowDetach(void) const
{
   return m_dClayFlowDetach + m_dSiltFlowDetach + m_dSandFlowDetach;
}

//! Returns the cumulative clay-sized surface water detachment for this cell
double CCellSoil::dGetCumulClayFlowDetach(void) const
{
   return m_dCumulClayFlowDetach;
}

//! Returns the cumulative silt-sized surface water detachment for this cell
double CCellSoil::dGetCumulSiltFlowDetach(void) const
{
   return m_dCumulSiltFlowDetach;
}

//! Returns the cumulative sand-sized surface water detachment for this cell
double CCellSoil::dGetCumulSandFlowDetach(void) const
{
   return m_dCumulSandFlowDetach;
}

//! Returns the cumulative surface water detachment (all sediment sizes) for this cell
double CCellSoil::dGetCumulAllSizeFlowDetach(void) const
{
   return m_dCumulClayFlowDetach + m_dCumulSiltFlowDetach + m_dCumulSandFlowDetach;
}

//! Returns the this-iteration clay-sized surface water deposition on this cell
double CCellSoil::dGetClayFlowDeposit(void) const
{
   return m_dClayFlowDeposit;
}

//! Returns the this-iteration silt-sized surface water deposition on this cell
double CCellSoil::dGetSiltFlowDeposit(void) const
{
   return m_dSiltFlowDeposit;
}

//! Returns the this-iteration sand-sized surface water deposition on this cell
double CCellSoil::dGetSandFlowDeposit(void) const
{
   return m_dSandFlowDeposit;
}

// //! Returns the this-iteration depth of surface water deposition (all size classes) for this cell
// double CCellSoil::dGetAllSizeFlowDeposit(void) const
// {
//    return (m_dClayFlowDeposit + m_dSiltFlowDeposit + m_dSandFlowDeposit);
// }

//! Returns the cumulative clay-sized surface water deposition on this cell
double CCellSoil::dGetCumulClayFlowDeposit(void) const
{
   return m_dCumulClayFlowDeposit;
}

//! Returns the cumulative silt-sized surface water deposition on this cell
double CCellSoil::dGetCumulSiltFlowDeposit(void) const
{
   return m_dCumulSiltFlowDeposit;
}

//! Returns the cumulative sand-sized surface water deposition on this cell
double CCellSoil::dGetCumulSandFlowDeposit(void) const
{
   return m_dCumulSandFlowDeposit;
}

//! Returns the cumulative depth of surface water deposition (all size classes) for this cell
double CCellSoil::dGetCumulAllSizeFlowDeposit(void) const
{
   return (m_dCumulClayFlowDeposit + m_dCumulSiltFlowDeposit + m_dCumulSandFlowDeposit);
}


//! Decreases this cell's elevation as a result of splash detachment TODO use temporary field
void CCellSoil::DoSplashDetach(double const dThickness, double& dTotClayEroded, double& dTotSiltEroded,  double& dTotSandEroded)
{
   dTotClayEroded =
   dTotSiltEroded =
   dTotSandEroded = 0;

   for (unsigned int nLayer = 0; nLayer < m_VLayer.size(); nLayer++)
   {
      CCellSoilLayer* pLayer = pLayerGetLayer(nLayer);

      double dClayDetached = 0;
      double dSiltDetached = 0;
      double dSandDetached = 0;

      // Erode the layer, supply-limited. Depths actually eroded are returned in dClayDetached, dSiltDetached, dSandDetached
      pLayer->DoLayerSplashErosion(dThickness, dClayDetached, dSiltDetached, dSandDetached);

      dTotClayEroded += dClayDetached;
      dTotSiltEroded += dSiltDetached;
      dTotSandEroded += dSandDetached;

      if (bFpEQ(dClayDetached + dSiltDetached + dSandDetached, dThickness, SEDIMENT_TOLERANCE))
      {
         // We have eroded enough
         break;
      }
   }

   // Add to the this-cell totals
   m_dClaySplashDetach      += dTotClayEroded;
   m_dSiltSplashDetach      += dTotSiltEroded;
   m_dSandSplashDetach      += dTotSandEroded;
   m_dCumulClaySplashDetach += dTotClayEroded;
   m_dCumulSiltSplashDetach += dTotSiltEroded;
   m_dCumulSandSplashDetach += dTotSandEroded;
}

//! Set the temporary splash deposition field for this cell
void CCellSoil::SetSplashDepositTemp(double const dTemp)
{
   m_dTempSplashDeposit = dTemp;
}

//! Returns the value of the splash deposition temporary field
double CCellSoil::dGetSplashDepositTemp(void) const
{
   return m_dTempSplashDeposit;
}

//! If the cell is dry, increases this cell's elevation as a result of splash deposition; or if it is wet, add to this cell's sediment load TODO use temporary fields
void CCellSoil::DoSplashToSedLoadOrDeposit(double const dClayChangeDepth, double const dSiltChangeDepth, double const dSandChangeDepth, bool& bToSedLoad)
{
   if (pCell->pGetSurfaceWater()->bIsWet())
   {
      // The cell is wet, so add to its sediment load
      pCell->pGetSedLoad()->AddToSplashSedLoad(dClayChangeDepth, dSiltChangeDepth, dSandChangeDepth);
      bToSedLoad = true;
   }
   else
   {
      // The cell is dry, so do the deposition (always deposit to the top layer, even if the top layer has previously been eroded to zero thickness)
      m_VLayer.front().DoLayerDeposition(dClayChangeDepth, dSiltChangeDepth, dSandChangeDepth);

      // Update this-cell totals for splash deposition
      m_dClaySplashDeposit      += dClayChangeDepth;
      m_dCumulClaySplashDeposit += dClayChangeDepth;
      m_dSiltSplashDeposit      += dSiltChangeDepth;
      m_dCumulSiltSplashDeposit += dSiltChangeDepth;
      m_dSandSplashDeposit      += dSandChangeDepth;
      m_dCumulSandSplashDeposit += dSandChangeDepth;
      bToSedLoad = false;
   }
}

//! Returns the depth of clay-sized splash detachment for this iteration on this cell
double CCellSoil::dGetClaySplashDetach(void) const
{
   return m_dClaySplashDetach;
}

//! Returns the depth of silt-sized splash detachment for this iteration on this cell
double CCellSoil::dGetSiltSplashDetach(void) const
{
   return m_dSiltSplashDetach;
}

//! Returns the depth of sand-sized splash detachment for this iteration on this cell
double CCellSoil::dGetSandSplashDetach(void) const
{
   return m_dSandSplashDetach;
}

//! Returns the depth of splash detachment (all sediment size classes) for this iteration on this cell
double CCellSoil::dGetAllSizeSplashDetach(void) const
{
   return (m_dClaySplashDetach + m_dSiltSplashDetach + m_dSandSplashDetach);
}

//! Returns the depth of clay-sized splash deposition for for this iteration on this cell
double CCellSoil::dGetClaySplashDeposit(void) const
{
   return m_dClaySplashDeposit;
}

//! Returns the depth of silt-sized splash deposition for for this iteration on this cell
double CCellSoil::dGetSiltSplashDeposit(void) const
{
   return m_dSiltSplashDeposit;
}

//! Returns the depth of sand-sized splash deposition for for this iteration on this cell
double CCellSoil::dGetSandSplashDeposit(void) const
{
   return m_dSandSplashDeposit;
}

//! Returns the depth of splash deposition (all sediment size classes) for for this iteration on this cell
double CCellSoil::dGetAllSizeSplashDeposit(void) const
{
   return (m_dClaySplashDeposit + m_dSiltSplashDeposit + m_dSandSplashDeposit);
}

//! Returns the cumulative depth of clay-sized splash detachment for this cell
double CCellSoil::dGetCumulClaySplashDetach(void) const
{
   return m_dCumulClaySplashDetach;
}

//! Returns the cumulative depth of silt-sized splash detachment for this cell
double CCellSoil::dGetCumulSiltSplashDetach(void) const
{
   return m_dCumulSiltSplashDetach;
}

//! Returns the cumulative depth of sand-sized splash detachment for this cell
double CCellSoil::dGetCumulSandSplashDetach(void) const
{
   return m_dCumulSandSplashDetach;
}

//! Returns the cumulative depth of splash detachment (all sediment size classes) for this cell
double CCellSoil::dGetCumulAllSizeSplashDetach(void) const
{
   return (m_dCumulClaySplashDetach + m_dCumulSiltSplashDetach + m_dCumulSandSplashDetach);
}

//! Returns the cumulative depth of clay-sized splash deposition for this cell
double CCellSoil::dGetCumulClaySplashDeposit(void) const
{
   return m_dCumulClaySplashDeposit;
}

//! Returns the cumulative depth of silt-sized splash deposition for this cell
double CCellSoil::dGetCumulSiltSplashDeposit(void) const
{
   return m_dCumulSiltSplashDeposit;
}

//! Returns the cumulative depth of sand-sized splash deposition for this cell
double CCellSoil::dGetCumulSandSplashDeposit(void) const
{
   return m_dCumulSandSplashDeposit;
}

//! Returns the cumulative depth of splash deposition (all sediment size classes) for this cell
double CCellSoil::dGetCumulAllSizeSplashDeposit(void) const
{
   return (m_dCumulClaySplashDeposit + m_dCumulSiltSplashDeposit + m_dCumulSandSplashDeposit);
}

//! Removes splashed clay-sized soil off-edge (only meaningful for edge cells)
void CCellSoil::DoClaySplashOffEdge(double const dSplash)
{
   m_dClaySplashOffEdge += dSplash;
   m_dCumulClaySplashOffEdge += dSplash;
}

//! Removes splashed silt-sized soil off-edge (only meaningful for edge cells)
void CCellSoil::DoSiltSplashOffEdge(double const dSplash)
{
   m_dSiltSplashOffEdge += dSplash;
   m_dCumulSiltSplashOffEdge += dSplash;
}

//! Removes splashed sand-sized soil off-edge (only meaningful for edge cells)
void CCellSoil::DoSandSplashOffEdge(double const dSplash)
{
   m_dSandSplashOffEdge += dSplash;
   m_dCumulSandSplashOffEdge += dSplash;
}

//! Gets the value of clay soil splashed off-edge during this iteration (only meaningful for edge cells)
double CCellSoil::dGetClaySplashOffEdge(void) const
{
   return m_dClaySplashOffEdge;
}

//! Gets the value of silt soil splashed off-edge during this iteration (only meaningful for edge cells)
double CCellSoil::dGetSiltSplashOffEdge(void) const
{
   return m_dSiltSplashOffEdge;
}

//! Gets the value of sand soil splashed off-edge during this iteration (only meaningful for edge cells)
double CCellSoil::dGetSandSplashOffEdge(void) const
{
   return m_dSandSplashOffEdge;
}

//! Gets the cumulative value of clay soil splashed off-edge (only meaningful for edge cells)
double CCellSoil::dGetCumulClaySplashOffEdge(void) const
{
   return m_dCumulClaySplashOffEdge;
}

//! Gets the cumulative value of silt soil splashed off-edge (only meaningful for edge cells)
double CCellSoil::dGetCumulSiltSplashOffEdge(void) const
{
   return m_dCumulSiltSplashOffEdge;
}

//! Gets the cumulative value of sand soil splashed off-edge (only meaningful for edge cells)
double CCellSoil::dGetCumulSandSplashOffEdge(void) const
{
   return m_dCumulSandSplashOffEdge;
}

//! Decreases this cell's elevation as a result of slumping
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

      if (bFpEQ(dClayDetached + dSiltDetached + dSandDetached, dThickness, SEDIMENT_TOLERANCE))
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

//! Adds sediment to this cell as a result of slump: to the top layer of the soil if dry, or to the sediment load if wet
void CCellSoil::DoSlumpDepositOrToSedLoad(double const dClayChngElev, double const dSiltChngElev, double const dSandChngElev, double& dClayDeposit, double& dSiltDeposit, double& dSandDeposit, double& dClayToSedLoad, double& dSiltToSedLoad, double& dSandToSedLoad)
{
   if (pCell->pGetSurfaceWater()->bIsWet())
   {
      // The cell is wet, so add to its sediment load
      pCell->pGetSedLoad()->AddToSlumpSedLoad(dClayChngElev, dSiltChngElev, dSandChngElev);

      dClayToSedLoad = dClayChngElev;
      dSiltToSedLoad = dSiltChngElev;
      dSandToSedLoad = dSandChngElev;
   }
   else
   {
      // Do the deposition (always deposit to the top layer, even if the top layer has previously been eroded to zero thickness)
      m_VLayer.front().DoLayerDeposition(dClayChngElev, dSiltChngElev, dSandChngElev);

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

//! Returns the this-iteration depth of clay-sized slump detachment for this cell
double CCellSoil::dGetClaySlumpDetach(void) const
{
   return m_dClaySlumpDetach;
}

//! Returns the this-iteration depth of silt-sized slump detachment for this cell
double CCellSoil::dGetSiltSlumpDetach(void) const
{
   return m_dSiltSlumpDetach;
}

//! Returns the this-iteration depth of sand-sized slump detachment for this cell
double CCellSoil::dGetSandSlumpDetach(void) const
{
   return m_dSandSlumpDetach;
}

//! Returns the this-iteration depth of slump detachment (all size classes) for this cell
double CCellSoil::dGetAllSizeSlumpDetach(void) const
{
   return (m_dClaySlumpDetach + m_dSiltSlumpDetach + m_dSandSlumpDetach);
}

//! Returns the cumulative depth of clay-sized slump detachment for this cell
double CCellSoil::dGetCumulClaySlumpDetach(void) const
{
   return m_dCumulClaySlumpDetach;
}

//! Returns the cumulative depth of silt-sized slump detachment for this cell
double CCellSoil::dGetCumulSiltSlumpDetach(void) const
{
   return m_dCumulSiltSlumpDetach;
}

//! Returns the cumulative depth of sand-sized slump detachment for this cell
double CCellSoil::dGetCumulSandSlumpDetach(void) const
{
   return m_dCumulSandSlumpDetach;
}

//! Returns the cumulative depth of slump detachment (all size classes) for this cell
double CCellSoil::dGetCumulAllSizeSlumpDetach(void) const
{
   return (m_dCumulClaySlumpDetach + m_dCumulSiltSlumpDetach + m_dCumulSandSlumpDetach);
}

//! Returns the this-iteration depth of clay-sized slump deposition for this cell
double CCellSoil::dGetClaySlumpDeposit(void) const
{
   return m_dClaySlumpDeposit;
}

//! Returns the this-iteration depth of silt-sized slump deposition for this cell
double CCellSoil::dGetSiltSlumpDeposit(void) const
{
   return m_dSiltSlumpDeposit;
}

//! Returns the this-iteration depth of sand-sized slump deposition for this cell
double CCellSoil::dGetSandSlumpDeposit(void) const
{
   return m_dSandSlumpDeposit;
}

//! Returns the this-iteration depth of slump deposition (all size classes) for this cell
double CCellSoil::dGetAllSizeSlumpDeposit(void) const
{
   return m_dClaySlumpDeposit + m_dSiltSlumpDeposit + m_dSandSlumpDeposit;
}

//! Returns the cumulative depth of clay-sized slump deposition for this cell
double CCellSoil::dGetCumulClaySlumpDeposit(void) const
{
   return m_dCumulClaySlumpDeposit;
}

//! Returns the cumulative depth of silt-sized slump deposition for this cell
double CCellSoil::dGetCumulSiltSlumpDeposit(void) const
{
   return m_dCumulSiltSlumpDeposit;
}

//! Returns the cumulative depth of sand-sized slump deposition for this cell
double CCellSoil::dGetCumulSandSlumpDeposit(void) const
{
   return m_dCumulSandSlumpDeposit;
}

//! Returns the cumulative depth of slump deposition (all size classes) for this cell
double CCellSoil::dGetCumulAllSizeSlumpDeposit(void) const
{
   return (m_dCumulClaySlumpDeposit + m_dCumulSiltSlumpDeposit + m_dCumulSandSlumpDeposit);
}


//! Decreases this cell's elevation as a result of toppling
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

      if (bFpEQ(dClayDetached + dSiltDetached + dSandDetached, dThickness, SEDIMENT_TOLERANCE))
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

//! Adds sediment to this cell as a result of toppling. If the cell is wet, the sediment goes to the cell's sediment load
void CCellSoil::DoToppleDepositOrToSedLoad(double const dClayChngElev, double const dSiltChngElev,double const dSandChngElev, double& dClayDeposited, double& dSiltDeposited, double& dSandDeposited, double& dClayToSedLoad, double& dSiltToSedLoad, double& dSandToSedLoad)
{
   if (pCell->pGetSurfaceWater()->bIsWet())
   {
      // The cell is wet, so add to its sediment load
      pCell->pGetSedLoad()->AddToToppleSedLoad(dClayChngElev, dSiltChngElev, dSandChngElev);

      dClayToSedLoad = dClayChngElev;
      dSiltToSedLoad = dSiltChngElev;
      dSandToSedLoad = dSandChngElev;
   }
   else
   {
      // Do the deposition (always deposit to the top layer, even if the top layer has previously been eroded to zero thickness)
      m_VLayer.front().DoLayerDeposition(dClayChngElev, dSiltChngElev, dSandChngElev);

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

//! Returns the depth of clay-sized toppling detachment for this cell
double CCellSoil::dGetClayToppleDetach(void) const
{
   return m_dClayToppleDetach;
}

//! Returns the depth of silt-sized toppling detachment for this cell
double CCellSoil::dGetSiltToppleDetach(void) const
{
   return m_dSiltToppleDetach;
}

//! Returns the depth of sand-sized toppling detachment for this cell
double CCellSoil::dGetSandToppleDetach(void) const
{
   return m_dSandToppleDetach;
}

//! Returns the depth of toppling detachment (all sediment size classes) for this cell
double CCellSoil::dGetAllSizeToppleDetach(void) const
{
   return m_dClayToppleDetach + m_dSiltToppleDetach + m_dSandToppleDetach;
}

//! Returns the cumulative depth of clay-sized toppling detachment for this cell
double CCellSoil::dGetCumulClayToppleDetach(void) const
{
   return m_dCumulClayToppleDetach;
}

//! Returns the cumulative depth of silt-sized toppling detachment for this cell
double CCellSoil::dGetCumulSiltToppleDetach(void) const
{
   return m_dCumulSiltToppleDetach;
}

//! Returns the cumulative depth of sand-sized toppling detachment for this cell
double CCellSoil::dGetCumulSandToppleDetach(void) const
{
   return m_dCumulSandToppleDetach;
}

//! Returns the cumulative depth of toppling detachment (all sediment size classes) for this cell
double CCellSoil::dGetCumulAllSizeToppleDetach(void) const
{
   return m_dCumulClayToppleDetach + m_dCumulSiltToppleDetach + m_dCumulSandToppleDetach;
}


//! Returns the depth of clay-sized toppling deposition for this cell
double CCellSoil::dGetClayToppleDeposit(void) const
{
   return m_dClayToppleDeposit;
}

//! Returns the depth of silt-sized toppling deposition for this cell
double CCellSoil::dGetSiltToppleDeposit(void) const
{
   return m_dSiltToppleDeposit;
}

//! Returns the depth of sand-sized toppling deposition for this cell
double CCellSoil::dGetSandToppleDeposit(void) const
{
   return m_dSandToppleDeposit;
}

//! Returns the depth of toppling deposition (all size classes) for this cell
double CCellSoil::dGetAllSizeToppleDeposit(void) const
{
   return m_dClayToppleDeposit + m_dSiltToppleDeposit + m_dSandToppleDeposit;
}

//! Returns the cumulative depth of clay-sized toppling deposition for this cell
double CCellSoil::dGetCumulClayToppleDeposit(void) const
{
   return m_dCumulClayToppleDeposit;
}

//! Returns the cumulative depth of silt-sized toppling deposition for this cell
double CCellSoil::dGetCumulSiltToppleDeposit(void) const
{
   return m_dCumulSiltToppleDeposit;
}

//! Returns the cumulative depth of sand-sized toppling deposition for this cell
double CCellSoil::dGetCumulSandToppleDeposit(void) const
{
   return m_dCumulSandToppleDeposit;
}

//! Returns the cumulative depth of toppling deposition (all size classes) for this cell
double CCellSoil::dGetCumulAllSizeToppleDeposit(void) const
{
   return m_dCumulClayToppleDeposit + m_dCumulSiltToppleDeposit + m_dCumulSandToppleDeposit;
}

//! Zeros the this-iteration (actually they are kept over several iterations) values for slumping, toppling and shear stress
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
   // Do the deposition (always deposit to the top layer, even if the top layer has previously been eroded to zero thickness)
   m_VLayer.front().DoLayerDeposition(dClayDeposit, dSiltDeposit, dSandDeposit);

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

//! Returns the clay-sized sediment that was deposited when all water was lost to infilt
double CCellSoil::dGetClayInfiltDeposit(void) const
{
   return m_dClayInfiltDeposit;
}

//! Returns the silt-sized sediment that was deposited when all water was lost to infilt
double CCellSoil::dGetSiltInfiltDeposit(void) const
{
   return m_dSiltInfiltDeposit;
}

//! Returns the sand-sized sediment that was deposited when all water was lost to infilt
double CCellSoil::dGetSandInfiltDeposit(void) const
{
   return m_dSandInfiltDeposit;
}

//! Returns the sediment (all size classes)  that was deposited when all water was lost to infilt
double CCellSoil::dGetAllSizeInfiltDeposit(void) const
{
   return (m_dClayInfiltDeposit + m_dSiltInfiltDeposit + m_dSandInfiltDeposit);
}

//! Returns the cumulative total of clay-sized sediment that was deposited when all water was lost to infilt
double CCellSoil::dGetCumulClayInfiltDeposit(void) const
{
   return m_dCumulClayInfiltDeposit;
}

//! Returns the cumulative total of silt-sized sediment that was deposited when all water was lost to infilt
double CCellSoil::dGetCumulSiltInfiltDeposit(void) const
{
   return m_dCumulSiltInfiltDeposit;
}

//! Returns the cumulative total of sand-sized sediment that was deposited when all water was lost to infilt
double CCellSoil::dGetCumulSandInfiltDeposit(void) const
{
   return m_dCumulSandInfiltDeposit;
}

//! Returns the cumulative total of sediment (all size classes)  that was deposited when all water was lost to infilt
double CCellSoil::dGetCumulAllSizeInfiltDeposit(void) const
{
   return (m_dCumulClayInfiltDeposit + m_dCumulSiltInfiltDeposit + m_dCumulSandInfiltDeposit);
}


//! Increments the shear stress and the cumulative shear stress for this cell
void CCellSoil::IncShearStress(double const dNewShearStress)
{
   m_dShearStress += dNewShearStress;
   m_dCumulShearStress += dNewShearStress;
}

//! Returns the shear stress for this cell
double CCellSoil::dGetShearStress(void) const
{
   return (m_dShearStress);
}

//! Returns the cumulative shear stress for this cell
double CCellSoil::dGetCumulShearStress(void) const
{
   return (m_dCumulShearStress);
}

//! Calculates and returns cumulative net soil loss from flow erosion, splash redistribution, slumping, and infilt deposition
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

//! Removes soil (i.e. decreases elevation) from this cell during headcut retreat
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

      if (bFpEQ(dClayDetached + dSiltDetached + dSandDetached, dThickness, SEDIMENT_TOLERANCE))
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

//! Adds sediment to this cell as a result of headcut retreat. If the cell is wet, the sediment is added to sediment load
void CCellSoil::DoHeadcutRetreatDepositOrToSedLoad(double const dClayChngElev, double const dSiltChngElev, double const dSandChngElev, double& dClayDeposited, double& dSiltDeposited, double& dSandDeposited, double& dClayToSedLoad, double& dSiltToSedLoad, double& dSandToSedLoad)
{
   if (pCell->pGetSurfaceWater()->bIsWet())
   {
      // The cell is wet, so add to its sediment load
      pCell->pGetSedLoad()->AddToHeadcutRetreatSedLoad(dClayChngElev, dSiltChngElev, dSandChngElev);

      dClayToSedLoad = dClayChngElev;
      dSiltToSedLoad = dSiltChngElev;
      dSandToSedLoad = dSandChngElev;
   }
   else
   {
      // Do the deposition (always deposit to the top layer, even if the top layer has previously been eroded to zero thickness)
      m_VLayer.front().DoLayerDeposition(dClayChngElev, dSiltChngElev, dSandChngElev);

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

//! Returns this cell's depth of clay soil lost (mm) by headcut retreat
double CCellSoil::dGetClayHeadcutRetreatDetach(void) const
{
   return m_dClayHeadcutRetreatDetach;
}

//! Returns this cell's depth of silt soil lost (mm) by headcut retreat
double CCellSoil::dGetSiltHeadcutRetreatDetach(void) const
{
   return m_dSiltHeadcutRetreatDetach;
}

//! Returns this cell's depth of sand soil lost (mm) by headcut retreat
double CCellSoil::dGetSandHeadcutRetreatDetach(void) const
{
   return m_dSandHeadcutRetreatDetach;
}

//! Returns this cell's depth of soil (all size classes) lost (mm) by headcut retreat
double CCellSoil::dGetAllSizeHeadcutRetreatDetach(void) const
{
   return m_dClayHeadcutRetreatDetach + m_dSiltHeadcutRetreatDetach + m_dSandHeadcutRetreatDetach;
}

//! Returns this cell's cumulative depth of clay soil lost (mm) by headcut retreat
double CCellSoil::dGetCumulClayHeadcutRetreatDetach(void) const
{
   return m_dCumulClayHeadcutRetreatDetach;
}

//! Returns this cell's cumulative depth of silt soil lost (mm) by headcut retreat
double CCellSoil::dGetCumulSiltHeadcutRetreatDetach(void) const
{
   return m_dCumulSiltHeadcutRetreatDetach;
}

//! Returns this cell's cumulative depth of sand soil lost (mm) by headcut retreat
double CCellSoil::dGetCumulSandHeadcutRetreatDetach(void) const
{
   return m_dCumulSandHeadcutRetreatDetach;
}

//! Returns this cell's depth of clay soil gained (mm) by headcut retreat
double CCellSoil::dGetClayHeadcutRetreatDeposit(void) const
{
   return m_dClayHeadcutRetreatDeposit;
}

//! Returns this cell's depth of silt soil gained (mm) by headcut retreat
double CCellSoil::dGetSiltHeadcutRetreatDeposit(void) const
{
   return m_dSiltHeadcutRetreatDeposit;
}

//! Returns this cell's depth of sand soil gained (mm) by headcut retreat
double CCellSoil::dGetSandHeadcutRetreatDeposit(void) const
{
   return m_dSandHeadcutRetreatDeposit;
}

//! Returns this cell's depth of soil (all size classes) gained (mm) by headcut retreat
double CCellSoil::dGetAllSizeHeadcutRetreatDeposit(void) const
{
   return m_dClayHeadcutRetreatDeposit + m_dSiltHeadcutRetreatDeposit + m_dSandHeadcutRetreatDeposit;
}

//! Returns this cell's cumulative depth of clay soil gained (mm) by headcut retreat
double CCellSoil::dGetCumulClayHeadcutRetreatDeposit(void) const
{
   return m_dCumulClayHeadcutRetreatDeposit;
}

//! Returns this cell's cumulative depth of silt soil gained (mm) by headcut retreat
double CCellSoil::dGetCumulSiltHeadcutRetreatDeposit(void) const
{
   return m_dCumulSiltHeadcutRetreatDeposit;
}

//! Returns this cell's cumulative depth of sand soil gained (mm) by headcut retreat
double CCellSoil::dGetCumulSandHeadcutRetreatDeposit(void) const
{
   return m_dCumulSandHeadcutRetreatDeposit;
}

//! Returns this cell's cumulative depth of soil (all size classes) gained (mm) by headcut retreat
double  CCellSoil::dGetCumulAllSizeHeadcutRetreatDeposit(void) const
{
   return m_dCumulClayHeadcutRetreatDeposit + m_dCumulSiltHeadcutRetreatDeposit + m_dCumulSandHeadcutRetreatDeposit;
}

// ============================================================== m_dLaplacian =========================================================
//! Sets the Laplacian value for this cell
void CCellSoil::SetLaplacian(double const dNewLapl)
{
   m_dLaplacian = dNewLapl;
}

//! Returns the Laplacian value for this cell
double CCellSoil::dGetLaplacian(void) const
{
   return (m_dLaplacian);
}

//! Sets the detachment and deposition values, and the splash off-edge values, for this cell to zero. if there is slumping this iteration, it also zeroes the slumping and toppling values for the cell
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
   m_dLaplacian         =
   m_dTempSplashDeposit =
   m_dClaySplashOffEdge =
   m_dSiltSplashOffEdge =
   m_dSandSplashOffEdge = 0;

   pCell->pGetSedLoad()->InitSplashOffEdge();

   if (bSlump)
   {
      m_dShearStress       =
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

      pCell->pGetSedLoad()->InitSlumpAndToppleSedLoads();
   }
}


