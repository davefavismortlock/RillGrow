#ifndef __SOIL_H__
   #define __SOIL_H__
/*=========================================================================================================================================

This is cell_soil.h: declaration for the RillGrow class used to represent the soil column

 Copyright (C) 2020 David Favis-Mortlock

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
      m_dCumulClayFlowDetach,                      // Cumulative clay-sized overland flow detachment as a thickness (mm)
      m_dCumulSiltFlowDetach,                      // Cumulative silt-sized overland flow detachment as a thickness (mm)
      m_dCumulSandFlowDetach,                      // Cumulative sand-sized overland flow detachment as a thickness (mm)
      m_dClayFlowDeposit,                          // This-timestep clay-sized overland flow deposition as a depth (mm)
      m_dSiltFlowDeposit,                          // This-timestep silt-sized overland flow deposition as a depth (mm)
      m_dSandFlowDeposit,                          // This-timestep sand-sized overland flow deposition as a depth (mm)
      m_dCumulClayFlowDeposit,                     // Cumulative clay-sized overland flow detachment as a thickness (mm)
      m_dCumulSiltFlowDeposit,                     // Cumulative silt-sized overland flow detachment as a thickness (mm)
      m_dCumulSandFlowDeposit,                     // Cumulative sand-sized overland flow detachment as a thickness (mm)
      m_dClaySplashDetach,                         // This-timestep clay-sized splash detachment as a thickness (mm)
      m_dSiltSplashDetach,                         // This-timestep silt-sized splash detachment as a thickness (mm)
      m_dSandSplashDetach,                         // This-timestep sand-sized splash detachment as a thickness (mm)
      m_dCumulClaySplashDetach,                    // Cumulative clay-sized splash detachment as a thickness (mm)
      m_dCumulSiltSplashDetach,                    // Cumulative silt-sized splash detachment as a thickness (mm)
      m_dCumulSandSplashDetach,                    // Cumulative sand-sized splash detachment as a thickness (mm)
      m_dTotSplashDepositToDo,                     // Temporary field, used for splash deposition
      m_dClaySplashDeposit,                        // This-timestep clay-sized splash deposition as a thickness (mm)
      m_dSiltSplashDeposit,                        // This-timestep silt-sized splash deposition as a thickness (mm)
      m_dSandSplashDeposit,                        // This-timestep sand-sized splash deposition as a thickness (mm)
      m_dCumulClaySplashDeposit,                   // Cumulative clay-sized splash deposition as a thickness (mm)
      m_dCumulSiltSplashDeposit,                   // Cumulative silt-sized splash deposition as a thickness (mm)
      m_dCumulSandSplashDeposit,                   // Cumulative sand-sized splash deposition as a thickness (mm)
      m_dClaySlumpDetach,                          // This-timestep clay-sized slump detachment as a thickness (mm)
      m_dSiltSlumpDetach,                          // This-timestep silt-sized slump detachment as a thickness (mm)
      m_dSandSlumpDetach,                          // This-timestep sand-sized slump detachment as a thickness (mm)
      m_dCumulClaySlumpDetach,                     // Cumulative clay-sized slump detachment as a thickness (mm)
      m_dCumulSiltSlumpDetach,                     // Cumulative silt-sized slump detachment as a thickness (mm)
      m_dCumulSandSlumpDetach,                     // Cumulative sand-sized slump detachment as a thickness (mm)
      m_dClaySlumpDeposit,                         // This-timestep clay-sized slump deposition as a thickness (mm)
      m_dSiltSlumpDeposit,                         // This-timestep silt-sized slump deposition as a thickness (mm)
      m_dSandSlumpDeposit,                         // This-timestep sand-sized slump deposition as a thickness (mm)
      m_dCumulClaySlumpDeposit,                    // Cumulative clay-sized slump deposition as a thickness (mm)
      m_dCumulSiltSlumpDeposit,                    // Cumulative silt-sized slump deposition as a thickness (mm)
      m_dCumulSandSlumpDeposit,                    // Cumulative sand-sized slump deposition as a thickness (mm)
      m_dClayToppleDetach,                         // This-timestep clay-sized toppling detachment as a thickness (mm)
      m_dSiltToppleDetach,                         // This-timestep silt-sized toppling detachment as a thickness (mm)
      m_dSandToppleDetach,                         // This-timestep sand-sized toppling detachment as a thickness (mm)
      m_dCumulClayToppleDetach,                    // Cumulative clay-sized toppling detachment as a thickness (mm)
      m_dCumulSiltToppleDetach,                    // Cumulative silt-sized toppling detachment as a thickness (mm)
      m_dCumulSandToppleDetach,                    // Cumulative sand-sized toppling detachment as a thickness (mm)
      m_dClayToppleDeposit,                        // This-timestep clay-sized toppling deposition as a thickness (mm)
      m_dSiltToppleDeposit,                        // This-timestep silt-sized toppling deposition as a thickness (mm)
      m_dSandToppleDeposit,                        // This-timestep sand-sized toppling deposition as a thickness (mm)
      m_dCumulClayToppleDeposit,                   // Cumulative clay-sized toppling deposition as a thickness (mm)
      m_dCumulSiltToppleDeposit,                   // Cumulative silt-sized toppling deposition as a thickness (mm)
      m_dCumulSandToppleDeposit,                   // Cumulative sand-sized toppling deposition as a thickness (mm)
      m_dClayInfiltDeposit,                        // This-timestep clay-sized infiltration deposition as a thickness (mm)
      m_dSiltInfiltDeposit,                        // This-timestep silt-sized infiltration deposition as a thickness (mm)
      m_dSandInfiltDeposit,                        // This-timestep sand-sized infiltration deposition as a thickness (mm)
      m_dCumulClayInfiltDeposit,                   // Cumulative clay-sized infiltration deposition as a thickness (mm)
      m_dCumulSiltInfiltDeposit,                   // Cumulative silt-sized infiltration deposition as a thickness (mm)
      m_dCumulSandInfiltDeposit,                   // This-timestep sand-sized infiltration deposition as a thickness (mm)
      m_dShearStress,                              // Current shear stress in kg/m s**2 (Pa)
      m_dCumulShearStress,                         // Cumulative shear stress in kg/m s**2 (Pa)
      m_dLaplacian,                                // For splash calcs
      m_dClayHeadcutRetreatDetach,                 // This-iteration clay-sized detachment (mm) due to headcut retreat
      m_dSiltHeadcutRetreatDetach,                 // This-iteration silt-sized detachment (mm) due to headcut retreat
      m_dSandHeadcutRetreatDetach,                 // This-iteration sand-sized detachment (mm) due to headcut retreat
      m_dCumulClayHeadcutRetreatDetach,            // Cumulative clay-sized detachment (mm) due to headcut retreat
      m_dCumulSiltHeadcutRetreatDetach,            // Cumulative silt-sized detachment (mm) due to headcut retreat
      m_dCumulSandHeadcutRetreatDetach,            // Cumulative sand-sized detachment (mm) due to headcut retreat
      m_dClayHeadcutRetreatDeposit,                // This-iteration clay-sized deposition (mm) due to headcut retreat
      m_dSiltHeadcutRetreatDeposit,                // This-iteration silt-sized deposition (mm) due to headcut retreat
      m_dSandHeadcutRetreatDeposit,                // This-iteration sand-sized deposition (mm) due to headcut retreat
      m_dCumulClayHeadcutRetreatDeposit,           // Cumulative clay-sized deposition (mm) due to headcut retreat
      m_dCumulSiltHeadcutRetreatDeposit,           // Cumulative silt-sized deposition (mm) due to headcut retreat
      m_dCumulSandHeadcutRetreatDeposit;           // Cumulative sand-sized deposition (mm) due to headcut retreat

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

//    void ChangeTopLayerThickness(double const);

   void DoFlowDetach(double const);
   void DoFlowDeposit(double const, double const, double const);

   double dGetClayFlowDetach(void) const;
   double dGetSiltFlowDetach(void) const;
   double dGetSandFlowDetach(void) const;
   double dGetTotFlowDetach(void) const;
   double dGetCumulClayFlowDetach(void) const;
   double dGetCumulSiltFlowDetach(void) const;
   double dGetCumulSandFlowDetach(void) const;
   double dGetCumulAllSizeFlowDetach(void) const;

   double dGetClayFlowDeposition(void) const;
   double dGetSiltFlowDeposition(void) const;
   double dGetSandFlowDeposition(void) const;
   double dGetAllSizeFlowDeposition(void) const;
   double dGetCumulClayFlowDeposition(void) const;
   double dGetCumulSiltFlowDeposition(void) const;
   double dGetCumulSandFlowDeposition(void) const;
   double dGetCumulAllSizeFlowDeposition(void) const;

   void DoSplashDetach(double const);
   void SetSplashDepositionTemporary(double const);
   double dGetSplashDepositionTemporary(void) const;
   void DoSplashDeposit(double const, double const, double const);

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

   void DoSlumpDetach(double const, double&, double&, double&);
   void DoSlumpDeposit(double const, double const, double const);
   void DoToppleDetach(double const, double&, double&, double&);
   void DoToppleDeposit(double const, double const, double const);
   void ZeroThisOperationSlump(void);

   double dGetClaySlumpDetach(void) const;
   double dGetSiltSlumpDetach(void) const;
   double dGetSandSlumpDetach(void) const;
   double dGetAllSizeSlumpDetach(void) const;
   double dGetCumulClaySlumpDetach(void) const;
   double dGetCumulSiltSlumpDetach(void) const;
   double dGetCumulSandSlumpDetach(void) const;
   double dGetCumulAllSizeSlumpDetach(void) const;
   double dGetCumulAllSizeSlumpDeposit(void) const;

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
   double dGetTotInfiltDeposit(void) const;
   double dGetCumulClayInfiltDeposit(void) const;
   double dGetCumulSiltInfiltDeposit(void) const;
   double dGetCumulSandInfiltDeposit(void) const;
   double dGetCumulTotInfiltDeposit(void) const;

   void IncShearStress(double const);
   double dGetShearStress(void) const;
   double dGetCumulShearStress(void) const;

   double dGetCumulAllSizeLowering(void) const;

   void SetLaplacian(double const);
   double dGetLaplacian(void) const;

   void DoHeadcutRetreatDetach(double const, double&, double&, double&);
   void DoHeadcutRetreatDeposit(double const, double const, double const);
};

#endif         // __SOIL_H__
