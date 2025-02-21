/*========================================================================================================================================

This is simulation.cpp: the start-of-simulation routine for RillGrow

Copyright (C) 2025 David Favis-Mortlock

=========================================================================================================================================

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

========================================================================================================================================*/
#include "rg.h"
#include "simulation.h"
#include "2d_vec.h"
#include "cell.h"

//=========================================================================================================================================
//! The CSimulation constructor
//=========================================================================================================================================
CSimulation::CSimulation(void)
{
   // initialization
   m_bFlowDetachSave          = false;
   m_bRainVarMSave            = false;
   m_bCumulRunOnSave          = false;
   m_bElevSave                = false;
   m_bInitElevSave            = false;
   m_bDetrendElevSave         = false;
   m_bInfiltSave              = false;
   m_bCumulInfiltSave         = false;
   m_bSoilWaterSave           = false;
   m_bInfiltDepositSave       = false;
   m_bCumulInfiltDepositSave  = false;
   m_bTopSurfaceSave          = false;
   m_bSplashSave              = false;
   m_bCumulSplashSave         = false;
   m_bInundationSave          = false;
   m_bFlowDirSave             = false;
   m_bStreamPowerSave         = false;
   m_bShearStressSave         = false;
   m_bFrictionFactorSave      = false;
   m_bCumulAvgShearStressSave = false;
   m_bReynoldsSave            = false;
   m_bFroudeSave              = false;
   m_bCumulAvgDepthSave       = false;
   m_bCumulAvgDWSpdSave       = false;
   m_bCumulAvgSpdSave         = false;
   m_bSedConcSave             = false;
   m_bSedLoadSave             = false;
   m_bAvgSedLoadSave          = false;
   m_bCumulFlowDepositSave    = false;
   m_bCumulLoweringSave       = false;
   m_bTCSave                  = false;
   m_bSlumpSave               = false;
   m_bToppleSave              = false;
   m_bSaveRegular             = false;
   m_bDetrend                 = false;
   m_bFFCheck                 = false;
   m_bSplash                  = false;
   m_bPoesenSplashEqn         = false;
   m_bPlanchonSplashEqn       = false;
   m_bSplashThisIter          = false;
   m_bSplashCheck             = false;
   m_bDoInfiltration          = false;
   m_bInfiltThisIter          = false;
   m_bFlowErosion             = false;
   m_bRunOn                   = false;
   m_bTimeVaryingRain         = false;
   m_bGISOutDiffers           = false;
   m_bSlumping                = false;
   m_bSlumpThisIter           = false;
   m_bHeadcutRetreat          = false;
   m_bHeadcutRetreatThisIter  = false;
   m_bTimeStepTS              = false;
   m_bAreaWetTS               = false;
   m_bRainTS                  = false;
   m_bInfiltTS                = false;
   m_bExfiltTS                = false;
   m_bRunOnTS                 = false;
   m_bSurfaceWaterTS          = false;
   m_bSurfaceWaterLostTS      = false;
   m_bFlowDetachTS            = false;
   m_bSlumpDetachTS           = false;
   m_bToppleDetachTS          = false;
   m_bSplashKETS              = false;
   m_bSplashRedistTS          = false;
   m_bInfiltDepositTS         = false;
   m_bSedLoadTS               = false;
   m_bSedOffEdgeTS           = false;
   m_bDoSedLoadDepositTS      = false;
   m_bSoilWaterTS             = false;
   m_bSaveGISThisIter         = false;
   m_bThisIterRainChange      = false;
   m_bHaveBaseLevel           = false;
   m_bOutDEMsUsingInputZUnits = false;
   m_bManningFlowSpeedEqn              = false;
   m_bDarcyWeisbachFlowSpeedEqn        = false;
   m_bFrictionFactorConstant  = false;
   m_bFrictionFactorReynolds  = false;
   m_bFrictionFactorLawrence  = false;
   m_bFrictionFactorCheng     = false;
   m_bLostSave                = false;
   m_bSplashForward           = false;
   m_bSettlingEqnCheng        = false;
   m_bSettlingEqnFergusonChurch = false;
   m_bSettlingEqnStokesBudryckRittinger = false;

   for (int n = 0; n < 4; n++)
   {
      m_bClosedThisEdge[n] = false;
      m_bRunOnThisEdge[n] = false;
   }

   m_nGISSave                 = 0;
   m_nUSave                   = 0;
   m_nThisSave                = 0;
   m_nXGridMax                = 0;
   m_nYGridMax                = 0;
   m_nHeaderSize              = 0;
   m_nRainChangeTimeMax       = 0;
   m_nNumSoilLayers           = 0;
   m_nSSSQuadrantSize         = 0;
   m_nTimeVaryingRainCount    = 0;
   m_nInfiltCount             = 0;
   m_nSlumpCount              = 0;
   m_nHeadcutRetreatCount     = 0;
   m_nZUnits                  = Z_UNIT_NONE;

   m_ulIter                   = 0;
   m_ulTotIter                = 0;
   m_ulVarChkStart            = 0;
   m_ulVarChkEnd              = 0;
   m_ulNActiveCells           = 0;
   m_ulNWet                   = 0;
   m_ulMissingValueCells      = 0;
   m_ulNumHead                = 0;

   for (int n = 0; n < NUMBER_OF_RNGS; n++)
      m_ulRandSeed[n] = 0;

   m_dMinX                          = 0;
   m_dMaxX                          = 0;
   m_dMinY                          = 0;
   m_dMaxY                          = 0;
   m_dMaxFlowSpeed                  = 0;
   m_dPossMaxSpeedNextIter          = 0;
   m_dBasementElevation             = 0;
   m_dAvgElev                       = 0;
   m_dMinElev                       = 0;
   m_dGradient                      = 0;
   m_dPlotEndDiff                   = 0;
   m_dPlotElevMin                   = 0;
   m_dPlotElevMax                   = 0;
   m_dBaseLevel                     = 0;
   m_dInvCos45                      = 0;
   m_dYInc                          = 0;
   m_dRainIntensity                 = 0;
   m_dSpecifiedRainIntensity        = 0;
   m_dStdRainInt                    = 0;
   m_dDropDiameter                  = 0;
   m_dStdDropDiameter               = 0;
   m_dRainSpeed                     = 0;
   m_dPartKE                        = 0;
   m_dSplashConstant                = 0;
   m_dSplashConstantNormalized      = 0;
   m_dPoesenSplashConstant          = 0;
   m_dPlanchonCellSizeKC            = 0;
   m_dMeanCellWaterVol              = 0;
   m_dStdCellWaterVol               = 0;
   m_dCellSide                      = 0;
   m_dCellDiag                      = 0;
   m_dInvCellSide                   = 0;
   m_dInvCellDiag                   = 0;
   m_dCellSquare                    = 0;
   m_dInvCellSquare                 = 0;
   m_dInvXGridMax                   = 0;
   m_dRho                           = 0;
   m_dG                             = 0;
   m_dNu                            = 0;
   m_dK                             = 0;
   m_dT                             = 0;
   m_dCVT                           = 0;
   m_dCVTaub                        = 0;
   m_dST2                           = 0;
   m_dBulkDensityForOutputCalcs     = 0;
   m_dAlpha                         = 0;
   m_dBeta                          = 0;
   m_dGamma                         = 0;
   m_dDelta                         = 0;
   m_dRunOnLen                      = 0;
   m_dRunOnSpd                      = 0;
   m_dCritSSSForSlump               = 0;
   m_dSlumpAngleOfRest              = 0;
   m_dSlumpAngleOfRestDiff          = 0;
   m_dSlumpAngleOfRestDiffDiag      = 0;
   m_dToppleCriticalAngle           = 0;
   m_dToppleCritDiff                = 0;
   m_dToppleCritDiffDiag            = 0;
   m_dToppleAngleOfRest             = 0;
   m_dToppleAngleOfRestDiff         = 0;
   m_dToppleAngleOfRestDiffDiag     = 0;
   m_dEndOfIterTotSurfaceWater      = 0;
   m_dEndOfIterClaySedLoad          = 0;
   m_dEndOfIterClaySedLoadKahanCorrection = 0;
   m_dEndOfIterSiltSedLoad          = 0;
   m_dEndOfIterSiltSedLoadKahanCorrection = 0;
   m_dEndOfIterSandSedLoad          = 0;
   m_dEndOfIterSandSedLoadKahanCorrection = 0;
   m_dEndOfIterRain                 = 0;
   m_dEndOfIterRunOn                = 0;
   m_dEndOfIterKE                   = 0;
   m_dEndOfIterSurfaceWaterOffEdge  = 0;
   m_dEndOfIterClayFlowDetach       = 0;
   m_dEndOfIterSiltFlowDetach       = 0;
   m_dEndOfIterSandFlowDetach       = 0;
   m_dEndOfIterClayFlowDeposit      = 0;
   m_dEndOfIterSiltFlowDeposit      = 0;
   m_dEndOfIterSandFlowDeposit      = 0;
   m_dEndOfIterClaySedLoadOffEdge   = 0;
   m_dEndOfIterSiltSedLoadOffEdge   = 0;
   m_dEndOfIterSandSedLoadOffEdge   = 0;
   m_dEndOfIterClaySplashDetach     = 0;
   m_dEndOfIterSiltSplashDetach     = 0;
   m_dEndOfIterSandSplashDetach     = 0;
   m_dEndOfIterClaySplashDeposit    = 0;
   m_dEndOfIterSiltSplashDeposit    = 0;
   m_dEndOfIterSandSplashDeposit    = 0;
   m_dEndOfIterClaySplashToSedLoad  = 0;
   m_dEndOfIterSiltSplashToSedLoad  = 0;
   m_dEndOfIterSandSplashToSedLoad  = 0;
   m_dEndOfIterClaySplashOffEdge    = 0;
   m_dEndOfIterSiltSplashOffEdge    = 0;
   m_dEndOfIterSandSplashOffEdge    = 0;
   m_dEndOfIterNetClayDetachment    = 0;
   m_dEndOfIterNetSiltDetachment    = 0;
   m_dEndOfIterNetSandDetachment    = 0;
   m_dEndOfIterClaySlumpDetach      = 0;
   m_dEndOfIterSiltSlumpDetach      = 0;
   m_dEndOfIterSandSlumpDetach      = 0;
   m_dEndOfIterClaySlumpDeposit     = 0;
   m_dEndOfIterSiltSlumpDeposit     = 0;
   m_dEndOfIterSandSlumpDeposit     = 0;
   m_dEndOfIterClaySlumpToSedLoad   = 0;
   m_dEndOfIterSiltSlumpToSedLoad   = 0;
   m_dEndOfIterSandSlumpToSedLoad   = 0;
   m_dEndOfIterClayToppleDetach     = 0;
   m_dEndOfIterSiltToppleDetach     = 0;
   m_dEndOfIterSandToppleDetach     = 0;
   m_dEndOfIterClayToppleDeposit    = 0;
   m_dEndOfIterSiltToppleDeposit    = 0;
   m_dEndOfIterSandToppleDeposit    = 0;
   m_dEndOfIterClayToppleToSedLoad  = 0;
   m_dEndOfIterSiltToppleToSedLoad  = 0;
   m_dEndOfIterSandToppleToSedLoad  = 0;
   m_dEndOfIterInfiltration         = 0;
   m_dEndOfIterExfiltration         = 0;
   m_dEndOfIterClayInfiltDeposit    = 0;
   m_dEndOfIterSiltInfiltDeposit    = 0;
   m_dEndOfIterSandInfiltDeposit    = 0;
   m_dEndOfIterClayHeadcutDetach    = 0;
   m_dEndOfIterSiltHeadcutDetach    = 0;
   m_dEndOfIterSandHeadcutDetach    = 0;
   m_dEndOfIterClayHeadcutDeposit   = 0;
   m_dEndOfIterSiltHeadcutDeposit   = 0;
   m_dEndOfIterSandHeadcutDeposit   = 0;
   m_dEndOfIterClayHeadcutToSedLoad = 0;
   m_dEndOfIterSiltHeadcutToSedLoad = 0;
   m_dEndOfIterSandHeadcutToSedLoad = 0;
   m_dLastIterAvgHead               = 0;
   m_dEndOfIterTotHead              = 0;
   m_dStartOfIterTotElev            = 0;
   m_dEndOfIterTotElev              = 0;
   m_dStartOfIterTotSurfaceWater    = 0;
   m_dEndOfIterTotSoilWater         = 0;
   m_dStartOfIterTotSoilWater       = 0;
   m_dStartOfIterTotClaySedLoad     = 0;
   m_dStartOfIterTotSiltSedLoad     = 0;
   m_dStartOfIterTotSandSedLoad     = 0;
   m_dWaterCorrection               = 0;
   m_dSimulationDuration            = 0;
   m_dSimulatedRainDuration         = 0;
   m_dTimeStep                      = 0;
   m_dSimulatedTimeElapsed          = 0;
   m_dRSaveTime                     = 0;
   m_dRSaveInterval                 = 0;
   m_dSplashCalcLast                = 0;
   m_dClkLast                       = 0;
   m_dCPUClock                      = 0;
   m_dTargetGTotDrops               = 0;
   m_dClayDiameter                  = 0;
   m_dSiltDiameter                  = 0;
   m_dSandDiameter                  = 0;
   m_dDepositionGrainDensity        = 0;
   m_dDensityDiffExpression         = 0;
   m_dClaySizeMin                   = 0;
   m_dClaySiltBoundarySize          = 0;
   m_dSiltSandBoundarySize          = 0;
   m_dSandSizeMax                   = 0;
   m_dLastSlumpCalcTime             = 0;
   m_dLastHeadcutRetreatCalcTime    = 0;
   m_dEndOfIterTSWriteInfiltration       = 0;
   m_dEndOfIterTSWriteExfiltration       = 0;
   m_dEndOfIterTSWriteClayInfiltDeposit  = 0;
   m_dEndOfIterTSWriteSiltInfiltDeposit  = 0;
   m_dEndOfIterTSWriteSandInfiltDeposit  = 0;
   m_dEndOfIterTSWriteClaySplashRedist   = 0;
   m_dEndOfIterTSWriteSiltSplashRedist   = 0;
   m_dEndOfIterTSWriteSandSplashRedist   = 0;
   m_dEndOfIterTSWriteKE                 = 0;
   m_dEndOfIterTSWriteClaySlumpDetach    = 0;
   m_dEndOfIterTSWriteSiltSlumpDetach    = 0;
   m_dEndOfIterTSWriteSandSlumpDetach    = 0;
   m_dEndOfIterTSWriteClayToppleDetach   = 0;
   m_dEndOfIterTSWriteSiltToppleDetach   = 0;
   m_dEndOfIterTSWriteSandToppleDetach   = 0;
   m_dHeadcutRetreatConst           = 0;
   m_dOffEdgeHeadConst              = 0;
   m_dSSSPatchSize                  = 0;
   m_dOffEdgeParamA                 = 0;
   m_dOffEdgeParamB                 = 0;
   m_dManningParamA                 = 0;
   m_dManningParamB                 = 0;
   m_dFFConstant                    = 0;
   m_dFFReynoldsParamA              = 0;
   m_dFFReynoldsParamB              = 0;
   m_dFFLawrenceD50                 = 0;
   m_dFFLawrenceEpsilon             = 0;
   m_dFFLawrencePr                  = 0;
   m_dFFLawrenceCd                  = 0;
   m_dChengRoughnessHeight          = 0;
   m_dWaterErrorLast                = 0;
   m_dSplashErrorLast               = 0;
   m_dSlumpErrorLast                = 0;
   m_dToppleErrorLast               = 0;
   m_dHeadcutErrorLast              = 0;
   m_dFlowErrorLast                 = 0;
   m_dElapsed                       = 0;
   m_dStillToGo                     = 0;
   m_dWaterStoredLast               = 0;
   m_dSedimentLoadDepthLast         = 0;
   m_dClaySettlingSpeed             = 0;
   m_dSiltSettlingSpeed             = 0;
   m_dSandSettlingSpeed             = 0;
   m_dEndOfSimElevChange = 0;

   for (int i = 0; i < 6; i++)
      m_dGeoTransform[i] = 0;

   m_dRunOnRainVarM                 =
   m_dRainVarMFileMean              = 1;

   m_dMissingValue                  = NODATA;

   m_ldGTotDrops                    =
   m_ldGTotRunOnDrops               =
   m_ldGTotRain                     =
   m_ldGTotRunOn                    =
   m_ldGTotInfilt                   =
   m_ldGTotExfilt                   =
   m_ldGTotWaterOffEdge                =
   m_ldGTotFlowDetach               =
   m_ldGTotFlowDeposit              =
   m_ldGTotSedLoad                  =
   m_ldGTotFlowSedLoadOffEdge                  =
   m_ldGTotSplashDetach             =
   m_ldGTotSplashDeposit            =
   m_ldGTotSplashToSedLoad          =
   m_ldGTotSplashOffEdge            =
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

//=========================================================================================================================================
//! The CSimulation destructor
//=========================================================================================================================================
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
      // Delete all cell objects
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

//=========================================================================================================================================
//! Within-file static member variable initialisations
//=========================================================================================================================================
CSimulation* CCell::m_pSim = NULL;                          // Initialize m_pSim, the static member of CCell
CSimulation* CCellSoil::m_pSim = NULL;                      // Ditto for the CCellSoil class
CSimulation* CCellRainAndRunon::m_pSim = NULL;              // Ditto for the CCellRainAndRunon class
CSimulation* CCellSurfaceWater::m_pSim = NULL;              // Ditto for the CCellSurfaceWater class
CSimulation* CCellSedimentLoad::m_pSim = NULL;              // Ditto for the CCellSediment class

//=========================================================================================================================================
//! This member function of CSimulation sets up and runs the simulation
//=========================================================================================================================================
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

   // Initialize the two random number generators
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

   // Mark edge cells
   MarkEdgeCells();

   // Create soil layers
   CreateSoilLayers();

   // If we are simulating infiltration, then create the this-operation and time series soil water variables
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

   // Calculate various things which will be constants for the duration of the run (e.g. diagonal of cell side, inverse of cell side and diagonal, area of cell, etc.) now, do this only once for efficiency
   m_ulNActiveCells = static_cast<unsigned long>((m_nXGridMax * m_nYGridMax)) - m_ulMissingValueCells++;
   m_dInvCellSide   = 1 / m_dCellSide;                             // mm-1
   m_dCellSquare    = m_dCellSide * m_dCellSide;                   // mm2
   m_dInvCellSquare = 1 / m_dCellSquare;                           // mm-2
   m_dCellDiag      = sqrt(2 * m_dCellSquare);                     // mm
   m_dInvCellDiag   = 1 / m_dCellDiag;                             // mm-1
   m_dInvCos45      = 1 / cos(PI/4);
   m_dInvXGridMax   = 1 / static_cast<double>(m_nXGridMax);

   // If necessary, set up some initial values for infiltration and write to the cell array
   if (m_bDoInfiltration)
      InitSoilWater();

   // Initialize total elevation and total soil water content
   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      for (int nY = 0; nY < m_nYGridMax; nY++)
      {
         m_Cell[nX][nY].AddToPreSimulationValues();
      }
   }

   // Calculate the off-edge head constant using the empirical relationship const = m_dOffEdgeParamA * (m_dGradient**m_dOffEdgeParamB) where m_dGradient is in %
   m_dOffEdgeHeadConst = m_dOffEdgeParamA * pow(m_dGradient, m_dOffEdgeParamB);

   // Calculate a within-program constant for detachment, do this now for efficiency
   m_dST2 = m_dCVT * m_dCVT * m_dT * m_dT;

   // Calculate some within-program constants for deposition
   m_dClayDiameter = (m_dClaySizeMin + m_dClaySiltBoundarySize) / 2;               // mm
   m_dSiltDiameter = (m_dClaySiltBoundarySize + m_dSiltSandBoundarySize) / 2;      // mm
   m_dSandDiameter = (m_dSiltSandBoundarySize + m_dSandSizeMax) / 2;               // mm

   CalcSettlingSpeed();

   // If desired, output friction factor for checking
   if (m_bFFCheck)
      CheckLawrenceFF();

   // If considering slumping, calculate some within-program constants for slumping and toppling
   if (m_bSlumping)
   {
      nRet = nInitSlumping();
      if (nRet != RTN_OK)
         return (nRet);
   }

   // If not doing time-varying rainfall, calculate target number of rain drops; this is used for rain intensity correction
   if (! m_bTimeVaryingRain)
      m_dTargetGTotDrops = m_dRainIntensity * static_cast<double>(m_ulNActiveCells) * m_dCellSquare * m_dSimulatedRainDuration / (3600 * m_dMeanCellWaterVol);

   // If we are doing splash redistribution, set up splash stuff now
   if (m_bSplash)
   {
      // Calculate these now, for efficiency
      m_dPartKE  = 0.5 * m_dCellSquare * m_dRho * 1e-3 * m_dRainSpeed * m_dRainSpeed * 1e-12;    // in Joules when multiplied by rain depth in mm
      m_dSplashConstantNormalized = m_dSplashConstant / (m_dCellSquare * m_dRainSpeed * m_dRainSpeed);

      // Read in water depth/splash attenuation parameters
      if (! bReadSplashAttenuationData())
         return (RTN_ERR_SPLASH_ATTENUATION);

      // Call initializing routine to calculate second derivatives for cubic spline, used in calculating splash attenuation
      InitSplashAttenuation();

      // If desired, output splash attenuation for checking
      if (m_bSplashCheck)
         CheckSplashAttenuation();

      if (m_bPoesenSplashEqn)
      {
         // TODO
      }

      if (m_bPlanchonSplashEqn)
      {
         // If using the Planchon splash equation, compensate for grid size in the Laplacian calculations, assuming that m_dSplashConstant (as read in) was calibrated for a 10 mm grid
         // TODO does this work correctly?
         double const dAC = 0.619;
         double const dBC = 0.1969;
         m_dPlanchonCellSizeKC = (10 * (dAC - dBC * log(10))) / (m_dCellSide * (dAC - dBC * log(m_dCellSide)));
         m_dPlanchonCellSizeKC /= m_dCellSquare;
      }
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

//=========================================================================================================================================
//! This function runs the simulation
//=========================================================================================================================================
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
      // m_ofsLog << string(110, '=') << endl << endl;
      // DEBUG_SEDLOAD("start of iteration");

      // Calculate the timestep for this iteration
      CalcTimestep();

      // Check that we haven't gone on too long
      if (bIsTimeToQuit())
         break;

      // Tell the user how the simulation is progressing
      AnnounceProgress();

      // Initialize this-iteration and start-of-iteration values
      m_bInfiltThisIter = false;

      // Inherit end-of-iteration values from the previous iteration (these will be zero for the first iteration)
      m_dStartOfIterTotSurfaceWater = m_dEndOfIterTotSurfaceWater;
      m_dStartOfIterTotClaySedLoad  = m_dEndOfIterClaySedLoad;
      m_dStartOfIterTotSiltSedLoad  = m_dEndOfIterSiltSedLoad;
      m_dStartOfIterTotSandSedLoad  = m_dEndOfIterSandSedLoad;

      if (m_ulIter > 1)
      {
         m_dStartOfIterTotElev = m_dEndOfIterTotElev;
         m_dStartOfIterTotSoilWater = m_dEndOfIterTotSoilWater;
      }

      m_ulNumHead = 0;
      m_dEndOfIterTotElev              =
      m_dEndOfIterTotSurfaceWater      =
      m_dEndOfIterTotSoilWater         =
      m_dEndOfIterClaySedLoad          =
      m_dEndOfIterSiltSedLoad          =
      m_dEndOfIterSandSedLoad          =
      m_dEndOfIterClayFlowDetach       =
      m_dEndOfIterSiltFlowDetach       =
      m_dEndOfIterSandFlowDetach       =
      m_dEndOfIterClayFlowDeposit      =
      m_dEndOfIterSiltFlowDeposit      =
      m_dEndOfIterSandFlowDeposit      =
      m_dEndOfIterClaySedLoadOffEdge   =
      m_dEndOfIterSiltSedLoadOffEdge   =
      m_dEndOfIterSandSedLoadOffEdge   =
      m_dEndOfIterTotHead              =
      m_dEndOfIterInfiltration         =
      m_dEndOfIterExfiltration         =
      m_dEndOfIterClayInfiltDeposit    =
      m_dEndOfIterSiltInfiltDeposit    =
      m_dEndOfIterSandInfiltDeposit    =
      m_dEndOfIterClaySplashDetach     =
      m_dEndOfIterSiltSplashDetach     =
      m_dEndOfIterSandSplashDetach     =
      m_dEndOfIterClaySplashDeposit    =
      m_dEndOfIterSiltSplashDeposit    =
      m_dEndOfIterSandSplashDeposit    =
      m_dEndOfIterClaySplashToSedLoad  =
      m_dEndOfIterSiltSplashToSedLoad  =
      m_dEndOfIterSandSplashToSedLoad  =
      m_dEndOfIterNetClayDetachment    =
      m_dEndOfIterNetSiltDetachment    =
      m_dEndOfIterNetSandDetachment    =
      m_dEndOfIterClaySplashOffEdge    =
      m_dEndOfIterSiltSplashOffEdge    =
      m_dEndOfIterSandSplashOffEdge    = 0;

      // Initialize all cells ready for this iteration
      for (int nX = 0; nX < m_nXGridMax; nX++)
      {
         for (int nY = 0; nY < m_nYGridMax; nY++)
         {
            m_Cell[nX][nY].pGetRainAndRunon()->InitializeRainAndRunon();
            m_Cell[nX][nY].pGetSurfaceWater()->InitializeFlow();
            m_Cell[nX][nY].pGetSoilWater()->InitializeInfiltration();
            m_Cell[nX][nY].pGetSoil()->InitializeDetachAndDeposit(m_bSlumpThisIter);
            m_Cell[nX][nY].pGetSedLoad()->ResetSedLoad();
         }
      }

      // If we are simulating infiltration then initialize for each soil layer
      if (m_bDoInfiltration)
      {
         for (unsigned int nLayer = 0; nLayer < static_cast<unsigned int>(m_nNumSoilLayers); nLayer++)
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
               {
                  // Do run on from this edge
                  DoRunOnFromOneEdge(n);
               }
            }
         }

         // Do rainfall
         DoAllRain();
      }

      // DEBUG_SEDLOAD("before splash");

      // Next, splash redistribution. If we are considering splash redistribution, then do it this iteration, provided it is still raining. Note: tried calculating splash only on a subset of interations, but caused problems e.g. splash rate depended too much on how often splash was calculated; and leaving splash calcs too long meant that big splash changes could occur, which is unrealistic)
      m_bSplashThisIter = false;
      m_dEndOfIterKE = 0;

      if (m_bSplash && (m_dRainIntensity > 0))
      {
         // Yes, simulating splash redistribution
         m_bSplashThisIter = true;

         // Now simulate the splash redistribution for this iteration
         DoAllSplash();

         // And reset the counter for next time
         m_dSplashCalcLast = m_dSimulatedTimeElapsed;
      }

      // DEBUG_SEDLOAD("after splash");

      // Now infiltration
      if (m_bDoInfiltration && (CALC_INFILT_INTERVAL-1 == ++m_nInfiltCount))                 // If we are considering infilt, simulate it this iteration?
      {
         // Yup, simulate infilt from the cell array
         m_nInfiltCount = 0;
         m_bInfiltThisIter = true;

         DoAllInfiltration();
      }

      // DEBUG_SEDLOAD("before flow routing");

      // Route all flow from wet cells, and maybe do flow erosion
      DoAllFlowRouting();

      // DEBUG_SEDLOAD("after flow routing");

      // When representing flow off an edge of the grid, we need a value for the off-edge head. Save this iteration's maximum and minimum on-grid values of head for this
      if (m_ulNumHead > 0)
         m_dLastIterAvgHead = m_dEndOfIterTotHead / static_cast<double>(m_ulNumHead);

      // Next, rill-wall slumping and toppling
      m_bSlumpThisIter = false;

      // Initialize 'since last' values for slumping and toppling
      m_dEndOfIterClaySlumpDetach =
      m_dEndOfIterSiltSlumpDetach =
      m_dEndOfIterSandSlumpDetach =
      m_dEndOfIterClaySlumpDeposit =
      m_dEndOfIterSiltSlumpDeposit =
      m_dEndOfIterSandSlumpDeposit =
      m_dEndOfIterClaySlumpToSedLoad =
      m_dEndOfIterSiltSlumpToSedLoad =
      m_dEndOfIterSandSlumpToSedLoad =
      m_dEndOfIterClayToppleDetach =
      m_dEndOfIterSiltToppleDetach =
      m_dEndOfIterSandToppleDetach =
      m_dEndOfIterClayToppleDeposit =
      m_dEndOfIterSiltToppleDeposit =
      m_dEndOfIterSandToppleDeposit =
      m_dEndOfIterClayToppleToSedLoad =
      m_dEndOfIterSiltToppleToSedLoad =
      m_dEndOfIterSandToppleToSedLoad = 0;

      // If we are considering slumping, simulate it this iteration?
      if (m_bSlumping && (CALC_SLUMP_INTERVAL-1 == ++m_nSlumpCount))
      {
         // Yup, simulate slumping and toppling
         m_nSlumpCount = 0;
         m_bSlumpThisIter = true;
         DoAllSlump();

         // Reset for next time
         m_dLastSlumpCalcTime = m_dSimulatedTimeElapsed;

         // DEBUG_SEDLOAD("after slump/topple");
      }

      // Finally, headcut retreat
      m_bHeadcutRetreatThisIter = false;

      // Initialize 'since last' values for headcut retreat
      m_dEndOfIterClayHeadcutDetach =
      m_dEndOfIterSiltHeadcutDetach =
      m_dEndOfIterSandHeadcutDetach =
      m_dEndOfIterClayHeadcutDeposit =
      m_dEndOfIterSiltHeadcutDeposit =
      m_dEndOfIterSandHeadcutDeposit =
      m_dEndOfIterClayHeadcutToSedLoad =
      m_dEndOfIterSiltHeadcutToSedLoad =
      m_dEndOfIterSandHeadcutToSedLoad = 0;

      // If we are considering headcut retreat, simulate it this iteration?
      if (m_bHeadcutRetreat && (CALC_HEADCUT_RETREAT_INTERVAL-1 == ++m_nHeadcutRetreatCount))
      {
         // Yup, simulate headcut retreat
         m_nHeadcutRetreatCount = 0;
         m_bHeadcutRetreatThisIter = true;
         DoAllHeadcutRetreat();

         // Reset for next time
         m_dLastHeadcutRetreatCalcTime = m_dSimulatedTimeElapsed;

         // DEBUG_SEDLOAD("after headcut retreat");
      }

      // Calc end-of-iteration totals then initialize some of the cell array ready for the next iteration (note: m_dEndOfIterKE)
      m_ulNWet          = 0;
      m_dEndOfIterRain  =
      m_dEndOfIterRunOn =
      m_dEndOfIterSurfaceWaterOffEdge = 0;

      for (int nX = 0; nX < m_nXGridMax; nX++)
      {
         for (int nY = 0; nY < m_nYGridMax; nY++)
         {
            if (m_Cell[nX][nY].bIsMissingValue())
               continue;

            m_Cell[nX][nY].GetEndOfIterValues();
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
      m_dEndOfIterTSWriteInfiltration      += m_dEndOfIterInfiltration;
      m_dEndOfIterTSWriteExfiltration      += m_dEndOfIterExfiltration;
      m_dEndOfIterTSWriteClayInfiltDeposit += m_dEndOfIterClayInfiltDeposit;
      m_dEndOfIterTSWriteSiltInfiltDeposit += m_dEndOfIterSiltInfiltDeposit;
      m_dEndOfIterTSWriteSandInfiltDeposit += m_dEndOfIterSandInfiltDeposit;
      m_dEndOfIterTSWriteClaySplashRedist  += m_dEndOfIterClaySplashDetach;
      m_dEndOfIterTSWriteSiltSplashRedist  += m_dEndOfIterSiltSplashDetach;
      m_dEndOfIterTSWriteSandSplashRedist  += m_dEndOfIterSandSplashDetach;
      m_dEndOfIterTSWriteKE                += m_dEndOfIterKE;
      m_dEndOfIterTSWriteClaySlumpDetach   += m_dEndOfIterClaySlumpDetach;
      m_dEndOfIterTSWriteSiltSlumpDetach   += m_dEndOfIterSiltSlumpDetach;
      m_dEndOfIterTSWriteSandSlumpDetach   += m_dEndOfIterSandSlumpDetach;
      m_dEndOfIterTSWriteClayToppleDetach  += m_dEndOfIterClayToppleDetach;
      m_dEndOfIterTSWriteSiltToppleDetach  += m_dEndOfIterSiltToppleDetach;
      m_dEndOfIterTSWriteSandToppleDetach  += m_dEndOfIterSandToppleDetach;

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
      UpdatePerIterGrandTotals();

   }  // ===================================================== End of main loop ===========================================================

   // ======================================================== post-loop tidying ==========================================================
   CalcEndOfSimDEMChange();

   int nRet = nWriteFilesAtEnd();
   if (nRet != RTN_OK)
      return nRet;

   // Final write to time series CSV files
   if (! bWriteTSFiles(true))
      return (RTN_ERR_TSFILEWRITE);

   WriteEndOfSimTotals();

   // Normal completion
   return (RTN_OK);
}


