/*=========================================================================================================================================

 This is cell_soil_layer.cpp: implementations of the RillGrow class used to represent a layer in the soil column

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
#include "cell_soil_layer.h"


CLayer::CLayer(void)
:
   m_dClayThickness(0),
   m_dSiltThickness(0),
   m_dSandThickness(0),
   m_dBulkDensity(0),
   m_dClayFlowErodibility(0),
   m_dSiltFlowErodibility(0),
   m_dSandFlowErodibility(0),
   m_dClaySplashErodibility(0),
   m_dSiltSplashErodibility(0),
   m_dSandSplashErodibility(0),
   m_dClaySlumpErodibility(0),
   m_dSiltSlumpErodibility(0),
   m_dSandSlumpErodibility(0),
   m_dSoilWater(0)
{
}

CLayer::~CLayer(void)
{
}


void CLayer::SetName(string const* pstrName)
{
   m_strName = *pstrName;
}

string* CLayer::pstrGetName(void)
{
   return &m_strName;
}


void CLayer::SetClayThickness(double const dThick)
{
   m_dClayThickness = dThick;
}

void CLayer::SetSiltThickness(double const dThick)
{
   m_dSiltThickness = dThick;
}

void CLayer::SetSandThickness(double const dThick)
{
   m_dSandThickness = dThick;
}

double CLayer::dGetClayThickness(void) const
{
   return m_dClayThickness;
}

double CLayer::dGetSiltThickness(void) const
{
   return m_dSiltThickness;
}

double CLayer::dGetSandThickness(void) const
{
   return m_dSandThickness;
}

double CLayer::dGetLayerThickness(void) const
{
   return m_dClayThickness + m_dSiltThickness + m_dSandThickness;
}

// bool CLayer::bIsZeroTotalThickness(void) const
// {
//    return ((m_dClayThickness + m_dSiltThickness + m_dSandThickness) > 0 ? false : true);
// }


void CLayer::SetBulkDensity(double const dBulkDensity)
{
   m_dBulkDensity = dBulkDensity;
}

double CLayer::dGetBulkDensity(void) const
{
   return m_dBulkDensity;
}


void CLayer::SetClayFlowErodibility(double const dErod)
{
   m_dClayFlowErodibility = dErod;
}

void CLayer::SetSiltFlowErodibility(double const dErod)
{
   m_dSiltFlowErodibility = dErod;
}

void CLayer::SetSandFlowErodibility(double const dErod)
{
   m_dSandFlowErodibility = dErod;
}

double CLayer::dGetClayFlowErodibility(void) const
{
   return m_dClayFlowErodibility;
}

double CLayer::dGetSiltFlowErodibility(void) const
{
   return m_dSiltFlowErodibility;
}

double CLayer::dGetSandFlowErodibility(void) const
{
   return m_dSandFlowErodibility;
}


void CLayer::SetClaySplashErodibility(double const dErod)
{
   m_dClaySplashErodibility = dErod;
}

void CLayer::SetSiltSplashErodibility(double const dErod)
{
   m_dSiltSplashErodibility = dErod;
}

void CLayer::SetSandSplashErodibility(double const dErod)
{
   m_dSandSplashErodibility = dErod;
}

double CLayer::dGetClaySplashErodibility(void) const
{
   return m_dClaySplashErodibility;
}

double CLayer::dGetSiltSplashErodibility(void) const
{
   return m_dSiltSplashErodibility;
}

double CLayer::dGetSandSplashErodibility(void) const
{
   return m_dSandSplashErodibility;
}


void CLayer::SetClaySlumpErodibility(double const dErod)
{
   m_dClaySlumpErodibility = dErod;
}

void CLayer::SetSiltSlumpErodibility(double const dErod)
{
   m_dSiltSlumpErodibility = dErod;
}

void CLayer::SetSandSlumpErodibility(double const dErod)
{
   m_dSandSlumpErodibility = dErod;
}

double CLayer::dGetClaySlumpErodibility(void) const
{
   return m_dClaySlumpErodibility;
}

double CLayer::dGetSiltSlumpErodibility(void) const
{
   return m_dSiltSlumpErodibility;
}

double CLayer::dGetSandSlumpErodibility(void) const
{
   return m_dSandSlumpErodibility;
}


void CLayer::DoLayerFlowErosion(double const dToErode, double& dClayEroded, double& dSiltEroded, double& dSandEroded)
{
   double dTotalThickness = m_dClayThickness + m_dSiltThickness + m_dSandThickness;

   // Is this a zero-thickness layer?
   if (dTotalThickness == 0)
      return;

   // OK, we have a layer that can be eroded by flow, so partition the total lowering for this cell between the three size fractions: do this by relative erodibility
   int
      nClayWeight = (m_dClayThickness > 0 ? 1 : 0),
      nSiltWeight = (m_dSiltThickness > 0 ? 1 : 0),
      nSandWeight = (m_dSandThickness > 0 ? 1 : 0);

   double dTotErodibility = (nClayWeight * m_dClayFlowErodibility) + (nSiltWeight * m_dSiltFlowErodibility) + (nSandWeight * m_dSandFlowErodibility);

   if (nClayWeight)
   {
      // Erode some clay-sized sediment
      double dTmp = (m_dClayFlowErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the cell
      dClayEroded = tMin(m_dClayThickness, dTmp);

      // Do the erosion
      m_dClayThickness -= dClayEroded;
   }

   if (nSiltWeight)
   {
      // Erode some silt-sized sediment
      double dTmp = (m_dSiltFlowErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the cell
      dSiltEroded = tMin(m_dSiltThickness, dTmp);

      // Do the erosion
      m_dSiltThickness -= dSiltEroded;
   }

   if (nSandWeight)
   {
      // Erode some sand-sized sediment
      double dTmp = (m_dSandFlowErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the cell
      dSandEroded = tMin(m_dSandThickness, dTmp);

      // Do the erosion
      m_dSandThickness -= dSandEroded;
   }
}


void CLayer::DoLayerSplashErosion(double const dToErode, double& dClayEroded, double& dSiltEroded, double& dSandEroded)
{
   double dTotalThickness = m_dClayThickness + m_dSiltThickness + m_dSandThickness;

   // Is this a zero-thickness layer?
   if (dTotalThickness == 0)
      return;

   // OK, we have a layer that can be eroded by splash, so partition the total lowering for this cell between the three size fractions: do this by relative erodibility
   int
      nClayWeight = (m_dClayThickness > 0 ? 1 : 0),
      nSiltWeight = (m_dSiltThickness > 0 ? 1 : 0),
      nSandWeight = (m_dSandThickness > 0 ? 1 : 0);

   double dTotErodibility = (nClayWeight * m_dClaySplashErodibility) + (nSiltWeight * m_dSiltSplashErodibility) + (nSandWeight * m_dSandSplashErodibility);

   if (nClayWeight)
   {
      // Erode some clay-sized sediment
      double dTmp = (m_dClaySplashErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the cell
      dClayEroded = tMin(m_dClayThickness, dTmp);

      // Do the erosion
      m_dClayThickness -= dClayEroded;
   }

   if (nSiltWeight)
   {
      // Erode some silt-sized sediment
      double dTmp = (m_dSiltSplashErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the cell
      dSiltEroded = tMin(m_dSiltThickness, dTmp);

      // Do the erosion
      m_dSiltThickness -= dSiltEroded;
   }

   if (nSandWeight)
   {
      // Erode some sand-sized sediment
      double dTmp = (m_dSandSplashErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the cell
      dSandEroded = tMin(m_dSandThickness, dTmp);

      // Do the erosion
      m_dSandThickness -= dSandEroded;
   }
}


void CLayer::DoLayerSlumpErosion(double const dToErode, double& dClayEroded, double& dSiltEroded, double& dSandEroded)
{
   double dTotalThickness = m_dClayThickness + m_dSiltThickness + m_dSandThickness;

   // Is this a zero-thickness layer?
   if (dTotalThickness == 0)
      return;

   // OK, we have a layer that can be eroded by slumping, so partition the total lowering for this cell between the three size fractions: do this by relative erodibility
   int
      nClayWeight = (m_dClayThickness > 0 ? 1 : 0),
      nSiltWeight = (m_dSiltThickness > 0 ? 1 : 0),
      nSandWeight = (m_dSandThickness > 0 ? 1 : 0);

   double dTotErodibility = (nClayWeight * m_dClaySlumpErodibility) + (nSiltWeight * m_dSiltSlumpErodibility) + (nSandWeight * m_dSandSlumpErodibility);

   if (nClayWeight)
   {
      // Erode some clay-sized sediment
      double dTmp = (m_dClaySlumpErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the cell
      dClayEroded = tMin(m_dClayThickness, dTmp);

      // Do the erosion
      m_dClayThickness -= dClayEroded;
   }

   if (nSiltWeight)
   {
      // Erode some silt-sized sediment
      double dTmp = (m_dSiltSlumpErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the cell
      dSiltEroded = tMin(m_dSiltThickness, dTmp);

      // Do the erosion
      m_dSiltThickness -= dSiltEroded;
   }

   if (nSandWeight)
   {
      // Erode some sand-sized sediment
      double dTmp = (m_dSandSlumpErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the cell
      dSandEroded = tMin(m_dSandThickness, dTmp);

      // Do the erosion
      m_dSandThickness -= dSandEroded;
   }
}

void CLayer::DoLayerToppleErosion(double const dToErode, double& dClayEroded, double& dSiltEroded, double& dSandEroded)
{
   double dTotalThickness = m_dClayThickness + m_dSiltThickness + m_dSandThickness;

   // Is this a zero-thickness layer?
   if (dTotalThickness == 0)
      return;

   // OK, we have a layer that can be eroded by toppling, so partition the total lowering for this cell between the three size fractions: do this by relative erodibility
   int
   nClayWeight = (m_dClayThickness > 0 ? 1 : 0),
   nSiltWeight = (m_dSiltThickness > 0 ? 1 : 0),
   nSandWeight = (m_dSandThickness > 0 ? 1 : 0);

   double dTotErodibility = (nClayWeight * m_dClaySlumpErodibility) + (nSiltWeight * m_dSiltSlumpErodibility) + (nSandWeight * m_dSandSlumpErodibility);

   if (nClayWeight)
   {
      // Erode some clay-sized sediment
      double dTmp = (m_dClaySlumpErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the cell
      dClayEroded = tMin(m_dClayThickness, dTmp);

      // Do the erosion
      m_dClayThickness -= dClayEroded;
   }

   if (nSiltWeight)
   {
      // Erode some silt-sized sediment
      double dTmp = (m_dSiltSlumpErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the cell
      dSiltEroded = tMin(m_dSiltThickness, dTmp);

      // Do the erosion
      m_dSiltThickness -= dSiltEroded;
   }

   if (nSandWeight)
   {
      // Erode some sand-sized sediment
      double dTmp = (m_dSandSlumpErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the cell
      dSandEroded = tMin(m_dSandThickness, dTmp);

      // Do the erosion
      m_dSandThickness -= dSandEroded;
   }
}


void CLayer::DoLayerDeposition(double const dClayDepth, double const dSiltDepth, double const dSandDepth)
{
   m_dClayThickness += dClayDepth;
   m_dSiltThickness += dSiltDepth;
   m_dSandThickness += dSandDepth;
}


void CLayer::SetSoilWater(double const dWater)
{
   m_dSoilWater = dWater;
}

void CLayer::ChangeSoilWater(double const dWater)
{
   m_dSoilWater += dWater;
}

double CLayer::dGetSoilWater(void) const
{
   return m_dSoilWater;
}


void CLayer::DoLayerHeadcutRetreatErosion(double const dToErode, double& dClayEroded, double& dSiltEroded, double& dSandEroded)
{
   double dTotalThickness = m_dClayThickness + m_dSiltThickness + m_dSandThickness;

   // Is this a zero-thickness layer?
   if (dTotalThickness == 0)
      return;

   // OK, we have a layer that can be eroded by slumping, so partition the total lowering for this cell between the three size fractions: do this by relative erodibility
   int
   nClayWeight = (m_dClayThickness > 0 ? 1 : 0),
   nSiltWeight = (m_dSiltThickness > 0 ? 1 : 0),
   nSandWeight = (m_dSandThickness > 0 ? 1 : 0);

   double dTotErodibility = (nClayWeight * m_dClaySlumpErodibility) + (nSiltWeight * m_dSiltSlumpErodibility) + (nSandWeight * m_dSandSlumpErodibility);

   if (nClayWeight)
   {
      // Erode some clay-sized sediment
      double dTmp = (m_dClaySlumpErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the cell
      dClayEroded = tMin(m_dClayThickness, dTmp);

      // Do the erosion
      m_dClayThickness -= dClayEroded;
   }

   if (nSiltWeight)
   {
      // Erode some silt-sized sediment
      double dTmp = (m_dSiltSlumpErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the cell
      dSiltEroded = tMin(m_dSiltThickness, dTmp);

      // Do the erosion
      m_dSiltThickness -= dSiltEroded;
   }

   if (nSandWeight)
   {
      // Erode some sand-sized sediment
      double dTmp = (m_dSandSlumpErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the cell
      dSandEroded = tMin(m_dSandThickness, dTmp);

      // Do the erosion
      m_dSandThickness -= dSandEroded;
   }
}


// This ugly-but-possibly-necessary method changes the thickness of a soil layer; it is for use in the AdjustUnboundedEdges() routine only
// void CLayer::ChangeThickness(double const dChange)
// {
//    double dTotalThickness = m_dClayThickness + m_dSiltThickness + m_dSandThickness;
//
//    if (dTotalThickness > 0)
//    {
//       // This is not a zero thickness layer, so change the thickness of each size component in proportion to that component's existing thickness
//       m_dClayThickness += (dChange * m_dClayThickness / dTotalThickness);
//       m_dSiltThickness += (dChange * m_dSiltThickness / dTotalThickness);
//       m_dSandThickness += (dChange * m_dSandThickness / dTotalThickness);
//    }
//    else if (dChange > 0)
//    {
//       // This is a zero thickness layer: we can't remove from it, so if dChange is -ve then do nothing. If dChange is +ve then add to each size component equally
//       m_dClayThickness += (dChange / 3);
//       m_dSiltThickness += (dChange / 3);
//       m_dSandThickness += (dChange / 3);
//    }
// }

