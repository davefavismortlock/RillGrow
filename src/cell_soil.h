#ifndef __SOIL_H__
   #define __SOIL_H__
/*=========================================================================================================================================

This is cell_soil.h: declaration for the RillGrow class used to represent the soil column

Copyright (C) 2025 David Favis-Mortlock

==========================================================================================================================================

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
class CCell;                                       // Forward declaration
class CSimulation;

#include <vector>
using std::vector;

#include "cell_soil_layer.h"

class CCellSoil
{
public:
   //! Pointer to the simulation object
   static CSimulation* m_pSim;

private:
   //! This-iteration clay-sized surface water detachment as a thickness (mm)
   double m_dClayFlowDetach;

   //! This-iteration silt-sized surface water detachment as a thickness (mm)
   double m_dSiltFlowDetach;

   //! This-iteration sand-sized surface water detachment as a thickness (mm)
   double m_dSandFlowDetach;

   //! Cumulative clay-sized surface water detachment as a thickness (mm)
   double m_dCumulClayFlowDetach;

   //! Cumulative silt-sized surface water detachment as a thickness (mm)
   double m_dCumulSiltFlowDetach;

   //! Cumulative sand-sized surface water detachment as a thickness (mm)
   double m_dCumulSandFlowDetach;

   //! This-iteration clay-sized surface water deposition as a depth (mm)
   double m_dClayFlowDeposit;

   //! This-iteration silt-sized surface water deposition as a depth (mm)
   double m_dSiltFlowDeposit;

   //! This-iteration sand-sized surface water deposition as a depth (mm)
   double m_dSandFlowDeposit;

   //! Cumulative clay-sized surface water deposition as a thickness (mm)
   double m_dCumulClayFlowDeposit;

   //! Cumulative silt-sized surface water deposition as a thickness (mm)
   double m_dCumulSiltFlowDeposit;

   //! Cumulative sand-sized surface water deposition as a thickness (mm)
   double m_dCumulSandFlowDeposit;

   //! This-iteration clay-sized splash detachment as a thickness (mm)
   double m_dClaySplashDetach;

   //! This-iteration silt-sized splash detachment as a thickness (mm)
   double m_dSiltSplashDetach;

   //! This-iteration sand-sized splash detachment as a thickness (mm)
   double m_dSandSplashDetach;

   //! Cumulative clay-sized splash detachment as a thickness (mm)
   double m_dCumulClaySplashDetach;

   //! Cumulative silt-sized splash detachment as a thickness (mm)
   double m_dCumulSiltSplashDetach;

   //! Cumulative sand-sized splash detachment as a thickness (mm)
   double m_dCumulSandSplashDetach;

   //! Temporary field, used for Planchon splash deposition
   double m_dTempSplashDeposit;

   //! This-iteration clay-sized splash deposition as a thickness (mm)
   double m_dClaySplashDeposit;

   //! This-iteration silt-sized splash deposition as a thickness (mm)
   double m_dSiltSplashDeposit;

   //! This-iteration sand-sized splash deposition as a thickness (mm)
   double m_dSandSplashDeposit;

   //! Cumulative clay-sized splash deposition as a thickness (mm)
   double m_dCumulClaySplashDeposit;

   //! Cumulative silt-sized splash deposition as a thickness (mm)
   double m_dCumulSiltSplashDeposit;

   //! Cumulative sand-sized splash deposition as a thickness (mm)
   double m_dCumulSandSplashDeposit;

   //! Clay-sized sediment splashed off-edge this iteration (as a thickness in mm, only meaningful for edge cells)
   double m_dClaySplashOffEdge;

   //! Silt-sized sediment splashed off-edge this iteration (as a thickness in mm, only meaningful for edge cells)
   double m_dSiltSplashOffEdge;

   //! Sand-sized sediment splashed off-edge this iteration (as a thickness in mm, only meaningful for edge cells)
   double m_dSandSplashOffEdge;

   //! Cumulative clay-sized sediment splashed off-edge (as a thickness in mm, only meaningful for edge cells)
   double m_dCumulClaySplashOffEdge;

   //! Cumulative silt-sized sediment splashed off-edge (as a thickness in mm, only meaningful for edge cells)
   double m_dCumulSiltSplashOffEdge;

   //! Cumulative sand-sized sediment splashed off-edge (as a thickness in mm, only meaningful for edge cells)
   double m_dCumulSandSplashOffEdge;

   //! This-iteration clay-sized slump detachment as a thickness (mm)
   double m_dClaySlumpDetach;

   //! This-iteration silt-sized slump detachment as a thickness (mm)
   double m_dSiltSlumpDetach;

   //! This-iteration sand-sized slump detachment as a thickness (mm)
   double m_dSandSlumpDetach;

   //! Cumulative clay-sized slump detachment as a thickness (mm)
   double m_dCumulClaySlumpDetach;

   //! Cumulative silt-sized slump detachment as a thickness (mm)
   double m_dCumulSiltSlumpDetach;

   //! Cumulative sand-sized slump detachment as a thickness (mm)
   double m_dCumulSandSlumpDetach;

   //! This-iteration clay-sized slump deposition as a thickness (mm)
   double m_dClaySlumpDeposit;

   //! This-iteration silt-sized slump deposition as a thickness (mm)
   double m_dSiltSlumpDeposit;

   //! This-iteration sand-sized slump deposition as a thickness (mm)
   double m_dSandSlumpDeposit;

   //! Cumulative clay-sized slump deposition as a thickness (mm)
   double m_dCumulClaySlumpDeposit;

   //! Cumulative silt-sized slump deposition as a thickness (mm)
   double m_dCumulSiltSlumpDeposit;

   //! Cumulative sand-sized slump deposition as a thickness (mm)
   double m_dCumulSandSlumpDeposit;

   //! This-iteration clay-sized toppling detachment as a thickness (mm)
   double m_dClayToppleDetach;

   //! This-iteration silt-sized toppling detachment as a thickness (mm)
   double m_dSiltToppleDetach;

   //! This-iteration sand-sized toppling detachment as a thickness (mm)
   double m_dSandToppleDetach;

   //! Cumulative clay-sized toppling detachment as a thickness (mm)
   double m_dCumulClayToppleDetach;

   //! Cumulative silt-sized toppling detachment as a thickness (mm)
   double m_dCumulSiltToppleDetach;

   //! Cumulative sand-sized toppling detachment as a thickness (mm)
   double m_dCumulSandToppleDetach;

   //! This-iteration clay-sized toppling deposition as a thickness (mm)
   double m_dClayToppleDeposit;

   //! This-iteration silt-sized toppling deposition as a thickness (mm)
   double m_dSiltToppleDeposit;

   //! This-iteration sand-sized toppling deposition as a thickness (mm)
   double m_dSandToppleDeposit;

   //! Cumulative clay-sized toppling deposition as a thickness (mm)
   double m_dCumulClayToppleDeposit;

   //! Cumulative silt-sized toppling deposition as a thickness (mm)
   double m_dCumulSiltToppleDeposit;

   //! Cumulative sand-sized toppling deposition as a thickness (mm)
   double m_dCumulSandToppleDeposit;

   //! This-iteration clay-sized infilt deposition as a thickness (mm)
   double m_dClayInfiltDeposit;

   //! This-iteration silt-sized infilt deposition as a thickness (mm)
   double m_dSiltInfiltDeposit;

   //! This-iteration sand-sized infilt deposition as a thickness (mm)
   double m_dSandInfiltDeposit;

   //! Cumulative clay-sized infilt deposition as a thickness (mm)
   double m_dCumulClayInfiltDeposit;

   //! Cumulative silt-sized infilt deposition as a thickness (mm)
   double m_dCumulSiltInfiltDeposit;

   //! Cumulative sand-sized infilt deposition as a thickness (mm)
   double m_dCumulSandInfiltDeposit;

   //! Current shear stress in kg/m s**2 (Pa)
   double m_dShearStress;

   //! Cumulative shear stress in kg/m s**2 (Pa)
   double m_dCumulShearStress;

   //! For Planchon splash calcs
   double m_dLaplacian;

   //! This-iteration clay-sized detachment (mm) due to headcut retreat
   double m_dClayHeadcutRetreatDetach;

   //! This-iteration silt-sized detachment (mm) due to headcut retreat
   double m_dSiltHeadcutRetreatDetach;

   //! This-iteration sand-sized detachment (mm) due to headcut retreat
   double m_dSandHeadcutRetreatDetach;

   //! Cumulative clay-sized detachment (mm) due to headcut retreat
   double m_dCumulClayHeadcutRetreatDetach;

   //! Cumulative silt-sized detachment (mm) due to headcut retreat
   double m_dCumulSiltHeadcutRetreatDetach;

   //! Cumulative sand-sized detachment (mm) due to headcut retreat
   double m_dCumulSandHeadcutRetreatDetach;

   //! This-iteration clay-sized deposition (mm) due to headcut retreat
   double m_dClayHeadcutRetreatDeposit;

   //! This-iteration silt-sized deposition (mm) due to headcut retreat
   double m_dSiltHeadcutRetreatDeposit;

   //! This-iteration sand-sized deposition (mm) due to headcut retreat
   double m_dSandHeadcutRetreatDeposit;

   //! Cumulative clay-sized deposition (mm) due to headcut retreat
   double m_dCumulClayHeadcutRetreatDeposit;

   //! Cumulative silt-sized deposition (mm) due to headcut retreat
   double m_dCumulSiltHeadcutRetreatDeposit;

   //! Cumulative sand-sized deposition (mm) due to headcut retreat
   double m_dCumulSandHeadcutRetreatDeposit;

   //! Vector of soil layer objects
   vector<CCellSoilLayer> m_VLayer;

   //! Pointer to the parent cell object
   CCell* pCell;

public:
   CCellSoil(void);
   ~CCellSoil(void);

   void SetParent(CCell* const);

   CCellSoilLayer* pLayerGetLayer(int const);
   int nGetNumLayers(void) const;
   void SetSoilLayers(double const, int const, vector<string> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double>*, vector<double>*);

   void InitTmpLayerThicknesses(void);

   void FinishTmpLayerThicknesses(void);

   double dGetSoilSurfaceElevation(void) const;

   double dGetBulkDensityOfTopNonZeroLayer(void);

   void InitializeDetachAndDeposit(bool const);

   void ChangeTopLayerThickness(double const);

   void DoFlowDetach(double const, bool const);
   double dGetClayFlowDetach(void) const;
   double dGetSiltFlowDetach(void) const;
   double dGetSandFlowDetach(void) const;
   double dGetAllSizeFlowDetach(void) const;
   double dGetCumulClayFlowDetach(void) const;
   double dGetCumulSiltFlowDetach(void) const;
   double dGetCumulSandFlowDetach(void) const;
   double dGetCumulAllSizeFlowDetach(void) const;
   void DoSedLoadDeposit(double const, double const, double const);
   double dGetClayFlowDeposit(void) const;
   double dGetSiltFlowDeposit(void) const;
   double dGetSandFlowDeposit(void) const;
   // double dGetAllSizeFlowDeposit(void) const;
   double dGetCumulClayFlowDeposit(void) const;
   double dGetCumulSiltFlowDeposit(void) const;
   double dGetCumulSandFlowDeposit(void) const;
   double dGetCumulAllSizeFlowDeposit(void) const;

   void DoSplashDetach(double const, double&, double&, double&);
   void SetSplashDepositTemp(double const);
   double dGetSplashDepositTemp(void) const;
   void SetLaplacian(double const);
   double dGetLaplacian(void) const;
   void DoSplashToSedLoadOrDeposit(double const, double const, double const, bool&);
   double dGetClaySplashDetach(void) const;
   double dGetSiltSplashDetach(void) const;
   double dGetSandSplashDetach(void) const;
   double dGetAllSizeSplashDetach(void) const;
   double dGetClaySplashDeposit(void) const;
   double dGetSiltSplashDeposit(void) const;
   double dGetSandSplashDeposit(void) const;
   double dGetAllSizeSplashDeposit(void) const;
   double dGetCumulClaySplashDetach(void) const;
   double dGetCumulSiltSplashDetach(void) const;
   double dGetCumulSandSplashDetach(void) const;
   double dGetCumulAllSizeSplashDetach(void) const;
   double dGetCumulClaySplashDeposit(void) const;
   double dGetCumulSiltSplashDeposit(void) const;
   double dGetCumulSandSplashDeposit(void) const;
   double dGetCumulAllSizeSplashDeposit(void) const;
   void DoClaySplashOffEdge(double const);
   void DoSiltSplashOffEdge(double const);
   void DoSandSplashOffEdge(double const);
   double dGetClaySplashOffEdge(void) const;
   double dGetSiltSplashOffEdge(void) const;
   double dGetSandSplashOffEdge(void) const;
   double dGetCumulClaySplashOffEdge(void) const;
   double dGetCumulSiltSplashOffEdge(void) const;
   double dGetCumulSandSplashOffEdge(void) const;

   void IncShearStress(double const);
   double dGetShearStress(void) const;
   double dGetCumulShearStress(void) const;

   void DoSlumpDetach(double const, double&, double&, double&);
   void DoSlumpDepositOrToSedLoad(double const, double const, double const, double&, double&, double&, double&, double&, double&);
   void ZeroThisOperationSlump(void);
   double dGetClaySlumpDetach(void) const;
   double dGetSiltSlumpDetach(void) const;
   double dGetSandSlumpDetach(void) const;
   double dGetAllSizeSlumpDetach(void) const;
   double dGetCumulClaySlumpDetach(void) const;
   double dGetCumulSiltSlumpDetach(void) const;
   double dGetCumulSandSlumpDetach(void) const;
   double dGetCumulAllSizeSlumpDetach(void) const;
   double dGetClaySlumpDeposit(void) const;
   double dGetSiltSlumpDeposit(void) const;
   double dGetSandSlumpDeposit(void) const;
   double dGetAllSizeSlumpDeposit(void) const;
   double dGetCumulClaySlumpDeposit(void) const;
   double dGetCumulSiltSlumpDeposit(void) const;
   double dGetCumulSandSlumpDeposit(void) const;
   double dGetCumulAllSizeSlumpDeposit(void) const;

   void DoToppleDetach(double const, double&, double&, double&);
   void DoToppleDepositOrToSedLoad(double const, double const, double const, double&, double&, double&, double&, double&, double&);
   double dGetClayToppleDetach(void) const;
   double dGetSiltToppleDetach(void) const;
   double dGetSandToppleDetach(void) const;
   double dGetAllSizeToppleDetach(void) const;
   double dGetCumulClayToppleDetach(void) const;
   double dGetCumulSiltToppleDetach(void) const;
   double dGetCumulSandToppleDetach(void) const;
   double dGetCumulAllSizeToppleDetach(void) const;
   double dGetClayToppleDeposit(void) const;
   double dGetSiltToppleDeposit(void) const;
   double dGetSandToppleDeposit(void) const;
   double dGetAllSizeToppleDeposit(void) const;
   double dGetCumulClayToppleDeposit(void) const;
   double dGetCumulSiltToppleDeposit(void) const;
   double dGetCumulSandToppleDeposit(void) const;
   double dGetCumulAllSizeToppleDeposit(void) const;

   void DoInfiltrationDeposit(double const, double const, double const);
   void SetInfiltrationDepositionZero(void);
   double dGetClayInfiltDeposit(void) const;
   double dGetSiltInfiltDeposit(void) const;
   double dGetSandInfiltDeposit(void) const;
   double dGetAllSizeInfiltDeposit(void) const;
   double dGetCumulClayInfiltDeposit(void) const;
   double dGetCumulSiltInfiltDeposit(void) const;
   double dGetCumulSandInfiltDeposit(void) const;
   double dGetCumulAllSizeInfiltDeposit(void) const;

   void DoHeadcutRetreatDetach(double const, double&, double&, double&);
   void DoHeadcutRetreatDepositOrToSedLoad(double const, double const, double const, double&, double&, double&, double&, double&, double&);
   double dGetClayHeadcutRetreatDetach(void) const;
   double dGetSiltHeadcutRetreatDetach(void) const;
   double dGetSandHeadcutRetreatDetach(void) const;
   double dGetAllSizeHeadcutRetreatDetach(void) const;
   double dGetCumulClayHeadcutRetreatDetach(void) const;
   double dGetCumulSiltHeadcutRetreatDetach(void) const;
   double dGetCumulSandHeadcutRetreatDetach(void) const;
   double dGetClayHeadcutRetreatDeposit(void) const;
   double dGetSiltHeadcutRetreatDeposit(void) const;
   double dGetSandHeadcutRetreatDeposit(void) const;
   double dGetAllSizeHeadcutRetreatDeposit(void) const;
   double dGetCumulClayHeadcutRetreatDeposit(void) const;
   double dGetCumulSiltHeadcutRetreatDeposit(void) const;
   double dGetCumulSandHeadcutRetreatDeposit(void) const;
   double dGetCumulAllSizeHeadcutRetreatDeposit(void) const;

   double dGetCumulAllSizeLowering(void) const;
};
#endif         // __SOIL_H__
