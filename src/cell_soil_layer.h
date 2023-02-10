#ifndef __SOIL_LAYER_H__
   #define __SOIL_LAYER_H__
/*=========================================================================================================================================

 This is m_Cell_soil_layer.h: declaration for the RillGrow class used to represent a layer in the soil column

 Copyright (C) 2023 David Favis-Mortlock

 ==========================================================================================================================================

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation,
 Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
#include <string>
using std::string;


class CCellSoilLayer
{
private:
   string m_strName;
   double
      m_dClayThickness,
      m_dSiltThickness,
      m_dSandThickness,
      m_dBulkDensity,
      m_dClayFlowErodibility,                   // All these are normalized (0-1) values
      m_dSiltFlowErodibility,
      m_dSandFlowErodibility,
      m_dClaySplashErodibility,
      m_dSiltSplashErodibility,
      m_dSandSplashErodibility,
      m_dClaySlumpErodibility,
      m_dSiltSlumpErodibility,
      m_dSandSlumpErodibility,
      m_dSoilWater;

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

   void SetBulkDensity(double const);
   double dGetBulkDensity(void) const;

   void SetClayFlowErodibility(double const);
   void SetSiltFlowErodibility(double const);
   void SetSandFlowErodibility(double const);
   double dGetClayFlowErodibility(void) const;
   double dGetSiltFlowErodibility(void) const;
   double dGetSandFlowErodibility(void) const;

   void SetClaySplashErodibility(double const);
   void SetSiltSplashErodibility(double const);
   void SetSandSplashErodibility(double const);
   double dGetClaySplashErodibility(void) const;
   double dGetSiltSplashErodibility(void) const;
   double dGetSandSplashErodibility(void) const;

   void SetClaySlumpErodibility(double const);
   void SetSiltSlumpErodibility(double const);
   void SetSandSlumpErodibility(double const);
   double dGetClaySlumpErodibility(void) const;
   double dGetSiltSlumpErodibility(void) const;
   double dGetSandSlumpErodibility(void) const;

   void DoLayerFlowErosion(double const, double&, double&, double&);
   void DoLayerSplashErosion(double const, double&, double&, double&);
   void DoLayerSlumpErosion(double const, double&, double&, double&);
   void DoLayerToppleErosion(double const, double&, double&, double&);

   void DoLayerDeposition(double const, double const, double const);

   void SetSoilWater(double const);
   void ChangeSoilWater(double const);
   double dGetSoilWater(void) const;

   void ChangeThickness(double const);

   void DoLayerHeadcutRetreatErosion(double const, double&, double&, double&);
};

#endif         // __SOIL_LAYER_H__
