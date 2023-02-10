/*=========================================================================================================================================

 This is m_Cell_soil_layer.cpp: implementations of the RillGrow class used to represent a layer in the soil column

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
#include "cell_soil_layer.h"


CCellSoilLayer::CCellSoilLayer(void)
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

CCellSoilLayer::~CCellSoilLayer(void)
{
}


void CCellSoilLayer::SetName(string const* pstrName)
{
   m_strName = *pstrName;
}

string* CCellSoilLayer::pstrGetName(void)
{
   return &m_strName;
}


void CCellSoilLayer::SetClayThickness(double const dThick)
{
   m_dClayThickness = dThick;
}

void CCellSoilLayer::SetSiltThickness(double const dThick)
{
   m_dSiltThickness = dThick;
}

void CCellSoilLayer::SetSandThickness(double const dThick)
{
   m_dSandThickness = dThick;
}

double CCellSoilLayer::dGetClayThickness(void) const
{
   return m_dClayThickness;
}

double CCellSoilLayer::dGetSiltThickness(void) const
{
   return m_dSiltThickness;
}

double CCellSoilLayer::dGetSandThickness(void) const
{
   return m_dSandThickness;
}

double CCellSoilLayer::dGetLayerThickness(void) const
{
   return m_dClayThickness + m_dSiltThickness + m_dSandThickness;
}

// bool CCellSoilLayer::bIsZeroTotalThickness(void) const
// {
//    return ((m_dClayThickness + m_dSiltThickness + m_dSandThickness) > 0 ? false : true);
// }


void CCellSoilLayer::SetBulkDensity(double const dBulkDensity)
{
   m_dBulkDensity = dBulkDensity;
}

double CCellSoilLayer::dGetBulkDensity(void) const
{
   return m_dBulkDensity;
}


void CCellSoilLayer::SetClayFlowErodibility(double const dErod)
{
   m_dClayFlowErodibility = dErod;
}

void CCellSoilLayer::SetSiltFlowErodibility(double const dErod)
{
   m_dSiltFlowErodibility = dErod;
}

void CCellSoilLayer::SetSandFlowErodibility(double const dErod)
{
   m_dSandFlowErodibility = dErod;
}

double CCellSoilLayer::dGetClayFlowErodibility(void) const
{
   return m_dClayFlowErodibility;
}

double CCellSoilLayer::dGetSiltFlowErodibility(void) const
{
   return m_dSiltFlowErodibility;
}

double CCellSoilLayer::dGetSandFlowErodibility(void) const
{
   return m_dSandFlowErodibility;
}


void CCellSoilLayer::SetClaySplashErodibility(double const dErod)
{
   m_dClaySplashErodibility = dErod;
}

void CCellSoilLayer::SetSiltSplashErodibility(double const dErod)
{
   m_dSiltSplashErodibility = dErod;
}

void CCellSoilLayer::SetSandSplashErodibility(double const dErod)
{
   m_dSandSplashErodibility = dErod;
}

double CCellSoilLayer::dGetClaySplashErodibility(void) const
{
   return m_dClaySplashErodibility;
}

double CCellSoilLayer::dGetSiltSplashErodibility(void) const
{
   return m_dSiltSplashErodibility;
}

double CCellSoilLayer::dGetSandSplashErodibility(void) const
{
   return m_dSandSplashErodibility;
}


void CCellSoilLayer::SetClaySlumpErodibility(double const dErod)
{
   m_dClaySlumpErodibility = dErod;
}

void CCellSoilLayer::SetSiltSlumpErodibility(double const dErod)
{
   m_dSiltSlumpErodibility = dErod;
}

void CCellSoilLayer::SetSandSlumpErodibility(double const dErod)
{
   m_dSandSlumpErodibility = dErod;
}

double CCellSoilLayer::dGetClaySlumpErodibility(void) const
{
   return m_dClaySlumpErodibility;
}

double CCellSoilLayer::dGetSiltSlumpErodibility(void) const
{
   return m_dSiltSlumpErodibility;
}

double CCellSoilLayer::dGetSandSlumpErodibility(void) const
{
   return m_dSandSlumpErodibility;
}


void CCellSoilLayer::DoLayerFlowErosion(double const dToErode, double& dClayEroded, double& dSiltEroded, double& dSandEroded)
{
   double dTotalThickness = m_dClayThickness + m_dSiltThickness + m_dSandThickness;

   // Is this a zero-thickness layer?
   if (dTotalThickness == 0)
      return;

   // OK, we have a layer that can be eroded by flow, so partition the total lowering for this m_Cell between the three size fractions: do this by relative erodibility
   int
      nClayWeight = (m_dClayThickness > 0 ? 1 : 0),
      nSiltWeight = (m_dSiltThickness > 0 ? 1 : 0),
      nSandWeight = (m_dSandThickness > 0 ? 1 : 0);

   double dTotErodibility = (nClayWeight * m_dClayFlowErodibility) + (nSiltWeight * m_dSiltFlowErodibility) + (nSandWeight * m_dSandFlowErodibility);

   if (nClayWeight)
   {
      // Erode some clay-sized sediment
      double dTmp = (m_dClayFlowErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the m_Cell
      dClayEroded = tMin(m_dClayThickness, dTmp);

      // Do the erosion
      m_dClayThickness -= dClayEroded;
   }

   if (nSiltWeight)
   {
      // Erode some silt-sized sediment
      double dTmp = (m_dSiltFlowErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the m_Cell
      dSiltEroded = tMin(m_dSiltThickness, dTmp);

      // Do the erosion
      m_dSiltThickness -= dSiltEroded;
   }

   if (nSandWeight)
   {
      // Erode some sand-sized sediment
      double dTmp = (m_dSandFlowErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the m_Cell
      dSandEroded = tMin(m_dSandThickness, dTmp);

      // Do the erosion
      m_dSandThickness -= dSandEroded;
   }
}


void CCellSoilLayer::DoLayerSplashErosion(double const dToErode, double& dClayEroded, double& dSiltEroded, double& dSandEroded)
{
   double dTotalThickness = m_dClayThickness + m_dSiltThickness + m_dSandThickness;

   // Is this a zero-thickness layer?
   if (dTotalThickness == 0)
      return;

   // OK, we have a layer that can be eroded by splash, so partition the total lowering for this m_Cell between the three size fractions: do this by relative erodibility
   int
      nClayWeight = (m_dClayThickness > 0 ? 1 : 0),
      nSiltWeight = (m_dSiltThickness > 0 ? 1 : 0),
      nSandWeight = (m_dSandThickness > 0 ? 1 : 0);

   double dTotErodibility = (nClayWeight * m_dClaySplashErodibility) + (nSiltWeight * m_dSiltSplashErodibility) + (nSandWeight * m_dSandSplashErodibility);

   if (nClayWeight)
   {
      // Erode some clay-sized sediment
      double dTmp = (m_dClaySplashErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the m_Cell
      dClayEroded = tMin(m_dClayThickness, dTmp);

      // Do the erosion
      m_dClayThickness -= dClayEroded;
   }

   if (nSiltWeight)
   {
      // Erode some silt-sized sediment
      double dTmp = (m_dSiltSplashErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the m_Cell
      dSiltEroded = tMin(m_dSiltThickness, dTmp);

      // Do the erosion
      m_dSiltThickness -= dSiltEroded;
   }

   if (nSandWeight)
   {
      // Erode some sand-sized sediment
      double dTmp = (m_dSandSplashErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the m_Cell
      dSandEroded = tMin(m_dSandThickness, dTmp);

      // Do the erosion
      m_dSandThickness -= dSandEroded;
   }
}


void CCellSoilLayer::DoLayerSlumpErosion(double const dToErode, double& dClayEroded, double& dSiltEroded, double& dSandEroded)
{
   double dTotalThickness = m_dClayThickness + m_dSiltThickness + m_dSandThickness;

   // Is this a zero-thickness layer?
   if (dTotalThickness == 0)
      return;

   // OK, we have a layer that can be eroded by slumping, so partition the total lowering for this m_Cell between the three size fractions: do this by relative erodibility
   int
      nClayWeight = (m_dClayThickness > 0 ? 1 : 0),
      nSiltWeight = (m_dSiltThickness > 0 ? 1 : 0),
      nSandWeight = (m_dSandThickness > 0 ? 1 : 0);

   double dTotErodibility = (nClayWeight * m_dClaySlumpErodibility) + (nSiltWeight * m_dSiltSlumpErodibility) + (nSandWeight * m_dSandSlumpErodibility);

   if (nClayWeight)
   {
      // Erode some clay-sized sediment
      double dTmp = (m_dClaySlumpErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the m_Cell
      dClayEroded = tMin(m_dClayThickness, dTmp);

      // Do the erosion
      m_dClayThickness -= dClayEroded;
   }

   if (nSiltWeight)
   {
      // Erode some silt-sized sediment
      double dTmp = (m_dSiltSlumpErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the m_Cell
      dSiltEroded = tMin(m_dSiltThickness, dTmp);

      // Do the erosion
      m_dSiltThickness -= dSiltEroded;
   }

   if (nSandWeight)
   {
      // Erode some sand-sized sediment
      double dTmp = (m_dSandSlumpErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the m_Cell
      dSandEroded = tMin(m_dSandThickness, dTmp);

      // Do the erosion
      m_dSandThickness -= dSandEroded;
   }
}

void CCellSoilLayer::DoLayerToppleErosion(double const dToErode, double& dClayEroded, double& dSiltEroded, double& dSandEroded)
{
   double dTotalThickness = m_dClayThickness + m_dSiltThickness + m_dSandThickness;

   // Is this a zero-thickness layer?
   if (dTotalThickness == 0)
      return;

   // OK, we have a layer that can be eroded by toppling, so partition the total lowering for this m_Cell between the three size fractions: do this by relative erodibility
   int
   nClayWeight = (m_dClayThickness > 0 ? 1 : 0),
   nSiltWeight = (m_dSiltThickness > 0 ? 1 : 0),
   nSandWeight = (m_dSandThickness > 0 ? 1 : 0);

   double dTotErodibility = (nClayWeight * m_dClaySlumpErodibility) + (nSiltWeight * m_dSiltSlumpErodibility) + (nSandWeight * m_dSandSlumpErodibility);

   if (nClayWeight)
   {
      // Erode some clay-sized sediment
      double dTmp = (m_dClaySlumpErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the m_Cell
      dClayEroded = tMin(m_dClayThickness, dTmp);

      // Do the erosion
      m_dClayThickness -= dClayEroded;
   }

   if (nSiltWeight)
   {
      // Erode some silt-sized sediment
      double dTmp = (m_dSiltSlumpErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the m_Cell
      dSiltEroded = tMin(m_dSiltThickness, dTmp);

      // Do the erosion
      m_dSiltThickness -= dSiltEroded;
   }

   if (nSandWeight)
   {
      // Erode some sand-sized sediment
      double dTmp = (m_dSandSlumpErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the m_Cell
      dSandEroded = tMin(m_dSandThickness, dTmp);

      // Do the erosion
      m_dSandThickness -= dSandEroded;
   }
}


void CCellSoilLayer::DoLayerDeposition(double const dClayDepth, double const dSiltDepth, double const dSandDepth)
{
   m_dClayThickness += dClayDepth;
   m_dSiltThickness += dSiltDepth;
   m_dSandThickness += dSandDepth;
}


void CCellSoilLayer::SetSoilWater(double const dWater)
{
   m_dSoilWater = dWater;
}

void CCellSoilLayer::ChangeSoilWater(double const dWater)
{
   m_dSoilWater += dWater;
}

double CCellSoilLayer::dGetSoilWater(void) const
{
   return m_dSoilWater;
}


void CCellSoilLayer::DoLayerHeadcutRetreatErosion(double const dToErode, double& dClayEroded, double& dSiltEroded, double& dSandEroded)
{
   double dTotalThickness = m_dClayThickness + m_dSiltThickness + m_dSandThickness;

   // Is this a zero-thickness layer?
   if (dTotalThickness == 0)
      return;

   // OK, we have a layer that can be eroded by slumping, so partition the total lowering for this m_Cell between the three size fractions: do this by relative erodibility
   int
   nClayWeight = (m_dClayThickness > 0 ? 1 : 0),
   nSiltWeight = (m_dSiltThickness > 0 ? 1 : 0),
   nSandWeight = (m_dSandThickness > 0 ? 1 : 0);

   double dTotErodibility = (nClayWeight * m_dClaySlumpErodibility) + (nSiltWeight * m_dSiltSlumpErodibility) + (nSandWeight * m_dSandSlumpErodibility);

   if (nClayWeight)
   {
      // Erode some clay-sized sediment
      double dTmp = (m_dClaySlumpErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the m_Cell
      dClayEroded = tMin(m_dClayThickness, dTmp);

      // Do the erosion
      m_dClayThickness -= dClayEroded;
   }

   if (nSiltWeight)
   {
      // Erode some silt-sized sediment
      double dTmp = (m_dSiltSlumpErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the m_Cell
      dSiltEroded = tMin(m_dSiltThickness, dTmp);

      // Do the erosion
      m_dSiltThickness -= dSiltEroded;
   }

   if (nSandWeight)
   {
      // Erode some sand-sized sediment
      double dTmp = (m_dSandSlumpErodibility * dToErode) / dTotErodibility;

      // Make sure we don't get -ve amounts left on the m_Cell
      dSandEroded = tMin(m_dSandThickness, dTmp);

      // Do the erosion
      m_dSandThickness -= dSandEroded;
   }
}


// This ugly-but-possibly-necessary method changes the thickness of a soil layer; it is for use in the AdjustUnboundedEdges() routine only
void CCellSoilLayer::ChangeThickness(double const dChange)
{
   double dTotalThickness = m_dClayThickness + m_dSiltThickness + m_dSandThickness;

   if (dTotalThickness > 0)
   {
      // This is not a zero thickness layer, so change the thickness of each size component in proportion to that component's existing thickness
      m_dClayThickness += (dChange * m_dClayThickness / dTotalThickness);
      m_dSiltThickness += (dChange * m_dSiltThickness / dTotalThickness);
      m_dSandThickness += (dChange * m_dSandThickness / dTotalThickness);
   }
   else if (dChange > 0)
   {
      // This is a zero thickness layer: we can't remove from it, so if dChange is -ve then do nothing. If dChange is +ve then add to each size component equally
      m_dClayThickness += (dChange / 3);
      m_dSiltThickness += (dChange / 3);
      m_dSandThickness += (dChange / 3);
   }
}

