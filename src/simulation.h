#ifndef __SIMULATION_H__
   #define __SIMULATION_H__

/*=========================================================================================================================================

 This is simulation.h: this class runs RillGrow simulations

 Copyright (C) 2020 David Favis-Mortlock

 ==========================================================================================================================================

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation,
 Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
#include "rg.h"


class CCell;            // Forward declarations
class C2DVec;
class CCellSoilLayer;

class CSimulation
{
private:
   bool
      m_bFlowDetachSave,
      m_bRainVarMSave,
      m_bCumulRunOnSave,
      m_bElevSave,
      m_bInitElevSave,
      m_bDetrendElevSave,
      m_bInfiltSave,
      m_bCumulInfiltSave,
      m_bSoilWaterSave,
      m_bInfiltDepositSave,
      m_bCumulInfiltDepositSave,
      m_bTopSurfaceSave,
      m_bSplashSave,
      m_bCumulSplashSave,
      m_bInundationSave,
      m_bFlowDirSave,
      m_bStreamPowerSave,
      m_bShearStressSave,
      m_bFrictionFactorSave,
      m_bCumulAvgShearStressSave,
      m_bReynoldsSave,
      m_bFroudeSave,
      m_bCumulAvgDepthSave,
      m_bCumulAvgDWSpdSave,
      m_bCumulAvgSpdSave,
      m_bSedConcSave,
      m_bSedLoadSave,
      m_bAvgSedLoadSave,
      m_bCumulFlowDepositSave,
      m_bCumulLoweringSave,
      m_bSlumpSave,
      m_bToppleSave,
      m_bTCSave,
      m_bSaveRegular,
      m_bDetrend,
      m_bDoInfiltration,
      m_bInfiltThisIter,
      m_bFFCheck,
      m_bSplash,
      m_bSplashThisIter,
      m_bSplashCheck,
      m_bFlowErosion,
      m_bRunOn,
      m_bTimeVaryingRain,
      m_bGISOutDiffers,
      m_bSlumping,
      m_bSlumpThisIter,
      m_bHeadcutRetreat,
      m_bHeadcutRetreatThisIter,
      m_bTimeStepTS,
      m_bAreaWetTS,
      m_bRainTS,
      m_bInfiltTS,
      m_bExfiltTS,
      m_bRunOnTS,
      m_bSurfaceWaterTS,
      m_bSurfaceWaterLostTS,
      m_bFlowDetachTS,
      m_bSlumpDetachTS,
      m_bToppleDetachTS,
      m_bDoSedLoadDepositTS,
      m_bSedLoadLostTS,
      m_bSedLoadTS,
      m_bInfiltDepositTS,
      m_bSplashRedistTS,
      m_bSplashKETS,
      m_bSoilWaterTS,
      m_bSaveGISThisIter,
      m_bThisIterRainChange,
      m_bHaveBaseLevel,
      m_bOutDEMsUsingInputZUnits,
      m_bClosedThisEdge[4],
      m_bRunOnThisEdge[4],
      m_bManningEqn,
      m_bDarcyWeisbachEqn,
      m_bFrictionFactorConstant,
      m_bFrictionFactorReynolds,
      m_bFrictionFactorLawrence,
      m_bLostSave,
      m_bFlumeTypeSim;

   int
      m_nGISSave,
      m_nUSave,
      m_nThisSave,
      m_nXGridMax,
      m_nYGridMax,
      m_nHeaderSize,
      m_nRainChangeTimeMax,
      m_nZUnits,
      m_nNumSoilLayers,
      m_nSSSQuadrantSize;

   unsigned long
      m_ulIter,
      m_ulTotIter,
      m_ulRandSeed[NUMBER_OF_RNGS],
      m_ulVarChkStart,
      m_ulVarChkEnd,
      m_ulNActiveCells,
      m_ulNWet,
      m_ulMissingValueCells,
      m_ulNumHead;

   double
      m_dMinX,
      m_dMaxX,
      m_dMinY,
      m_dMaxY,
      m_dMaxFlowSpeed,
      m_dPossMaxSpeedNextIter,
      m_dBasementElevation,
      m_dAvgElev,
      m_dMinElev,
      m_dGradient,
      m_dPlotEndDiff,
      m_dPlotElevMin,
      m_dPlotElevMax,
      m_dBaseLevel,
      m_dInvCos45,
      m_dYInc,
      m_dRunOnRainVarM,
      m_dRainIntensity,
      m_dSpecifiedRainIntensity,
      m_dRainVarMFileMean,
      m_dStdRainInt,
      m_dDropDiameter,
      m_dStdDropDiameter,
      m_dRainSpeed,
      m_dPartKE,
      m_VdSplashEffN,
      m_dCellSizeKC,
      m_dMeanCellWaterVol,
      m_dStdCellWaterVol,
      m_dCellSide,               // in mm
      m_dCellDiag,
      m_dInvCellSide,
      m_dInvCellDiag,
      m_dCellSquare,
      m_dInvCellSquare,
      m_dInvXGridMax,
      m_dRho,
      m_dG,
      m_dNu,
      m_dK,
      m_dT,
      m_dCVT,
      m_dCVTaub,
      m_dST2,
      m_dBulkDensityForOutputCalcs,
      m_dAlpha,
      m_dBeta,
      m_dGamma,
      m_dDelta,
      m_dRunOnLen,
      m_dRunOnSpd,
      m_dCritSSSForSlump,
      m_dSlumpAngleOfRest,
      m_dSlumpAngleOfRestDiff,
      m_dSlumpAngleOfRestDiffDiag,
      m_dToppleCriticalAngle,
      m_dToppleCritDiff,
      m_dToppleCritDiffDiag,
      m_dToppleAngleOfRest,
      m_dToppleAngleOfRestDiff,
      m_dToppleAngleOfRestDiffDiag,
      m_dThisIterStoredSurfaceWater,
      m_dThisIterClaySedLoad,
      m_dThisIterSiltSedLoad,
      m_dThisIterSandSedLoad,
      m_dThisIterRain,
      m_dThisIterRunOn,
      m_dThisIterKE,
      m_dThisIterLostSurfaceWater,
      m_dThisIterClayFlowDetach,
      m_dThisIterSiltFlowDetach,
      m_dThisIterSandFlowDetach,
      m_dThisIterClayFlowDeposit,
      m_dThisIterSiltFlowDeposit,
      m_dThisIterSandFlowDeposit,
      m_dThisIterClaySedLost,
      m_dThisIterSiltSedLost,
      m_dThisIterSandSedLost,
      m_dThisIterClaySplashDetach,
      m_dThisIterSiltSplashDetach,
      m_dThisIterSandSplashDetach,
      m_dThisIterClaySplashDeposit,
      m_dThisIterSiltSplashDeposit,
      m_dThisIterSandSplashDeposit,
      m_dThisIterClaySplashToSedLoad,
      m_dThisIterSiltSplashToSedLoad,
      m_dThisIterSandSplashToSedLoad,
      m_dSinceLastClaySlumpDetach,
      m_dSinceLastSiltSlumpDetach,
      m_dSinceLastSandSlumpDetach,
      m_dSinceLastClaySlumpDeposit,
      m_dSinceLastSiltSlumpDeposit,
      m_dSinceLastSandSlumpDeposit,
      m_dSinceLastClaySlumpToSedLoad,
      m_dSinceLastSiltSlumpToSedLoad,
      m_dSinceLastSandSlumpToSedLoad,
      m_dSinceLastClayToppleDetach,
      m_dSinceLastSiltToppleDetach,
      m_dSinceLastSandToppleDetach,
      m_dSinceLastClayToppleDeposit,
      m_dSinceLastSiltToppleDeposit,
      m_dSinceLastSandToppleDeposit,
      m_dSinceLastClayToppleToSedLoad,
      m_dSinceLastSiltToppleToSedLoad,
      m_dSinceLastSandToppleToSedLoad,
      m_dThisIterInfiltration,
      m_dThisIterClayInfiltDeposit,
      m_dThisIterSiltInfiltDeposit,
      m_dThisIterSandInfiltDeposit,
      m_dSinceLastClayHeadcutDetach,
      m_dSinceLastSiltHeadcutDetach,
      m_dSinceLastSandHeadcutDetach,
      m_dSinceLastClayHeadcutDeposit,
      m_dSinceLastSiltHeadcutDeposit,
      m_dSinceLastSandHeadcutDeposit,
      m_dSinceLastClayHeadcutToSedLoad,
      m_dSinceLastSiltHeadcutToSedLoad,
      m_dSinceLastSandHeadcutToSedLoad,
      m_dThisIterExfiltration,
      m_dLastIterAvgHead,
      m_dThisIterTotHead,
      m_dSimulationDuration,           // Duration of simulation in secs
      m_dSimulatedRainDuration,        // Duration of simulated rainfall, secs
      m_dTimeStep,
      m_dSimulatedTimeElapsed,         // Simulated time elapsed in secs
      m_dRSaveTime,
      m_dRSaveInterval,
      m_dSplashCalcLast,
      m_dClkLast,                      // Last value returned by clock()
      m_dCPUClock,                     // Total elapsed CPU time
      m_dTargetGTotDrops,
      m_dGeoTransform[6],
      m_dMissingValue,
      m_dWaterCorrection,
      m_dClayDiameter,
      m_dSiltDiameter,
      m_dSandDiameter,
      m_dDepositionGrainDensity,
      m_dDensityDiffExpression,
      m_dClaySizeMin,
      m_dClaySiltBoundarySize,
      m_dSiltSandBoundarySize,
      m_dSandSizeMax,
      m_dLastSlumpCalcTime,
      m_dLastHeadcutRetreatCalcTime,
      m_dSinceLastTSWriteInfiltration,
      m_dSinceLastTSWriteExfiltration,
      m_dSinceLastTSWriteClayInfiltDeposit,
      m_dSinceLastTSWriteSiltInfiltDeposit,
      m_dSinceLastTSWriteSandInfiltDeposit,
      m_dSinceLastTSWriteClaySplashRedist,
      m_dSinceLastTSWriteSiltSplashRedist,
      m_dSinceLastTSWriteSandSplashRedist,
      m_dSinceLastTSWriteKE,
      m_dSinceLastTSWriteClaySlumpDetach,
      m_dSinceLastTSWriteSiltSlumpDetach,
      m_dSinceLastTSWriteSandSlumpDetach,
      m_dSinceLastTSWriteClayToppleDetach,
      m_dSinceLastTSWriteSiltToppleDetach,
      m_dSinceLastTSWriteSandToppleDetach,
      m_dHeadcutRetreatConst,
      m_dOffEdgeConst,
      m_dSSSPatchSize,
      m_dOffEdgeParamA,
      m_dOffEdgeParamB,
      m_dManningParamA,
      m_dManningParamB,
      m_dFFConstant,
      m_dFFReynoldsParamA,
      m_dFFReynoldsParamB,
      m_dFFLawrenceD50,
      m_dFFLawrenceEpsilon,
      m_dFFLawrencePr,
      m_dFFLawrenceCd,
      m_dWaterErrorLast,
      m_dSplashErrorLast,
      m_dSlumpErrorLast,
      m_dToppleErrorLast,
      m_dHeadcutErrorLast,
      m_dFlowErrorLast;

   // These grand totals are all long doubles, the aim is to minimize rounding errors when many very small numbers are added to a single much larger number, see e.g. http://www.ddj.com/cpp/184403224
   long double
      m_ldGTotDrops,
      m_ldGTotRunOnDrops,
      m_ldGTotRain,                    // This and all below are as a volume
      m_ldGTotRunOn,
      m_ldGTotInfilt,
      m_ldGTotExfilt,
      m_ldGTotWaterLost,
      m_ldGTotFlowDetach,
      m_ldGTotFlowDeposit,
      m_ldGTotSedLost,
      m_ldGTotSplashDetach,
      m_ldGTotSplashDeposit,
      m_ldGTotSplashToSedLoad,
      m_ldGTotSlumpDetach,
      m_ldGTotSlumpDeposit,
      m_ldGTotSlumpToSedLoad,
      m_ldGTotToppleDetach,
      m_ldGTotToppleDeposit,
      m_ldGTotToppleToSedLoad,
      m_ldGTotInfiltDeposit,
      m_ldGTotHeadcutRetreatDetach,
      m_ldGTotHeadcutRetreatDeposit,
      m_ldGTotHeadcutRetreatToSedLoad;

   string
      m_strRGDir,
      m_strRGIni,
      m_strMailAddress,
      m_strDataPathName,
      m_strDEMFile,
      m_strRainVarMFile,
      m_strGISOutFormat,
      m_strSplshFile,
      m_strLogFile,
      m_strOutputPath,
      m_strOutFile,
      m_strPalFile,
      m_strRainTSFile,
      m_strGDALDEMDriverCode,
      m_strGDALDEMDriverDesc,
      m_strGDALDEMProjection,
      m_strGDALDEMDataType,
      m_strGDALRainVarDriverCode,
      m_strGDALRainVarDriverDesc,
      m_strGDALRainVarProjection,
      m_strGDALRainVarDataType,
      m_strGDALOutputDriverLongname,
      m_strRunName,
      m_strGDALOutputDriverExtension;

   vector<double>
      m_VdSaveTime,
      m_VdSplashDepth,
      m_VdSplashEff,
      m_VdSplashEffCoeff,
      m_VdRainChangeTime,
      m_VdRainChangeIntensity,
      m_VdInputSoilLayerThickness,
      m_VdInputSoilLayerPerCentClay,
      m_VdInputSoilLayerPerCentSilt,
      m_VdInputSoilLayerPerCentSand,
      m_VdInputSoilLayerBulkDensity,
      m_VdInputSoilLayerClayFlowErodibility,
      m_VdInputSoilLayerSiltFlowErodibility,
      m_VdInputSoilLayerSandFlowErodibility,
      m_VdInputSoilLayerClaySplashErodibility,
      m_VdInputSoilLayerSiltSplashErodibility,
      m_VdInputSoilLayerSandSplashErodibility,
      m_VdInputSoilLayerClaySlumpErodibility,
      m_VdInputSoilLayerSiltSlumpErodibility,
      m_VdInputSoilLayerSandSlumpErodibility,
      m_VdInputSoilLayerInfiltAirHead,
      m_VdInputSoilLayerInfiltLambda,
      m_VdInputSoilLayerInfiltSatWater,
      m_VdInputSoilLayerInfiltInitWater,
      m_VdInputSoilLayerInfiltKSat,
      m_VdInfiltCPHWF,
      m_VdInfiltChiPart,
      m_VdThisIterSoilWater,
      m_VdSinceLastTSSoilWater;

   vector<string>
      m_VstrInputSoilLayerName;

   struct RandState
   {
      unsigned long s1, s2, s3;
   } m_ulRState[NUMBER_OF_RNGS];

   time_t
      m_tSysStartTime,
      m_tSysEndTime;

   ofstream
      m_ofsOut,
      m_ofsLog,
      m_ofsErrorTS,
      m_ofsTimestepTS,
      m_ofsAreaWetTS,
      m_ofsRainTS,
      m_ofsInfiltTS,
      m_ofsExfiltTS,
      m_ofsRunOnTS,
      m_ofsSurfaceWaterTS,
      m_ofsSurfaceWaterLostTS,
      m_ofsFlowDetachTS,
      m_ofsSlumpDetachTS,
      m_ofsToppleDetachTS,
      m_ofsFlowDepositSS,
      m_ofsSedLostTS,
      m_ofsSedLoadTS,
      m_ofsInfiltDepositTS,
      m_ofsSplashDetachTS,
      m_ofsSplashDepositTS,
      m_ofsSplashKETS,
      m_ofsSoilWaterTS;

   CCell** Cell;                       // Pointer to 2D array of soil cell objects
   double** m_SSSWeightQuadrant;       // Pointer to 2D array for weights for soil shear stress spatial distribution, used for slumping

private:
   // Initialization
   static void AnnounceStart(void);
   void StartClock(void);
   bool bFindExeDir(char* pcArg);
   void AnnounceLicence(void);
   bool bReadIni(void);
   bool bReadRunData(void);
   bool bOpenLogFile(void);
   bool bSetUpTSFiles(void);
   void AnnounceReadDEM(void) const;
   static void AnnounceAllocateMemory(void);
   void AnnounceReadRainVar(void) const;
   void WriteRunDetails(void);
   static void AnnounceIsRunning(void);
   void CalcGradient(void);
   void InitSoilWater(void);
   static void AnnounceSimEnd(void);
   int nInitSlumping(void);

   // Input and output
   int nHandleCommandLineParams(int, char*[]);
   bool bCheckGISOutputFormat(void);
   int nReadMicrotopographyDEMData(void);
   int nReadRainVarData(void);
   bool bReadSplashEffData(void);
   bool bReadRainfallTimeSeries(void);
   bool bSaveGISFiles(void);
   bool bWriteGISFileFloat(int const, string const*);
   bool bWriteGISFileInt(int const, string const*);
   bool bWritePerIterationResults(void);
   bool bWriteTSFiles(bool const);
   int nWriteFilesAtEnd(void);
   void WriteGrandTotals(void);

   // Soil
   void CreateSoilLayers(void);

   // Simulation routines
   int nDoSimulation(void);
   void CalcTimestep(void);
   void MarkEdgeCells(void);
   void DoRunOnFromOneEdge(int const);
   void DoAllRain(void);
   void DoAllFlowRouting(void);
   void DoAllInfiltration(void);
   void DoAllSplash(void);
   void DoAllSlump(void);
   void DoAllHeadcutRetreat(void);

   // Lower-level simulation routines
   void TryCellOutFlow(int const, int const);
   void TryEdgeCellOutFlow(int const, int const, int const);
   int nFindSteepestEnergySlope(int const, int const, double const, int&, int&, double&, double&, double&);
   void CellMoveWater(int const, int const, int const, int const, double const&, double&);
   double dTimeToCrossCell(int const, int const, int const, double const, double, double const, C2DVec&, double&);
   double dCalcHydraulicRadius(int const, int const);
   double dCalcLawrenceFrictionFactor(int const, int const, double const, bool const);
   void CalcTransportCapacity(int const, int const, int const, int const, int const, double const, double const, double const, double const, double const, double const);
   void DoCellErosion(int const, int const, int const, int const, int const, double const, double const, double const, double const, double const);
   void DoCellSedLoadDeposit(int const, int const, double const, double const, double const);
   double dCalcSplashCubicSpline(double) const;
   double dCalcLaplacian(int const, int const);
   int nFindSteepestSoilSurface(int const, int const, double const, int&, int&, double&, bool&);
   void TryToppleCellsAbove(int const, int const, int);
   void DoToppleCells(int const, int const, int const, int const, double, bool const);
   void DoCellInfiltration(int const, int const, int const, CCellSoilLayer*, double const);
   void DoCellExfiltration(int const, int const, int const, CCellSoilLayer*, double const);
   void DoHeadcutRetreatMoveSoil(int const, int const, int const, int const, int const, double const);
   void DoDistributeShearStress(int const, int const, double const);
   double dGetReynolds(int const, int const);

   // Utility
   bool bIsTimeToQuit(void);
   bool bSetUpRainfallIntensity(void);
   void CheckMassBalance(void);
   int nCheckForInstability(void);
   void UpdateGrandTotals(void);
   void AdjustUnboundedEdges(void);
   static string strGetBuild(void);
   static string strGetComputerName(void);
   void DoCPUClockReset(void);
   void CalcTime(double const);
   void AnnounceProgress(void);
   static string strDispTime(double const, bool const, bool const);
   static char const* pszGetErrorText(int const);
   void WrapLongString(string*);
   void CheckLawrenceFF(void);
   void CheckSplashEff(void);
   void InitSplashEff(void);
#if defined RANDCHECK
   void CheckRand(void) const;
#endif
   void CalcProcessStats(void);
   static string strTrim(string const*);
   static string strTrimLeft(string const*);
   static string strTrimRight(string const*);
   static string strToLower(string const*);
   static string strRemoveSubstr(string*, string const*);
   static vector<string>* VstrSplit(string const*, char const, vector<string>*);
   static vector<string> VstrSplit(string const*, char const);
   string strSplitRH(string const*) const;

   // Random number stuff
   static unsigned long ulGetTausworthe(unsigned long const, unsigned long const, unsigned long const, unsigned long const, unsigned long const);
   void InitRand0(unsigned long);
   void InitRand1(unsigned long);
   unsigned long ulGetRand0(void);
   unsigned long ulGetRand1(void);
   static unsigned long ulGetLCG(unsigned long const);            // used by all generators
   double dGetRand0d1(void);
   int nGetRand0To(int const);
   int nGetRand1To(int const);
   double dGetRand0GaussPos(double const, double const);
   double dGetRand0Gaussian(void);
   static double dGetCGaussianPDF(double const);

public:
   CSimulation(void);
   ~CSimulation(void);

   int nDoRun(int, char*[]);
   void DoEndRun(int const);

   int nCalcOppositeDirection(int const) const;

   double dGetTimeStep(void) const;
   double dGetMissingValue(void) const;
   double dGetCellSide(void) const;
   double dGetCellSideDiag(void) const;

   double dGetRandGaussian(void);
};

#endif                  // __SIMULATION_H__
