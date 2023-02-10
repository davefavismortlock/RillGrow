/*========================================================================================================================================

 This is simulation.cpp: the start-of-simulation routine for RillGrow

 Copyright (C) 2023 David Favis-Mortlock

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
   m_bFlowDetachSave          =
   m_bRainVarMSave            =
   m_bCumulRunOnSave          =
   m_bElevSave                =
   m_bInitElevSave            =
   m_bDetrendElevSave         =
   m_bInfiltSave              =
   m_bCumulInfiltSave         =
   m_bSoilWaterSave           =
   m_bInfiltDepositSave       =
   m_bCumulInfiltDepositSave  =
   m_bTopSurfaceSave          =
   m_bSplashSave              =
   m_bCumulSplashSave         =
   m_bInundationSave          =
   m_bFlowDirSave             =
   m_bStreamPowerSave         =
   m_bShearStressSave         =
   m_bFrictionFactorSave      =
   m_bCumulAvgShearStressSave =
   m_bReynoldsSave            =
   m_bFroudeSave              =
   m_bCumulAvgDepthSave       =
   m_bCumulAvgDWSpdSave       =
   m_bCumulAvgSpdSave         =
   m_bSedConcSave             =
   m_bSedLoadSave             =
   m_bAvgSedLoadSave          =
   m_bCumulFlowDepositSave    =
   m_bCumulLoweringSave       =
   m_bTCSave                  =
   m_bSlumpSave               =
   m_bToppleSave              =
   m_bSaveRegular             =
   m_bDetrend                 =
   m_bFFCheck                 =
   m_bSplash                  =
   m_bSplashThisIter          =
   m_bSplashCheck             =
   m_bDoInfiltration          =
   m_bInfiltThisIter          =
   m_bFlowErosion             =
   m_bRunOn                   =
   m_bTimeVaryingRain         =
   m_bGISOutDiffers           =
   m_bSlumping                =
   m_bSlumpThisIter           =
   m_bHeadcutRetreat          =
   m_bHeadcutRetreatThisIter  =
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
   m_bSedLoadLostTS           =
   m_bDoSedLoadDepositTS      =
   m_bSoilWaterTS             =
   m_bSaveGISThisIter         =
   m_bThisIterRainChange      =
   m_bHaveBaseLevel           =
   m_bOutDEMsUsingInputZUnits =
   m_bManningEqn              =
   m_bDarcyWeisbachEqn        =
   m_bFrictionFactorConstant  =
   m_bFrictionFactorReynolds  =
   m_bFrictionFactorLawrence  =
   m_bLostSave                =
   m_bFlumeTypeSim            =
   m_bSplashForward           = false;

   for (int n = 0; n < 4; n++)
   {
      m_bClosedThisEdge[n] =
      m_bRunOnThisEdge[n] = false;
   }

   m_nGISSave                 =
   m_nUSave                   =
   m_nThisSave                =
   m_nXGridMax                =
   m_nYGridMax                =
   m_nHeaderSize              =
   m_nRainChangeTimeMax       =
   m_nNumSoilLayers           =
   m_nSSSQuadrantSize         =
   m_nTimeVaryingRainCount    =
   m_nInfiltCount             =
   m_nSlumpCount              =
   m_nHeadcutRetreatCount     = 0;
   m_nZUnits                  = Z_UNIT_NONE;

   m_ulIter                   =
   m_ulTotIter                =
   m_ulVarChkStart            =
   m_ulVarChkEnd              =
   m_ulNActivem_Cells           =
   m_ulNWet                   =
   m_ulMissingValuem_Cells      =
   m_ulNumHead                = 0;

   for (int n = 0; n < NUMBER_OF_RNGS; n++)
      m_ulRandSeed[n] = rand();

   m_dMinX                          =
   m_dMaxX                          =
   m_dMinY                          =
   m_dMaxY                          =
   m_dMaxFlowSpeed                  =
   m_dPossMaxSpeedNextIter          =
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
   m_dSplashEffN                   =
   m_dm_CellSizeKC                    =
   m_dMeanm_CellWaterVol              =
   m_dStdm_CellWaterVol               =
   m_dm_CellSide                      =
   m_dm_CellDiag                      =
   m_dInvm_CellSide                   =
   m_dInvm_CellDiag                   =
   m_dm_CellSquare                    =
   m_dInvm_CellSquare                 =
   m_dInvXGridMax                   =
   m_dRho                           =
   m_dG                             =
   m_dNu                            =
   m_dK                             =
   m_dT                             =
   m_dCVT                           =
   m_dCVTaub                        =
   m_dST2                           =
   m_dBulkDensityForOutputCalcs     =
   m_dAlpha                         =
   m_dBeta                          =
   m_dGamma                         =
   m_dDelta                         =
   m_dRunOnLen                      =
   m_dRunOnSpd                      =
   m_dCritSSSForSlump               =
   m_dSlumpAngleOfRest              =
   m_dSlumpAngleOfRestDiff          =
   m_dSlumpAngleOfRestDiffDiag      =
   m_dToppleCriticalAngle           =
   m_dToppleCritDiff                =
   m_dToppleCritDiffDiag            =
   m_dToppleAngleOfRest             =
   m_dToppleAngleOfRestDiff         =
   m_dToppleAngleOfRestDiffDiag     =
   m_dThisIterStoredSurfaceWater    =
   m_dThisIterClaySedLoad           =
   m_dThisIterSiltSedLoad           =
   m_dThisIterSandSedLoad           =
   m_dThisIterRain                  =
   m_dThisIterRunOn                 =
   m_dThisIterKE                    =
   m_dThisIterLostSurfaceWater      =
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
   m_dThisIterClaySplashToSedLoad   =
   m_dThisIterSiltSplashToSedLoad   =
   m_dThisIterSandSplashToSedLoad   =
   m_dSinceLastClaySlumpDetach      =
   m_dSinceLastSiltSlumpDetach      =
   m_dSinceLastSandSlumpDetach      =
   m_dSinceLastClaySlumpDeposit     =
   m_dSinceLastSiltSlumpDeposit     =
   m_dSinceLastSandSlumpDeposit     =
   m_dSinceLastClaySlumpToSedLoad   =
   m_dSinceLastSiltSlumpToSedLoad   =
   m_dSinceLastSandSlumpToSedLoad   =
   m_dSinceLastClayToppleDetach     =
   m_dSinceLastSiltToppleDetach     =
   m_dSinceLastSandToppleDetach     =
   m_dSinceLastClayToppleDeposit    =
   m_dSinceLastSiltToppleDeposit    =
   m_dSinceLastSandToppleDeposit    =
   m_dSinceLastClayToppleToSedLoad  =
   m_dSinceLastSiltToppleToSedLoad  =
   m_dSinceLastSandToppleToSedLoad  =
   m_dSinceLastInfiltration         =
   m_dSinceLastExfiltration         =
   m_dSinceLastClayInfiltDeposit    =
   m_dSinceLastSiltInfiltDeposit    =
   m_dSinceLastSandInfiltDeposit    =
   m_dSinceLastClayHeadcutDetach    =
   m_dSinceLastSiltHeadcutDetach    =
   m_dSinceLastSandHeadcutDetach    =
   m_dSinceLastClayHeadcutDeposit   =
   m_dSinceLastSiltHeadcutDeposit   =
   m_dSinceLastSandHeadcutDeposit   =
   m_dSinceLastClayHeadcutToSedLoad =
   m_dSinceLastSiltHeadcutToSedLoad =
   m_dSinceLastSandHeadcutToSedLoad =
   m_dLastIterAvgHead               =
   m_dThisIterTotHead               =
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
   m_dLastHeadcutRetreatCalcTime    =
   m_dSinceLastTSWriteInfiltration       =
   m_dSinceLastTSWriteExfiltration       =
   m_dSinceLastTSWriteClayInfiltDeposit  =
   m_dSinceLastTSWriteSiltInfiltDeposit  =
   m_dSinceLastTSWriteSandInfiltDeposit  =
   m_dSinceLastTSWriteClaySplashRedist   =
   m_dSinceLastTSWriteSiltSplashRedist   =
   m_dSinceLastTSWriteSandSplashRedist   =
   m_dSinceLastTSWriteKE                 =
   m_dSinceLastTSWriteClaySlumpDetach    =
   m_dSinceLastTSWriteSiltSlumpDetach    =
   m_dSinceLastTSWriteSandSlumpDetach    =
   m_dSinceLastTSWriteClayToppleDetach   =
   m_dSinceLastTSWriteSiltToppleDetach   =
   m_dSinceLastTSWriteSandToppleDetach   =
   m_dHeadcutRetreatConst           =
   m_dOffEdgeConst                  =
   m_dSSSPatchSize                  =
   m_dOffEdgeParamA                 =
   m_dOffEdgeParamB                 =
   m_dManningParamA                 =
   m_dManningParamB                 =
   m_dFFConstant                    =
   m_dFFReynoldsParamA              =
   m_dFFReynoldsParamB              =
   m_dFFLawrenceD50                 =
   m_dFFLawrenceEpsilon             =
   m_dFFLawrencePr                  =
   m_dFFLawrenceCd                  =
   m_dWaterErrorLast                =
   m_dSplashErrorLast               =
   m_dSlumpErrorLast                =
   m_dToppleErrorLast               =
   m_dHeadcutErrorLast              =
   m_dFlowErrorLast                 =
   m_dElapsed                       =
   m_dStillToGo                     =
   m_dWaterStoredLast               =
   m_dSedimentLoadDepthLast         = 0;

   for (int i = 0; i < 6; i++)
      m_dGeoTransform[i] = 0;

   m_dRunOnRainVarM                 =
   m_dRainVarMFileMean              = 1;

   m_dMissingValue                  = -9999;

   m_ldGTotDrops                    =
   m_ldGTotRunOnDrops               =
   m_ldGTotRain                     =
   m_ldGTotRunOn               =
   m_ldGTotInfilt                   =
   m_ldGTotExfilt                   =
   m_ldGTotWaterLost                =
   m_ldGTotFlowDetach               =
   m_ldGTotFlowDeposit              =
   m_ldGTotSedLost                  =
   m_ldGTotSplashDetach             =
   m_ldGTotSplashDeposit            =
   m_ldGTotSplashToSedLoad          =
   m_ldGTotSlumpDetach              =
   m_ldGTotSlumpDeposit             =
   m_ldGTotSlumpToSedLoad           =
   m_ldGTotToppleDetach             =
   m_ldGTotToppleDeposit            =
   m_ldGTotToppleToSedLoad          =
   m_ldGTotInfiltDeposit            =
   m_ldGTotHeadcutRetreatDetach     =
   m_ldGTotHeadcutRetreatDeposit    =
   m_ldGTotHeadcutRetreatToSedLoad  = 0;

   for (int i = 0; i < 2; i++)
   {
      m_ulRState[i].s1 =
      m_ulRState[i].s2 =
      m_ulRState[i].s3 = 0;
   }

   m_tSysStartTime =
   m_tSysEndTime   = 0;

   m_Cell = NULL;
   m_SSSWeightQuadrant= NULL;
}


/*========================================================================================================================================

 The CSimulation destructor

========================================================================================================================================*/
CSimulation::~CSimulation(void)
{
   // Close output files if open
   if (m_ofsLog && m_ofsLog.is_open())
      m_ofsLog.close();

   if (m_ofsOut && m_ofsOut.is_open())
      m_ofsOut.close();

   m_ofsErrorTS.close();

   if (m_ofsTimestepTS && m_ofsTimestepTS.is_open())
      m_ofsTimestepTS.close();

   if (m_ofsAreaWetTS && m_ofsAreaWetTS.is_open())
      m_ofsAreaWetTS.close();

   if (m_ofsRainTS && m_ofsRainTS.is_open())
      m_ofsRainTS.close();

   if (m_ofsInfiltTS && m_ofsInfiltTS.is_open())
      m_ofsInfiltTS.close();

   if (m_ofsExfiltTS && m_ofsExfiltTS.is_open())
      m_ofsExfiltTS.close();

   if (m_ofsRunOnTS && m_ofsRunOnTS.is_open())
      m_ofsRunOnTS.close();

   if (m_ofsSurfaceWaterTS && m_ofsSurfaceWaterTS.is_open())
      m_ofsSurfaceWaterTS.close();

   if (m_ofsSurfaceWaterLostTS && m_ofsSurfaceWaterLostTS.is_open())
      m_ofsSurfaceWaterLostTS.close();

   if (m_ofsFlowDetachTS && m_ofsFlowDetachTS.is_open())
      m_ofsFlowDetachTS.close();

   if (m_ofsSlumpDetachTS && m_ofsSlumpDetachTS.is_open())
      m_ofsSlumpDetachTS.close();

   if (m_ofsToppleDetachTS && m_ofsToppleDetachTS.is_open())
      m_ofsToppleDetachTS.close();

   if (m_ofsSedLostTS && m_ofsSedLostTS.is_open())
      m_ofsSedLostTS.close();

   if (m_ofsSedLoadTS && m_ofsSedLoadTS.is_open())
      m_ofsSedLoadTS.close();

   if (m_ofsInfiltDepositTS && m_ofsInfiltDepositTS.is_open())
      m_ofsInfiltDepositTS.close();

   if (m_ofsSplashDetachTS && m_ofsSplashDetachTS.is_open())
      m_ofsSplashDetachTS.close();

   if (m_ofsSplashDepositTS && m_ofsSplashDepositTS.is_open())
      m_ofsSplashDepositTS.close();

   if (m_ofsSplashKETS && m_ofsSplashKETS.is_open())
      m_ofsSplashKETS.close();

   if (m_ofsSoilWaterTS && m_ofsSoilWaterTS.is_open())
      m_ofsSoilWaterTS.close();

   if (m_Cell)
   {
      // Delete all m_Cell objects
      for (int nX = 0; nX < m_nXGridMax; nX++)
         delete [] m_Cell[nX];
      delete [] m_Cell;
   }

   if (m_SSSWeightQuadrant)
   {
      for (int nX = 0; nX < m_nSSSQuadrantSize; nX++)
         delete [] m_SSSWeightQuadrant[nX];
      delete [] m_SSSWeightQuadrant;
   }
}


/*========================================================================================================================================

 Within-file static member variable initialisations

========================================================================================================================================*/
CSimulation* CCell::m_pSim = NULL;                    // Initialize m_pSim, the static member of CCell
CSimulation* CCellSoil::m_pSim = NULL;                    // Ditto for the CCellSoil class
CSimulation* CCellRainAndRunon::m_pSim = NULL;            // Ditto for the CCellRainAndRunon class
CSimulation* CCellSurfaceWater::m_pSim = NULL;            // Ditto for the CCellSurfaceWater class
CSimulation* CCellSedimentLoad::m_pSim = NULL;                // Ditto for the cellSediment class


/*========================================================================================================================================

 This publicly visible member function of CSimulation sets up and runs the simulation

========================================================================================================================================*/
int CSimulation::nDoSetUpRun(int nArg, char* pcArgv[])
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

   // Handle command-line parameters
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

   // Read in the microtography DEM and create the cell array
   nRet = nReadMicrotopographyDEMData();
   if (nRet != RTN_OK)
      return (nRet);

   // Set the shared pointers to the CSimulation object
   CCell::m_pSim = this;
   CCellSoil::m_pSim = this;
   CCellRainAndRunon::m_pSim = this;
   CCellSurfaceWater::m_pSim = this;
   CCellSedimentLoad::m_pSim = this;

   // Mark edge m_Cells
   MarkEdgem_Cells();

   // Create soil layers
   CreateSoilLayers();

   // If we are simulating infilt, then create the this-operation and time series soil water variables
   if (m_bDoInfiltration)
   {
      for (int nLayer = 0; nLayer < m_nNumSoilLayers; nLayer++)
      {
         m_VdThisIterSoilWater.push_back(0);
         m_VdSinceLastTSSoilWater.push_back(0);
      }
   }

   // If an overall gradient has been specified, impose this on the basement and initial surface elevation values in the cell array; otherwise estimate the array's pre-existing slope
   CalcGradient();

   // Check GIS output format
   if (! bCheckGISOutputFormat())
      return (RTN_ERR_GISOUTPUTFORMAT);

   // If we have a rainfall intensity time-series-file, read it in now
   if (m_bTimeVaryingRain && ! bReadRainfallTimeSeries())
      return (RTN_ERR_RAINFALLTSFILE);

   // If we have a file for the rainfall variation multiplier mask, read it in to the cell array
   if (! m_strRainVarMFile.empty())
   {
      nRet = nReadRainVarData();
      if (nRet != RTN_OK)
         return (nRet);
   }

   // Calculate various things which will be constants for the duration of the run (e.g. diagonal of m_Cell side, inverse of m_Cell side and diagonal, area of m_Cell, etc.) now, do this only once for efficiency
   m_ulNActivem_Cells = (m_nXGridMax * m_nYGridMax) - m_ulMissingValuem_Cells++;
   m_dInvm_CellSide   = 1 / m_dm_CellSide;                             // mm-1
   m_dm_CellSquare    = m_dm_CellSide * m_dm_CellSide;                   // mm2
   m_dInvm_CellSquare = 1 / m_dm_CellSquare;                           // mm-2
   m_dm_CellDiag      = sqrt(2 * m_dm_CellSquare);                     // mm
   m_dInvm_CellDiag   = 1 / m_dm_CellDiag;                             // mm-1
   m_dInvCos45      = 1 / cos(PI/4);
   m_dInvXGridMax   = 1 / static_cast<double>(m_nXGridMax);

   // If necessary, set up some initial values for infilt, write to the cell array
   if (m_bDoInfiltration)
      InitSoilWater();

   // Calculate the off-edge head constant using the empirical relationship const = m_dOffEdgeParamA * m_dGradient^m_dOffEdgeParamB where m_dGradient is in %
   m_dOffEdgeConst = m_dOffEdgeParamA * pow(m_dGradient, m_dOffEdgeParamB);

   // Calculate a 'constant' for detachment, do this now for efficiency
   m_dST2 = m_dCVT * m_dCVT * m_dT * m_dT;

   // Calculate some 'constants' for deposition
   m_dClayDiameter = ((m_dClaySizeMin + m_dClaySiltBoundarySize) / 2) * 0.001;               // Convert from mm to m
   m_dSiltDiameter = ((m_dClaySiltBoundarySize + m_dSiltSandBoundarySize) / 2) * 0.001;      // Ditto
   m_dSandDiameter = ((m_dSiltSandBoundarySize + m_dSandSizeMax) / 2) * 0.001;               // Ditto
   m_dDensityDiffExpression = (m_dDepositionGrainDensity - m_dRho) / m_dRho;

   // If desired, output friction factor for checking
   if (m_bFFCheck)
      CheckLawrenceFF();

   // If considering slumping, calculate some 'constants' for slumping and toppling
   if (m_bSlumping)
   {
      nRet = nInitSlumping();
      if (nRet != RTN_OK)
         return (nRet);
   }

   // If not doing time-varying rainfall, calculate 'target' number of rain drops; this is used for rain intensity correction
   if (! m_bTimeVaryingRain)
      m_dTargetGTotDrops = m_dRainIntensity * static_cast<double>(m_ulNActivem_Cells) * m_dm_CellSquare * m_dSimulatedRainDuration / (3600 * m_dMeanm_CellWaterVol);

   // If we are doing splash redistribution, set up splash efficiency stuff now
   if (m_bSplash)
   {
      // Calculate these now, for efficiency
      m_dPartKE  = 0.5 * m_dm_CellSquare * m_dRho * 1e-3 * m_dRainSpeed * m_dRainSpeed * 1e-12;    // Will be in Joules when multiplied by rain depth in mm
      m_dSplashEffN /= (m_dm_CellSquare * m_dRainSpeed * m_dRainSpeed);

      // Compensate for grid size in the Laplacian classic splash calculations, assuming that m_dSplashEffN (as read in) was calibrated for a 10mm grid
      // TODO does this work correctly?
      double const fAC = 0.619;
      double const fBC = 0.1969;
      m_dm_CellSizeKC = (10 * (fAC - fBC * log(10))) / (m_dm_CellSide * (fAC - fBC * log(m_dm_CellSide)));
      m_dm_CellSizeKC /= m_dm_CellSquare;

      // Read in splash efficiency depth-attenuation parameters
      if (! bReadSplashEffData())
         return (RTN_ERR_SPLASHEFF);

      // Call initializing routine to calculate second derivatives for cubic spline
      InitSplashEff();

      // If desired, output splash efficiency for checking
      if (m_bSplashCheck)
         CheckSplashEff();
   }

   // If just outputting friction factor check file, or splash efficiency check file, then don't do any more
   if (m_bFFCheck || m_bSplashCheck)
      return (RTN_CHECKONLY);

   // Open OUT file
   m_ofsOut.open(m_strOutFile, ios::out | ios::trunc);
   if (! m_ofsOut)
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
   m_ofsOut << PERFORMHEAD << endl;
   m_ofsOut << "Time simulated: " << strDispTime(m_dSimulationDuration, true, false) << endl << endl;

#ifndef RANDCHECK
   // Calculate length of run, write in file
   CalcTime(m_dSimulationDuration);
#endif

   // Calculate statistics re. memory usage etc.
   CalcProcessStats();
   m_ofsOut << endl << "END OF RUN" << endl;

   // Need to flush these here (if we don't, the buffer doesn't get written)
   m_ofsLog.flush();
   m_ofsOut.flush();

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
         double dTotalSoilDepth = m_Cell[nX][nY].dGetInitialSoilSurfaceElevation() - m_dBasementElevation;

         m_Cell[nX][nY].pGetSoil()->SetSoilLayers(dTotalSoilDepth, m_nNumSoilLayers, &m_VstrInputSoilLayerName, &m_VdInputSoilLayerThickness, &m_VdInputSoilLayerPerCentClay, &m_VdInputSoilLayerPerCentSilt, &m_VdInputSoilLayerPerCentSand, &m_VdInputSoilLayerBulkDensity, &m_VdInputSoilLayerClayFlowErodibility, &m_VdInputSoilLayerSiltFlowErodibility, &m_VdInputSoilLayerSandFlowErodibility, &m_VdInputSoilLayerClaySplashErodibility, &m_VdInputSoilLayerSiltSplashErodibility, &m_VdInputSoilLayerSandSplashErodibility, &m_VdInputSoilLayerClaySlumpErodibility, &m_VdInputSoilLayerSiltSlumpErodibility, &m_VdInputSoilLayerSandSlumpErodibility, &m_VdInfiltCPHWF, &m_VdInfiltChiPart);
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
   if (m_dPossMaxSpeedNextIter == 0)
   {
      // No flow occurred, so set the timestep for the next iteration based on a guessed-in value for flow speed
      m_dTimeStep = m_dm_CellSide / INIT_MAX_SPEED_GUESS;                             // In sec
   }
   else
   {
      // Some flow occurred: calculate the possible next timestep. Start by constraining the max possible flow speed
      m_dPossMaxSpeedNextIter = tMin(m_dPossMaxSpeedNextIter, m_dMaxFlowSpeed);

      // OK now calculate the possible timestep
      double dPossNextTimeStep = m_dm_CellSide / m_dPossMaxSpeedNextIter;             // In sec

      // Is the timestep increasing or decreasing?
      if (dPossNextTimeStep > m_dTimeStep)
      {
         // Timestep is increasing i.e. flow is slowing down
         double dTmp = m_dTimeStep / dPossNextTimeStep;
         if (dTmp > COURANT_ALPHA)
         {
            // The change in timestep is small
            m_dTimeStep = dPossNextTimeStep;
         }
         else
         {
            // The change in timestep is large, so we need to make a smaller change. This is equivalent to the 'Courant–Friedrichs–Lewy condition' i.e. the time needed for flow to cross a m_Cell at the maximum speed during the last iteration, plus an arbitrary safety margin. COURANT_ALPHA = delta_time / delta_distance See e.g. http://en.wikipedia.org/wiki/Courant%E2%80%93Friedrichs%E2%80%93Lewy_condition
            m_dTimeStep = m_dTimeStep / COURANT_ALPHA;
         }
      }
      else
      {
         // Timestep is decreasing i.e. flow is speeding up
         double dTmp = dPossNextTimeStep / m_dTimeStep;
         if (dTmp > COURANT_ALPHA)
         {
            // The change in timestep is small
            m_dTimeStep = dPossNextTimeStep;
         }
         else
         {
            // The change in timestep is large, so we need to make a smaller change. This is equivalent to the 'Courant–Friedrichs–Lewy condition' i.e. the time needed for flow to cross a m_Cell at the maximum speed during the last iteration, plus an arbitrary safety margin. COURANT_ALPHA = delta_time / delta_distance See e.g. http://en.wikipedia.org/wiki/Courant%E2%80%93Friedrichs%E2%80%93Lewy_condition
            m_dTimeStep = m_dTimeStep * COURANT_ALPHA;
         }
      }

      // Reset for the coming interation
      m_dPossMaxSpeedNextIter = 0;
   }
}

/*========================================================================================================================================

 Calculate and checks the mass balance of water and of sediment) for this iteration in order to remove rounding errors. These always accumulate when many small values are summed using finite-precision arithmetic (see e.g. "Floating-Point Summation" http://www.ddj.com/cpp/184403224#REF1). Even after a few iterations, mass-balance errors will start to appear here, unless a correction is made every iteration

========================================================================================================================================*/
void CSimulation::CheckMassBalance(void)
{
   // Check the this-iteration water balance: start by calculating the change in surface water since the last iteration. Calculate LHS = pre-iteration stored water + rain + run-on + exfiltration, RHS = infilt + outflow + end-of-iteration stored water
   double
      dTotLHS = m_dThisIterRain + m_dThisIterRunOn + m_dSinceLastExfiltration + m_dWaterStoredLast - m_dWaterErrorLast,
      dTotRHS = m_dSinceLastInfiltration + m_dThisIterLostSurfaceWater + m_dThisIterStoredSurfaceWater;

   if (! bFPIsEqual(dTotLHS, dTotRHS, WATER_TOLERANCE))
   {
#if defined _DEBUG
      m_ofsLog << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(6);

      m_ofsLog << m_ulIter << " " << WARN << "water balance, LHS = " << dTotLHS << " RHS = " << dTotRHS << endl;
      m_ofsLog << "\tLHS (input + previous storage -last error) - RHS (losses + storage) = " << dTotLHS - dTotRHS << endl;
      m_ofsLog << "\tm_dThisIterRain                 = " << m_dThisIterRain << endl;
      m_ofsLog << "\tm_dThisIterRunOn                = " << m_dThisIterRunOn << endl;
      m_ofsLog << "\tm_dWaterStoredLast               = " << m_dWaterStoredLast << endl;
      m_ofsLog << "\tm_dWaterErrorLast               = " << m_dWaterErrorLast << endl;
      m_ofsLog << "\t\tm_dSinceLastInfiltration         = " << m_dSinceLastInfiltration << endl;
      m_ofsLog << "\t\tm_dThisIterLostSurfaceWater      = " << m_dThisIterLostSurfaceWater << endl;
      m_ofsLog << "\t\tm_dThisIterStoredSurfaceWater    = " << m_dThisIterStoredSurfaceWater << endl;

      m_ofsLog << resetiosflags(ios::floatfield) << setiosflags(ios::scientific) << setprecision(6);
#endif
      m_dWaterErrorLast = dTotLHS - dTotRHS;
   }

   // Store for next iteration
   m_dWaterStoredLast = m_dThisIterStoredSurfaceWater;

   // Now check the various this-iteration sediment balances
   double
      dThisIterSplashDetach = 0,
      dThisIterSplashDeposit = 0;

   if (m_bSplash && m_bSplashThisIter)
   {
      // Check splash mass balance
      dThisIterSplashDetach = m_dThisIterClaySplashDetach + m_dThisIterSiltSplashDetach + m_dThisIterSandSplashDetach;
      dThisIterSplashDeposit = m_dThisIterClaySplashDeposit + m_dThisIterSiltSplashDeposit + m_dThisIterSandSplashDeposit;
      double dThisIterSplashToSedLoad = m_dThisIterClaySplashToSedLoad + m_dThisIterSiltSplashToSedLoad + m_dThisIterSandSplashToSedLoad;

      dTotLHS = dThisIterSplashDetach - m_dSplashErrorLast;
      dTotRHS = dThisIterSplashDeposit + dThisIterSplashToSedLoad;

      if (! bFPIsEqual(dTotLHS, dTotRHS, SEDIMENT_TOLERANCE))
      {
#if defined _DEBUG
         m_ofsLog << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(6);

         m_ofsLog << m_ulIter << " " << WARN << "splash sediment balance, LHS = " << dTotLHS << " RHS = " << dTotRHS << endl;
         m_ofsLog << "\tLHS (detach - last error) - RHS (deposition + to flow) = " << dTotLHS - dTotRHS << endl;
         m_ofsLog << "\tdThisIterSplashDetach           = " << dThisIterSplashDetach << endl;
         m_ofsLog << "\tm_dSplashErrorLast              = " << m_dSplashErrorLast << endl;
         m_ofsLog << "\t\tdThisIterSplashDeposit           = " << dThisIterSplashDeposit << endl;
         m_ofsLog << "\t\tdThisIterSplashToSedLoad         = " << dThisIterSplashToSedLoad << endl;

         m_ofsLog << resetiosflags(ios::floatfield) << setiosflags(ios::scientific) << setprecision(6);
#endif
         m_dSplashErrorLast = dTotLHS - dTotRHS;
      }
   }

   double
      dThisIterSlumpDetach = 0,
      dThisIterSlumpDeposit = 0,
      dThisIterToppleDetach = 0,
      dThisIterToppleDeposit = 0;

   if (m_bSlumping && m_bSlumpThisIter)
   {
      // Check the slumping mass balance
      dThisIterSlumpDetach = m_dSinceLastClaySlumpDetach + m_dSinceLastSiltSlumpDetach  + m_dSinceLastSandSlumpDetach;
      dThisIterSlumpDeposit = m_dSinceLastClaySlumpDeposit + m_dSinceLastSiltSlumpDeposit  + m_dSinceLastSandSlumpDeposit;
      double dThisIterSlumpToSedLoad = m_dSinceLastClaySlumpToSedLoad + m_dSinceLastSiltSlumpToSedLoad  + m_dSinceLastSandSlumpToSedLoad;

      dTotLHS = dThisIterSlumpDetach - m_dSlumpErrorLast,
      dTotRHS = dThisIterSlumpDeposit + dThisIterSlumpToSedLoad;

      if (! bFPIsEqual(dTotLHS, dTotRHS, SEDIMENT_TOLERANCE))
      {
#if defined _DEBUG
         m_ofsLog << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(6);

         m_ofsLog << m_ulIter << " " << WARN << "slump sediment balance, LHS = " << dTotLHS << " RHS = " << dTotRHS << endl;
         m_ofsLog << "\tLHS (detach - last error) - RHS (deposition + to flow) = " << dTotLHS - dTotRHS << endl;
         m_ofsLog << "\tdThisIterSlumpDetach            = " << dThisIterSlumpDetach << endl;
         m_ofsLog << "\tm_dSlumpErrorLast               = " << m_dSlumpErrorLast << endl;
         m_ofsLog << "\t\tdThisIterSlumpDeposit            = " << dThisIterSlumpDeposit << endl;
         m_ofsLog << "\t\tdThisIterSlumpToSedLoad          = " << dThisIterSlumpToSedLoad << endl;

         m_ofsLog << resetiosflags(ios::floatfield) << setiosflags(ios::scientific) << setprecision(6);
#endif
         m_dSlumpErrorLast = dTotLHS - dTotRHS;
      }

      // Now check the toppling mass balance
      dThisIterToppleDetach = m_dSinceLastClayToppleDetach + m_dSinceLastSiltToppleDetach  + m_dSinceLastSandToppleDetach;
      dThisIterToppleDeposit = m_dSinceLastClayToppleDeposit + m_dSinceLastSiltToppleDeposit  + m_dSinceLastSandToppleDeposit;
      double dThisIterToppleToSedLoad = m_dSinceLastClayToppleToSedLoad + m_dSinceLastSiltToppleToSedLoad  + m_dSinceLastSandToppleToSedLoad;

      dTotLHS = dThisIterToppleDetach - m_dToppleErrorLast,
      dTotRHS = dThisIterToppleDeposit + dThisIterToppleToSedLoad;

      if (! bFPIsEqual(dTotLHS, dTotRHS, SEDIMENT_TOLERANCE))
      {
#if defined _DEBUG
         m_ofsLog << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(6);

         m_ofsLog << m_ulIter << " " << WARN << "topple sediment balance, LHS = " << dTotLHS << " RHS = " << dTotRHS << endl;
         m_ofsLog << "\tLHS (detach - last error) - RHS (deposition + to flow) = " << dTotLHS - dTotRHS << endl;
         m_ofsLog << "\tdThisIterToppleDetach           = " << dThisIterToppleDetach << endl;
         m_ofsLog << "\tm_dToppleErrorLast             = " << m_dToppleErrorLast << endl;
         m_ofsLog << "\t\tdThisIterToppleDeposit           = " << dThisIterToppleDeposit << endl;
         m_ofsLog << "\t\tdThisIterToppleToSedLoad         = " << dThisIterToppleToSedLoad << endl;

         m_ofsLog << resetiosflags(ios::floatfield) << setiosflags(ios::scientific) << setprecision(6);
#endif
         m_dToppleErrorLast = dTotLHS - dTotRHS;
      }
   }

   double
      dThisIterHeadcutRetreatDetach = 0,
      dThisIterHeadcutRetreatDeposit = 0;

   if (m_bHeadcutRetreat && m_bHeadcutRetreatThisIter)
   {
      // Check the headcut retreat mass balance
      dThisIterHeadcutRetreatDetach = m_dSinceLastClayHeadcutDetach + m_dSinceLastSiltHeadcutDetach  + m_dSinceLastSandHeadcutDetach;
      dThisIterHeadcutRetreatDeposit = m_dSinceLastClayHeadcutDeposit + m_dSinceLastSiltHeadcutDeposit  + m_dSinceLastSandHeadcutDeposit;
      double dThisIterHeadcutRetreatToSedLoad = m_dSinceLastClayHeadcutToSedLoad + m_dSinceLastSiltHeadcutToSedLoad  + m_dSinceLastSandHeadcutToSedLoad;

      dTotLHS = dThisIterHeadcutRetreatDetach - m_dHeadcutErrorLast,
      dTotRHS = dThisIterHeadcutRetreatDeposit + dThisIterHeadcutRetreatToSedLoad;

      if (! bFPIsEqual(dTotLHS, dTotRHS, SEDIMENT_TOLERANCE))
      {
   #if defined _DEBUG
         m_ofsLog << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(6);

         m_ofsLog << m_ulIter << " " << WARN << "headcut retreat sediment balance, LHS = " << dTotLHS << " RHS = " << dTotRHS << endl;
         m_ofsLog << "\tLHS (detach - last error) - RHS (deposition + to flow) = " << dTotLHS - dTotRHS << endl;
         m_ofsLog << "\tdThisIterHeadcutRetreatDetach   = " << dThisIterHeadcutRetreatDetach << endl;
         m_ofsLog << "\tm_dHeadcutErrorLast            = " << m_dHeadcutErrorLast << endl;
         m_ofsLog << "\t\tdThisIterHeadcutRetreatDeposit   = " << dThisIterHeadcutRetreatDeposit << endl;
         m_ofsLog << "\t\tdThisIterHeadcutRetreatToSedLoad = " << dThisIterHeadcutRetreatToSedLoad << endl;

         m_ofsLog << resetiosflags(ios::floatfield) << setiosflags(ios::scientific) << setprecision(6);
   #endif
         m_dHeadcutErrorLast = dTotLHS - dTotRHS;
      }
   }

   // Check the flow mass balance. LHS = pre-iteration sediment load + flow detachment + splash detachment + slump detachment + topple detachment, RHS = flow deposition + splash deposition + slump deposition + topple deposition + infiltration deposition + sediment lost + end-of-iteration sediment load
   double
      dTotSedLoad = m_dThisIterClaySedLoad + m_dThisIterSiltSedLoad + m_dThisIterSandSedLoad,
      dThisIterFlowDetach = m_dThisIterClayFlowDetach + m_dThisIterSiltFlowDetach + m_dThisIterSandFlowDetach,
      dThisIterFlowDeposit = m_dThisIterClayFlowDeposit + m_dThisIterSiltFlowDeposit + m_dThisIterSandFlowDeposit,
      dThisIterInfiltDeposit = m_dSinceLastClayInfiltDeposit + m_dSinceLastSiltInfiltDeposit + m_dSinceLastSandInfiltDeposit,
      dThisIterSedLost = m_dThisIterClaySedLost + m_dThisIterSiltSedLost + m_dThisIterSandSedLost;

   dTotLHS = m_dSedimentLoadDepthLast + dThisIterFlowDetach + dThisIterSplashDetach + dThisIterSlumpDetach + dThisIterToppleDetach + dThisIterHeadcutRetreatDetach -m_dFlowErrorLast,
   dTotRHS = dThisIterFlowDeposit + dThisIterSplashDeposit + dThisIterSlumpDeposit + dThisIterToppleDeposit + dThisIterInfiltDeposit + dThisIterHeadcutRetreatDeposit + dThisIterSedLost + dTotSedLoad;

   if (! bFPIsEqual(dTotLHS, dTotRHS, SEDIMENT_TOLERANCE))
   {
#if defined _DEBUG
      m_ofsLog << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(6);

      m_ofsLog << m_ulIter << " " << WARN << "sediment balance, LHS = " << dTotLHS << " RHS = " << dTotRHS << endl;
      m_ofsLog << "\tLHS (last sedload + detach - last error) - RHS (deposition + lost + this sedload) = " << dTotLHS - dTotRHS << endl;

      m_ofsLog << "\tdThisIterFlowDetach             = " << dThisIterFlowDetach << endl;
      m_ofsLog << "\tdThisIterSplashDetach           = " << dThisIterSplashDetach << endl;
      m_ofsLog << "\tdThisIterSlumpDetach            = " << dThisIterSlumpDetach << endl;
      m_ofsLog << "\tdThisIterToppleDetach           = " << dThisIterToppleDetach << endl;
      m_ofsLog << "\tdThisIterHeadcutRetreatDetach   = " << dThisIterHeadcutRetreatDetach << endl;
      m_ofsLog << "\tm_dSedimentLoadDepthLast         = " << m_dSedimentLoadDepthLast << endl;
      m_ofsLog << "\tm_dFlowErrorLast                = " << m_dFlowErrorLast << endl;
      m_ofsLog << "\t\tdThisIterFlowDeposit             = " << dThisIterFlowDeposit << endl;
      m_ofsLog << "\t\tdThisIterSplashDeposit           = " << dThisIterSplashDeposit << endl;
      m_ofsLog << "\t\tdThisIterSlumpDeposit            = " << dThisIterSlumpDeposit << endl;
      m_ofsLog << "\t\tdThisIterToppleDeposit           = " << dThisIterToppleDeposit << endl;
      m_ofsLog << "\t\tdThisIterInfiltDeposit           = " << dThisIterInfiltDeposit << endl;
      m_ofsLog << "\t\tdThisIterHeadcutRetreatDeposit   = " << dThisIterHeadcutRetreatDeposit << endl;
      m_ofsLog << "\t\tdThisIterSedLost                 = " << dThisIterSedLost << endl;
      m_ofsLog << "\t\tdTotSedLoad                      = " << dTotSedLoad << endl;

      m_ofsLog << resetiosflags(ios::floatfield) << setiosflags(ios::scientific) << setprecision(6);
#endif
      m_dFlowErrorLast = dTotLHS - dTotRHS;
   }

   // Store for next iteration
   m_dSedimentLoadDepthLast = dTotSedLoad;
}


/*========================================================================================================================================

 Checks to see if the simulation has gone on too long, amongst other things

========================================================================================================================================*/
bool CSimulation::bIsTimeToQuit(void)
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
   m_ldGTotRain        += (m_dThisIterRain * m_dm_CellSquare);
   m_ldGTotRunOn  += (m_dThisIterRunOn * m_dm_CellSquare);
   m_ldGTotWaterLost   += (m_dThisIterLostSurfaceWater * m_dm_CellSquare);
   m_ldGTotFlowDetach  += (m_dThisIterClayFlowDetach + m_dThisIterSiltFlowDetach + m_dThisIterSandFlowDetach) * m_dm_CellSquare;

   // Note that ldGTotFlowDeposit will be an over-estimate of total deposition since there is no allowance made for deposited sediment that is subsequently re-entrained. This is taken care of in the final mass balance calculation
   m_ldGTotFlowDeposit += (m_dThisIterClayFlowDeposit + m_dThisIterSiltFlowDeposit + m_dThisIterSandFlowDeposit) * m_dm_CellSquare;
   m_ldGTotSedLost     += (m_dThisIterClaySedLost + m_dThisIterSiltSedLost + m_dThisIterSandSedLost) * m_dm_CellSquare;

   if (m_bInfiltThisIter)
   {
      m_ldGTotInfilt        += (m_dSinceLastInfiltration * m_dm_CellSquare);
      m_ldGTotExfilt        += (m_dSinceLastExfiltration * m_dm_CellSquare);
      m_ldGTotInfiltDeposit += (m_dSinceLastClayInfiltDeposit + m_dSinceLastSiltInfiltDeposit + m_dSinceLastSandInfiltDeposit) * m_dm_CellSquare;
   }

   if (m_bSplashThisIter)
   {
      m_ldGTotSplashDetach  += (m_dThisIterClaySplashDetach + m_dThisIterSiltSplashDetach + m_dThisIterSandSplashDetach) * m_dm_CellSquare;
      m_ldGTotSplashDeposit += (m_dThisIterClaySplashDeposit + m_dThisIterSiltSplashDeposit + m_dThisIterSandSplashDeposit) * m_dm_CellSquare;
      m_ldGTotSplashToSedLoad += (m_dThisIterClaySplashToSedLoad + m_dThisIterSiltSplashToSedLoad + m_dThisIterSandSplashToSedLoad) * m_dm_CellSquare;
   }

   if (m_bSlumpThisIter)
   {
      m_ldGTotSlumpDetach   += (m_dSinceLastClaySlumpDetach + m_dSinceLastSiltSlumpDetach + m_dSinceLastSandSlumpDetach) * m_dm_CellSquare;
      m_ldGTotSlumpDeposit   += (m_dSinceLastClaySlumpDeposit + m_dSinceLastSiltSlumpDeposit + m_dSinceLastSandSlumpDeposit) * m_dm_CellSquare;
      m_ldGTotSlumpToSedLoad   += (m_dSinceLastClaySlumpToSedLoad + m_dSinceLastSiltSlumpToSedLoad + m_dSinceLastSandSlumpToSedLoad) * m_dm_CellSquare;

      m_ldGTotToppleDetach  += (m_dSinceLastClayToppleDetach + m_dSinceLastSiltToppleDetach + m_dSinceLastSandToppleDetach) * m_dm_CellSquare;
      m_ldGTotToppleDeposit += (m_dSinceLastClayToppleDeposit + m_dSinceLastSiltToppleDeposit + m_dSinceLastSandToppleDeposit) * m_dm_CellSquare;
      m_ldGTotToppleToSedLoad += (m_dSinceLastClayToppleToSedLoad + m_dSinceLastSiltToppleToSedLoad + m_dSinceLastSandToppleToSedLoad) * m_dm_CellSquare;
   }

   if (m_bHeadcutRetreatThisIter)
   {
      m_ldGTotHeadcutRetreatDetach  += (m_dSinceLastClayHeadcutDetach + m_dSinceLastSiltHeadcutDetach + m_dSinceLastSandHeadcutDetach) * m_dm_CellSquare;
      m_ldGTotHeadcutRetreatDeposit += (m_dSinceLastClayHeadcutDeposit + m_dSinceLastSiltHeadcutDeposit + m_dSinceLastSandHeadcutDeposit) * m_dm_CellSquare;
      m_ldGTotHeadcutRetreatToSedLoad += (m_dSinceLastClayHeadcutToSedLoad + m_dSinceLastSiltHeadcutToSedLoad + m_dSinceLastSandHeadcutToSedLoad) * m_dm_CellSquare;
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
   if (m_bInitElevSave && (! bWriteGISFileFloat(GIS_ELEVATION, &GIS_ELEVATION_TITLE)))
      return (RTN_ERR_GISFILEWRITE);

   // If requested, write out the rainfall variation multiplier file
   if ((m_bRainVarMSave) && (! bWriteGISFileFloat(GIS_RAIN_SPATIAL_VARIATION, &GIS_RAIN_SPATIAL_VARIATION_TITLE)))
      return (RTN_ERR_GISFILEWRITE);

   // ========================================================== The main loop ===========================================================
   while (true)
   {
      // Calculate the timestep for this iteration
      CalcTimestep();

      // Check that we haven't gone on too long
      if (bIsTimeToQuit())
         break;

      // Tell the user how the simulation is progressing
      AnnounceProgress();

      // If we are simulating infilt then do some more initialization
      if (m_bDoInfiltration)
      {
         for (int nLayer = 0; nLayer < m_nNumSoilLayers; nLayer++)
            m_VdThisIterSoilWater[nLayer] = 0;
      }

      // OK, start simulating for this iteration. First initialize the rainfall intensity for this iteration
      m_bThisIterRainChange = bSetUpRainfallIntensity();

      // If it is still raining, then simulate rainfall, and maybe run-on
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

      // Initialise ready for flow routing
      m_ulNumHead = 0;
      m_dThisIterClaySedLost =
      m_dThisIterSiltSedLost =
      m_dThisIterSandSedLost =
      m_dThisIterTotHead = 0;

      // Route all flow from wet m_Cells
      DoAllFlowRouting();

      // When representing flow off an edge of the grid, we need a value for the off-edge head. Save this iteration's maximum and minimum on-grid values of head for this
      if (m_ulNumHead > 0)
         m_dLastIterAvgHead = m_dThisIterTotHead / static_cast<double>(m_ulNumHead);

      // Infiltration next
      m_bInfiltThisIter = false;
      m_dSinceLastInfiltration =
      m_dSinceLastExfiltration =
      m_dSinceLastClayInfiltDeposit =
      m_dSinceLastSiltInfiltDeposit =
      m_dSinceLastSandInfiltDeposit = 0;

      if (m_bDoInfiltration && (CALC_INFILT_INTERVAL-1 == ++m_nInfiltCount))                 // If we are considering infilt, simulate it this iteration?
      {
         // Yup, simulate infilt from the cell array
         m_nInfiltCount = 0;
         m_bInfiltThisIter = true;

         DoAllInfiltration();
      }

      // Next, splash redistribution. If we are considering splash redistribution, then do it this iteration, provided it is still raining
      // Note: tried calculating splash only on a subset of interations, but caused problems e.g. splash rate depended too much on how often splash was calculated; and leaving splash calcs too long meant that big splash changes could occur, which is unrealistic)
      m_bSplashThisIter = false;
      m_dThisIterKE = 0;

      if (m_bSplash && (m_dRainIntensity > 0))
      {
         // Yes, simulating splash redistribution
         m_bSplashThisIter = true;

         // Now simulate the splash redistribution for this iteration
         DoAllSplash();

         // And reset the counter for next time
         m_dSplashCalcLast = m_dSimulatedTimeElapsed;
      }

      // Next, rill-wall slumping and toppling
      m_bSlumpThisIter = false;

      // Initialize 'since last' values for slumping and toppling
      m_dSinceLastClaySlumpDetach =
      m_dSinceLastSiltSlumpDetach =
      m_dSinceLastSandSlumpDetach =
      m_dSinceLastClaySlumpDeposit =
      m_dSinceLastSiltSlumpDeposit =
      m_dSinceLastSandSlumpDeposit =
      m_dSinceLastClaySlumpToSedLoad =
      m_dSinceLastSiltSlumpToSedLoad =
      m_dSinceLastSandSlumpToSedLoad =
      m_dSinceLastClayToppleDetach =
      m_dSinceLastSiltToppleDetach =
      m_dSinceLastSandToppleDetach =
      m_dSinceLastClayToppleDeposit =
      m_dSinceLastSiltToppleDeposit =
      m_dSinceLastSandToppleDeposit =
      m_dSinceLastClayToppleToSedLoad =
      m_dSinceLastSiltToppleToSedLoad =
      m_dSinceLastSandToppleToSedLoad = 0;

      // If we are considering slumping, simulate it this iteration?
      if (m_bSlumping && (CALC_SLUMP_INTERVAL-1 == ++m_nSlumpCount))
      {
         // Yup, simulate slumping and toppling
         m_nSlumpCount = 0;
         m_bSlumpThisIter = true;
         DoAllSlump();

         // Reset for next time
         m_dLastSlumpCalcTime = m_dSimulatedTimeElapsed;
      }

      // Finally, headcut retreat
      m_bHeadcutRetreatThisIter = false;

      // Initialize 'since last' values for headcut retreat
      m_dSinceLastClayHeadcutDetach =
      m_dSinceLastSiltHeadcutDetach =
      m_dSinceLastSandHeadcutDetach =
      m_dSinceLastClayHeadcutDeposit =
      m_dSinceLastSiltHeadcutDeposit =
      m_dSinceLastSandHeadcutDeposit =
      m_dSinceLastClayHeadcutToSedLoad =
      m_dSinceLastSiltHeadcutToSedLoad =
      m_dSinceLastSandHeadcutToSedLoad = 0;

      // If we are considering headcut retreat, simulate it this iteration?
      if (m_bHeadcutRetreat && (CALC_HEADCUT_RETREAT_INTERVAL-1 == ++m_nHeadcutRetreatCount))
      {
         // Yup, simulate headcut retreat
         m_nHeadcutRetreatCount = 0;
         m_bHeadcutRetreatThisIter = true;
         DoAllHeadcutRetreat();

         // Reset for next time
         m_dLastHeadcutRetreatCalcTime = m_dSimulatedTimeElapsed;
      }

      // Calc end-of-iteration totals then initialize some of the cell array ready for the next iteration (note: m_dThisIterKE is calculated earlier, as are m_dThisIterClaySedLost, m_dThisIterSiltSedLost, m_dThisIterSandSedLost, m_dThisIterClaySplashToSedLoad, m_dThisIterSiltSplashToSedLoad,  m_dThisIterSandSplashToSedLoad)
      m_ulNWet = 0;
      m_dThisIterRain                  =
      m_dThisIterRunOn                 =
      m_dThisIterStoredSurfaceWater    =
      m_dThisIterLostSurfaceWater      =
      m_dThisIterClaySedLoad           =
      m_dThisIterSiltSedLoad           =
      m_dThisIterSandSedLoad           =
      m_dThisIterClayFlowDetach        =
      m_dThisIterSiltFlowDetach        =
      m_dThisIterSandFlowDetach        =
      m_dThisIterClayFlowDeposit       =
      m_dThisIterSiltFlowDeposit       =
      m_dThisIterSandFlowDeposit       =
      m_dThisIterClaySplashDetach      =
      m_dThisIterSiltSplashDetach      =
      m_dThisIterSandSplashDetach      =
      m_dThisIterClaySplashDeposit     =
      m_dThisIterSiltSplashDeposit     =
      m_dThisIterSandSplashDeposit     =
      m_dThisIterClaySplashToSedLoad   =
      m_dThisIterSiltSplashToSedLoad   =
      m_dThisIterSandSplashToSedLoad   = 0;

      for (int nX = 0; nX < m_nXGridMax; nX++)
      {
         for (int nY = 0; nY < m_nYGridMax; nY++)
         {
            bool bIsWet = false;
            double
               dRain = 0,
               dRunOn = 0,
               dSurfaceWaterDepth = 0,
               dSurfaceWaterLost = 0,
               dClaySedLoad = 0,
               dSiltSedLoad = 0,
               dSandSedLoad = 0,
               dClayFlowDetach = 0,
               dSiltFlowDetach = 0,
               dSandFlowDetach = 0,
               dClayFlowDeposit = 0,
               dSiltFlowDeposit = 0,
               dSandFlowDeposit = 0,
               dClaySplashDetach = 0,
               dSiltSplashDetach = 0,
               dSandSplashDetach = 0,
               dClaySplashDeposit = 0,
               dSiltSplashDeposit = 0,
               dSandSplashDeposit = 0,
               dClaySplashToSedLoad = 0,
               dSiltSplashToSedLoad = 0,
               dSandSplashToSedLoad = 0;

            m_Cell[nX][nY].CalcIterTotalsAndInit(bIsWet, dRain, dRunOn, dSurfaceWaterDepth, dSurfaceWaterLost, dClaySedLoad, dSiltSedLoad, dSandSedLoad, dClayFlowDetach, dSiltFlowDetach, dSandFlowDetach, dClayFlowDeposit, dSiltFlowDeposit, dSandFlowDeposit, dClaySplashDetach, dSiltSplashDetach, dSandSplashDetach, dClaySplashDeposit, dSiltSplashDeposit, dSandSplashDeposit, dClaySplashToSedLoad, dSiltSplashToSedLoad, dSandSplashToSedLoad, m_bSlumpThisIter);

            if (m_dRainIntensity > 0)
            {
               m_dThisIterRain += dRain;

               if (m_bRunOn)
                  m_dThisIterRunOn += dRunOn;
            }

            if (bIsWet)
            {
               m_ulNWet++;

               m_dThisIterStoredSurfaceWater += dSurfaceWaterDepth;

               m_dThisIterClaySedLoad += dClaySedLoad;
               m_dThisIterSiltSedLoad += dSiltSedLoad;
               m_dThisIterSandSedLoad += dSandSedLoad;
            }

            m_dThisIterLostSurfaceWater += dSurfaceWaterLost;

            m_dThisIterClayFlowDetach += dClayFlowDetach;
            m_dThisIterSiltFlowDetach += dSiltFlowDetach;
            m_dThisIterSandFlowDetach += dSandFlowDetach;
            m_dThisIterClayFlowDeposit += dClayFlowDeposit;
            m_dThisIterSiltFlowDeposit += dSiltFlowDeposit;
            m_dThisIterSandFlowDeposit += dSandFlowDeposit;

            if (m_bSplash && m_bSplashThisIter)
            {
               m_dThisIterClaySplashDetach += dClaySplashDetach;
               m_dThisIterSiltSplashDetach += dSiltSplashDetach;
               m_dThisIterSandSplashDetach += dSandSplashDetach;

               m_dThisIterClaySplashDeposit += dClaySplashDeposit;
               m_dThisIterSiltSplashDeposit += dSiltSplashDeposit;
               m_dThisIterSandSplashDeposit += dSandSplashDeposit;

               m_dThisIterClaySplashToSedLoad += dClaySplashToSedLoad;
               m_dThisIterSiltSplashToSedLoad += dSiltSplashToSedLoad;
               m_dThisIterSandSplashToSedLoad += dSandSplashToSedLoad;
            }
         }
      }

      // Now save results and do per-iteration book-keeping. First see if we need to save the GIS files now
      m_bSaveGISThisIter = false;
      if ((m_bSaveRegular && (m_dSimulatedTimeElapsed >= m_dRSaveTime) && (m_dSimulatedTimeElapsed < m_dSimulationDuration)) || (! m_bSaveRegular && (m_dSimulatedTimeElapsed >= m_VdSaveTime[m_nThisSave])))
      {
         // Yes, save the values from the cell array into GIS files
         m_bSaveGISThisIter = true;
         if (! bSaveGISFiles())
            return (RTN_ERR_GISFILEWRITE);
      }

      // Calculate and check this-iteration hydrology and sediment balance
      CheckMassBalance();

      // Output per-iteration results to the .out file
      if (! bWritePerIterationResults())
         return (RTN_ERR_TEXTFILEWRITE);

      // Update totals for time series output which are not output every iteration
      m_dSinceLastTSWriteInfiltration      += m_dSinceLastInfiltration;
      m_dSinceLastTSWriteExfiltration      += m_dSinceLastExfiltration;
      m_dSinceLastTSWriteClayInfiltDeposit += m_dSinceLastClayInfiltDeposit;
      m_dSinceLastTSWriteSiltInfiltDeposit += m_dSinceLastSiltInfiltDeposit;
      m_dSinceLastTSWriteSandInfiltDeposit += m_dSinceLastSandInfiltDeposit;
      m_dSinceLastTSWriteClaySplashRedist  += m_dThisIterClaySplashDetach;
      m_dSinceLastTSWriteSiltSplashRedist  += m_dThisIterSiltSplashDetach;
      m_dSinceLastTSWriteSandSplashRedist  += m_dThisIterSandSplashDetach;
      m_dSinceLastTSWriteKE                += m_dThisIterKE;
      m_dSinceLastTSWriteClaySlumpDetach   += m_dSinceLastClaySlumpDetach;
      m_dSinceLastTSWriteSiltSlumpDetach   += m_dSinceLastSiltSlumpDetach;
      m_dSinceLastTSWriteSandSlumpDetach   += m_dSinceLastSandSlumpDetach;
      m_dSinceLastTSWriteClayToppleDetach  += m_dSinceLastClayToppleDetach;
      m_dSinceLastTSWriteSiltToppleDetach  += m_dSinceLastSiltToppleDetach;
      m_dSinceLastTSWriteSandToppleDetach  += m_dSinceLastSandToppleDetach;

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

      // If there has been any noticeable loss of sediment from any edge this iteration, then adjust the elevation of the unbounded edge(s), based on the cell values: ugly but necessary
      int nOpenEdgeLength = 0;
      if (! m_bClosedThisEdge[EDGE_BOTTOM])
         nOpenEdgeLength += m_nXGridMax;
      if (! m_bClosedThisEdge[EDGE_TOP])
         nOpenEdgeLength += m_nXGridMax;
      if (! m_bClosedThisEdge[EDGE_LEFT])
         nOpenEdgeLength += m_nYGridMax;
      if (! m_bClosedThisEdge[EDGE_RIGHT])
         nOpenEdgeLength += m_nYGridMax;

      // If this is a flume-like situation, where the edges can be eroded -- and if we had enough sediment lost from the edge(s) during the last iteration -- then try to lower the unbounded edges. Ugly but necessary
      if (m_bFlumeTypeSim && ((m_dThisIterClaySedLost + m_dThisIterSiltSedLost + m_dThisIterSandSedLost) / nOpenEdgeLength > 0))
         AdjustUnboundedEdges();

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

 Publicly visible, returns the current timestep

==============================================================================================================================*/
double CSimulation::dGetTimeStep(void) const
{
   return m_dTimeStep;
}

/*==============================================================================================================================

 Publicly-visible wrapper around dGetRand0Gaussian(), needed for m_Cell surface water initialisation

==============================================================================================================================*/
double CSimulation::dGetRandGaussian(void)
{
   return dGetRand0Gaussian();
}
