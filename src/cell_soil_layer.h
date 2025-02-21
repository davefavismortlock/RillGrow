#ifndef __SOIL_LAYER_H__
   #define __SOIL_LAYER_H__
/*=========================================================================================================================================

This is cell_soil_layer.h: declaration for the RillGrow class used to represent a layer in the soil column

Copyright (C) 2025 David Favis-Mortlock

==========================================================================================================================================

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
#include <string>
using std::string;

class CCellSoilLayer
{
private:
   // The name of this soil layer
   string m_strName;
   
   //! Clay in this soil layer, as a thickness (mm)
   double m_dClayThickness;
   
   //! Silt in this soil layer, as a thickness (mm)
   double m_dSiltThickness;
   
   //! Sand in this soil layer, as a thickness (mm)
   double m_dSandThickness;

   //! Temporary thickness of clay in this soil layer (mm)
   double m_dTmpClayThickness;

   //! Temporary thickness of silt in this soil layer (mm)
   double m_dTmpSiltThickness;

   //! Temporary thickness of sand in this soil layer (mm)
   double m_dTmpSandThickness;
   
   //! The bulk density of this soil layer
   double m_dBulkDensity;
   
   //! The flow erodibility of clay in this soil layer, a normalized (0-1) value
   double m_dClayFlowErodibility;
   
      //! The flow erodibility of silt in this soil layer, a normalized (0-1) value
   double m_dSiltFlowErodibility;
   
   //! The flow erodibility of sand in this soil layer, a normalized (0-1) value
   double m_dSandFlowErodibility;
   
   //! The splash erodibility of clay in this soil layer, a normalized (0-1) value
   double m_dClaySplashErodibility;
   
   //! The splash erodibility of silt in this soil layer, a normalized (0-1) value
   double m_dSiltSplashErodibility;
   
   //! The splash erodibility of sand in this soil layer, a normalized (0-1) value
   double m_dSandSplashErodibility;
   
   //! The slumping erodibility of clay in this soil layer, a normalized (0-1) value
   double m_dClaySlumpErodibility;
   
   //! The slumping erodibility of silt in this soil layer, a normalized (0-1) value
   double m_dSiltSlumpErodibility;
   
   //! The slumping erodibility of sand in this soil layer, a normalized (0-1) value
   double m_dSandSlumpErodibility;

   //! The layer's soil water content
   double m_dSoilWater;

public:
   CCellSoilLayer(void);
   ~CCellSoilLayer(void);

   void SetName(string const*);
   string* pstrGetName(void);

   void SetClayThickness(double const);
   void SetSiltThickness(double const);
   void SetSandThickness(double const);
   double dGetClayThickness(void) const;
   double dGetSiltThickness(void) const;
   double dGetSandThickness(void) const;

   double dGetLayerThickness(void) const;
//    bool bIsZeroTotalThickness(void) const;

   void SetTmpClayThickness(double const);
   void SetTmpSiltThickness(double const);
   void SetTmpSandThickness(double const);
   double dGetTmpClayThickness(void) const;
   double dGetTmpSiltThickness(void) const;
   double dGetTmpSandThickness(void) const;

   void SetBulkDensity(double const);
   double dGetBulkDensity(void) const;

   void SetClayFlowErodibility(double const);
   void SetSiltFlowErodibility(double const);
   void SetSandFlowErodibility(double const);
   // double dGetClayFlowErodibility(void) const;
   // double dGetSiltFlowErodibility(void) const;
   // double dGetSandFlowErodibility(void) const;

   void SetClaySplashErodibility(double const);
   void SetSiltSplashErodibility(double const);
   void SetSandSplashErodibility(double const);
   // double dGetClaySplashErodibility(void) const;
   // double dGetSiltSplashErodibility(void) const;
   // double dGetSandSplashErodibility(void) const;

   void SetClaySlumpErodibility(double const);
   void SetSiltSlumpErodibility(double const);
   void SetSandSlumpErodibility(double const);
   // double dGetClaySlumpErodibility(void) const;
   // double dGetSiltSlumpErodibility(void) const;
   // double dGetSandSlumpErodibility(void) const;

   void DoLayerFlowErosion(double const, double&, double&, double&);
   void DoLayerSplashErosion(double const, double&, double&, double&);
   void DoLayerSlumpErosion(double const, double&, double&, double&);
   void DoLayerToppleErosion(double const, double&, double&, double&);

   void DoTmpLayerDeposition(double const, double const, double const);
   void DoLayerDeposition(double const, double const, double const);

   void SetSoilWater(double const);
   void ChangeSoilWater(double const);
   double dGetSoilWater(void) const;

   // void ChangeThickness(double const);

   void DoLayerHeadcutRetreatErosion(double const, double&, double&, double&);
};

#endif         // __SOIL_LAYER_H__
