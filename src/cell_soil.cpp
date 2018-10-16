/*=========================================================================================================================================

 This is cell_soil.cpp: implementations of the RillGrow class used to represent the soil column

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
#include "cell_soil.h"


CSoil::CSoil(void)
:
   m_dClayFlowDetach(0),
   m_dSiltFlowDetach(0),
   m_dSandFlowDetach(0),
   m_dTotClayFlowDetach(0),
   m_dTotSiltFlowDetach(0),
   m_dTotSandFlowDetach(0),
   m_dClayFlowDeposit(0),
   m_dSiltFlowDeposit(0),
   m_dSandFlowDeposit(0),
   m_dTotClayFlowDeposit(0),
   m_dTotSiltFlowDeposit(0),
   m_dTotSandFlowDeposit(0),
   m_dClaySplashDetach(0),
   m_dSiltSplashDetach(0),
   m_dSandSplashDetach(0),
   m_dTotClaySplashDetach(0),
   m_dTotSiltSplashDetach(0),
   m_dTotSandSplashDetach(0),
   m_dTotSplashDepositToDo(0),
   m_dClaySplashDeposit(0),
   m_dSiltSplashDeposit(0),
   m_dSandSplashDeposit(0),
   m_dTotClaySplashDeposit(0),
   m_dTotSiltSplashDeposit(0),
   m_dTotSandSplashDeposit(0),
   m_dClaySlumpDetach(0),
   m_dSiltSlumpDetach(0),
   m_dSandSlumpDetach(0),
   m_dTotClaySlumpDetach(0),
   m_dTotSiltSlumpDetach(0),
   m_dTotSandSlumpDetach(0),
   m_dClaySlumpDeposit(0),
   m_dSiltSlumpDeposit(0),
   m_dSandSlumpDeposit(0),
   m_dTotClaySlumpDeposit(0),
   m_dTotSiltSlumpDeposit(0),
   m_dTotSandSlumpDeposit(0),
   m_dClayToppleDetach(0),
   m_dSiltToppleDetach(0),
   m_dSandToppleDetach(0),
   m_dTotClayToppleDetach(0),
   m_dTotSiltToppleDetach(0),
   m_dTotSandToppleDetach(0),
   m_dClayToppleDeposit(0),
   m_dSiltToppleDeposit(0),
   m_dSandToppleDeposit(0),
   m_dTotClayToppleDeposit(0),
   m_dTotSiltToppleDeposit(0),
   m_dTotSandToppleDeposit(0),
   m_dClayInfiltDeposit(0),
   m_dSiltInfiltDeposit(0),
   m_dSandInfiltDeposit(0),
   m_dTotClayInfiltDeposit(0),
   m_dTotSiltInfiltDeposit(0),
   m_dTotSandInfiltDeposit(0),
   m_dShearStress(0),
   m_dTotShearStress(0),
   m_dLaplacian(0)
{   
   m_pCell = NULL;
}

CSoil::~CSoil(void)
{
}


void CSoil::SetParent(CCell* const pParent)
{
   m_pCell = pParent;
}

CLayer* CSoil::pLayerGetLayer(int const nLayer)
{
   return &m_VLayer[nLayer];
}

// Sets up the soil layers
void CSoil::SetSoilLayers(double const dTotalDepth, int const nLayers, vector<string> const* pVstrInputSoilLayerName, vector<double> const* pVdInputSoilLayerThickness, vector<double> const* pVdInputSoilLayerPerCentClay, vector<double> const* pVdInputSoilLayerPerCentSilt, vector<double> const* pVdInputSoilLayerPerCentSand, vector<double> const* pVdInputSoilLayerBulkDensity, vector<double> const* pVdInputSoilLayerClayFlowErodibility, vector<double> const* pVdInputSoilLayerSiltFlowErodibility, vector<double> const* pVdInputSoilLayerSandFlowErodibility, vector<double> const* pVdInputSoilLayerClaySplashErodibility, vector<double> const* pVdInputSoilLayerSiltSplashErodibility, vector<double> const* pVdInputSoilLayerSandSplashErodibility, vector<double> const* pVdInputSoilLayerClaySlumpErodibility, vector<double> const* pVdInputSoilLayerSiltSlumpErodibility, vector<double> const* pVdInputSoilLayerSandSlumpErodibility, vector<double>* pVdInfiltCPHWF, vector<double>* pVdInfiltChiPart)
{
   double dDepthRemaining = dTotalDepth;
   
   for (int nLay = nLayers-1; nLay >= 0; nLay--)
   {
      CLayer layerNew;
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

double CSoil::dGetSoilSurfaceElevation(void)
{
   double dSoilSurfaceTop = m_pCell->dGetBasementElevation();
   
   for (unsigned int nLay = 0; nLay < m_VLayer.size(); nLay++)
      dSoilSurfaceTop += m_VLayer[nLay].dGetLayerThickness();
   
   return dSoilSurfaceTop;  
}


// Returns the bulk density (in kg/m**3) of the topmost layer with non-zero thickness. If there are no layers with non-zero thickness (i.e. we are down to unerodible basement) returns -1
double CSoil::dGetBulkDensityOfTopNonZeroLayer(void)
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
void CSoil::ChangeTopLayerThickness(double const dChange)
{
   m_VLayer[0].ChangeThickness(dChange);   
}


// Do supply-limited detachment from overland flow on this cell
void CSoil::DoFlowDetach(double const dDepthToErode)
{
   double 
      dTotClayEroded = 0,
      dTotSiltDetached = 0,
      dTotSandDetached = 0;
      
   for (unsigned int nLayer = 0; nLayer < m_VLayer.size(); nLayer++)
   {
      CLayer* pLayer = pLayerGetLayer(nLayer);
      
      double
         dClayDetached = 0,
         dSiltDetached = 0,
         dSandDetached = 0;
      
      pLayer->DoFlowErosionLayer(dDepthToErode, dClayDetached, dSiltDetached, dSandDetached);
      
      dTotClayEroded += dClayDetached;
      dTotSiltDetached += dSiltDetached;
      dTotSandDetached += dSandDetached;
      
      if (bFPIsEqual(dClayDetached + dSiltDetached + dSandDetached, dDepthToErode, TOLERANCE))
      {
         // We have eroded enough
         break;
      }
   }
   
   // Add to the cell's sediment load
   m_pCell->pGetSediment()->dChangeClaySedimentLoad(dTotClayEroded);
   m_pCell->pGetSediment()->dChangeSiltSedimentLoad(dTotSiltDetached);
   m_pCell->pGetSediment()->dChangeSandSedimentLoad(dTotSandDetached);
   
   // Add to the cell's totals
   m_dClayFlowDetach += dTotClayEroded;
   m_dSiltFlowDetach += dTotSiltDetached;
   m_dSandFlowDetach += dTotSandDetached;
   m_dTotClayFlowDetach += dTotClayEroded;
   m_dTotSiltFlowDetach += dTotSiltDetached;
   m_dTotSandFlowDetach += dTotSandDetached;
   
   // Add to the this-iteration totals
   m_pSim->AddThisIterClayFlowDetach(dTotClayEroded);
   m_pSim->AddThisIterSiltFlowDetach(dTotSiltDetached);
   m_pSim->AddThisIterSandFlowDetach(dTotSandDetached);
}


// Do deposition from overland flow on this cell, constraining if the full amount cannot be deposited
void CSoil::DoFlowDeposit(double const dClayFraction, double const dSiltFraction, double const dSandFraction)
{
   double
      dClaySedimentLoad = m_pCell->pGetSediment()->dGetClaySedimentLoad(),
      dSiltSedimentLoad = m_pCell->pGetSediment()->dGetSiltSedimentLoad(),
      dSandSedimentLoad = m_pCell->pGetSediment()->dGetSandSedimentLoad(),
      dClayToDeposit = dClayFraction * dClaySedimentLoad,
      dSiltToDeposit = dSiltFraction * dSiltSedimentLoad,
      dSandToDeposit = dSandFraction * dSandSedimentLoad;


//    // dChangeElev is +ve
//    assert(dChangeElev > 0);
//    
//    double dTotSedimentLoad = m_pCell->pGetSediment()->dGetAllSizeSedimentLoad();
//    
//    if (dTotSedimentLoad == 0)
//       // No sediment to deposit
//       return;
//    
//    double   
//       dRemaining = dTotSedimentLoad - dChangeElev,
//       dClaySedLoad = m_pCell->pGetSediment()->dGetClaySedimentLoad(),
//       dSiltSedLoad = m_pCell->pGetSediment()->dGetSiltSedimentLoad(),
//       dSandSedimentLoad = m_pCell->pGetSediment()->dGetSandSedimentLoad();
//       
//    // Start by assuming that we have insufficient sediment in transport: i.e. that we will deposit the full amount that is being transported    
//    double
//       dClayToDeposit = dClaySedLoad,                  
//       dSiltToDeposit = dSiltSedLoad,
//       dSandToDeposit = dSandSedimentLoad;
//       
//    // Now check whether the above assuimption is true i.e. whether there will be some sediment remaining in transport after we have deposited the required amount
//    if (dRemaining > 0)
//    {
//       // Yes, there will be some sediment remaining, so deposit a fraction of each size class TODO CHECK THIS LOGIC
//       double
//          dClayWeight = dClaySedLoad / dTotSedimentLoad,
//          dSiltWeight = dSiltSedLoad / dTotSedimentLoad,
//          dSandWeight = dSandSedimentLoad / dTotSedimentLoad;
//          
//       dClayToDeposit = dChangeElev * dClayWeight,
//       dSiltToDeposit = dChangeElev * dSiltWeight,
//       dSandToDeposit = dChangeElev * dSandWeight;
//    }
   
   // Do the deposition
   // TODO Should we always deposit to the top layer? What about if top layer has been eroded to zero thickness?
   m_VLayer.back().DoFlowDepositionLayer(dClayToDeposit, dSiltToDeposit, dSandToDeposit);      
   
   // Add to the this-cell totals
   m_dClayFlowDeposit += dClayToDeposit;
   m_dSiltFlowDeposit += dSiltToDeposit;
   m_dSandFlowDeposit += dSandToDeposit;
   m_dTotClayFlowDeposit += dClayToDeposit;
   m_dTotSiltFlowDeposit += dSiltToDeposit;
   m_dTotSandFlowDeposit += dSandToDeposit;
   
   // Add to the this-iteration totals
   m_pSim->AddThisIterClayFlowDeposit(dClayToDeposit);
   m_pSim->AddThisIterSiltFlowDeposit(dSiltToDeposit);
   m_pSim->AddThisIterSandFlowDeposit(dSandToDeposit);   
}

// Returns the clay-sized overland flow detachment for this cell
double CSoil::dGetClayFlowDetach(void) const
{
   return m_dClayFlowDetach;
}

// Returns the silt-sized overland flow detachment for this cell
double CSoil::dGetSiltFlowDetach(void) const
{
   return m_dSiltFlowDetach;
}

// Returns the sand-sized overland flow detachment for this cell
double CSoil::dGetSandFlowDetach(void) const
{
   return m_dSandFlowDetach;
}

// Returns the overland flow detachment (all sediment sizes) for this cell
double CSoil::dGetTotFlowDetach(void) const
{
   return m_dClayFlowDetach + m_dSiltFlowDetach + m_dSandFlowDetach;
}

// Returns the cumulative clay-sized overland flow detachment for this cell
double CSoil::dGetCumulativeClayFlowDetach(void) const
{
   return m_dTotClayFlowDetach;
}

// Returns the cumulative silt-sized overland flow detachment for this cell
double CSoil::dGetCumulativeSiltFlowDetach(void) const
{
   return m_dTotSiltFlowDetach;
}

// Returns the cumulative sand-sized overland flow detachment for this cell
double CSoil::dGetCumulativeSandFlowDetach(void) const
{
   return m_dTotSandFlowDetach;
}

// Returns the cumulative overland flow detachment (all sediment sizes) for this cell
double CSoil::dGetCumulativeTotFlowDetach(void) const
{
   return m_dTotClayFlowDetach + m_dTotSiltFlowDetach + m_dTotSandFlowDetach;
}

// Returns the clay-sized overland flow deposition on this cell
double CSoil::dGetClayFlowDeposition(void) const
{
   return m_dClayFlowDeposit;
}

// Returns the silt-sized overland flow deposition on this cell
double CSoil::dGetSiltFlowDeposition(void) const
{
   return m_dSiltFlowDeposit;
}

// Returns the sand-sized overland flow deposition on this cell
double CSoil::dGetSandFlowDeposition(void) const
{
   return m_dSandFlowDeposit;
}

// Returns the depth of overland flow deposition (all size classes) for this cell
double CSoil::dGetTotFlowDeposition(void) const
{
   return (m_dClayFlowDeposit + m_dSiltFlowDeposit + m_dSandFlowDeposit);
}

// Returns the cumulative clay-sized overland flow deposition on this cell
double CSoil::dGetCumulativeClayFlowDeposition(void) const
{
   return m_dTotClayFlowDeposit;
}

// Returns the cumulative silt-sized overland flow deposition on this cell
double CSoil::dGetCumulativeSiltFlowDeposition(void) const
{
   return m_dTotSiltFlowDeposit;
}

// Returns the cumulative sand-sized overland flow deposition on this cell
double CSoil::dGetCumulativeSandFlowDeposition(void) const
{
   return m_dTotSandFlowDeposit;
}

// Returns the cumulative depth of overland flow deposition (all size classes) for this cell
double CSoil::dGetCumulativeTotFlowDeposition(void) const
{
   return (m_dTotClayFlowDeposit + m_dTotSiltFlowDeposit + m_dTotSandFlowDeposit);
}


// Decreases this cell's elevation as a result of splash detachment
void CSoil::DoSplashDetach(double const dThickness)
{
   double 
      dTotClayEroded = 0,
      dTotSiltDetached = 0,
      dTotSandDetached = 0;
   
   for (unsigned int nLayer = 0; nLayer < m_VLayer.size(); nLayer++)
   {
      CLayer* pLayer = pLayerGetLayer(nLayer);
      
      double
         dClayDetached = 0,
         dSiltDetached = 0,
         dSandDetached = 0;
      
      pLayer->DoSplashErosionLayer(dThickness, dClayDetached, dSiltDetached, dSandDetached);
      
      dTotClayEroded += dClayDetached;
      dTotSiltDetached += dSiltDetached;
      dTotSandDetached += dSandDetached;
      
      if (bFPIsEqual(dClayDetached + dSiltDetached + dSandDetached, dThickness, TOLERANCE))
      {
         // We have eroded enough
         break;
      }
   }
   
   // Add to the this-cell totals   
   m_dClaySplashDetach += dTotClayEroded;
   m_dSiltSplashDetach += dTotSiltDetached;
   m_dSandSplashDetach += dTotSandDetached;
   m_dTotClaySplashDetach += dTotClayEroded;
   m_dTotSiltSplashDetach += dTotSiltDetached;
   m_dTotSandSplashDetach += dTotSandDetached;
   
   // Add to the this-iteration totals
   m_pSim->AddThisIterClaySplashDetach(dTotClayEroded);
   m_pSim->AddThisIterSiltSplashDetach(dTotSiltDetached);
   m_pSim->AddThisIterSandSplashDetach(dTotSandDetached);   
}

// Set the temporary splash deposition field for this cell
void CSoil::SetSplashDepositionTemporary(double const dTemp)
{
   m_dTotSplashDepositToDo = dTemp;
}

// Returns the value of the splash deposition temporary field
double CSoil::dGetSplashDepositionTemporary(void) const
{
   return m_dTotSplashDepositToDo;
}

// For clay-sized sediment: either increases this cell's elevation as a result of splash deposition, or (if the cell is wet) adds to its sediment load
void CSoil::DoClaySplashDeposit(double const dChangeElev)
{
   // dChangeElev is +ve
   assert(dChangeElev > 0);
   
   if (m_pCell->pGetSurfaceWater()->bIsWet())
   {
      // Cell is wet, add to sediment load
      m_pCell->pGetSediment()->dChangeClaySedimentLoad(dChangeElev);      
   }
   else
   {
      // Cell is dry, so do the deposition
      // TODO Should we always deposit to the top layer? What about if top layer has been eroded to zero thickness?
      m_VLayer.back().DoSplashDepositionLayer(dChangeElev, 0, 0);            
   }
   
   // Update this-cell totals
   m_dClaySplashDeposit    += dChangeElev;
   m_dTotClaySplashDeposit += dChangeElev;
   
   // Add to the this-iteration totals
   m_pSim->AddThisIterClaySplashDeposit(dChangeElev);
}

// For silt-sized sediment: either increases this cell's elevation as a result of splash deposition, or (if the cell is wet) adds to its sediment load
void CSoil::DoSiltSplashDeposit(double const dChangeElev)
{
   // dChangeElev is +ve
   assert(dChangeElev > 0);
   
   if (m_pCell->pGetSurfaceWater()->bIsWet())
   {
      // Cell is wet, add to sediment load
      m_pCell->pGetSediment()->dChangeSiltSedimentLoad(dChangeElev);      
   }
   else
   {
      // Cell is dry, so do the deposition
      // TODO Should we always deposit to the top layer? What about if top layer has been eroded to zero thickness?
      m_VLayer.back().DoSplashDepositionLayer(0, dChangeElev, 0);            
   }
   
   // Update this-cell totals
   m_dSiltSplashDeposit    += dChangeElev;
   m_dTotSiltSplashDeposit += dChangeElev;
   
   // Add to the this-iteration totals
   m_pSim->AddThisIterSiltSplashDeposit(dChangeElev);
}

// For sand-sized sediment: either increases this cell's elevation as a result of splash deposition, or (if the cell is wet) adds to its sediment load
void CSoil::DoSandSplashDeposit(double const dChangeElev)
{
   // dChangeElev is +ve
   assert(dChangeElev > 0);
   
   if (m_pCell->pGetSurfaceWater()->bIsWet())
   {
      // Cell is wet, add to sediment load
      m_pCell->pGetSediment()->dChangeSandSedimentLoad(dChangeElev);      
   }
   else
   {
      // Cell is dry, so do the deposition
      // TODO Should we always deposit to the top layer? What about if top layer has been eroded to zero thickness?
      m_VLayer.back().DoSplashDepositionLayer(0, 0, dChangeElev);            
   }
   
   // Update this-cell totals
   m_dSandSplashDeposit    += dChangeElev;
   m_dTotSandSplashDeposit += dChangeElev;
   
   // Add to the this-iteration totals
   m_pSim->AddThisIterSandSplashDeposit(dChangeElev);
}


// Returns the depth of clay-sized splash detachment for this cell
double CSoil::dGetClaySplashDetach(void) const
{
   return m_dClaySplashDetach;
}

// Returns the depth of silt-sized splash detachment for this cell
double CSoil::dGetSiltSplashDetach(void) const
{
   return m_dSiltSplashDetach;
}

// Returns the depth of sand-sized splash detachment for this cell
double CSoil::dGetSandSplashDetach(void) const
{
   return m_dSandSplashDetach;
}

// Returns the depth of splash detachment (all sediment size classes) for this cell
double CSoil::dGetTotSplashDetach(void) const
{
   return (m_dClaySplashDetach + m_dSiltSplashDetach + m_dSandSplashDetach);
}


// Returns the depth of clay-sized splash deposition for this cell
double CSoil::dGetClaySplashDeposit(void) const
{
   return m_dClaySplashDeposit;
}

// Returns the depth of silt-sized splash deposition for this cell
double CSoil::dGetSiltSplashDeposit(void) const
{
   return m_dSiltSplashDeposit;
}

// Returns the depth of sand-sized splash deposition for this cell
double CSoil::dGetSandSplashDeposit(void) const
{
   return m_dSandSplashDeposit;
}

// Returns the depth of splash deposition (all sediment size classes) for this cell
double CSoil::dGetTotSplashDeposit(void) const
{
   return (m_dClaySplashDeposit + m_dSiltSplashDeposit + m_dSandSplashDeposit);
}


// Returns the cumulative depth of clay-sized splash detachment for this cell
double CSoil::dGetCumulativeClaySplashDetach(void) const
{
   return m_dTotClaySplashDetach;
}

// Returns the cumulative depth of silt-sized splash detachment for this cell
double CSoil::dGetCumulativeSiltSplashDetach(void) const
{
   return m_dTotSiltSplashDetach;
}

// Returns the cumulative depth of sand-sized splash detachment for this cell
double CSoil::dGetCumulativeSandSplashDetach(void) const
{
   return m_dTotSandSplashDetach;
}

// Returns the cumulative depth of splash detachment (all sediment size classes) for this cell
double CSoil::dGetCumulativeTotSplashDetach(void) const
{
   return (m_dTotClaySplashDetach + m_dTotSiltSplashDetach + m_dTotSandSplashDetach);
}


// Returns the cumulative depth of clay-sized splash deposition for this cell
double CSoil::dGetCumulativeClaySplashDeposit(void) const
{
   return m_dTotClaySplashDeposit;
}

// Returns the cumulative depth of silt-sized splash deposition for this cell
double CSoil::dGetCumulativeSiltSplashDeposit(void) const
{
   return m_dTotSiltSplashDeposit;
}

// Returns the cumulative depth of sand-sized splash deposition for this cell
double CSoil::dGetCumulativeSandSplashDeposit(void) const
{
   return m_dTotSandSplashDeposit;
}

// Returns the cumulative depth of splash deposition (all sediment size classes) for this cell
double CSoil::dGetCumulativeTotSplashDeposit(void) const
{
   return (m_dTotClaySplashDeposit + m_dTotSiltSplashDeposit + m_dTotSandSplashDeposit);
}


// Decreases this cell's elevation as a result of slumping
void CSoil::DoSlumpDetach(double const dThickness, double& dTotClayDetached, double& dTotSiltDetached, double& dTotSandDetached)
{
   for (unsigned int nLayer = 0; nLayer < m_VLayer.size(); nLayer++)
   {
      CLayer* pLayer = pLayerGetLayer(nLayer);
      
      double
         dClayDetached = 0,
         dSiltDetached = 0,
         dSandDetached = 0;
      
      pLayer->DoSlumpErosionLayer(dThickness, dClayDetached, dSiltDetached, dSandDetached);
      
      dTotClayDetached += dClayDetached;
      dTotSiltDetached += dSiltDetached;
      dTotSandDetached += dSandDetached;
      
      if (bFPIsEqual(dClayDetached + dSiltDetached + dSandDetached, dThickness, TOLERANCE))
      {
         // We have eroded enough
         break;
      }
   }
   
   // Add to this-cell totals
   m_dClaySlumpDetach += dTotClayDetached; 
   m_dSiltSlumpDetach += dTotSiltDetached; 
   m_dSandSlumpDetach += dTotSandDetached; 
   m_dTotClaySlumpDetach += dTotClayDetached; 
   m_dTotSiltSlumpDetach += dTotSiltDetached; 
   m_dTotSandSlumpDetach += dTotSandDetached; 
   
   // Add to the this-iteration totals
   m_pSim->AddThisIterClaySlumpDetach(dTotClayDetached);
   m_pSim->AddThisIterSiltSlumpDetach(dTotSiltDetached);
   m_pSim->AddThisIterSandSlumpDetach(dTotSandDetached);
}

void CSoil::AddSlumpDeposition(double const dClayToSedLoad, double const dSiltToSedLoad, double const dSandToSedLoad)
{
   m_dClaySlumpDeposit += dClayToSedLoad;
   m_dSiltSlumpDeposit += dSiltToSedLoad;
   m_dSandSlumpDeposit += dSandToSedLoad;
   m_dTotClaySlumpDeposit += dClayToSedLoad;
   m_dTotSiltSlumpDeposit += dSiltToSedLoad;
   m_dTotSandSlumpDeposit += dSandToSedLoad;
}


// Decreases this cell's elevation as a result of toppling
void CSoil::DoToppleDetach(double const dThickness, double& dTotClayDetached, double& dTotSiltDetached, double& dTotSandDetached)
{
   for (unsigned int nLayer = 0; nLayer < m_VLayer.size(); nLayer++)
   {
      CLayer* pLayer = pLayerGetLayer(nLayer);
      
      double
         dClayDetached = 0,
         dSiltDetached = 0,
         dSandDetached = 0;
      
      pLayer->DoToppleErosionLayer(dThickness, dClayDetached, dSiltDetached, dSandDetached);
      
      dTotClayDetached += dClayDetached;
      dTotSiltDetached += dSiltDetached;
      dTotSandDetached += dSandDetached;
      
      if (bFPIsEqual(dClayDetached + dSiltDetached + dSandDetached, dThickness, TOLERANCE))
      {
         // We have eroded enough
         break;
      }
   }
   
   // Add to this-cell totals
   m_dClayToppleDetach += dTotClayDetached; 
   m_dSiltToppleDetach += dTotSiltDetached; 
   m_dSandToppleDetach += dTotSandDetached; 
   m_dTotClayToppleDetach += dTotClayDetached; 
   m_dTotSiltToppleDetach += dTotSiltDetached; 
   m_dTotSandToppleDetach += dTotSandDetached; 
   
   // Add to the this-iteration totals
   m_pSim->AddThisIterClayToppleDetach(dTotClayDetached);
   m_pSim->AddThisIterSiltToppleDetach(dTotSiltDetached);
   m_pSim->AddThisIterSandToppleDetach(dTotSandDetached);
}

// Adds clay-sized sediment to this cell as a result of toppling
void CSoil::DoClayToppleDeposit(double const dChngElev)
{
   // Do the deposition
   // TODO Should we always deposit to the top layer? What about if top layer has been eroded to zero thickness?
   m_VLayer.back().DoToppleClayDepositionLayer(dChngElev);      
   
   m_dClayToppleDeposit    += dChngElev;
   m_dTotClayToppleDeposit += dChngElev;
}

// Adds silt-sized sediment to this cell as a result of toppling
void CSoil::DoSiltToppleDeposit(double const dChngElev)
{
   // Do the deposition
   // TODO Should we always deposit to the top layer? What about if top layer has been eroded to zero thickness?
   m_VLayer.back().DoToppleSiltDepositionLayer(dChngElev);      

   m_dSiltToppleDeposit    += dChngElev;
   m_dTotSiltToppleDeposit += dChngElev;
}

// Adds sand-sized sediment to this cell as a result of toppling
void CSoil::DoSandToppleDeposit(double const dChngElev)
{
   // Do the deposition
   // TODO Should we always deposit to the top layer? What about if top layer has been eroded to zero thickness?
   m_VLayer.back().DoToppleSandDepositionLayer(dChngElev);      

   m_dSandToppleDeposit    += dChngElev;
   m_dTotSandToppleDeposit += dChngElev;
}


// Returns the depth of clay-sized toppling detachment for this cell
double CSoil::dGetClayToppleDetach(void) const
{
   return m_dClayToppleDetach;
}

// Returns the depth of silt-sized toppling detachment for this cell
double CSoil::dGetSiltToppleDetach(void) const
{
   return m_dSiltToppleDetach;
}

// Returns the depth of sand-sized toppling detachment for this cell
double CSoil::dGetSandToppleDetach(void) const
{
   return m_dSandToppleDetach;
}

// Returns the depth of toppling detachment (all sediment size classes) for this cell
double CSoil::dGetTotToppleDetach(void) const
{
   return m_dClayToppleDetach + m_dSiltToppleDetach + m_dSandToppleDetach;
}

// Returns the cumulative depth of clay-sized toppling detachment for this cell
double CSoil::dGetCumulativeClayToppleDetach(void) const
{
   return m_dTotClayToppleDetach;
}

// Returns the cumulative depth of silt-sized toppling detachment for this cell
double CSoil::dGetCumulativeSiltToppleDetach(void) const
{
   return m_dTotSiltToppleDetach;
}

// Returns the cumulative depth of sand-sized toppling detachment for this cell
double CSoil::dGetCumulativeSandToppleDetach(void) const
{
   return m_dTotSandToppleDetach;
}

// Returns the cumulative depth of toppling detachment (all sediment size classes) for this cell
double CSoil::dGetCumulativeTotToppleDetach(void) const
{
   return m_dTotClayToppleDetach + m_dTotSiltToppleDetach + m_dTotSandToppleDetach;
}


// Returns the depth of clay-sized toppling deposition for this cell
double CSoil::dGetClayToppleDeposit(void) const
{
   return m_dClayToppleDeposit;
}

// Returns the depth of silt-sized toppling deposition for this cell
double CSoil::dGetSiltToppleDeposit(void) const
{
   return m_dSiltToppleDeposit;
}

// Returns the depth of sand-sized toppling deposition for this cell
double CSoil::dGetSandToppleDeposit(void) const
{
   return m_dSandToppleDeposit;
}

// Returns the depth of toppling deposition (all size classes) for this cell
double CSoil::dGetTotToppleDeposit(void) const
{
   return m_dClayToppleDeposit + m_dSiltToppleDeposit + m_dSandToppleDeposit;
}

// Returns the cumulative depth of clay-sized toppling deposition for this cell
double CSoil::dGetCumulativeClayToppleDeposit(void) const
{
   return m_dTotClayToppleDeposit;
}

// Returns the cumulative depth of silt-sized toppling deposition for this cell
double CSoil::dGetCumulativeSiltToppleDeposit(void) const
{
   return m_dTotSiltToppleDeposit;
}

// Returns the cumulative depth of sand-sized toppling deposition for this cell
double CSoil::dGetCumulativeSandToppleDeposit(void) const
{
   return m_dTotSandToppleDeposit;
}

// Returns the cumulative depth of toppling deposition (all size classes) for this cell
double CSoil::dGetCumulativeTotToppleDeposit(void) const
{
   return m_dTotClayToppleDeposit + m_dTotSiltToppleDeposit + m_dTotSandToppleDeposit;
}




// Zeros the this-iteration (actually they are kept over several iterations) values for slumping, toppling and shear stress
void CSoil::ZeroThisIterSlump(void)
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


// Returns the depth of clay-sized slump detachment for this cell
double CSoil::dGetClaySlumpDetach(void) const
{
   return m_dClaySlumpDetach;
}

// Returns the depth of silt-sized slump detachment for this cell
double CSoil::dGetSiltSlumpDetach(void) const
{
   return m_dSiltSlumpDetach;
}

// Returns the depth of sand-sized slump detachment for this cell
double CSoil::dGetSandSlumpDetach(void) const
{
   return m_dSandSlumpDetach;
}

// Returns the depth of slump detachment (all size classes) for this cell
double CSoil::dGetTotSlumpDetach(void) const
{
   return (m_dClaySlumpDetach + m_dSiltSlumpDetach + m_dSandSlumpDetach);
}


// Returns the cumulative depth of clay-sized slump detachment for this cell
double CSoil::dGetCumulativeClaySlumpDetach(void) const
{
   return m_dTotClaySlumpDetach;
}

// Returns the cumulative depth of silt-sized slump detachment for this cell
double CSoil::dGetCumulativeSiltSlumpDetach(void) const
{
   return m_dTotSiltSlumpDetach;
}

// Returns the cumulative depth of sand-sized slump detachment for this cell
double CSoil::dGetCumulativeSandSlumpDetach(void) const
{
   return m_dTotSandSlumpDetach;
}

// Returns the cumulative depth of slump detachment (all size classes) for this cell
double CSoil::dGetCumulativeTotSlumpDetach(void) const
{
   return (m_dTotClaySlumpDetach + m_dTotSiltSlumpDetach + m_dTotSandSlumpDetach);
}


void CSoil::DoInfiltrationDeposit(double const dClayDeposit, double const dSiltDeposit, double const dSandDeposit)
{
   // Cell is dry, so do the deposition
   // TODO Should we always deposit to the top layer? What about if top layer has been eroded to zero thickness?
   m_VLayer.back().DoInfiltrationDepositionLayer(dClayDeposit, dSiltDeposit, dSandDeposit);        
   
   // Add to the cell's totals
   m_dClayInfiltDeposit += dClayDeposit;
   m_dSiltInfiltDeposit += dSiltDeposit;
   m_dSandInfiltDeposit += dSandDeposit;
   
   // Add to the this-iteration totals
   m_pSim->AddThisIterClayFlowDeposit(dClayDeposit);
   m_pSim->AddThisIterSiltFlowDeposit(dSiltDeposit);
   m_pSim->AddThisIterSandFlowDeposit(dSandDeposit);
}

void CSoil::SetInfiltrationDepositionZero(void)
{
   m_dClayInfiltDeposit = 
   m_dSiltInfiltDeposit = 
   m_dSandInfiltDeposit = 0;
}

// Returns the clay-sized sediment that was deposited when all water was lost to infiltration
double CSoil::dGetClayInfiltDeposit(void) const
{
   return m_dClayInfiltDeposit;
}

// Returns the silt-sized sediment that was deposited when all water was lost to infiltration
double CSoil::dGetSiltInfiltDeposit(void) const
{
   return m_dSiltInfiltDeposit;
}

// Returns the sand-sized sediment that was deposited when all water was lost to infiltration
double CSoil::dGetSandInfiltDeposit(void) const
{
   return m_dSandInfiltDeposit;
}

// Returns the sediment (all size classes)  that was deposited when all water was lost to infiltration
double CSoil::dGetTotInfiltDeposit(void) const
{
   return (m_dClayInfiltDeposit + m_dSiltInfiltDeposit + m_dSandInfiltDeposit);
}

// Returns the cumulative total of clay-sized sediment that was deposited when all water was lost to infiltration
double CSoil::dGetCumulativeClayInfiltDeposit(void) const
{
   return m_dTotClayInfiltDeposit;
}

// Returns the cumulative total of silt-sized sediment that was deposited when all water was lost to infiltration
double CSoil::dGetCumulativeSiltInfiltDeposit(void) const
{
   return m_dTotSiltInfiltDeposit;
}

// Returns the cumulative total of sand-sized sediment that was deposited when all water was lost to infiltration
double CSoil::dGetCumulativeSandInfiltDeposit(void) const
{
   return m_dTotSandInfiltDeposit;
}

// Returns the cumulative total of sediment (all size classes)  that was deposited when all water was lost to infiltration
double CSoil::dGetCumulativeTotInfiltDeposit(void) const
{
   return (m_dTotClayInfiltDeposit + m_dTotSiltInfiltDeposit + m_dTotSandInfiltDeposit);
}


// Increments the shear stress for this cell
void CSoil::IncShearStress(double const dNewShearStress)
{
   m_dShearStress += dNewShearStress;
}

// Returns the shear stress for this cell
double CSoil::dGetShearStress(void) const
{
   return (m_dShearStress);
}


// Increments this cell's total (cumulative) shear stress
void CSoil::IncTotShearStress(double const dNewShearStress)
{
   m_dTotShearStress += dNewShearStress;
}

// Returns the total (cumulative) shear stress for this cell
double CSoil::dGetTotShearStress(void) const
{
   return (m_dTotShearStress);
}

// Calculates and returns total (cumulative) net soil loss from flow erosion, splash redistribution, slumping, and infiltration deposition
double CSoil::dGetTotNetSoilLoss(void) const
{
   double 
      dTotFlowDetach = m_dTotClayFlowDetach + m_dTotSiltFlowDetach + m_dTotSandFlowDetach,
      dTotFlowDeposit = m_dTotClayFlowDeposit + m_dTotSiltFlowDeposit + m_dTotSandFlowDeposit,
      dTotSplashDetach = m_dTotClaySplashDetach + m_dTotSiltSplashDetach + m_dTotSandSplashDetach,
      dTotSplashDeposit = m_dTotClaySplashDeposit + m_dTotSiltSplashDeposit + m_dTotSandSplashDeposit,
      dTotSlumpDetach = m_dTotClaySlumpDetach + m_dTotSiltSlumpDetach + m_dTotSandSlumpDetach,
      dTotInfiltDeposit = m_dTotClayInfiltDeposit + m_dTotSiltInfiltDeposit + m_dTotSandInfiltDeposit;
      
   return (dTotFlowDetach - dTotFlowDeposit + dTotSplashDetach - dTotSplashDeposit + dTotSlumpDetach - dTotInfiltDeposit);
}

// ============================================================== m_dLaplacian =========================================================
// Sets the Laplacian value for this cell
void CSoil::SetLaplacian(double const dNewLapl)
{
   m_dLaplacian = dNewLapl;
   
   // Also initialize the splash deposition temporary field
   m_dTotSplashDepositToDo = 0;
}

// Returns the Laplacian value for this cell
double CSoil::dGetLaplacian(void) const
{
   return (m_dLaplacian);
};


// Sets several this-iteration values to zero
void CSoil::InitializeDetachmentAndDeposition(bool const bSlump)
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
   m_dClaySlumpDetach   =
   m_dSiltSlumpDetach   =
   m_dSandSlumpDetach   =
   m_dClayToppleDetach  =
   m_dSiltToppleDetach  =
   m_dSandToppleDetach  =
   m_dClayToppleDeposit =
   m_dSiltToppleDeposit =
   m_dSandToppleDeposit =
   m_dLaplacian         = 0;
   
   if (bSlump)
      m_dShearStress = 0;
}


