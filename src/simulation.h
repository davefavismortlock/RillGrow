#ifndef __SIMULATION_H__
   #define __SIMULATION_H__

/*=========================================================================================================================================

 This is simulation.h: this class runs RillGrow simulations

 Copyright (C) 2018 David Favis-Mortlock

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
class CLayer;

class CSimulation
{
private:
   bool
      m_bSlosSave,
      m_bRainVarMSave,
      m_bRunOnSave,
      m_bElevSave,
      m_bInitElevSave,
      m_bDetrendElevSave,
      m_bInfiltSave,
      m_bTotInfiltSave,
      m_bSoilWaterSave,
      m_bInfiltDepositSave,
      m_bTotInfiltDepositSave,
      m_bTopSurfaceSave,
      m_bSplashSave,
      m_bTotSplashSave,
      m_bInundationSave,
      m_bFlowDirSave,
      m_bStreamPowerSave,
      m_bShearStressSave,
      m_bFrictionFactorSave,
      m_bAvgShearStressSave,
      m_bReynoldsSave,
      m_bFroudeSave,
      m_bAvgDepthSave,
      m_bAvgDWSpdSave,
      m_bAvgSpdSave,
      m_bSedConcSave,
      m_bSedLoadSave,
      m_bAvgSedLoadSave,
      m_bTotDepositSave,
      m_bTotNetSlosSave,
      m_bNetSlumpSave,
      m_bNetToppleSave,
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
      m_bDoFlowDepositionTS,
      m_bSedLostTS,
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
      m_bRunOnThisEdge[4];

#if defined _DEBUG
      bool m_bLostSave;
#endif

   int
      m_nGISSave,
      m_nUSave,
      m_nThisSave,
      m_nXGridMax,
      m_nYGridMax,
      m_nHeaderSize,
      m_nRainChangeTimeMax,
      m_nZUnits,
      m_nNumSoilLayers;

   unsigned long
      m_ulIter,
      m_ulTotIter,
      m_ulRandSeed[NUMBER_OF_RNGS],
      m_ulVarChkStart,
      m_ulVarChkEnd,
      m_ulNActiveCells,
      m_ulNWet,
      m_ulMissingValueCells;

   double
      m_dMinX,
      m_dMaxX,
      m_dMinY,
      m_dMaxY,
      m_dMaxSpeed,
      m_dPossibleMaxSpeed,
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
      m_dSplashVertical,
      m_dClaySplashedError,
      m_dSiltSplashedError,
      m_dSandSplashedError,
      m_dCellSizeKC,
      m_dMeanCellWaterVol,
      m_dStdCellWaterVol,
      m_dCellSide,
      m_dCellDiag,
      m_dInvCellSide,
      m_dInvCellDiag,
      m_dCellSquare,
      m_dInvCellSquare,
      m_dInvXGridMax,
      m_dRoughnessScaling,
      m_dD50,
      m_dEpsilon,
      m_dPr,
      m_dCd,
      m_dRho,
      m_dG,
      m_dNu,
      m_dK,
      m_dT,
      m_dCVT,
      m_dCVTaub,
      m_dST2,
      m_dSourceDestCritAngle,
      m_dBulkDensityForOutputCalcs,
      m_dAlpha,
      m_dBeta,
      m_dGamma,
      m_dDelta,
      m_dPrevAdj,
      m_dRunOnLen,
      m_dRunOnSpd,
      m_dSlumpCritShearStress,
      m_dSlumpAngleOfRest,
      m_dSlumpAngleOfRestDiff,
      m_dSlumpAngleOfRestDiffDiag,
      m_dToppleCriticalAngle,
      m_dToppleCritDiff,
      m_dToppleCritDiffDiag,
      m_dToppleAngleOfRest,
      m_dToppleAngleOfRestDiff,
      m_dToppleAngleOfRestDiffDiag,
      m_dThisIterSurfaceWaterStored,
      m_dThisIterClaySedLoad,
      m_dThisIterSiltSedLoad,
      m_dThisIterSandSedimentLoad,
      m_dThisIterRain,
      m_dThisIterRunOn,
      m_dThisIterKE,
      m_dThisIterWaterLost,
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
      m_dThisIterClaySlumpDetach,
      m_dThisIterSiltSlumpDetach,
      m_dThisIterSandSlumpDetach,
      m_dThisIterClaySlumpDeposit,
      m_dThisIterSiltSlumpDeposit,
      m_dThisIterSandSlumpDeposit,
      m_dThisIterClayToppleDetach,
      m_dThisIterSiltToppleDetach,
      m_dThisIterSandToppleDetach,
      m_dThisIterClayToppleDeposit,
      m_dThisIterSiltToppleDeposit,
      m_dThisIterSandToppleDeposit,      
      m_dThisIterInfiltration,
      m_dThisIterClayInfiltDeposit,
      m_dThisIterSiltInfiltDeposit,
      m_dThisIterSandInfiltDeposit,
      m_dThisIterExfiltration,
      m_dLastIterMaxHead,
      m_dLastIterMinHead,
      m_dThisIterMaxHead,
      m_dThisIterMinHead,
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
      m_dSinceLastTSInfiltration,
      m_dSinceLastTSExfiltration,
      m_dSinceLastTSClayInfiltDeposit,
      m_dSinceLastTSSiltInfiltDeposit,
      m_dSinceLastTSSandInfiltDeposit,
      m_dSinceLastTSClaySplashRedist,
      m_dSinceLastTSSiltSplashRedist,
      m_dSinceLastTSSandSplashRedist,
      m_dSinceLastTSKE,
      m_dSinceLastTSClaySlumpDetach,
      m_dSinceLastTSSiltSlumpDetach,
      m_dSinceLastTSSandSlumpDetach,
      m_dSinceLastTSClayToppleDetach,
      m_dSinceLastTSSiltToppleDetach,
      m_dSinceLastTSSandToppleDetach;
      
   // These grand totals are all long doubles, the aim is to minimize rounding errors when many very small numbers are added to a single much larger number, see e.g. http://www.ddj.com/cpp/184403224
   long double
      m_ldGTotDrops,
      m_ldGTotRunOnDrops,
      m_ldGTotRain,                    // This and all below are as a volume
      m_ldGTotRunOnWater,
      m_ldGTotInfilt,
      m_ldGTotExfilt,
      m_ldGTotWaterLost,
      m_ldGTotFlowDetach,
      m_ldGTotFlowDeposition,
      m_ldGTotSedLost,
      m_ldGTotSplashDetach,
      m_ldGTotSplashDeposit,
      m_ldGTotSlumpDetach,
      m_ldGTotToppleDetach,
      m_ldGTotToppleDeposit,
      m_ldGTotInfiltDeposit;

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
      OutStream,
      LogStream,
      TimeStepTSStream,
      AreaWetTSStream,
      RainTSStream,
      InfiltrationTSStream,
      ExfiltrationTSStream,
      RunOnTSStream,
      SurfaceWaterTSStream,
      SurfaceWaterLostTSStream,
      FlowDetachmentTSStream,
      SlumpDetachmentTSStream,
      ToppleDetachmentTSStream,
      FlowDepositionTSStream,
      SedimentLostTSStream,
      SedimentLoadTSStream,
      InfiltrationDepositionTSStream,
      SplashDetachmentTSStream,
      SplashDepositionTSStream,
      SplashKineticEnergyTSStream,
      SoilWaterTSStream;

   CCell** Cell;           // Array of soil cell objects

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
   
   // Input and output
   int nHandleCommandLineParams(int, char*[]);
   bool bCheckGISOutputFormat(void);
   int nReadMicrotopographyDEMData(void);
   int nReadRainVarData(void);
   bool bReadSplashEffData(void);
   bool bReadRainfallTimeSeries(void);
   bool bSaveGISFiles(void);
   bool bWriteFileFloat(int const, string const*);
   bool bWriteFileInt(int const, string const*);
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
   
   // Lower-level simulation routines
   void TryCellOutFlow(int const, int const);
   void TryEdgeCellOutFlow(int const, int const, int const);
   int nFindSteepestEnergySlope(int const, int const, double const, int&, int&, double&, double&, double&);
   void CellMoveWater(int const, int const, int const, int const, double const&, double const&);
   double dTimeToCrossCell(int const, int const, int const, double const, double, double const, C2DVec&, double&);
   double dCalcHydraulicRadius(int const, int const, int const, double const);
   double dCalcFrictionFactor(int const, int const, double const, bool const);
   void CalcTransportCapacity(int const, int const, int const, int const, double const, double const, double const, double const, double const, double const);
   void DoCellErosion(int const, int const, int const, int const, double const, double const, double const, double const, double const);
   void DoCellDeposition(int const, int const, double const, double const, double const);
   double dCalcSplashCubicSpline(double) const;
   double dCalcLaplacian(int const, int const);
   int nFindSteepestSoilSurface(int const, int const, double const, int&, int&, double&, bool&);
   void TryToppleCellsAbove(int const, int const, int);
   void DoToppleCells(int const, int const, int const, int const, double, bool const);
   void DoCellInfiltration(int const, int const, int const, CLayer*, double const);
   void DoCellExfiltration(int const, int const, int const, CLayer*, double const);
   
   // Utility
   bool bTimeToQuit(void);
   bool bSetUpRainfallIntensity(void);
   void RemoveCumulativeRoundingErrors(void);
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
   void CheckFF(void);
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
   
   double dGetTimeStep(void) const;
   
   double dGetMissingValue(void) const;
   
   void IncrThisIterNumWetCells(void);
   void DecrThisIterNumWetCells(void);
   
   void AddThisIterSurfaceWater(double const);
   
   void AddThisIterClaySedimentLoad(double const);
   void AddThisIterSiltSedimentLoad(double const);
   void AddThisIterSandSedimentLoad(double const);
   
   void AddThisIterClayFlowDetach(double const);
   void AddThisIterSiltFlowDetach(double const);
   void AddThisIterSandFlowDetach(double const);
   
   void AddThisIterClayFlowDeposit(double const);
   void AddThisIterSiltFlowDeposit(double const);
   void AddThisIterSandFlowDeposit(double const);
   
   void AddThisIterClaySplashDetach(double const);
   void AddThisIterSiltSplashDetach(double const);
   void AddThisIterSandSplashDetach(double const);
   
   void AddThisIterClaySplashDeposit(double const);
   void AddThisIterSiltSplashDeposit(double const);
   void AddThisIterSandSplashDeposit(double const);
   
   void AddThisIterClaySlumpDetach(double const);
   void AddThisIterSiltSlumpDetach(double const);
   void AddThisIterSandSlumpDetach(double const);
   
   void AddThisIterClayToppleDetach(double const);
   void AddThisIterSiltToppleDetach(double const);
   void AddThisIterSandToppleDetach(double const);
};

#endif                  // __SIMULATION_H__
