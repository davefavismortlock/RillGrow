/*=========================================================================================================================================

 This is simulate_infilt.cpp: it simulates infiltration

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
#include "simulation.h"
#include "cell.h"


/*=========================================================================================================================================

 Sets an initial value for subsurface water for every cell

=========================================================================================================================================*/
void CSimulation::InitSoilWater(void)
{
   // Do this for all soil layers
   for (int nLayer = 0; nLayer < m_nNumSoilLayers; nLayer++)
   {
      // Calculate and set initial soil water content in each layer (as a depth equivalent) on all cells
      for (int nX = 0; nX < m_nXGridMax; nX++)
      {
         for (int nY = 0; nY < m_nYGridMax; nY++)
         {
            CCellSoilLayer* pLayer = Cell[nX][nY].pGetSoil()->pLayerGetLayer(nLayer);

            double
               dLayerThickness = pLayer->dGetLayerThickness(),
               dInitialSoilWaterDepth = m_VdInputSoilLayerInfiltInitWater[nLayer] * dLayerThickness;       // in mm

            pLayer->SetSoilWater(dInitialSoilWaterDepth);
         }
      }

      // For this soil, calculate the capillary pressure head at the wetting front. This is equation 1 and 2 of the EPA Explicit Green-Ampt Model (GAEXP) at https://www.epa.gov/water-research/infilt-models#Explicitgreen
      double const GAEXP_CONST_A = 2;
      double const GAEXP_CONST_B = 3;
      double dNu = GAEXP_CONST_A + (GAEXP_CONST_B * m_VdInputSoilLayerInfiltLambda[nLayer]);
      m_VdInfiltCPHWF[nLayer] = (dNu * m_VdInputSoilLayerInfiltAirHead[nLayer]) / (dNu - 1.0);

      // Finally calculate part of GAEXP equation 3, do this now for efficiency
      m_VdInfiltChiPart[nLayer] = (m_VdInputSoilLayerInfiltSatWater[nLayer] - m_VdInputSoilLayerInfiltInitWater[nLayer]) / m_VdInputSoilLayerInfiltKSat[nLayer];
   }
}


/*=========================================================================================================================================

 This calculates subsurface water movement for all cells

=========================================================================================================================================*/
void CSimulation::DoAllInfiltration()
{
   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      for (int nY = 0; nY < m_nYGridMax; nY++)
      {
         // Start at the top soil layer and work downwards
         for (int nLayer = 0; nLayer < m_nNumSoilLayers; nLayer++)
         {
            CCellSoilLayer* pLayer = Cell[nX][nY].pGetSoil()->pLayerGetLayer(nLayer);

            // Get the subsurface water content (a depth equivalent) for this layer
            double dLayerSoilWaterDepth = pLayer->dGetSoilWater();

            // Now calculate the saturated (maximum) soil water content (also a depth equivalent) for this layer
            double
               dLayerThickness = pLayer->dGetLayerThickness(),
               dLayerMaxSoilWaterDepth = m_VdInputSoilLayerInfiltSatWater[nLayer] * dLayerThickness,  // In mm
               dDiff = dLayerMaxSoilWaterDepth - dLayerSoilWaterDepth;

            // Is this soil layer over-saturated?
            if ((dDiff + TOLERANCE) < 0)
            {
               // Yes, so do exfilt
               DoCellExfiltration(nX, nY, nLayer, pLayer, -dDiff);

               if (nLayer == 0)
                  // This is the top layer
                  m_dSinceLastExfiltration += dDiff;

//                m_ofsLog << m_ulIter << ": exfiltration " << -dDiff << " from layer " << nLayer << " at [" << nX << "][" << nY << "] since dLayerMaxSoilWaterDepth = " << dLayerMaxSoilWaterDepth << ", dLayerSoilWaterDepth = " << dLayerSoilWaterDepth << endl;

               continue;
            }

            // Is this soil layer under-saturated?
            if ((dDiff - TOLERANCE) > 0)
            {
               // Yes, so do infilt
//                m_ofsLog << m_ulIter << ": infiltration " << dDiff << " into layer " << nLayer << " at [" << nX << "][" << nY << "] since dLayerMaxSoilWaterDepth = " << dLayerMaxSoilWaterDepth << ", dLayerSoilWaterDepth = " << dLayerSoilWaterDepth << endl;

               DoCellInfiltration(nX, nY, nLayer, pLayer, dDiff);

               if (nLayer == 0)
                  // This is the top layer
                  m_dSinceLastInfiltration += dDiff;
            }

            // Update the this-operation total
            m_VdThisIterSoilWater[nLayer] += Cell[nX][nY].pGetSoil()->pLayerGetLayer(nLayer)->dGetSoilWater();
         }
      }
   }
}


/*=========================================================================================================================================

 This member function of CSimulation calculates water loss from infilt for one cell using the EPA Explicit Green-Ampt Model (GAEXP), see
 http://www.epa.gov/ada/csmos/ninflmod.html

=========================================================================================================================================*/
void CSimulation::DoCellInfiltration(int const nX, int const nY, int const nLayer, CCellSoilLayer* pLayer, double const dDeficit)
{
   // The layer is not fully saturated, so maybe can get water from the layer above, or from surface water if this is the top layer
   CCellSoilLayer* pLayerAbove = NULL;
   double dWaterDepthAbove = 0;
   if (nLayer == 0)
   {
      // This is the top layer
      if (! Cell[nX][nY].pGetSurfaceWater()->bIsWet())
         return;

      // Cell is wet, so get the depth of surface water
      dWaterDepthAbove = Cell[nX][nY].pGetSurfaceWater()->dGetSurfaceWaterDepth();
   }
   else
   {
      // This is not the top layer
      pLayerAbove = Cell[nX][nY].pGetSoil()->pLayerGetLayer(nLayer-1);

      // Get the depth of water in the soil layer above
      dWaterDepthAbove = pLayerAbove->dGetSoilWater();
   }

   // Calculate the remaining part of equation 3
   double dChi = (dWaterDepthAbove - m_VdInfiltCPHWF[nLayer]) * m_VdInfiltChiPart[nLayer];

   double dTimeElapsedinHours = m_dSimulatedTimeElapsed / 3600;           // in hours

   // Equation 4
   double dTauT = dTimeElapsedinHours / (dTimeElapsedinHours + dChi);

   // Equation 5
   double dInfiltrationRate = (((sqrt(2.0)/2.0) * pow(dTauT, -0.5)) + (2.0/3.0) - ((sqrt(2.0)/6.0) * pow(dTauT, 0.5)) + (((1.0-sqrt(2.0))/3.0) * dTauT)) * m_VdInputSoilLayerInfiltKSat[nLayer];

   // Convert from cm/hr to mm/sec
   dInfiltrationRate /= 360.0;

   // And calculate the potential depth to try to infiltrate. However, this must not result in soil water being greater than the saturated maximum
   double dPotentialDepthToInfiltrate = tMin(dInfiltrationRate * m_dTimeStep, dDeficit);

   double dDepthToInfiltrate = 0;
   if (dWaterDepthAbove > dPotentialDepthToInfiltrate)
   {
      // After infiltration, there will still be some water above (either in the layer above, or on the surface)
      dDepthToInfiltrate = dPotentialDepthToInfiltrate;

      if (nLayer == 0)
      {
         // This is the top layer, so remove from the surface water and update total infilt for this cell. Note that if there is insufficient surface water, dDepthToInfiltrate is reduced
         Cell[nX][nY].pGetSoilWater()->DoInfiltration(dDepthToInfiltrate);
      }
      else
      {
         // This is not the top layer: remove water from the soil layer above and add to this layer
         pLayerAbove->ChangeSoilWater(-dDepthToInfiltrate);
         pLayer->ChangeSoilWater(dDepthToInfiltrate);
      }
   }
   else
   {
      // All water lost to infiltration
      dDepthToInfiltrate = dWaterDepthAbove;

      if (nLayer == 0)
      {
         // This is the top layer, so remove the water, update total infilt for this cell, assume that any in-transport sediment is deposited and add this to the per-iteration total of infitration-deposited sediment
         double
            dClayDeposited = 0,
            dSiltDeposited = 0,
            dSandDeposited = 0;

         Cell[nX][nY].pGetSoilWater()->InfiltrateAndMakeDry(dClayDeposited, dSiltDeposited, dSandDeposited);

         m_dSinceLastClayInfiltDeposit += dClayDeposited;
         m_dSinceLastSiltInfiltDeposit += dSiltDeposited;
         m_dSinceLastSandInfiltDeposit += dSandDeposited;
      }
      else
      {
         // This is not the top layer: remove water from the soil layer above and add to this layer
         pLayerAbove->ChangeSoilWater(-dDepthToInfiltrate);
         pLayer->ChangeSoilWater(dDepthToInfiltrate);
      }
   }
}


/*=========================================================================================================================================

Calculates water loss from exfilt for one cell TODO this needs to be looked at

=========================================================================================================================================*/
void CSimulation::DoCellExfiltration(int const nX, int const nY, int const nLayer, CCellSoilLayer* pLayer, double const dExcess)
{
   // The current soil layer is over-saturated, so we must try to get rid of some water from it. First try to move it downwards to the layer below
   if (nLayer < m_nNumSoilLayers-1)
   {
      // We are not on the lowest (i.e. just above basement) layer, so get a pointer to the layer below
      CCellSoilLayer* pLayerBelow = Cell[nX][nY].pGetSoil()->pLayerGetLayer(nLayer+1);

      // Get the soil water content for the layer below
      double dLayerBelowSoilWater = pLayerBelow->dGetSoilWater();

      // Now calculate the saturated (maximum) soil water content (a depth equivalent) for the layer below
      double
         dLayerBelowThickness = pLayerBelow->dGetLayerThickness(),
         dLayerBelowMaxSoilWater = m_VdInputSoilLayerInfiltSatWater[nLayer+1] * dLayerBelowThickness,
         dLayerBelowDiff = dLayerBelowMaxSoilWater - dLayerBelowSoilWater;

      if (dLayerBelowDiff > 0)
      {
         // The layer below is not fully saturated, so infiltrate some water into it TODO need to use GAEXP for this
         double dToMoveToLayerBelow = tMin(dExcess, dLayerBelowMaxSoilWater - dLayerBelowSoilWater);

         // Remove water from this layer and add to the layer below
         pLayerBelow->ChangeSoilWater(dToMoveToLayerBelow);
         pLayer->ChangeSoilWater(-dToMoveToLayerBelow);

         return;
      }
   }

   // We could not infiltrate to the layer below, so try to exfiltrate the excess water upwards
   if (nLayer == 0)
   {
      // This is the top layer
//       m_ofsLog << m_ulIter << " [" << nX << "][" << nY << "] exfilt to surface water = " << dExcess << endl;

      // Exfiltrate i.e. remove water from this layer and add it to the surface water
      Cell[nX][nY].pGetSoilWater()->DoExfiltration(dExcess);

      return;
   }
   else
   {
      // This is not the top layer
//       m_ofsLog << m_ulIter << " [" << nX << "][" << nY << "] exfilt from layer " << nLayer << " to " << nLayer-1 << " = " << dExcess << endl;

      // Remove water from this layer
      pLayer->ChangeSoilWater(-dExcess);

      // And add it to the layer above
      CCellSoilLayer* pLayerAbove = Cell[nX][nY].pGetSoil()->pLayerGetLayer(nLayer-1);
      pLayerAbove->ChangeSoilWater(dExcess);
   }
}
