/*========================================================================================================================================

 This is simulation.cpp: the start-of-simulation routine for RillGrow

 Copyright (C) 2018 David Favis-Mortlock

 =========================================================================================================================================

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation,
 Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

========================================================================================================================================*/
#include "rg.h"
#include "simulation.h"
#include "2d_vec.h"
#include "cell.h"


/*========================================================================================================================================

 The CSimulation constructor

========================================================================================================================================*/
CSimulation::CSimulation(void)
{
   // initialization
   m_bSlosSave                =
   m_bRainVarMSave            =
   m_bRunOnSave               =
   m_bElevSave                =
   m_bInitElevSave            =
   m_bDetrendElevSave         =
   m_bInfiltSave              =
   m_bTotInfiltSave           =
   m_bSoilWaterSave           =
   m_bInfiltDepositSave       =
   m_bTotInfiltDepositSave    =
   m_bTopSurfaceSave          =
   m_bSplashSave              =
   m_bTotSplashSave           =
   m_bInundationSave          =
   m_bFlowDirSave             =
   m_bStreamPowerSave         =
   m_bShearStressSave         =
   m_bFrictionFactorSave      =
   m_bAvgShearStressSave      =
   m_bReynoldsSave            =
   m_bFroudeSave              =
   m_bAvgDepthSave            =
   m_bAvgDWSpdSave            =
   m_bAvgSpdSave              =
   m_bSedConcSave             =
   m_bSedLoadSave             =
   m_bAvgSedLoadSave          =
   m_bTotDepositSave          =
   m_bTotNetSlosSave          =
   m_bTCSave                  =
   m_bNetSlumpSave            =
   m_bNetToppleSave           =
   m_bSaveRegular             =
   m_bDetrend                 =
   m_bFFCheck                 =
   m_bSplash                  =
   m_bSplashThisIter          =
   m_bSplashCheck             =
   m_bDoInfiltration            =
   m_bInfiltThisIter          =
   m_bFlowErosion             =
   m_bRunOn                   =
   m_bTimeVaryingRain         =
   m_bGISOutDiffers           =
   m_bSlumping                =
   m_bSlumpThisIter           =
   m_bTimeStepTS              =
   m_bAreaWetTS               =
   m_bRainTS                  =
   m_bInfiltTS                =
   m_bExfiltTS                =
   m_bRunOnTS                 =
   m_bSurfaceWaterTS          =
   m_bSurfaceWaterLostTS      =
   m_bFlowDetachTS            =
   m_bSlumpDetachTS           =
   m_bToppleDetachTS          =
   m_bSplashKETS              =
   m_bSplashRedistTS          =
   m_bInfiltDepositTS         =
   m_bSedLoadTS               =
   m_bSedLostTS               =
   m_bDoFlowDepositionTS      =
   m_bSoilWaterTS             =
   m_bSaveGISThisIter         =
   m_bThisIterRainChange      =
   m_bHaveBaseLevel           = 
   m_bOutDEMsUsingInputZUnits = false;
   
   for (int n = 0; n < 4; n++)
   {
      m_bClosedThisEdge[n] =
      m_bRunOnThisEdge[n] = false;
   }

#if defined _DEBUG
   m_bLostSave = false;
#endif

   m_nGISSave           =
   m_nUSave             =
   m_nThisSave          =
   m_nXGridMax          =
   m_nYGridMax          =
   m_nHeaderSize        =
   m_nRainChangeTimeMax = 
   m_nNumSoilLayers     = 0;
   m_nZUnits            = Z_UNIT_NONE;

   m_ulIter              =
   m_ulTotIter           =
   m_ulVarChkStart       =
   m_ulVarChkEnd         =
   m_ulNActiveCells      =
   m_ulNWet              = 
   m_ulMissingValueCells = 0;

   for (int n = 0; n < NUMBER_OF_RNGS; n++)
      m_ulRandSeed[n] = rand();

   m_dMinX                          =
   m_dMaxX                          =
   m_dMinY                          =
   m_dMaxY                          =
   m_dMaxSpeed                      =
   m_dPossibleMaxSpeed              =
   m_dBasementElevation             =
   m_dAvgElev                       =
   m_dMinElev                       =
   m_dGradient                      =
   m_dPlotEndDiff                   =
   m_dPlotElevMin                   =
   m_dPlotElevMax                   =
   m_dBaseLevel                     =
   m_dInvCos45                      =
   m_dYInc                          =
   m_dRainIntensity                 =
   m_dSpecifiedRainIntensity        =
   m_dStdRainInt                    =
   m_dDropDiameter                  =
   m_dStdDropDiameter               =
   m_dRainSpeed                     =
   m_dPartKE                        =
   m_VdSplashEffN                   =
   m_dSplashVertical                =
   m_dClaySplashedError             =
   m_dSiltSplashedError             =
   m_dSandSplashedError             =
   m_dCellSizeKC                    =
   m_dMeanCellWaterVol              =
   m_dStdCellWaterVol               =
   m_dCellSide                      =
   m_dCellDiag                      =
   m_dInvCellSide                   =
   m_dInvCellDiag                   =
   m_dCellSquare                    =
   m_dInvCellSquare                 =
   m_dInvXGridMax                   =
   m_dRoughnessScaling              =
   m_dD50                           =
   m_dEpsilon                       =
   m_dPr                            =
   m_dCd                            =
   m_dRho                           =
   m_dG                             =
   m_dNu                            =
   m_dK                             =
   m_dT                             =
   m_dCVT                           =
   m_dCVTaub                        =
   m_dST2                           =
   m_dSourceDestCritAngle           =
   m_dBulkDensityForOutputCalcs     = 
   m_dAlpha                         =
   m_dBeta                          =
   m_dGamma                         =
   m_dDelta                         =
   m_dPrevAdj                       =
   m_dRunOnLen                      =
   m_dRunOnSpd                      =
   m_dSlumpCritShearStress          =
   m_dSlumpAngleOfRest              =
   m_dSlumpAngleOfRestDiff          =
   m_dSlumpAngleOfRestDiffDiag      =
   m_dToppleCriticalAngle           =
   m_dToppleCritDiff                =
   m_dToppleCritDiffDiag            =
   m_dToppleAngleOfRest             =
   m_dToppleAngleOfRestDiff         =
   m_dToppleAngleOfRestDiffDiag     =
   m_dThisIterSurfaceWaterStored    =
   m_dThisIterClaySedLoad           =
   m_dThisIterSiltSedLoad           =
   m_dThisIterSandSedimentLoad      =
   m_dThisIterRain                  =
   m_dThisIterRunOn                 =
   m_dThisIterKE                    =
   m_dThisIterWaterLost             =
   m_dThisIterClayFlowDetach        =
   m_dThisIterSiltFlowDetach        =
   m_dThisIterSandFlowDetach        =
   m_dThisIterClayFlowDeposit       =
   m_dThisIterSiltFlowDeposit       =
   m_dThisIterSandFlowDeposit       =
   m_dThisIterClaySedLost           =
   m_dThisIterSiltSedLost           =
   m_dThisIterSandSedLost           =
   m_dThisIterClaySplashDetach      =
   m_dThisIterSiltSplashDetach      =
   m_dThisIterSandSplashDetach      =
   m_dThisIterClaySplashDeposit     =
   m_dThisIterSiltSplashDeposit     =
   m_dThisIterSandSplashDeposit     =
   m_dThisIterClaySlumpDetach       =
   m_dThisIterSiltSlumpDetach       =
   m_dThisIterSandSlumpDetach       =
   m_dThisIterClaySlumpDeposit      =
   m_dThisIterSiltSlumpDeposit      =
   m_dThisIterSandSlumpDeposit      =
   m_dThisIterClayToppleDetach      =
   m_dThisIterSiltToppleDetach      =
   m_dThisIterSandToppleDetach      =
   m_dThisIterClayToppleDeposit     =
   m_dThisIterSiltToppleDeposit     =
   m_dThisIterSandToppleDeposit     =
   m_dThisIterInfiltration          =
   m_dThisIterClayInfiltDeposit     =
   m_dThisIterSiltInfiltDeposit     =
   m_dThisIterSandInfiltDeposit     =
   m_dThisIterExfiltration          =
   m_dLastIterMaxHead               =
   m_dLastIterMinHead               =
   m_dThisIterMaxHead               = 
   m_dThisIterMinHead               = 
   m_dWaterCorrection               = 
   m_dSimulationDuration            =
   m_dSimulatedRainDuration         =
   m_dTimeStep                      =
   m_dSimulatedTimeElapsed          =
   m_dRSaveTime                     =
   m_dRSaveInterval                 =
   m_dSplashCalcLast                =
   m_dClkLast                       =
   m_dCPUClock                      =
   m_dTargetGTotDrops               = 
   m_dClayDiameter                  =
   m_dSiltDiameter                  =
   m_dSandDiameter                  =
   m_dDepositionGrainDensity        = 
   m_dDensityDiffExpression         = 
   m_dClaySizeMin                   =
   m_dClaySiltBoundarySize          =
   m_dSiltSandBoundarySize          =
   m_dSandSizeMax                   = 
   m_dLastSlumpCalcTime             = 
   m_dSinceLastTSInfiltration       = 
   m_dSinceLastTSExfiltration       =
   m_dSinceLastTSClayInfiltDeposit  =
   m_dSinceLastTSSiltInfiltDeposit  =
   m_dSinceLastTSSandInfiltDeposit  = 
   m_dSinceLastTSClaySplashRedist   =
   m_dSinceLastTSSiltSplashRedist   =
   m_dSinceLastTSSandSplashRedist   = 
   m_dSinceLastTSKE                 = 
   m_dSinceLastTSClaySlumpDetach    =
   m_dSinceLastTSSiltSlumpDetach    =
   m_dSinceLastTSSandSlumpDetach    = 
   m_dSinceLastTSClayToppleDetach   =
   m_dSinceLastTSSiltToppleDetach   =
   m_dSinceLastTSSandToppleDetach   = 0;
   
   for (int i = 0; i < 6; i++)
      m_dGeoTransform[i] = 0;

   m_dRunOnRainVarM              =
   m_dRainVarMFileMean           = 1;

   m_dMissingValue               = -9999;
   
   m_ldGTotDrops                 =
   m_ldGTotRunOnDrops            =
   m_ldGTotRain                  =
   m_ldGTotRunOnWater            =
   m_ldGTotInfilt                =
   m_ldGTotExfilt                =
   m_ldGTotWaterLost             =
   m_ldGTotFlowDetach            =
   m_ldGTotFlowDeposition        =
   m_ldGTotSedLost               =
   m_ldGTotSplashDetach          =
   m_ldGTotSplashDeposit         =
   m_ldGTotSlumpDetach           =
   m_ldGTotToppleDetach          =
   m_ldGTotToppleDeposit         =
   m_ldGTotInfiltDeposit         = 0;

   for (int i = 0; i < 2; i++)
   {
      m_ulRState[i].s1 =
      m_ulRState[i].s2 =
      m_ulRState[i].s3 = 0;
   }

   m_tSysStartTime =
   m_tSysEndTime   = 0;

   Cell = NULL;
}


/*========================================================================================================================================

 The CSimulation destructor

========================================================================================================================================*/
CSimulation::~CSimulation(void)
{
   // Close output files if open
   if (LogStream && LogStream.is_open())
      LogStream.close();

   if (OutStream && OutStream.is_open())
      OutStream.close();

   if (TimeStepTSStream && TimeStepTSStream.is_open())
      TimeStepTSStream.close();

   if (AreaWetTSStream && AreaWetTSStream.is_open())
      AreaWetTSStream.close();

   if (RainTSStream && RainTSStream.is_open())
      RainTSStream.close();

   if (InfiltrationTSStream && InfiltrationTSStream.is_open())
      InfiltrationTSStream.close();

   if (ExfiltrationTSStream && ExfiltrationTSStream.is_open())
      ExfiltrationTSStream.close();
   
   if (RunOnTSStream && RunOnTSStream.is_open())
      RunOnTSStream.close();

   if (SurfaceWaterTSStream && SurfaceWaterTSStream.is_open())
      SurfaceWaterTSStream.close();

   if (SurfaceWaterLostTSStream && SurfaceWaterLostTSStream.is_open())
      SurfaceWaterLostTSStream.close();

   if (FlowDetachmentTSStream && FlowDetachmentTSStream.is_open())
      FlowDetachmentTSStream.close();

   if (SlumpDetachmentTSStream && SlumpDetachmentTSStream.is_open())
      SlumpDetachmentTSStream.close();

   if (ToppleDetachmentTSStream && ToppleDetachmentTSStream.is_open())
      ToppleDetachmentTSStream.close();

   if (SedimentLostTSStream && SedimentLostTSStream.is_open())
      SedimentLostTSStream.close();

   if (SedimentLoadTSStream && SedimentLoadTSStream.is_open())
      SedimentLoadTSStream.close();

   if (InfiltrationDepositionTSStream && InfiltrationDepositionTSStream.is_open())
      InfiltrationDepositionTSStream.close();

   if (SplashDetachmentTSStream && SplashDetachmentTSStream.is_open())
      SplashDetachmentTSStream.close();

   if (SplashDepositionTSStream && SplashDepositionTSStream.is_open())
      SplashDepositionTSStream.close();

   if (SplashKineticEnergyTSStream && SplashKineticEnergyTSStream.is_open())
      SplashKineticEnergyTSStream.close();

   if (SoilWaterTSStream && SoilWaterTSStream.is_open())
      SoilWaterTSStream.close();
   
   if (Cell)
   {
      // Delete all Cell objects
      for (int nX = 0; nX < m_nXGridMax; nX++)
         delete [] Cell[nX];
      delete [] Cell;
   }
}


CSimulation* CCell::m_pSim = NULL;                    // Initialise m_pSim, the static member of CCell
CSimulation* CSoil::m_pSim = NULL;                    // Ditto for the CSoil class
CSimulation* CRainAndRunon::m_pSim = NULL;            // Ditto for the CRainAndRunon class
CSimulation* CSurfaceWater::m_pSim = NULL;            // Ditto for the CSurfaceWater class
CSimulation* CSediment::m_pSim = NULL;                // Ditto for the CSediment class


/*========================================================================================================================================

 The nDoRun member function of CSimulation sets up and runs the simulation

========================================================================================================================================*/
int CSimulation::nDoRun(int nArg, char* pcArgv[])
{
   // ======================================================= initialization section =====================================================
   // Hello, World!
   AnnounceStart();

   // Start the clock ticking
   StartClock();

   // Find out the folder in which the RG executable sits, in order to open the .ini file (they are assumed to be in the same folder)
   if (! bFindExeDir(pcArgv[0]))
      return (RTN_ERR_RGDIR);
   
   // Register all available GDAL drivers
   GDALAllRegister();

   // Deal with command-line parameters
   int nRet = nHandleCommandLineParams(nArg, pcArgv);
   if (nRet != RTN_OK)
      return (nRet);

   // OK, we are off, tell the user about the licence
   AnnounceLicence();

   // Read the .ini file and get the name of the run-data file, and path for output etc.
   if (! bReadIni())
      return (RTN_ERR_INI);

   // We have the name of the run-data input file, so read it
   if (! bReadRunData())
      return (RTN_ERR_RUNDATA);

   // Open log file
   if (! bOpenLogFile())
      return (RTN_ERR_LOGFILE);

   // Set up the time series output files
   if (! bSetUpTSFiles())
      return (RTN_ERR_TSFILE);

   // Initialize the random number generators
   InitRand0(m_ulRandSeed[0]);
   InitRand1(m_ulRandSeed[1]);

   // Tell the user what is happening
   AnnounceReadDEM();

   // Read in the microtography DEM and create the Cell array
   nRet = nReadMicrotopographyDEMData();
   if (nRet != RTN_OK)
      return (nRet);
   
   // Set the shared pointers to the CSimulation object
   CCell::m_pSim = this;   
   CSoil::m_pSim = this;   
   CRainAndRunon::m_pSim = this;
   CSurfaceWater::m_pSim = this;
   CSediment::m_pSim = this;

   // Mark edge cells
   MarkEdgeCells();
   
   // Create soil layers
   CreateSoilLayers();
   
   // If we are simulating infiltration, then create the this-iteration and time series soil water variables
   if (m_bDoInfiltration)
   {
      for (int nLayer = 0; nLayer < m_nNumSoilLayers; nLayer++)
      {
         m_VdThisIterSoilWater.push_back(0);
         m_VdSinceLastTSSoilWater.push_back(0);
      }
   }
   
   // If an overall gradient has been specified, impose this on the basement and initial surface elevation values in the Cell array; otherwise estimate the array's pre-existing slope
   CalcGradient();
   
   // Check GIS output format
   if (! bCheckGISOutputFormat())
      return (RTN_ERR_GISOUTPUTFORMAT);
   
   // If we have a rainfall intensity time-series-file, read it in now
   if (m_bTimeVaryingRain && ! bReadRainfallTimeSeries())
      return (RTN_ERR_RAINFALLTSFILE);

   // If we have a file for the rainfall variation multiplier mask, read it in to the Cell array
   if (! m_strRainVarMFile.empty())
   {
      nRet = nReadRainVarData();
      if (nRet != RTN_OK)
         return (nRet);
   }

   // Calculate various things which will be constants for the duration of the run (e.g. diagonal of cell side, inverse of cell side and diagonal, area of cell, etc.) now, do this only once for efficiency
   m_ulNActiveCells = (m_nXGridMax * m_nYGridMax) - m_ulMissingValueCells++;
   m_dInvCellSide   = 1 / m_dCellSide;                             // mm-1
   m_dCellSquare    = m_dCellSide * m_dCellSide;                   // mm2
   m_dInvCellSquare = 1 / m_dCellSquare;                           // mm-2
   m_dCellDiag      = sqrt(2 * m_dCellSquare);                     // mm
   m_dInvCellDiag   = 1 / m_dCellDiag;                             // mm-1
   m_dInvCos45      = 1 / cos(PI/4);
   m_dInvXGridMax   = 1 / static_cast<double>(m_nXGridMax);

   // If necessary, set up some initial values for infiltration, write to the Cell array
   if (m_bDoInfiltration)
      InitSoilWater();

   // Start with a guessed-in maximum flow speed
   m_dMaxSpeed = INIT_MAX_SPEED_GUESS;

   // Calculate a 'constant' for detachment, do this now for efficiency
   m_dST2 = m_dCVT * m_dCVT * m_dT * m_dT;
   
   // Calculate some 'constants' for deposition
   m_dClayDiameter = ((m_dClaySizeMin + m_dClaySiltBoundarySize) / 2) * 0.001;               // Convert from mm to m
   m_dSiltDiameter = ((m_dClaySiltBoundarySize + m_dSiltSandBoundarySize) / 2) * 0.001;      // Ditto   
   m_dSandDiameter = ((m_dSiltSandBoundarySize + m_dSandSizeMax) / 2) * 0.001;               // Ditto
   m_dDensityDiffExpression = (m_dDepositionGrainDensity - m_dRho) / m_dRho;
   
   // If desired, output friction factor for checking
   if (m_bFFCheck)
      CheckFF();

   // If considering slumping, calculate some 'constants' for slumping and toppling
   if (m_bSlumping)
   {
      m_dSlumpAngleOfRestDiff      = m_dCellSide * m_dSlumpAngleOfRest / 100;
      m_dSlumpAngleOfRestDiffDiag  = m_dCellDiag * m_dSlumpAngleOfRest / 100;
      m_dToppleCritDiff            = m_dCellSide * m_dToppleCriticalAngle / 100;
      m_dToppleCritDiffDiag        = m_dCellDiag * m_dToppleCriticalAngle / 100;
      m_dToppleAngleOfRestDiff     = m_dCellSide * m_dToppleAngleOfRest / 100;
      m_dToppleAngleOfRestDiffDiag = m_dCellDiag * m_dToppleAngleOfRest / 100;
   }

   // If not doing time-varying rainfall, calculate 'target' number of rain drops; this is used for rain intensity correction
   if (! m_bTimeVaryingRain)
      m_dTargetGTotDrops = m_dRainIntensity * m_ulNActiveCells * m_dCellSquare * m_dSimulatedRainDuration / (3600 * m_dMeanCellWaterVol);

   // If we are doing splash redistribution, set up splash efficiency stuff now
   if (m_bSplash)
   {
      // Calculate these now, for efficiency
      m_dPartKE  = 0.5 * m_dCellSquare * m_dRho * 1e-3 * m_dRainSpeed * m_dRainSpeed * 1e-12;    // Will be in Joules when multiplied by rain depth in mm
      m_VdSplashEffN /= (m_dCellSquare * m_dRainSpeed * m_dRainSpeed);

      // Compensate for grid size in the Laplacian classic splash calculations, assuming that m_VdSplashEffN (as read in) was calibrated for a 10mm grid 
      // TODO does this work correctly?
      double const fAC = 0.619;
      double const fBC = 0.1969;
      m_dCellSizeKC = (10 * (fAC - fBC * log(10))) / (m_dCellSide * (fAC - fBC * log(m_dCellSide)));
      m_dCellSizeKC /= m_dCellSquare;

      // Read in splash efficiency depth-attenuation parameters
      if (! bReadSplashEffData())
         return (RTN_ERR_SPLASHEFF);

      // Call initializing routine to calculate second derivatives for cubic spline
      InitSplashEff();

      // Now the vertical lowering, convert from mm/hr to mm/sec
      m_dSplashVertical /= 3600;

      // If desired, output splash efficiency for checking
      if (m_bSplashCheck)
         CheckSplashEff();
   }

   // If just outputting friction factor check file, or splash efficiency check file, then don't do any more
   if (m_bFFCheck || m_bSplashCheck)
      return (RTN_CHECKONLY);

   // Open OUT file
   OutStream.open(m_strOutFile, ios::out | ios::trunc);
   if (! OutStream)
   {
      // Error, cannot open Out file
      cerr << ERR << "cannot open " << m_strOutFile << " for output" << endl;
      return (RTN_ERR_OUTFILE);
   }

   // Write run details to Out and Log files
   WriteRunDetails();

   // ========================================================= Run simulation ===========================================================
   // Tell the user what is happening
   AnnounceIsRunning();

   nRet = nDoSimulation();
   if (nRet != RTN_OK)
      return nRet;

   // ====================================================== Post-simulation section =====================================================
   // Tell the user what is happening
   AnnounceSimEnd();

   // Finally calculate performance details
   OutStream << PERFORMHEAD << endl;
   OutStream << "Time simulated: " << strDispTime(m_dSimulationDuration, true, false) << endl << endl;

#ifndef RANDCHECK
   // Calculate length of run, write in file
   CalcTime(m_dSimulationDuration);
#endif

   // Calculate statistics re. memory usage etc.
   CalcProcessStats();
   OutStream << endl << "END OF RUN" << endl;

   // Need to flush these here (if we don't, the buffer doesn't get written)
   LogStream.flush();
   OutStream.flush();

   return (RTN_OK);
}


/*========================================================================================================================================
 
 Creates the soil layers, note that this must be done before any overall gradient is imposed on the DEM
 
========================================================================================================================================*/
void CSimulation::CreateSoilLayers(void)
{
   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      for (int nY = 0; nY < m_nYGridMax; nY++)
      {
         double dTotalSoilDepth = Cell[nX][nY].dGetInitialSoilSurfaceElevation() - m_dBasementElevation;
         
         Cell[nX][nY].pGetSoil()->SetSoilLayers(dTotalSoilDepth, m_nNumSoilLayers, &m_VstrInputSoilLayerName, &m_VdInputSoilLayerThickness, &m_VdInputSoilLayerPerCentClay, &m_VdInputSoilLayerPerCentSilt, &m_VdInputSoilLayerPerCentSand, &m_VdInputSoilLayerBulkDensity, &m_VdInputSoilLayerClayFlowErodibility, &m_VdInputSoilLayerSiltFlowErodibility, &m_VdInputSoilLayerSandFlowErodibility, &m_VdInputSoilLayerClaySplashErodibility, &m_VdInputSoilLayerSiltSplashErodibility, &m_VdInputSoilLayerSandSplashErodibility, &m_VdInputSoilLayerClaySlumpErodibility, &m_VdInputSoilLayerSiltSlumpErodibility, &m_VdInputSoilLayerSandSlumpErodibility, &m_VdInfiltCPHWF, &m_VdInfiltChiPart);
      }
   }
   
   // TODO improve this   
   m_dBulkDensityForOutputCalcs = m_VdInputSoilLayerBulkDensity[0];   //  / 1000;
}


/*========================================================================================================================================
 
 Calculates the timestep for the next iteration
 
========================================================================================================================================*/
void CSimulation::CalcTimestep(void)
{
   // Increase m_dMaxSpeed if necessary
   m_dMaxSpeed = tMax(m_dMaxSpeed, m_dPossibleMaxSpeed);
   
   if (0 == m_dPossibleMaxSpeed)
   {
      // No flow occurred. Is it raining?
      if (m_dRainIntensity > 0)
         m_dTimeStep = DEFAULT_TIMESTEP_RAIN;
      else
         m_dTimeStep = DEFAULT_TIMESTEP_NORAIN;
   }
   else
   {
      // Some flow occurred: first work out an equivalent of the 'Courant–Friedrichs–Lewy condition' i.e. the time needed for flow to cross a cell at the maximum speed during the last iteration, plus an arbitrary safety margin. COURANT_ALPHA = delta_time / delta_distance See e.g. http://en.wikipedia.org/wiki/Courant%E2%80%93Friedrichs%E2%80%93Lewy_condition
      double dTmp = COURANT_ALPHA * m_dCellSide / m_dMaxSpeed;                   // in sec
      
      // However, don't want to change timestep too dramatically at the beginning of the simulation. We can get a relatively long timestep as soon as the first, very slow, flow occurs; during which a great deal of splash occurs, probably too much to be comfortably handled by the splash routines). So constrain the newly calculated value
      if (dTmp > m_dTimeStep)
         // Timestep getting longer
         m_dTimeStep = tMin(dTmp, m_dTimeStep * (1 + TIMESTEP_CHANGE_FACTOR));
      
      m_dPossibleMaxSpeed = 0;                                                   // reset for next time
   }
}


/*========================================================================================================================================
 
 Calculate and checks the mass balance (water and sediment) for this iteration in order to remove rounding errors, which always accumulate when many small values are summed using finite-precision arithmetic (see e.g. "Floating-Point Summation" http://www.ddj.com/cpp/184403224#REF1). Even after a few iterations, mass-balance errors will start to appear here, unless a correction is made every iteration
 
========================================================================================================================================*/
void CSimulation::RemoveCumulativeRoundingErrors(void)
{
   static double
      sdWaterStoredLast = 0,
      sdWaterCorrection = 0;
   double
      dDeltaWaterStored = m_dThisIterSurfaceWaterStored - sdWaterStoredLast;       // Note can be -ve
   
   // Next calculate LHS = rain + run-on + correction, RHS = infiltration + outflow + change in storage
   double
      dTotWaterLHS = m_dThisIterRain + m_dThisIterRunOn + sdWaterCorrection,
      dTotWaterRHS = m_dThisIterInfiltration + m_dThisIterWaterLost + dDeltaWaterStored;
   
   // Force mass balance by correcting dWaterCorrection each iteration
   sdWaterCorrection += (dTotWaterRHS - dTotWaterLHS);
   
#if defined _DEBUG
   // Recalculate dTotWaterLHS (since dWaterCorrection was changed) and check
   dTotWaterLHS = m_dThisIterRain + m_dThisIterRunOn + sdWaterCorrection;
   if (! bFPIsEqual(dTotWaterLHS, dTotWaterRHS, TOLERANCE))
   {
      LogStream << m_ulIter << " " << WARN << "water balance, LHS = " << dTotWaterLHS << " RHS = " << dTotWaterRHS << endl;
      LogStream << "\tm_dThisIterRain           = " << m_dThisIterRain << endl;
      LogStream << "\tm_dThisIterRunOn          = " << m_dThisIterRunOn << endl;
      LogStream << "\t\tm_dThisIterInfiltration = " << m_dThisIterInfiltration << endl;
      LogStream << "\t\tm_dThisIterWaterLost    = " << m_dThisIterWaterLost << endl;
      LogStream << "\t\tfDeltaWaterStored       = " << dDeltaWaterStored << endl;
   } /*
   else
   {
      LogStream << m_ulIter << " OK water balance, LHS = " << fTotWaterLHS << " RHS = " << fTotWaterRHS << endl;
      LogStream << "\tm_dThisIterRain           = " << m_dThisIterRain << endl;
      LogStream << "\tm_dThisIterRunOn          = " << m_dThisIterRunOn << endl;
      LogStream << "\t\tm_dThisIterInfiltration = " << m_dThisIterInfiltration << endl;
      LogStream << "\t\tm_dThisIterWaterLost    = " << m_dThisIterWaterLost << endl;
      LogStream << "\t\tfDeltaWaterStored       = " << fDeltaWaterStored << endl;
   } */
#endif

   // Store for next iteration
   sdWaterStoredLast = m_dThisIterSurfaceWaterStored;

   // Now calculate this-iteration sediment balance. Again, must deal with accumulation of rounding errors due to summation of many small values. Therefore mass balance for sediment (all as depths) must be corrected here. First calculate change in sediment storage, between this and last iteration
   static double
      sdSedimentLoadDepthLast = 0,
      sdSedCorrection = 0;
      
   double dDeltaSedLoadStored = (m_dThisIterClaySedLoad + m_dThisIterSiltSedLoad + m_dThisIterSandSedimentLoad) - sdSedimentLoadDepthLast;

   // LHS = flow detachment + slump and topple detachment + correction, RHS = flow deposition + slump and topple deposition + sediment lost + change in storage. Note cannot include deposition resulting from cells drying up due to infiltration, because doing so results in double-counting of this depth of sediment
   double
      dThisIterFlowDetach = m_dThisIterClayFlowDetach + m_dThisIterSiltFlowDetach + m_dThisIterSandFlowDetach,
      dThisIterFlowDeposit = m_dThisIterClayFlowDeposit + m_dThisIterSiltFlowDeposit + m_dThisIterSandFlowDeposit;
   
   double   
      dThisIterSplashDetach = m_dThisIterClaySplashDetach + m_dThisIterSiltSplashDetach + m_dThisIterSandSplashDetach,
      dThisIterSplashDeposit = m_dThisIterClaySplashDeposit + m_dThisIterSiltSplashDeposit + m_dThisIterSandSplashDeposit;
      
   double   
      dThisIterSlumpDetach = m_dThisIterClaySlumpDetach + m_dThisIterSiltSlumpDetach  + m_dThisIterSandSlumpDetach,
      dThisIterSlumpDeposit = m_dThisIterClaySlumpDeposit + m_dThisIterSiltSlumpDeposit  + m_dThisIterSandSlumpDeposit;
      
   double  
      dThisIterToppleDetach = m_dThisIterClayToppleDetach + m_dThisIterSiltToppleDetach  + m_dThisIterSandToppleDetach,
      dThisIterToppleDeposit = m_dThisIterClayToppleDeposit + m_dThisIterSiltToppleDeposit  + m_dThisIterSandToppleDeposit;
      
   double 
      dThisIterSedLost = m_dThisIterClaySedLost + m_dThisIterSiltSedLost + m_dThisIterSandSedLost;

   double   
      dTotSedLHS = dThisIterFlowDetach + dThisIterSplashDetach + dThisIterSlumpDetach + dThisIterToppleDetach + sdSedCorrection,
      dTotSedRHS = dThisIterFlowDeposit + dThisIterSplashDeposit + dThisIterSlumpDeposit + dThisIterToppleDeposit + dThisIterSedLost + dDeltaSedLoadStored;

   // Force mass balance by correcting dSedCorrection each iteration
   sdSedCorrection += dTotSedRHS - dTotSedLHS;

#if defined _DEBUG
   // Recalculate dTotSedLHS (since dSedCorrection was changed) and check
   dTotSedLHS = dThisIterFlowDetach + dThisIterSplashDetach + dThisIterSlumpDetach + dThisIterToppleDetach + sdSedCorrection;
   if (! bFPIsEqual(dTotSedLHS, dTotSedRHS, TOLERANCE))
   {
      LogStream << m_ulIter << " " << WARN << "sediment balance, LHS = " << dTotSedLHS << " RHS = " << dTotSedRHS << endl;
      LogStream << "\tdThisIterFlowDetach           = " << dThisIterFlowDetach << endl;
      LogStream << "\tdThisIterSlumpDetach          = " << dThisIterSlumpDetach << endl;
      LogStream << "\tdThisIterToppleDetach         = " << dThisIterToppleDetach << endl;
      LogStream << "\t\tdThisIterFlowDeposit           = " << dThisIterFlowDeposit << endl;
      LogStream << "\t\tdThisIterSlumpDeposit          = " << dThisIterSlumpDeposit << endl;
      LogStream << "\t\tdThisIterToppleDeposit         = " << dThisIterToppleDeposit << endl;
      LogStream << "\t\tdThisIterSedLost               = " << dThisIterSedLost << endl;
      LogStream << "\t\tdDeltaSedLoadStored              = " << dDeltaSedLoadStored << endl;
   } /*
   else
   {
   LogStream << m_ulIter << " OK sediment balance (splash iteration), LHS = " << fTotSedLHS << " RHS = " << fTotSedRHS << endl;
   LogStream << "\tdThisIterFlowDetach           = " << dThisIterFlowDetach << endl;
   LogStream << "\tdThisIterSlumpDetach          = " << dThisIterSlumpDetach << endl;
   LogStream << "\t\tdThisIterFlowDeposit           = " << dThisIterFlowDeposit << endl;
   LogStream << "\t\tdThisIterSlumpDeposit          = " << dThisIterSlumpDeposit << endl;
   LogStream << "\t\tdThisIterSedLost               = " << dThisIterSedLost << endl;
   LogStream << "\t\tdDeltaSedLoadStored              = " << dDeltaSedLoadStored << endl;
   } */
#endif

   // Store for next iteration
   sdSedimentLoadDepthLast = (m_dThisIterClaySedLoad + m_dThisIterSiltSedLoad + m_dThisIterSandSedimentLoad);
}


/*========================================================================================================================================
 
 Checks to see if the simulation has gone on too long, amongst other things
 
========================================================================================================================================*/
bool CSimulation::bTimeToQuit(void)
{
   // Add timestep to the total time simulated so far
   m_dSimulatedTimeElapsed += m_dTimeStep;
   
   if (m_dSimulatedTimeElapsed >= m_dSimulationDuration)
   {
      // It is time to quit
      m_dSimulatedTimeElapsed = m_dSimulationDuration;
      AnnounceProgress();
      return true;
   }
   
   // Not quitting, so increment the iteration count, and recalc total iterations
   m_ulIter++;
   m_ulTotIter = static_cast<unsigned long>(dRound(m_dSimulationDuration / m_dTimeStep));
   
   // Check to see if we have done CLOCK_CHECK_ITERATIONS iterations: if so, it is time to reset the CPU time running total in case the clock()
   // function later rolls over
   if (0 == m_ulIter % CLOCK_CHECK_ITERATIONS)
      DoCPUClockReset();
   
   // Not yet time to quit
   return false;
}


/*========================================================================================================================================
 
 Update grand totals at the end of each iteration (these are all volumes)
 
========================================================================================================================================*/
void CSimulation::UpdateGrandTotals(void)
{
   m_ldGTotRain        += (m_dThisIterRain * m_dCellSquare);
   m_ldGTotRunOnWater  += (m_dThisIterRunOn * m_dCellSquare);
   m_ldGTotWaterLost   += (m_dThisIterWaterLost * m_dCellSquare);
   m_ldGTotFlowDetach  += (m_dThisIterClayFlowDetach + m_dThisIterSiltFlowDetach + m_dThisIterSandFlowDetach) * m_dCellSquare;
   
   // Note that ldGTotFlowDeposition will be an over-estimate of total deposition since there is no allowance made for deposited sediment
   // that is subsequently re-entrained. This is taken care of in the final mass balance calculation
   m_ldGTotFlowDeposition += (m_dThisIterClayFlowDeposit + m_dThisIterSiltFlowDeposit + m_dThisIterSandFlowDeposit) * m_dCellSquare;
   m_ldGTotSedLost     += (m_dThisIterClaySedLost + m_dThisIterSiltSedLost + m_dThisIterSandSedLost) * m_dCellSquare;
   
   if (m_bInfiltThisIter)
   {
      m_ldGTotInfilt        += (m_dThisIterInfiltration * m_dCellSquare);
      m_ldGTotInfiltDeposit += (m_dThisIterClayInfiltDeposit + m_dThisIterSiltInfiltDeposit + m_dThisIterSandInfiltDeposit) * m_dCellSquare;
      
      m_ldGTotExfilt        += (m_dThisIterExfiltration * m_dCellSquare);
   }
   
   if (m_bSplashThisIter)
   {
      m_ldGTotSplashDetach  += (m_dThisIterClaySplashDetach + m_dThisIterSiltSplashDetach + m_dThisIterSandSplashDetach) * m_dCellSquare;
      m_ldGTotSplashDeposit += (m_dThisIterClaySplashDeposit + m_dThisIterSiltSplashDeposit + m_dThisIterSandSplashDeposit) * m_dCellSquare;
   }
   
   if (m_bSlumpThisIter)
   {
      m_ldGTotSlumpDetach   += (m_dThisIterClaySlumpDetach + m_dThisIterSiltSlumpDetach + m_dThisIterSandSlumpDetach) * m_dCellSquare;
      m_ldGTotToppleDetach  += (m_dThisIterClayToppleDetach + m_dThisIterSiltToppleDetach + m_dThisIterSandToppleDetach) * m_dCellSquare;
      m_ldGTotToppleDeposit += (m_dThisIterClayToppleDeposit + m_dThisIterSiltToppleDeposit + m_dThisIterSandToppleDeposit) * m_dCellSquare;
   }
}


/*========================================================================================================================================
 
 This function does the simulation
 
========================================================================================================================================*/
int CSimulation::nDoSimulation(void)
{
#if defined RANDCHECK
   CheckRand();
   return (RTN_OK);
#endif
   
   // If requested, write an initial microtopography file (not detrended)
   if (m_bInitElevSave && (! bWriteFileFloat(PLOT_ELEVATION, &PLOT_ELEVATION_TITLE)))
      return (RTN_ERR_GISFILEWRITE);
   
   // If requested, write out the rainfall variation multiplier file
   if ((m_bRainVarMSave) && (! bWriteFileFloat(PLOT_RAIN_SPATIAL_VARIATION, &PLOT_RAIN_SPATIAL_VARIATION_TITLE)))
      return (RTN_ERR_GISFILEWRITE);
   
   // ========================================================== The main loop ===========================================================
   while (true)
   {
      // Calculate the timestep for this iteration
      CalcTimestep();
      
      // Check that we haven't gone on too long
      if (bTimeToQuit())
         break;
      
      // Tell the user how the simulation is progressing
      AnnounceProgress();
      
      // Initialize some fields of the Cell array ready for flow routing
      for (int nX = 0; nX < m_nXGridMax; nX++)
         for (int nY = 0; nY < m_nYGridMax; nY++)
            Cell[nX][nY].InitializeAtStartOfIteration(m_bSlumpThisIter);
      
      // If we are simulating infiltration then do some more initialization   
      if (m_bDoInfiltration)
      {
         for (int nLayer = 0; nLayer < m_nNumSoilLayers; nLayer++)
            m_VdThisIterSoilWater[nLayer] = 0;
      }
         
      // OK, start simulating for this iteration. First initialize the rainfall intensity for this iteration
      m_bThisIterRainChange = bSetUpRainfallIntensity();
      
      // Now simulate rainfall, and maybe run-on, provided it is still raining. This extra water goes onto the Cell array
      m_dThisIterRain  =
      m_dThisIterRunOn = 0;
      if (m_dRainIntensity > 0)
      {
         if (m_bRunOn)
         {
            for (int n = 0; n < 4; n++)
            {
               if (m_bRunOnThisEdge[n])
                  DoRunOnFromOneEdge(n);                 // Do run on from this edge
            }
         }
            
         DoAllRain();                                    // Do rainfall
      }
         
      // Route all flow from wet cells
      m_dThisIterMaxHead = DBL_MIN;
      m_dThisIterMinHead = DBL_MAX;
      
      m_dThisIterWaterLost       =
      m_dThisIterClayFlowDetach  =
      m_dThisIterSiltFlowDetach  =
      m_dThisIterSandFlowDetach  =
      m_dThisIterClayFlowDeposit =
      m_dThisIterSiltFlowDeposit =
      m_dThisIterSandFlowDeposit =
      m_dThisIterClaySedLost     = 
      m_dThisIterSiltSedLost     = 
      m_dThisIterSandSedLost     = 0;
      
      DoAllFlowRouting();
      
      // When representing flow off an edge of the grid, we need a value for the off-edge head. Save this iteration's maximum and minimum on-grid values of head for this
      m_dLastIterMaxHead = m_dThisIterMaxHead;
      m_dLastIterMinHead = m_dThisIterMinHead;
      
      // Infiltration next: first zero the this-iteration totals
      m_bInfiltThisIter = false;
      static int snInfiltCount = 0;
      
      m_dThisIterInfiltration      =
      m_dThisIterExfiltration      =
      m_dThisIterClayInfiltDeposit =
      m_dThisIterSiltInfiltDeposit = 
      m_dThisIterSandInfiltDeposit = 0;

      if (m_bDoInfiltration && (CALC_INFILTRATION_INTERVAL-1 == ++snInfiltCount))                 // If we are considering infiltration, simulate it this iteration?
      {
         // Yup, simulate infiltration from the Cell array
         snInfiltCount = 0;
         m_bInfiltThisIter = true;
         
         DoAllInfiltration();
      }
      
      // Next, splash redistribution. If we are considering splash redistribution, then do it this iteration, provided it is still raining
      // Note: tried calculating splash only on a subset of interations, but caused problems e.g. splash rate depended too much on how often splash was calculated; and leaving splash calcs too long meant that big splash changes could occur, which is unrealistic)
      m_bSplashThisIter = false;

      m_dClaySplashedError += (m_dThisIterClaySplashDetach - m_dThisIterClaySplashDeposit);     // Use values from the previous iteration
      m_dSiltSplashedError += (m_dThisIterSiltSplashDetach - m_dThisIterSiltSplashDeposit);     // Ditto
      m_dSandSplashedError += (m_dThisIterSandSplashDetach - m_dThisIterSandSplashDeposit);     // Ditto

      m_dThisIterClaySplashDetach  =
      m_dThisIterSiltSplashDetach  =
      m_dThisIterSandSplashDetach  =
      m_dThisIterClaySplashDeposit =
      m_dThisIterSiltSplashDeposit =
      m_dThisIterSandSplashDeposit =
      m_dThisIterKE                = 0;
      
      if (m_bSplash && (m_dRainIntensity > RAIN_MIN_CONSIDERED))
      {
         // Yes, simulating splash redistribution
         m_bSplashThisIter = true;
         
         // Now simulate the splash redistribution for this iteration
         DoAllSplash();
         
         // And reset the counter for next time
         m_dSplashCalcLast = m_dSimulatedTimeElapsed;
      }
      
      // Finally, rill-wall slumping and toppling
      m_bSlumpThisIter = false;
      static int snSlumpCount = 0;
      
      m_dThisIterClaySlumpDetach   =
      m_dThisIterSiltSlumpDetach   =
      m_dThisIterSandSlumpDetach   =
      m_dThisIterClaySlumpDeposit  =
      m_dThisIterSiltSlumpDeposit  =
      m_dThisIterSandSlumpDeposit  =
      m_dThisIterClayToppleDetach  =
      m_dThisIterSiltToppleDetach  =
      m_dThisIterSandToppleDetach  =
      m_dThisIterClayToppleDeposit =
      m_dThisIterSiltToppleDeposit =
      m_dThisIterSandToppleDeposit = 0;
      
      if (m_bSlumping && (CALC_SLUMP_INTERVAL-1 == ++snSlumpCount))            // If we are considering slumping, simulate it this iteration?
      {
         // Yup, simulate slumping and toppling
         snSlumpCount = 0;
         m_bSlumpThisIter = true;
         DoAllSlump();
         
         // Reset for next time
         m_dLastSlumpCalcTime = m_dSimulatedTimeElapsed;
      }
      
      // Now save results and do per-iteration book-keeping. First see if we need to save the GIS files now
      m_bSaveGISThisIter = false;
      if ((m_bSaveRegular && (m_dSimulatedTimeElapsed >= m_dRSaveTime) && (m_dSimulatedTimeElapsed < m_dSimulationDuration)) || (! m_bSaveRegular && (m_dSimulatedTimeElapsed >= m_VdSaveTime[m_nThisSave])))
      {
         // Yes, save the values from the Cell array into GIS files
         m_bSaveGISThisIter = true;
         if (! bSaveGISFiles())
            return (RTN_ERR_GISFILEWRITE);
      }
      
      // Calculate and check this-iteration hydrology and sediment balance
      RemoveCumulativeRoundingErrors();
      
      // Output per-iteration results to the .out file
      if (! bWritePerIterationResults())
         return (RTN_ERR_TEXTFILEWRITE);
      
      // Update totals for time series output which are not output every iteration
      m_dSinceLastTSInfiltration      += m_dThisIterInfiltration;
      m_dSinceLastTSExfiltration      += m_dThisIterExfiltration;
      m_dSinceLastTSClayInfiltDeposit += m_dThisIterClayInfiltDeposit;
      m_dSinceLastTSSiltInfiltDeposit += m_dThisIterSiltInfiltDeposit;
      m_dSinceLastTSSandInfiltDeposit += m_dThisIterSandInfiltDeposit;
      m_dSinceLastTSClaySplashRedist  += m_dThisIterClaySplashDetach;
      m_dSinceLastTSSiltSplashRedist  += m_dThisIterSiltSplashDetach;
      m_dSinceLastTSSandSplashRedist  += m_dThisIterSandSplashDetach;            
      m_dSinceLastTSKE                += m_dThisIterKE;
      m_dSinceLastTSClaySlumpDetach   += m_dThisIterClaySlumpDetach;
      m_dSinceLastTSSiltSlumpDetach   += m_dThisIterSiltSlumpDetach;
      m_dSinceLastTSSandSlumpDetach   += m_dThisIterSandSlumpDetach;      
      m_dSinceLastTSClayToppleDetach  += m_dThisIterClayToppleDetach;
      m_dSinceLastTSSiltToppleDetach  += m_dThisIterSiltToppleDetach;
      m_dSinceLastTSSandToppleDetach  += m_dThisIterSandToppleDetach;
      
      if (m_bDoInfiltration)
      {
         for (int nLayer = 0; nLayer < m_nNumSoilLayers; nLayer++)
            m_VdSinceLastTSSoilWater[nLayer] += m_VdThisIterSoilWater[nLayer];
      }
      
      // Now output time series CSV stuff
      if (! bWriteTSFiles(false))
         return (RTN_ERR_TSFILEWRITE);
      
      // Next, check for instability
      int nRet = nCheckForInstability();
      if (nRet != RTN_OK)
         return (nRet);
      
      // Update grand totals (these are all volumes)
      UpdateGrandTotals();
      
      // If there has been any noticeable loss of sediment from any edge this iteration, then adjust the elevation of the unbounded edge(s), based on the Cell values: ugly but necessary
//       if ((m_dThisIterClaySedLost + m_dThisIterSiltSedLost + m_dThisIterSandSedLost) > (SEDIMENT_MIN_CONSIDERED * m_nXGridMax)) 
//          AdjustUnboundedEdges();
      
   }  // ===================================================== End of main loop ===========================================================
   
   
   // ======================================================== post-loop tidying ==========================================================
   int nRet = nWriteFilesAtEnd();
   if (nRet != RTN_OK)
      return nRet;
   
   // Final write to time series CSV files
   if (! bWriteTSFiles(true))
      return (RTN_ERR_TSFILEWRITE);
   
   WriteGrandTotals();
   
   // Normal completion
   return (RTN_OK);
}


/*==============================================================================================================================
 
 Returns the current timestep
 
==============================================================================================================================*/
double CSimulation::dGetTimeStep(void) const
{
   return m_dTimeStep;
}


/*==============================================================================================================================
 
 Increments the this-iteration total of wet cells
 
==============================================================================================================================*/
void CSimulation::IncrThisIterNumWetCells(void)
{
   m_ulNWet++;
}

/*==============================================================================================================================
 
 Decrements the this-iteration total of wet cells
 
 ==============================================================================================================================*/
void CSimulation::DecrThisIterNumWetCells(void)
{
   m_ulNWet--;
}
