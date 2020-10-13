/*========================================================================================================================================

 This is simulation.cpp: the start-of-simulation routine for RillGrow

 Copyright (C) 2020 David Favis-Mortlock

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
   m_bSedLoadLostTS               =
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
   m_bFrictionFactorLawrence  = false;

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
   m_nNumSoilLayers     =
   m_nSSSQuadrantSize   = 0;
   m_nZUnits            = Z_UNIT_NONE;

   m_ulIter              =
   m_ulTotIter           =
   m_ulVarChkStart       =
   m_ulVarChkEnd         =
   m_ulNActiveCells      =
   m_ulNWet              =
   m_ulMissingValueCells =
   m_ulNumHead           = 0;

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
   m_VdSplashEffN                   =
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
   m_dThisIterSurfaceWaterStored    =
   m_dThisIterClaySedLoad           =
   m_dThisIterSiltSedLoad           =
   m_dThisIterSandSedLoad           =
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
   m_dThisIterClaySplashDepositOnly =
   m_dThisIterSiltSplashDepositOnly =
   m_dThisIterSandSplashDepositOnly =
   m_dThisIterClaySplashDepositAndSedLoad     =
   m_dThisIterSiltSplashDepositAndSedLoad     =
   m_dThisIterSandSplashDepositAndSedLoad     =
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
   m_dThisIterClayHeadcutRetreatDetach =
   m_dThisIterSiltHeadcutRetreatDetach =
   m_dThisIterSandHeadcutRetreatDetach =
   m_dThisIterExfiltration          =
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
   m_dSinceLastTSSandToppleDetach   =
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
   m_dFFLawrenceCd                  = 0;

   for (int i = 0; i < 6; i++)
      m_dGeoTransform[i] = 0;

   m_dRunOnRainVarM                 =
   m_dRainVarMFileMean              = 1;

   m_dMissingValue                  = -9999;

   m_ldGTotDrops                    =
   m_ldGTotRunOnDrops               =
   m_ldGTotRain                     =
   m_ldGTotRunOnWater               =
   m_ldGTotInfilt                   =
   m_ldGTotExfilt                   =
   m_ldGTotWaterLost                =
   m_ldGTotFlowDetach               =
   m_ldGTotFlowDeposition           =
   m_ldGTotSedLost                  =
   m_ldGTotSplashDetach             =
   m_ldGTotSplashDeposit            =
   m_ldGTotSlumpDetach              =
   m_ldGTotToppleDetach             =
   m_ldGTotToppleDeposit            =
   m_ldGTotInfiltDeposit            = 0;

   for (int i = 0; i < 2; i++)
   {
      m_ulRState[i].s1 =
      m_ulRState[i].s2 =
      m_ulRState[i].s3 = 0;
   }

   m_tSysStartTime =
   m_tSysEndTime   = 0;

   Cell = NULL;
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

   if (Cell)
   {
      // Delete all Cell objects
      for (int nX = 0; nX < m_nXGridMax; nX++)
         delete [] Cell[nX];
      delete [] Cell;
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
CSimulation* CCellSedimentLoad::m_pSim = NULL;                // Ditto for the CellSediment class


/*========================================================================================================================================

 This publicly visible member function of CSimulation sets up and runs the simulation

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

   // Read in the microtography DEM and create the Cell array
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

   // If we are simulating infilt, then create the this-operation and time series soil water variables
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

   // If necessary, set up some initial values for infilt, write to the Cell array
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
   if (m_dPossMaxSpeedNextIter == 0)
   {
      // No flow occurred, so set the timestep for the next iteration based on a guessed-in value for flow speed
      m_dTimeStep = m_dCellSide / INIT_MAX_SPEED_GUESS;                             // In sec
   }
   else
   {
      // Some flow occurred: calculate the possible next timestep. Start by constraining the max possible flow speed
      m_dPossMaxSpeedNextIter = tMin(m_dPossMaxSpeedNextIter, m_dMaxFlowSpeed);

      // OK now calculate the possible timestep
      double dPossNextTimeStep = m_dCellSide / m_dPossMaxSpeedNextIter;             // In sec

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
            // The change in timestep is large, so we need to make a smaller change. This is equivalent to the 'Courant–Friedrichs–Lewy condition' i.e. the time needed for flow to cross a cell at the maximum speed during the last iteration, plus an arbitrary safety margin. COURANT_ALPHA = delta_time / delta_distance See e.g. http://en.wikipedia.org/wiki/Courant%E2%80%93Friedrichs%E2%80%93Lewy_condition
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
            // The change in timestep is large, so we need to make a smaller change. This is equivalent to the 'Courant–Friedrichs–Lewy condition' i.e. the time needed for flow to cross a cell at the maximum speed during the last iteration, plus an arbitrary safety margin. COURANT_ALPHA = delta_time / delta_distance See e.g. http://en.wikipedia.org/wiki/Courant%E2%80%93Friedrichs%E2%80%93Lewy_condition
            m_dTimeStep = m_dTimeStep * COURANT_ALPHA;
         }
      }

      // Reset for the coming interation
      m_dPossMaxSpeedNextIter = 0;
   }
}

#if defined _DEBUG
/*========================================================================================================================================

 Calculate and checks the mass balance of water and of sediment) for this iteration in order to remove rounding errors. These always accumulate when many small values are summed using finite-precision arithmetic (see e.g. "Floating-Point Summation" http://www.ddj.com/cpp/184403224#REF1). Even after a few iterations, mass-balance errors will start to appear here, unless a correction is made every iteration

========================================================================================================================================*/
void CSimulation::CheckMassBalance(void)
{
   // Check the this-iteration water balance: start by calculating the change in surface water since the last iteration
   static double sdWaterStoredLast = 0;
   double dDeltaWaterStored = m_dThisIterSurfaceWaterStored - sdWaterStoredLast;       // Note can be -ve

   // Calculate LHS = rain + run-on + correction, RHS = infilt + outflow + change in storage
   double
      dTotWaterLHS = m_dThisIterRain + m_dThisIterRunOn,
      dTotWaterRHS = m_dThisIterInfiltration + m_dThisIterWaterLost + dDeltaWaterStored;

   if (! bFPIsEqual(dTotWaterLHS, dTotWaterRHS, WATER_TOLERANCE))
   {
      m_ofsLog << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(6);

      m_ofsLog << m_ulIter << " " << WARN << "water balance, LHS = " << dTotWaterLHS << " RHS = " << dTotWaterRHS << endl;
      m_ofsLog << "\tm_dThisIterRain           = " << m_dThisIterRain << endl;
      m_ofsLog << "\tm_dThisIterRunOn          = " << m_dThisIterRunOn << endl;
      m_ofsLog << "\t\tm_dThisIterInfiltration = " << m_dThisIterInfiltration << endl;
      m_ofsLog << "\t\tm_dThisIterWaterLost    = " << m_dThisIterWaterLost << endl;
      m_ofsLog << "\t\tfDeltaWaterStored       = " << dDeltaWaterStored << endl;

      m_ofsLog << resetiosflags(ios::floatfield) << setiosflags(ios::scientific) << setprecision(6);
   } /*
   else
   {
      m_ofsLog << m_ulIter << " OK water balance, LHS = " << fTotWaterLHS << " RHS = " << fTotWaterRHS << endl;
      m_ofsLog << "\tm_dThisIterRain           = " << m_dThisIterRain << endl;
      m_ofsLog << "\tm_dThisIterRunOn          = " << m_dThisIterRunOn << endl;
      m_ofsLog << "\t\tm_dThisIterInfiltration = " << m_dThisIterInfiltration << endl;
      m_ofsLog << "\t\tm_dThisIterWaterLost    = " << m_dThisIterWaterLost << endl;
      m_ofsLog << "\t\tfDeltaWaterStored       = " << fDeltaWaterStored << endl;
   } */

   // Store for next iteration
   sdWaterStoredLast = m_dThisIterSurfaceWaterStored;

   // Now check the this-iteration sediment balance: start by ca;lculating the change in sediment load since the last iteration
   static double sdSedimentLoadDepthLast = 0;

   double
      dTotSedStored = m_dThisIterClaySedLoad + m_dThisIterSiltSedLoad + m_dThisIterSandSedLoad,
      dDeltaSedLoadStored = dTotSedStored - sdSedimentLoadDepthLast;

   // LHS = flow detachment + slump and topple detachment + correction, RHS = flow deposition + slump and topple deposition + sediment lost + change in storage. Note cannot include deposition resulting from cells drying up due to infilt, because doing so results in double-counting of this depth of sediment
   double
      dThisIterFlowDetach = m_dThisIterClayFlowDetach + m_dThisIterSiltFlowDetach + m_dThisIterSandFlowDetach,
      dThisIterFlowDeposit = m_dThisIterClayFlowDeposit + m_dThisIterSiltFlowDeposit + m_dThisIterSandFlowDeposit;

   double
      dThisIterSplashDetach = m_dThisIterClaySplashDetach + m_dThisIterSiltSplashDetach + m_dThisIterSandSplashDetach,
      dThisIterSplashDepositOnly = m_dThisIterClaySplashDepositOnly + m_dThisIterSiltSplashDepositOnly + m_dThisIterSandSplashDepositOnly;

   double
      dThisIterSlumpDetach = m_dThisIterClaySlumpDetach + m_dThisIterSiltSlumpDetach  + m_dThisIterSandSlumpDetach,
      dThisIterSlumpDeposit = m_dThisIterClaySlumpDeposit + m_dThisIterSiltSlumpDeposit  + m_dThisIterSandSlumpDeposit;

   double
      dThisIterToppleDetach = m_dThisIterClayToppleDetach + m_dThisIterSiltToppleDetach  + m_dThisIterSandToppleDetach,
      dThisIterToppleDeposit = m_dThisIterClayToppleDeposit + m_dThisIterSiltToppleDeposit  + m_dThisIterSandToppleDeposit;

   double
      dThisIterSedLost = m_dThisIterClaySedLost + m_dThisIterSiltSedLost + m_dThisIterSandSedLost;

   double
      dTotSedLHS = dThisIterFlowDetach + dThisIterSplashDetach + dThisIterSlumpDetach + dThisIterToppleDetach,
      dTotSedRHS = dThisIterFlowDeposit + dThisIterSplashDepositOnly + dThisIterSlumpDeposit + dThisIterToppleDeposit + dThisIterSedLost + dDeltaSedLoadStored;

   if (! bFPIsEqual(dTotSedLHS, dTotSedRHS, SEDIMENT_TOLERANCE))
   {
      m_ofsLog << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(6);

      m_ofsLog << m_ulIter << " " << WARN << "sediment balance, LHS = " << dTotSedLHS << " RHS = " << dTotSedRHS << endl;
      m_ofsLog << "\tLHS (detach) - RHS (losses + storage) = " << dTotSedLHS - dTotSedRHS << endl;
      m_ofsLog << "\tdThisIterFlowDetach           = " << dThisIterFlowDetach << endl;
      m_ofsLog << "\tdThisIterSplashDetach         = " << dThisIterSplashDetach << endl;
      m_ofsLog << "\tdThisIterSlumpDetach          = " << dThisIterSlumpDetach << endl;
      m_ofsLog << "\tdThisIterToppleDetach         = " << dThisIterToppleDetach << endl;
      m_ofsLog << "\t\tdThisIterFlowDeposit           = " << dThisIterFlowDeposit << endl;
      m_ofsLog << "\t\tdThisIterSplashDepositOnly     = " << dThisIterSplashDepositOnly << endl;
      m_ofsLog << "\t\tdThisIterSlumpDeposit          = " << dThisIterSlumpDeposit << endl;
      m_ofsLog << "\t\tdThisIterToppleDeposit         = " << dThisIterToppleDeposit << endl;
      m_ofsLog << "\t\tdThisIterSedLost               = " << dThisIterSedLost << endl;
      m_ofsLog << "\t\tdDeltaSedLoadStored              = " << dDeltaSedLoadStored << endl;

      m_ofsLog << resetiosflags(ios::floatfield) << setiosflags(ios::scientific) << setprecision(6);
   } /*
   else
   {
   m_ofsLog << m_ulIter << " OK sediment balance (splash iteration), LHS = " << fTotSedLHS << " RHS = " << fTotSedRHS << endl;
   m_ofsLog << "\tdThisIterFlowDetach           = " << dThisIterFlowDetach << endl;
   m_ofsLog << "\tdThisIterSlumpDetach          = " << dThisIterSlumpDetach << endl;
   m_ofsLog << "\t\tdThisIterFlowDeposit           = " << dThisIterFlowDeposit << endl;
   m_ofsLog << "\t\tdThisIterSlumpDeposit          = " << dThisIterSlumpDeposit << endl;
   m_ofsLog << "\t\tdThisIterSedLost               = " << dThisIterSedLost << endl;
   m_ofsLog << "\t\tdDeltaSedLoadStored              = " << dDeltaSedLoadStored << endl;
   } */

   // Store for next iteration
   sdSedimentLoadDepthLast = (m_dThisIterClaySedLoad + m_dThisIterSiltSedLoad + m_dThisIterSandSedLoad);
}
#endif


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
      m_ldGTotSplashDeposit += (m_dThisIterClaySplashDepositAndSedLoad + m_dThisIterSiltSplashDepositAndSedLoad + m_dThisIterSandSplashDepositAndSedLoad) * m_dCellSquare;
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

      // Initialize some fields of the Cell array ready for flow routing
      for (int nX = 0; nX < m_nXGridMax; nX++)
         for (int nY = 0; nY < m_nYGridMax; nY++)
            Cell[nX][nY].InitializeAtStartOfIteration(m_bSlumpThisIter);

      // If we are simulating infilt then do some more initialization
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

      // Initialise ready for flow routing
      m_ulNumHead = 0;

      // Note that m_dThisIterClaySedLoad, m_dThisIterSiltSedLoad, m_dThisIterSandSedLoad do not get initialised every timestep, they are rfunning totals
      m_dThisIterTotHead         =
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

      // Route all flow from wet cells
      DoAllFlowRouting();

      // When representing flow off an edge of the grid, we need a value for the off-edge head. Save this iteration's maximum and minimum on-grid values of head for this
      m_dLastIterAvgHead = m_dThisIterTotHead / m_ulNumHead;

      // Infiltration next: first zero the this-operation totals
      m_bInfiltThisIter = false;
      static int snInfiltCount = 0;

      m_dThisIterInfiltration      =
      m_dThisIterExfiltration      =
      m_dThisIterClayInfiltDeposit =
      m_dThisIterSiltInfiltDeposit =
      m_dThisIterSandInfiltDeposit = 0;

      if (m_bDoInfiltration && (CALC_INFILT_INTERVAL-1 == ++snInfiltCount))                 // If we are considering infilt, simulate it this iteration?
      {
         // Yup, simulate infilt from the Cell array
         snInfiltCount = 0;
         m_bInfiltThisIter = true;

         DoAllInfiltration();
      }

      // Next, splash redistribution. If we are considering splash redistribution, then do it this iteration, provided it is still raining
      // Note: tried calculating splash only on a subset of interations, but caused problems e.g. splash rate depended too much on how often splash was calculated; and leaving splash calcs too long meant that big splash changes could occur, which is unrealistic)
      m_bSplashThisIter = false;

      m_dClaySplashedError += (m_dThisIterClaySplashDetach - m_dThisIterClaySplashDepositAndSedLoad);     // Use values from the previous iteration
      m_dSiltSplashedError += (m_dThisIterSiltSplashDetach - m_dThisIterSiltSplashDepositAndSedLoad);     // Ditto
      m_dSandSplashedError += (m_dThisIterSandSplashDetach - m_dThisIterSandSplashDepositAndSedLoad);     // Ditto

      m_dThisIterClaySplashDetach            =
      m_dThisIterSiltSplashDetach            =
      m_dThisIterSandSplashDetach            =
      m_dThisIterClaySplashDepositOnly       =
      m_dThisIterSiltSplashDepositOnly       =
      m_dThisIterSandSplashDepositOnly       =
      m_dThisIterClaySplashDepositAndSedLoad =
      m_dThisIterSiltSplashDepositAndSedLoad =
      m_dThisIterSandSplashDepositAndSedLoad =
      m_dThisIterKE                          = 0;

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

      // If we are considering slumping, simulate it this iteration?
      if (m_bSlumping && (CALC_SLUMP_INTERVAL-1 == ++snSlumpCount))
      {
         // Yup, simulate slumping and toppling
         snSlumpCount = 0;
         m_bSlumpThisIter = true;
         DoAllSlump();

         // Reset for next time
         m_dLastSlumpCalcTime = m_dSimulatedTimeElapsed;
      }

      // Finally, headcut retreat
      m_bHeadcutRetreatThisIter = false;
      static int snHeadcutRetreatCount = 0;

      // If we are considering headcut retreat, simulate it this iteration?
      if (m_bHeadcutRetreat && (CALC_HEADCUT_RETREAT_INTERVAL-1 == ++snHeadcutRetreatCount))
      {
         // Yup, simulate headcut retreat
         snHeadcutRetreatCount = 0;
         m_bHeadcutRetreatThisIter = true;
         DoAllHeadcutRetreat();

         // Reset for next time
         m_dLastHeadcutRetreatCalcTime = m_dSimulatedTimeElapsed;
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

#if defined _DEBUG
      // Calculate and check this-iteration hydrology and sediment balance
      CheckMassBalance();
#endif

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

 Publicly visible, returns the current timestep

==============================================================================================================================*/
double CSimulation::dGetTimeStep(void) const
{
   return m_dTimeStep;
}

/*==============================================================================================================================

 Publicly visible, increments the this-iteration total of wet cells

==============================================================================================================================*/
void CSimulation::IncrNumWetCells(void)
{
   m_ulNWet++;
}

/*==============================================================================================================================

 Publicly visible, decrements the this-iteration total of wet cells

 ==============================================================================================================================*/
void CSimulation::DecrNumWetCells(void)
{
   m_ulNWet--;
}

/*==============================================================================================================================

 Publicly-visible wrapper around dGetRand0Gaussian(), needed for cell surface water initialisation

==============================================================================================================================*/
double CSimulation::dGetRandGaussian(void)
{
   return dGetRand0Gaussian();
}
