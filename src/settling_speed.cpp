/*=========================================================================================================================================

This is settling_speed.cpp: it calculates settling speed for three sediment size classes for RillGrow

Copyright (C) 2025 David Favis-Mortlock

==========================================================================================================================================

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
#include "rg.h"
#include "simulation.h"

//=========================================================================================================================================
//! Calculates settling speed for the three sediment size classes, using a user-selected settling speed equation
//=========================================================================================================================================
void CSimulation::CalcSettlingSpeed(void)
{
   // Calculate the settling speed of clay, silt, and sand-sized sediment
   double dR = (m_dDepositionGrainDensity - m_dRho) / m_dRho;                      // Relative submerged density, units do not matter

   if (m_bSettlingEqnCheng)
   {
      // Calculate settling speed using equation (9) from Cheng N-S. (1997). Simplified Settling Velocity Formula for Sediment Particle. Journal of Hydraulic Engineering 123 : 149–152. DOI: 10/fkvvcg
      //
      // w = (nu / d) * (sqrt(25 + 1.2 * (dStar * dStar)) - 5)^1.5
      //
      // where:
      //    w = settling velocity of sediment particle (cm/s)
      //    nu = kinetic viscosity of water (Stoke = cm**2/s)
      //    d = diameter of sediment particle (cm)
      //    delta = (density of sediment - density of water) / density of water (units do not matter, but all kg/m**3 here)
      //    g = acceleration due to gravity (cm/s**2)
      //    dStar = dimensionless particle diameter = (delta * g / nu * nu)

      double dGC = m_dG * 100;                                             // multiply by 100 to convert from m/s**2 to cm/s**2
      double dNuS = m_dNu * 10000;                                         // multiply by 10**4 to convert m**2/s to cm**2/s (= 1 Stoke)

      double dClayDiamC = m_dClayDiameter * 0.1;                           // divide by 10 to convert from mm to cm
      double dSiltDiamC = m_dSiltDiameter * 0.1;                           // divide by 10 to convert from mm to cm
      double dSandDiamC = m_dSandDiameter * 0.1;                           // divide by 10 to convert from mm to cm

      double dStarPart = pow((dR * dGC) / (dNuS * dNuS), 1.0 / 3.0);
      double dDStarClay = dStarPart * dClayDiamC;
      double dDStarSilt = dStarPart * dSiltDiamC;
      double dDStarSand = dStarPart * dSandDiamC;

      m_dClaySettlingSpeed = (dNuS / dClayDiamC) * pow(sqrt(25.0 + (1.2 * (dDStarClay * dDStarClay))) - 5.0, 1.5);     // in cm/sec
      m_dSiltSettlingSpeed = (dNuS / dSiltDiamC) * pow(sqrt(25.0 + (1.2 * (dDStarSilt * dDStarSilt))) - 5.0, 1.5);     // in cm/sec
      m_dSandSettlingSpeed = (dNuS / dSandDiamC) * pow(sqrt(25.0 + (1.2 * (dDStarSand * dDStarSand))) - 5.0, 1.5);     // in cm/sec

      m_dClaySettlingSpeed *= 10;                                          // multiply by 10 to convert from cm/sec to mm/sec
      m_dSiltSettlingSpeed *= 10;                                          // multiply by 10 to convert from cm/sec to mm/sec
      m_dSandSettlingSpeed *= 10;                                          // multiply by 10 to convert from cm/sec to mm/sec
   }

   if (m_bSettlingEqnStokesBudryckRittinger)
   {
      // From https://eng.libretexts.org/Bookshelves/Civil_Engineering/Book%3A_Slurry_Transport_(Miedema)/04%3A_The_Terminal_Settling_Velocity_of_Particles/4.04%3A_Terminal_Settling_Velocity_Equations

      // Stokes, Budryck and Rittinger used these drag coefficients to calculate settling velocities for laminar settling (Stokes), a transition zone (Budryck) and turbulent settling (Rittinger) of real sand grains.

      // For laminar flow, d < 0.1 mm, according to Stokes:
      //
      // w = 424 * R * d**2
      //
      // where:
      //
      //    R = submerged specific gravity = (density of sediment - density of water) / density of water (units do not matter, but all kg/m**3 here)
      //    d = diameter of sediment particle (mm)

      // For the transition zone, d > 0.1 mm and d < 1 mm, according to Budryck:
      //
      // w = 8.925 * ( ((1 + 95 * R * d**3)**0.5 - 1) / d)
      //
      // where:
      //
      //    R = submerged specific gravity = (density of sediment - density of water) / density of water (units do not matter, but all kg/m**3 here)
      //    d = diameter of sediment particle (mm)

      // For turbulent flow, d > 1 mm, according to Rittinger:
      //
      // w = 87 * (R * d)**0.5
      //
      // where:
      //
      //    R = submerged specific gravity = (density of sediment - density of water) / density of water (units do not matter, but all kg/m**3 here)
      //    d = diameter of sediment particle (mm)
      for (int n = 0; n < 3; n++)
      {
         double dD = 0;
         if (n == 0)
            dD = m_dClayDiameter;      // in mm
         else if (n == 1)
            dD = m_dSiltDiameter;      // in mm
         else if (n == 2)
            dD = m_dSandDiameter;      // in mm

         double dSettlingSpeed = 0;
         if (dD < 0.1)
            dSettlingSpeed = 424 * dR * dD * dD;
         else if ((dD >= 0.1) && (dD < 1.0))
            dSettlingSpeed = 8.925 * ( (pow(1 + (95 * dR * dD * dD * dD), 0.5) - 1) / dD);
         else if (dD >= 1.0)
            dSettlingSpeed = 87 * pow(dR * dD, 0.5);

         if (n == 0)
            m_dClaySettlingSpeed = dSettlingSpeed;    // in mm/sec
         else if (n == 1)
            m_dSiltSettlingSpeed = dSettlingSpeed;    // in mm/sec
         else if (n == 2)
            m_dSandSettlingSpeed = dSettlingSpeed;    // in mm/sec
      }
   }

   if (m_bSettlingEqnFergusonChurch)
   {
      // Calculate settling speed using equation (4) from Ferguson, R.I. and Church, M.A. (2004). A Simple Universal Equation for Grain Settling Velocity. Journal of Sedimentary Research 74(6), 933-937. DOI: 10.1306/051204740933
      //
      // w = (R * g * d**2) / ((C1 * nu) + (0.75 * C2 * R * g * d**3)**0.5)
      //
      // where:
      //    w = settling velocity of sediment particle (m/s)
      //    R = submerged specific gravity = (density of sediment - density of water) / density of water (units do not matter, but all kg/m**3 here)
      //    g = acceleration due to gravity (m/s**2)
      //    d = diameter of sediment particle (m)
      //    nu = kinematic viscosity (m**2/s)
      //    C1 is a constant = 18 for smooth spheres
      //    C2 is a constant = 0.4 for smooth spheres, 1 for natural sand grains
      double const dC1 = 18;
      double const dC2 = 1;

      double dClayDiaM = m_dClayDiameter * 0.001;                    // divide by 1000 to convert from mm to m
      m_dClaySettlingSpeed = (dR * m_dG * dClayDiaM * dClayDiaM) / ((dC1 * m_dNu) + pow((0.75 * dC2 * dR * m_dG * dClayDiaM * dClayDiaM * dClayDiaM), 0.5));

      double dSiltDiaM = m_dSiltDiameter * 0.001;                    // divide by 1000 to convert from mm to m
      m_dSiltSettlingSpeed = (dR * m_dG * dSiltDiaM * dSiltDiaM) / ((dC1 * m_dNu) + pow((0.75 * dC2 * dR * m_dG * dSiltDiaM * dSiltDiaM * dSiltDiaM), 0.5));

      double dSandDiaM = m_dSandDiameter * 0.001;                    // divide by 1000 to convert from mm to m
      m_dSandSettlingSpeed = (dR * m_dG * dSandDiaM * dSandDiaM) / ((dC1 * m_dNu) + pow((0.75 * dC2 * dR * m_dG * dSandDiaM * dSandDiaM * dSandDiaM), 0.5));

      // Convert from m/sec to mm/sec
      m_dClaySettlingSpeed *= 1000;
      m_dSiltSettlingSpeed *= 1000;
      m_dSandSettlingSpeed *= 1000;
   }
}
