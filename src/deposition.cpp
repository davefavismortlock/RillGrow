/*=========================================================================================================================================

This is deposition.cpp: it calculates deposition of sediment from overland flow for RillGrow

Copyright (C) 2025 David Favis-Mortlock

==========================================================================================================================================

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
#include "rg.h"
#include "simulation.h"
#include "cell.h"

//=========================================================================================================================================
//! Calculates sediment load deposition on a single wet cell
//=========================================================================================================================================
void CSimulation::DoCellSedLoadDeposition(int const nX, int const nY, double const dWaterDepth, double const dSedimentLoad, double const dTransportCapacity)
{
   // Calculate the distance fallen during this timestep by a grain of each sediment size class
   double dClayDistance = m_dTimeStep * m_dClaySettlingSpeed;     // in mm
   double dSiltDistance = m_dTimeStep * m_dSiltSettlingSpeed;     // in mm
   double dSandDistance = m_dTimeStep * m_dSandSettlingSpeed;     // in mm

   // If the distance fallen is greater than or equal to the water depth, assume that all the sediment is deposited. Otherwise, assume a linear decrease in sediment deposited
   double dClayFrac, dSiltFrac, dSandFrac;

   if (dClayDistance >= dWaterDepth)
      dClayFrac = 1;
   else
      dClayFrac = dClayDistance / dWaterDepth;

   if (dSiltDistance >= dWaterDepth)
      dSiltFrac = 1;
   else
      dSiltFrac = dSiltDistance / dWaterDepth;

   if (dSandDistance >= dWaterDepth)
      dSandFrac = 1;
   else
      dSandFrac = dSandDistance / dWaterDepth;

   // Also assume that, for all sediment size classes, deposition is a linear function of the difference between sediment load and transport capacity: see equation 12 in Lei et al. (1998)
   double dTCFrac = (dSedimentLoad - dTransportCapacity) / (dSedimentLoad + dTransportCapacity);
   // assert(dTCFrac <= 1);
   // assert(dTCFrac >= 0);

   dClayFrac *= dTCFrac;
   dSiltFrac *= dTCFrac;
   dSandFrac *= dTCFrac;

   // Deposit these fractions of each sediment size class. Note that if there isn't enough being transported, then we will reduce the amount (= depth) which gets deposited. Note too that what is implied here is that deposited sediment becomes indistinguishable from the original soil. TODO To be considered in a later version
   m_Cell[nX][nY].pGetSoil()->DoSedLoadDeposit(dClayFrac, dSiltFrac, dSandFrac);
}

