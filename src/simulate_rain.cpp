/*=========================================================================================================================================

 This is simulate_rain.cpp: it handles rainfall

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

 Simulates run-on from a single edge

=========================================================================================================================================*/
void CSimulation::DoRunOnFromOneEdge(int const nEdge)
{
   // There is run-on from this edge
   double dEdgeLen = m_nYGridMax;         // Start by assuming run-on from the left or right edges
   if ((nEdge == EDGE_TOP) || (nEdge == EDGE_BOTTOM))
      // Nope, run-on is from the top or bottom edge
      dEdgeLen = m_nXGridMax;

   // Calculate the average number of raindrops falling during a timestep of this duration. Note that this assumes that m_dRunOnLen is an extension orthogonal to the run-on edge, and is in mm
   double dRunOnAvgNDrops = m_dTimeStep * m_dRainIntensity * m_dRunOnLen * m_dCellSide * dEdgeLen / (3600 * m_dMeanCellWaterVol);

   // Now calculate the standard deviation of number of raindrops falling on the run-on area during a timestep of this duration
   double dRunOnStdNDrops = m_dTimeStep * m_dStdRainInt * m_dRunOnLen * m_dCellSide * dEdgeLen / (3600 * m_dMeanCellWaterVol);

   // Using ulGetRand0(), calculate the number of raindrops reaching the lower edge of the run-on area (i.e. the top edge of the soil area). The number of drops reaching the lower edge of the run-on area will depend on how much time has elapsed, but only while insufficient time has elapsed for water to have flowed from the top edge of the run-on area. Here, calculate number of drops (can be zero, need not be a whole number)
   double dDrops = dGetRand0GaussPos(tMin(dRunOnAvgNDrops, (dRunOnAvgNDrops * m_dRunOnSpd * static_cast<double>(m_dSimulatedTimeElapsed)) / m_dRunOnLen), dRunOnStdNDrops);

   if (dDrops > 0)
   {
      // Add to grand total of run-on raindrops
      m_ldGTotRunOnDrops += dDrops;

      // Set the depth for a 'representative' single drop, correct for spatial variation in rainfall (assume rainfall is the same all over run-on area), and assume we have dDrops of these (assume identical: an unrealistic assumption but convenient. Note that this has the effect of making rainfall onto run-on area more variable, because otherwise would get some cancelling of variability per iteration)
      double dTmpDpth = m_dMeanCellWaterVol * m_dInvCellSquare * m_dRunOnRainVarM * dDrops;
      m_dThisIterRunOn += dTmpDpth;

      // Now add the same fraction of this depth of water to every cell at this edge of the plot. Note that this implies that there is no infiltration on the run-on area
      dTmpDpth /= dEdgeLen;

      if (nEdge == EDGE_TOP)
      {
         // Top edge
         for (int nX = 0; nX < m_nXGridMax; nX++)
         {
            for (int nY = 0; nY < m_nYGridMax; nY++)
            {
               if (Cell[nX][nY].bIsEdgeCell())
               {
                  // Add the run-on to this edge cell of the Cell array
                  Cell[nX][nY].pGetRainAndRunon()->AddRunOn(dTmpDpth);
                  break;
               }
            }
         }
      }

      else if (nEdge == EDGE_RIGHT)
      {
         // Right edge
         for (int nY = 0; nY < m_nYGridMax; nY++)
         {
            for (int nX = m_nXGridMax-1; nX >= 0; nX--)
            {
               if (Cell[nX][nY].bIsEdgeCell())
               {
                  // Add the run-on to this edge cell of the Cell array
                  Cell[nX][nY].pGetRainAndRunon()->AddRunOn(dTmpDpth);
                  break;
               }
            }
         }
      }

      else if (nEdge == EDGE_BOTTOM)
      {
         // Bottom Edge
         for (int nX = 0; nX < m_nXGridMax; nX++)
         {
            for (int nY = m_nYGridMax-1; nY >= 0; nY--)
            {
               if (Cell[nX][nY].bIsEdgeCell())
               {
                  // Add the run-on to this edge cell of the Cell array
                  Cell[nX][nY].pGetRainAndRunon()->AddRunOn(dTmpDpth);
                  break;
               }
            }
         }
      }

      else if (nEdge == EDGE_LEFT)
      {
         // Left edge
         for (int nY = 0; nY < m_nYGridMax; nY++)
         {
            for (int nX = 0; nX < m_nXGridMax; nX++)
            {
               if (Cell[nX][nY].bIsEdgeCell())
               {
                  // Add the run-on to this edge cell of the Cell array
                  Cell[nX][nY].pGetRainAndRunon()->AddRunOn(dTmpDpth);
                  break;
               }
            }
         }
      }
   }
}


/*=========================================================================================================================================

 Simulates rain on the whole grid

=========================================================================================================================================*/
void CSimulation::DoAllRain(void)
{
   // First calculate the average number of raindrops falling during a timestep of this duration
   double dAvgNDrops = m_dTimeStep * m_dRainIntensity * m_ulNActiveCells * m_dCellSquare / (3600 * m_dMeanCellWaterVol);

   // Now calculate standard deviation of number of raindrops during a timestep of this duration
   double dStdNDrops = m_dTimeStep * m_dStdRainInt * m_ulNActiveCells * m_dCellSquare / (3600 * m_dMeanCellWaterVol);

   // Using ulGetRand0(), calculate the integer number of new raindrops which will fall during this timestep
   int nDrops = nRound(dGetRand0GaussPos(dAvgNDrops, dStdNDrops));
   m_ldGTotDrops += nDrops;

   // If not doing time-varying rain, do the rainfall intensity correction routine, for low intensities only (arbitrarily, less than 10 drops per timestep), corrects for too few drops or too many drops falling per timestep
   if (! m_bTimeVaryingRain && (nDrops < 10))
   {
      // Calculate number of drops that should have fallen so far
      double dTargetDrops = m_dSimulatedTimeElapsed * m_dTargetGTotDrops / m_dSimulatedRainDuration;

      if (m_ldGTotDrops < dTargetDrops)
      {
         // Too few, so add some extra drops
         int nExtraDrops = nRound(dTargetDrops - m_ldGTotDrops);
         if (nExtraDrops > 0)
         {
            nDrops += nExtraDrops;
            m_ldGTotDrops += nExtraDrops;
         }
      }
      else if (m_ldGTotDrops > dTargetDrops)
      {
         // Too many, so don't drop any this time
         m_ldGTotDrops -= nDrops;
         nDrops = 0;
      }
   }

   // Needed for  tiny grids
   nDrops = tMax(nDrops, 1);

   // Actually drop each raindrop
   for (int n = 1; n <= nDrops; n++)
   {
      // Using ulGetRand0(), first decide where raindrop will fall
      int nX, nY;
      do
      {
         nX = nGetRand0To(m_nXGridMax);
         nY = nGetRand0To(m_nYGridMax);
      }
      while (Cell[nX][nY].bIsMissingValue());

      // And then use ulGetRand0() to calculate its depth, correcting for spatial variation in rainfall
      double dTmpDpth = dGetRand0GaussPos(m_dMeanCellWaterVol, m_dStdCellWaterVol) * m_dInvCellSquare * Cell[nX][nY].pGetRainAndRunon()->dGetRainVarM();

      // Add to rainfall amount and water depth for this cell on the Cell array
      Cell[nX][nY].pGetRainAndRunon()->dAddRain(dTmpDpth);

      // Also add to the this-iteration total
      m_dThisIterRain += dTmpDpth;
   }
}


/*========================================================================================================================================

 Sets up rainfall intensity for this iteration

========================================================================================================================================*/
bool CSimulation::bSetUpRainfallIntensity(void)
{
   if (m_bTimeVaryingRain)
   {
      // We have time-varying rainfall intensity, so check if rain intensity has changed
      static int snChange = 0;
      if ((snChange <= m_nRainChangeTimeMax) && (m_dSimulatedTimeElapsed >= m_VdRainChangeTime[snChange]))
      {
         // Change rainfall intensity
         m_dRainIntensity = m_VdRainChangeIntensity[snChange];
         snChange++;
         return true;
      }
   }
   else
   {
      // We don't have time-varying rainfall intensity, so check if rain has stopped
      if ((m_dSimulatedTimeElapsed > m_dSimulatedRainDuration) && (m_dRainIntensity > 0))
      {
         m_dRainIntensity = 0;
         return true;
      }
   }

   return false;
}


