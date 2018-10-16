#ifndef __SOIL_H__
   #define __SOIL_H__
/*=========================================================================================================================================

This is cell_soil.h: declaration for the RillGrow class used to represent the soil column

 Copyright (C) 2018 David Favis-Mortlock

 ==========================================================================================================================================

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation,
 Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
class CCell;                                       // Forward declaration
class CSimulation;

#include <vector>
using std::vector;

#include "cell_soil_layer.h"


class CSoil
{
public:
   static CSimulation* m_pSim; 

private:
   double
      m_dClayFlowDetach,                           // This-timestep clay-sized overland flow detachment as a thickness (mm)
      m_dSiltFlowDetach,                           // This-timestep silt-sized overland flow detachment as a thickness (mm)
      m_dSandFlowDetach,                           // This-timestep sand-sized overland flow detachment as a thickness (mm)
      m_dTotClayFlowDetach,                        // Cumulative clay-sized overland flow detachment as a thickness (mm)
      m_dTotSiltFlowDetach,                        // Cumulative silt-sized overland flow detachment as a thickness (mm)
      m_dTotSandFlowDetach,                        // Cumulative sand-sized overland flow detachment as a thickness (mm)
      m_dClayFlowDeposit,                          // This-timestep clay-sized overland flow deposition as a depth (mm)
      m_dSiltFlowDeposit,                          // This-timestep silt-sized overland flow deposition as a depth (mm)
      m_dSandFlowDeposit,                          // This-timestep sand-sized overland flow deposition as a depth (mm)
      m_dTotClayFlowDeposit,                       // Cumulative clay-sized overland flow detachment as a thickness (mm)
      m_dTotSiltFlowDeposit,                       // Cumulative silt-sized overland flow detachment as a thickness (mm)
      m_dTotSandFlowDeposit,                       // Cumulative sand-sized overland flow detachment as a thickness (mm)
      m_dClaySplashDetach,                         // This-timestep clay-sized splash detachment as a thickness (mm)
      m_dSiltSplashDetach,                         // This-timestep silt-sized splash detachment as a thickness (mm)
      m_dSandSplashDetach,                         // This-timestep sand-sized splash detachment as a thickness (mm)
      m_dTotClaySplashDetach,                      // Cumulative clay-sized splash detachment as a thickness (mm)
      m_dTotSiltSplashDetach,                      // Cumulative silt-sized splash detachment as a thickness (mm)
      m_dTotSandSplashDetach,                      // Cumulative sand-sized splash detachment as a thickness (mm)   
      m_dTotSplashDepositToDo,                     // Temporary field, used for splash deposition
      m_dClaySplashDeposit,                        // This-timestep clay-sized splash deposition as a thickness (mm)
      m_dSiltSplashDeposit,                        // This-timestep silt-sized splash deposition as a thickness (mm)
      m_dSandSplashDeposit,                        // This-timestep sand-sized splash deposition as a thickness (mm)
      m_dTotClaySplashDeposit,                     // Cumulative clay-sized splash deposition as a thickness (mm)
      m_dTotSiltSplashDeposit,                     // Cumulative silt-sized splash deposition as a thickness (mm)
      m_dTotSandSplashDeposit,                     // Cumulative sand-sized splash deposition as a thickness (mm)
      m_dClaySlumpDetach,                          // This-timestep clay-sized slump detachment as a thickness (mm)
      m_dSiltSlumpDetach,                          // This-timestep silt-sized slump detachment as a thickness (mm)
      m_dSandSlumpDetach,                          // This-timestep sand-sized slump detachment as a thickness (mm)      
      m_dTotClaySlumpDetach,                       // Cumulative clay-sized slump detachment as a thickness (mm)
      m_dTotSiltSlumpDetach,                       // Cumulative silt-sized slump detachment as a thickness (mm)
      m_dTotSandSlumpDetach,                       // Cumulative sand-sized slump detachment as a thickness (mm)
      m_dClaySlumpDeposit,                         // This-timestep clay-sized slump deposition as a thickness (mm)
      m_dSiltSlumpDeposit,                         // This-timestep silt-sized slump deposition as a thickness (mm)
      m_dSandSlumpDeposit,                         // This-timestep sand-sized slump deposition as a thickness (mm)      
      m_dTotClaySlumpDeposit,                      // Cumulative clay-sized slump deposition as a thickness (mm)
      m_dTotSiltSlumpDeposit,                      // Cumulative silt-sized slump deposition as a thickness (mm)
      m_dTotSandSlumpDeposit,                      // Cumulative sand-sized slump deposition as a thickness (mm)
      m_dClayToppleDetach,                         // This-timestep clay-sized toppling detachment as a thickness (mm)
      m_dSiltToppleDetach,                         // This-timestep silt-sized toppling detachment as a thickness (mm)
      m_dSandToppleDetach,                         // This-timestep sand-sized toppling detachment as a thickness (mm)
      m_dTotClayToppleDetach,                      // Cumulative clay-sized toppling detachment as a thickness (mm)
      m_dTotSiltToppleDetach,                      // Cumulative silt-sized toppling detachment as a thickness (mm)
      m_dTotSandToppleDetach,                      // Cumulative sand-sized toppling detachment as a thickness (mm)      
      m_dClayToppleDeposit,                        // This-timestep clay-sized toppling deposition as a thickness (mm)
      m_dSiltToppleDeposit,                        // This-timestep silt-sized toppling deposition as a thickness (mm)
      m_dSandToppleDeposit,                        // This-timestep sand-sized toppling deposition as a thickness (mm)
      m_dTotClayToppleDeposit,                     // Cumulative clay-sized toppling deposition as a thickness (mm)
      m_dTotSiltToppleDeposit,                     // Cumulative silt-sized toppling deposition as a thickness (mm)
      m_dTotSandToppleDeposit,                     // Cumulative sand-sized toppling deposition as a thickness (mm)
      m_dClayInfiltDeposit,                        // This-timestep clay-sized infiltration deposition as a thickness (mm)
      m_dSiltInfiltDeposit,                        // This-timestep silt-sized infiltration deposition as a thickness (mm)
      m_dSandInfiltDeposit,                        // This-timestep sand-sized infiltration deposition as a thickness (mm)
      m_dTotClayInfiltDeposit,                     // Cumulative clay-sized infiltration deposition as a thickness (mm)
      m_dTotSiltInfiltDeposit,                     // Cumulative silt-sized infiltration deposition as a thickness (mm)
      m_dTotSandInfiltDeposit,                     // This-timestep sand-sized infiltration deposition as a thickness (mm)
      m_dShearStress,                              // Current shear stress in kg/m s**2 (Pa)
      m_dTotShearStress,                           // Total ditto, since start of simulation
      m_dLaplacian;                                // For splash calcs      
      
   vector<CLayer> m_VLayer;   

   CCell* m_pCell;   
   
public:
   CSoil(void);
   ~CSoil(void);
   
   void SetParent(CCell* const);
   
   CLayer* pLayerGetLayer(int const);   
   void SetSoilLayers(double const, int const, vector<string> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double> const*, vector<double>*, vector<double>*);
   double dGetSoilSurfaceElevation(void);
   
   double dGetBulkDensityOfTopNonZeroLayer(void);
   
   void InitializeDetachmentAndDeposition(bool const);
   
   void ChangeTopLayerThickness(double const);

   void DoFlowDetach(double const);   
   void DoFlowDeposit(double const, double const, double const);
   
   double dGetClayFlowDetach(void) const;   
   double dGetSiltFlowDetach(void) const;   
   double dGetSandFlowDetach(void) const;   
   double dGetTotFlowDetach(void) const;   
   double dGetCumulativeClayFlowDetach(void) const;
   double dGetCumulativeSiltFlowDetach(void) const;
   double dGetCumulativeSandFlowDetach(void) const;
   double dGetCumulativeTotFlowDetach(void) const;
   
   double dGetClayFlowDeposition(void) const;
   double dGetSiltFlowDeposition(void) const;
   double dGetSandFlowDeposition(void) const;
   double dGetTotFlowDeposition(void) const;
   double dGetCumulativeClayFlowDeposition(void) const;
   double dGetCumulativeSiltFlowDeposition(void) const;
   double dGetCumulativeSandFlowDeposition(void) const;
   double dGetCumulativeTotFlowDeposition(void) const;

   void DoSplashDetach(double const);   
   void SetSplashDepositionTemporary(double const);
   double dGetSplashDepositionTemporary(void) const;
   void DoClaySplashDeposit(double const);
   void DoSiltSplashDeposit(double const);
   void DoSandSplashDeposit(double const);
   
   double dGetClaySplashDetach(void) const;
   double dGetSiltSplashDetach(void) const;
   double dGetSandSplashDetach(void) const;
   double dGetTotSplashDetach(void) const;
   double dGetClaySplashDeposit(void) const;
   double dGetSiltSplashDeposit(void) const;
   double dGetSandSplashDeposit(void) const;
   double dGetTotSplashDeposit(void) const;
   double dGetCumulativeClaySplashDetach(void) const;
   double dGetCumulativeSiltSplashDetach(void) const;
   double dGetCumulativeSandSplashDetach(void) const;
   double dGetCumulativeTotSplashDetach(void) const;
   double dGetCumulativeClaySplashDeposit(void) const;
   double dGetCumulativeSiltSplashDeposit(void) const;
   double dGetCumulativeSandSplashDeposit(void) const;
   double dGetCumulativeTotSplashDeposit(void) const;

   void DoSlumpDetach(double const, double&, double&, double&);
   void AddSlumpDeposition(double const, double const, double const);
   void DoToppleDetach(double const, double&, double&, double&);
   void DoClayToppleDeposit(double const);
   void DoSiltToppleDeposit(double const);
   void DoSandToppleDeposit(double const);
   void ZeroThisIterSlump(void);
   
   double dGetClaySlumpDetach(void) const;
   double dGetSiltSlumpDetach(void) const;
   double dGetSandSlumpDetach(void) const;
   double dGetTotSlumpDetach(void) const;
   double dGetCumulativeClaySlumpDetach(void) const;
   double dGetCumulativeSiltSlumpDetach(void) const;
   double dGetCumulativeSandSlumpDetach(void) const;
   double dGetCumulativeTotSlumpDetach(void) const;   
   double dGetClayToppleDetach(void) const;
   double dGetSiltToppleDetach(void) const;
   double dGetSandToppleDetach(void) const;
   double dGetTotToppleDetach(void) const;   
   double dGetCumulativeClayToppleDetach(void) const;
   double dGetCumulativeSiltToppleDetach(void) const;
   double dGetCumulativeSandToppleDetach(void) const;
   double dGetCumulativeTotToppleDetach(void) const; 
   double dGetClayToppleDeposit(void) const;
   double dGetSiltToppleDeposit(void) const;
   double dGetSandToppleDeposit(void) const;
   double dGetTotToppleDeposit(void) const;   
   double dGetCumulativeClayToppleDeposit(void) const;
   double dGetCumulativeSiltToppleDeposit(void) const;
   double dGetCumulativeSandToppleDeposit(void) const;
   double dGetCumulativeTotToppleDeposit(void) const;
   
   void DoInfiltrationDeposit(double const, double const, double const);
   void SetInfiltrationDepositionZero(void);
   
   double dGetClayInfiltDeposit(void) const;
   double dGetSiltInfiltDeposit(void) const;
   double dGetSandInfiltDeposit(void) const;
   double dGetTotInfiltDeposit(void) const;
   double dGetCumulativeClayInfiltDeposit(void) const;
   double dGetCumulativeSiltInfiltDeposit(void) const;
   double dGetCumulativeSandInfiltDeposit(void) const;
   double dGetCumulativeTotInfiltDeposit(void) const;
   
   void IncShearStress(double const);
   double dGetShearStress(void) const;
   
   void IncTotShearStress(double const);
   double dGetTotShearStress(void) const;
   
   double dGetTotNetSoilLoss(void) const;
   
   void SetLaplacian(double const);
   double dGetLaplacian(void) const;
};

#endif         // __SOIL_H__
