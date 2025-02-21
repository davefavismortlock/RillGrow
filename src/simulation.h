#ifndef __SIMULATION_H__
   #define __SIMULATION_H__

/*=========================================================================================================================================

This is simulation.h: this class runs RillGrow simulations

Copyright (C) 2025 David Favis-Mortlock

==========================================================================================================================================

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
class CCell;            // Forward declarations
class C2DVec;
class CCellSoilLayer;

class CSimulation
{
private:
   //! Are we saving flow detachment as a GIS file?
   bool m_bFlowDetachSave;

   //! Are we saving rainfall spatial variation as a GIS file?
   bool m_bRainVarMSave;
   bool m_bCumulRunOnSave;
   bool m_bElevSave;
   bool m_bInitElevSave;
   bool m_bDetrendElevSave;
   bool m_bInfiltSave;
   bool m_bCumulInfiltSave;
   bool m_bSoilWaterSave;
   bool m_bInfiltDepositSave;
   bool m_bCumulInfiltDepositSave;
   bool m_bTopSurfaceSave;
   bool m_bSplashSave;
   bool m_bCumulSplashSave;
   bool m_bInundationSave;
   bool m_bFlowDirSave;
   bool m_bStreamPowerSave;
   bool m_bShearStressSave;
   bool m_bFrictionFactorSave;
   bool m_bCumulAvgShearStressSave;
   bool m_bReynoldsSave;
   bool m_bFroudeSave;
   bool m_bCumulAvgDepthSave;
   bool m_bCumulAvgDWSpdSave;
   bool m_bCumulAvgSpdSave;
   bool m_bSedConcSave;
   bool m_bSedLoadSave;
   bool m_bAvgSedLoadSave;
   bool m_bCumulFlowDepositSave;
   bool m_bCumulLoweringSave;
   bool m_bSlumpSave;
   bool m_bToppleSave;
   bool m_bTCSave;
   bool m_bSaveRegular;
   bool m_bDetrend;
   bool m_bDoInfiltration;
   bool m_bInfiltThisIter;
   bool m_bFFCheck;
   bool m_bSplash;
   bool m_bPoesenSplashEqn;
   bool m_bPlanchonSplashEqn;
   bool m_bSplashThisIter;
   bool m_bSplashCheck;
   bool m_bFlowErosion;
   bool m_bRunOn;
   bool m_bTimeVaryingRain;
   bool m_bGISOutDiffers;
   bool m_bSlumping;
   bool m_bSlumpThisIter;
   bool m_bHeadcutRetreat;
   bool m_bHeadcutRetreatThisIter;
   bool m_bTimeStepTS;
   bool m_bAreaWetTS;
   bool m_bRainTS;
   bool m_bInfiltTS;
   bool m_bExfiltTS;
   bool m_bRunOnTS;
   bool m_bSurfaceWaterTS;
   bool m_bSurfaceWaterLostTS;
   bool m_bFlowDetachTS;
   bool m_bSlumpDetachTS;
   bool m_bToppleDetachTS;
   bool m_bDoSedLoadDepositTS;
   bool m_bSedOffEdgeTS;
   bool m_bSedLoadTS;
   bool m_bInfiltDepositTS;
   bool m_bSplashRedistTS;
   bool m_bSplashKETS;
   bool m_bSoilWaterTS;
   bool m_bSaveGISThisIter;
   bool m_bThisIterRainChange;
   bool m_bHaveBaseLevel;
   bool m_bOutDEMsUsingInputZUnits;
   bool m_bClosedThisEdge[4];
   bool m_bRunOnThisEdge[4];
   bool m_bManningFlowSpeedEqn;
   bool m_bDarcyWeisbachFlowSpeedEqn;
   bool m_bFrictionFactorConstant;
   bool m_bFrictionFactorReynolds;
   bool m_bFrictionFactorLawrence;
   bool m_bFrictionFactorCheng;
   bool m_bLostSave;
   bool m_bSplashForward;
   bool m_bSettlingEqnCheng;
   bool m_bSettlingEqnFergusonChurch;
   bool m_bSettlingEqnStokesBudryckRittinger;

   int m_nGISSave;
   int m_nUSave;
   int m_nThisSave;
   int m_nXGridMax;
   int m_nYGridMax;
   int m_nHeaderSize;
   int m_nRainChangeTimeMax;
   int m_nZUnits;
   int m_nNumSoilLayers;
   int m_nSSSQuadrantSize;
   int m_nTimeVaryingRainCount;
   int m_nInfiltCount;
   int m_nSlumpCount;
   int m_nHeadcutRetreatCount;

   unsigned long m_ulIter;
   unsigned long m_ulTotIter;
   unsigned long m_ulRandSeed[NUMBER_OF_RNGS];
   unsigned long m_ulVarChkStart;
   unsigned long m_ulVarChkEnd;
   unsigned long m_ulNActiveCells;
   unsigned long m_ulNWet;
   unsigned long m_ulMissingValueCells;
   unsigned long m_ulNumHead;

   double m_dMinX;
   double m_dMaxX;
   double m_dMinY;
   double m_dMaxY;
   double m_dMaxFlowSpeed;
   double m_dPossMaxSpeedNextIter;
   double m_dBasementElevation;
   double m_dAvgElev;
   double m_dMinElev;
   double m_dGradient;
   double m_dPlotEndDiff;
   double m_dPlotElevMin;
   double m_dPlotElevMax;
   double m_dBaseLevel;
   double m_dInvCos45;
   double m_dYInc;
   double m_dRunOnRainVarM;
   double m_dRainIntensity;
   double m_dSpecifiedRainIntensity;
   double m_dRainVarMFileMean;
   double m_dStdRainInt;
   double m_dDropDiameter;
   double m_dStdDropDiameter;
   double m_dRainSpeed;
   double m_dPartKE;
   double m_dSplashConstant;
   double m_dSplashConstantNormalized;
   double m_dPoesenSplashConstant;
   double m_dPlanchonCellSizeKC;
   double m_dMeanCellWaterVol;
   double m_dStdCellWaterVol;

   //! Length of cell side, in mm
   double m_dCellSide;
   double m_dCellDiag;
   double m_dInvCellSide;
   double m_dInvCellDiag;
   double m_dCellSquare;
   double m_dInvCellSquare;
   double m_dInvXGridMax;
   double m_dRho;
   double m_dG;
   double m_dNu;
   double m_dK;
   double m_dT;
   double m_dCVT;
   double m_dCVTaub;
   double m_dST2;
   double m_dBulkDensityForOutputCalcs;
   double m_dAlpha;
   double m_dBeta;
   double m_dGamma;
   double m_dDelta;
   double m_dRunOnLen;
   double m_dRunOnSpd;
   double m_dCritSSSForSlump;
   double m_dSlumpAngleOfRest;
   double m_dSlumpAngleOfRestDiff;
   double m_dSlumpAngleOfRestDiffDiag;
   double m_dToppleCriticalAngle;
   double m_dToppleCritDiff;
   double m_dToppleCritDiffDiag;
   double m_dToppleAngleOfRest;
   double m_dToppleAngleOfRestDiff;
   double m_dToppleAngleOfRestDiffDiag;
   double m_dEndOfIterTotSurfaceWater;
   double m_dEndOfIterRain;
   double m_dEndOfIterRunOn;
   double m_dEndOfIterKE;
   double m_dEndOfIterSurfaceWaterOffEdge;
   double m_dEndOfIterClaySedLoadOffEdge;
   double m_dEndOfIterSiltSedLoadOffEdge;
   double m_dEndOfIterSandSedLoadOffEdge;
   double m_dEndOfIterClaySplashDetach;
   double m_dEndOfIterSiltSplashDetach;
   double m_dEndOfIterSandSplashDetach;
   double m_dEndOfIterClaySplashToSedLoad;
   double m_dEndOfIterSiltSplashToSedLoad;
   double m_dEndOfIterSandSplashToSedLoad;
   double m_dEndOfIterClaySplashOffEdge;
   double m_dEndOfIterSiltSplashOffEdge;
   double m_dEndOfIterSandSplashOffEdge;
   double m_dEndOfIterNetClayDetachment;
   double m_dEndOfIterNetSiltDetachment;
   double m_dEndOfIterNetSandDetachment;
   double m_dEndOfIterClayFlowDetach;
   double m_dEndOfIterSiltFlowDetach;
   double m_dEndOfIterSandFlowDetach;
   double m_dEndOfIterClayFlowDeposit;
   double m_dEndOfIterSiltFlowDeposit;
   double m_dEndOfIterSandFlowDeposit;
   double m_dEndOfIterClaySplashDeposit;
   double m_dEndOfIterSiltSplashDeposit;
   double m_dEndOfIterSandSplashDeposit;
   double m_dEndOfIterClaySlumpDetach;
   double m_dEndOfIterSiltSlumpDetach;
   double m_dEndOfIterSandSlumpDetach;
   double m_dEndOfIterClaySlumpDeposit;
   double m_dEndOfIterSiltSlumpDeposit;
   double m_dEndOfIterSandSlumpDeposit;
   double m_dEndOfIterClaySlumpToSedLoad;
   double m_dEndOfIterSiltSlumpToSedLoad;
   double m_dEndOfIterSandSlumpToSedLoad;
   double m_dEndOfIterClayToppleDetach;
   double m_dEndOfIterSiltToppleDetach;
   double m_dEndOfIterSandToppleDetach;
   double m_dEndOfIterClayToppleDeposit;
   double m_dEndOfIterSiltToppleDeposit;
   double m_dEndOfIterSandToppleDeposit;
   double m_dEndOfIterClayToppleToSedLoad;
   double m_dEndOfIterSiltToppleToSedLoad;
   double m_dEndOfIterSandToppleToSedLoad;
   double m_dEndOfIterInfiltration;
   double m_dEndOfIterExfiltration;
   double m_dEndOfIterClayInfiltDeposit;
   double m_dEndOfIterSiltInfiltDeposit;
   double m_dEndOfIterSandInfiltDeposit;
   double m_dEndOfIterClayHeadcutDetach;
   double m_dEndOfIterSiltHeadcutDetach;
   double m_dEndOfIterSandHeadcutDetach;
   double m_dEndOfIterClayHeadcutDeposit;
   double m_dEndOfIterSiltHeadcutDeposit;
   double m_dEndOfIterSandHeadcutDeposit;
   double m_dEndOfIterClayHeadcutToSedLoad;
   double m_dEndOfIterSiltHeadcutToSedLoad;
   double m_dEndOfIterSandHeadcutToSedLoad;
   double m_dLastIterAvgHead;
   double m_dEndOfIterTotHead;
   double m_dStartOfIterTotElev;
   double m_dEndOfIterTotElev;
   double m_dStartOfIterTotSurfaceWater;
   double m_dStartOfIterTotSoilWater;
   double m_dEndOfIterTotSoilWater;
   double m_dStartOfIterTotClaySedLoad;
   double m_dStartOfIterTotSiltSedLoad;
   double m_dStartOfIterTotSandSedLoad;
   double m_dEndOfIterClaySedLoad;
   double m_dEndOfIterClaySedLoadKahanCorrection;
   double m_dEndOfIterSiltSedLoad;
   double m_dEndOfIterSiltSedLoadKahanCorrection;
   double m_dEndOfIterSandSedLoad;
   double m_dEndOfIterSandSedLoadKahanCorrection;

   //! Duration of simulation, in secs
   double m_dSimulationDuration;

   //! Duration of simulated rainfall, secs
   double m_dSimulatedRainDuration;
   double m_dTimeStep;

   //! Simulated time elapsed, in secs
   double m_dSimulatedTimeElapsed;
   double m_dRSaveTime;
   double m_dRSaveInterval;
   double m_dSplashCalcLast;

   //! Last value returned by clock()
   double m_dClkLast;

   //! Total elapsed CPU time
   double m_dCPUClock;
   double m_dTargetGTotDrops;
   double m_dGeoTransform[6];
   double m_dMissingValue;
   double m_dWaterCorrection;
   double m_dClayDiameter;
   double m_dSiltDiameter;
   double m_dSandDiameter;
   double m_dDepositionGrainDensity;
   double m_dDensityDiffExpression;
   double m_dClaySizeMin;
   double m_dClaySiltBoundarySize;
   double m_dSiltSandBoundarySize;
   double m_dSandSizeMax;
   double m_dLastSlumpCalcTime;
   double m_dLastHeadcutRetreatCalcTime;
   double m_dEndOfIterTSWriteInfiltration;
   double m_dEndOfIterTSWriteExfiltration;
   double m_dEndOfIterTSWriteClayInfiltDeposit;
   double m_dEndOfIterTSWriteSiltInfiltDeposit;
   double m_dEndOfIterTSWriteSandInfiltDeposit;
   double m_dEndOfIterTSWriteClaySplashRedist;
   double m_dEndOfIterTSWriteSiltSplashRedist;
   double m_dEndOfIterTSWriteSandSplashRedist;
   double m_dEndOfIterTSWriteKE;
   double m_dEndOfIterTSWriteClaySlumpDetach;
   double m_dEndOfIterTSWriteSiltSlumpDetach;
   double m_dEndOfIterTSWriteSandSlumpDetach;
   double m_dEndOfIterTSWriteClayToppleDetach;
   double m_dEndOfIterTSWriteSiltToppleDetach;
   double m_dEndOfIterTSWriteSandToppleDetach;
   double m_dHeadcutRetreatConst;
   double m_dOffEdgeHeadConst;
   double m_dSSSPatchSize;
   double m_dOffEdgeParamA;
   double m_dOffEdgeParamB;
   double m_dManningParamA;
   double m_dManningParamB;
   double m_dFFConstant;
   double m_dFFReynoldsParamA;
   double m_dFFReynoldsParamB;
   double m_dFFLawrenceD50;
   double m_dFFLawrenceEpsilon;
   double m_dFFLawrencePr;
   double m_dFFLawrenceCd;
   double m_dChengRoughnessHeight;
   double m_dWaterErrorLast;
   double m_dSplashErrorLast;
   double m_dSlumpErrorLast;
   double m_dToppleErrorLast;
   double m_dHeadcutErrorLast;
   double m_dFlowErrorLast;
   double m_dElapsed;
   double m_dStillToGo;
   double m_dWaterStoredLast;
   double m_dSedimentLoadDepthLast;

   //! Settling speed of clay-sized sediment, in mm/sec
   double m_dClaySettlingSpeed;

   //! Settling speed of silt-sized sediment, in mm/sec
   double m_dSiltSettlingSpeed;

   //! Settling speed of sand-sized sediment, in mm/sec
   double m_dSandSettlingSpeed;

   //! End-of-simulation change in elevation, total for all grid cells (mm: -ve is erosion, +ve is deposition)
   double m_dEndOfSimElevChange;

   // These grand totals are all long doubles, the aim is to minimize rounding errors when many very small numbers are added to a single much larger number, see e.g. http://www.ddj.com/cpp/184403224
   long double m_ldGTotDrops;
   long double m_ldGTotRunOnDrops;
   long double m_ldGTotRain;                    // This and all below are as a volume
   long double m_ldGTotRunOn;
   long double m_ldGTotInfilt;
   long double m_ldGTotExfilt;
   long double m_ldGTotWaterOffEdge;
   long double m_ldGTotFlowDetach;
   long double m_ldGTotFlowDeposit;
   long double m_ldGTotSedLoad;
   long double m_ldGTotFlowSedLoadOffEdge;
   long double m_ldGTotSplashDetach;
   long double m_ldGTotSplashDeposit;
   long double m_ldGTotSplashToSedLoad;
   long double m_ldGTotSplashOffEdge;
   long double m_ldGTotSlumpDetach;
   long double m_ldGTotSlumpDeposit;
   long double m_ldGTotSlumpToSedLoad;
   long double m_ldGTotToppleDetach;
   long double m_ldGTotToppleDeposit;
   long double m_ldGTotToppleToSedLoad;
   long double m_ldGTotInfiltDeposit;
   long double m_ldGTotHeadcutRetreatDetach;
   long double m_ldGTotHeadcutRetreatDeposit;
   long double m_ldGTotHeadcutRetreatToSedLoad;

   string m_strRGDir;
   string m_strRGIni;
   string m_strMailAddress;
   string m_strDataPathName;
   string m_strDEMFile;
   string m_strRainVarMFile;
   string m_strGISOutFormat;
   string m_strSplashAttenuationFile;
   string m_strLogFile;
   string m_strOutputPath;
   string m_strOutFile;
   string m_strPalFile;
   string m_strRainTSFile;
   string m_strGDALDEMDriverCode;
   string m_strGDALDEMDriverDesc;
   string m_strGDALDEMProjection;
   string m_strGDALDEMDataType;
   string m_strGDALRainVarDriverCode;
   string m_strGDALRainVarDriverDesc;
   string m_strGDALRainVarProjection;
   string m_strGDALRainVarDataType;
   string m_strGDALOutputDriverLongname;
   string m_strRunName;
   string m_strGDALOutputDriverExtension;

   vector<double> m_VdSaveTime;
   vector<double> m_VdSplashDepth;
   vector<double> m_VdSplashEff;
   vector<double> m_VdSplashEffCoeff;
   vector<double> m_VdRainChangeTime;
   vector<double> m_VdRainChangeIntensity;
   vector<double> m_VdInputSoilLayerThickness;
   vector<double> m_VdInputSoilLayerPerCentClay;
   vector<double> m_VdInputSoilLayerPerCentSilt;
   vector<double> m_VdInputSoilLayerPerCentSand;
   vector<double> m_VdInputSoilLayerBulkDensity;
   vector<double> m_VdInputSoilLayerClayFlowErodibility;
   vector<double> m_VdInputSoilLayerSiltFlowErodibility;
   vector<double> m_VdInputSoilLayerSandFlowErodibility;
   vector<double> m_VdInputSoilLayerClaySplashErodibility;
   vector<double> m_VdInputSoilLayerSiltSplashErodibility;
   vector<double> m_VdInputSoilLayerSandSplashErodibility;
   vector<double> m_VdInputSoilLayerClaySlumpErodibility;
   vector<double> m_VdInputSoilLayerSiltSlumpErodibility;
   vector<double> m_VdInputSoilLayerSandSlumpErodibility;
   vector<double> m_VdInputSoilLayerInfiltAirHead;
   vector<double> m_VdInputSoilLayerInfiltLambda;
   vector<double> m_VdInputSoilLayerInfiltSatWater;
   vector<double> m_VdInputSoilLayerInfiltInitWater;
   vector<double> m_VdInputSoilLayerInfiltKSat;
   vector<double> m_VdInfiltCPHWF;
   vector<double> m_VdInfiltChiPart;
   vector<double> m_VdThisIterSoilWater;
   vector<double> m_VdSinceLastTSSoilWater;

   vector<string> m_VstrInputSoilLayerName;

   struct RandState
   {
      unsigned long s1, s2, s3;
   } m_ulRState[NUMBER_OF_RNGS];

   time_t m_tSysStartTime;
   time_t m_tSysEndTime;

   ofstream m_ofsOut;
   ofstream m_ofsLog;
   ofstream m_ofsErrorTS;
   ofstream m_ofsTimestepTS;
   ofstream m_ofsAreaWetTS;
   ofstream m_ofsRainTS;
   ofstream m_ofsInfiltTS;
   ofstream m_ofsExfiltTS;
   ofstream m_ofsRunOnTS;
   ofstream m_ofsSurfaceWaterTS;
   ofstream m_ofsSurfaceWaterLostTS;
   ofstream m_ofsFlowDetachTS;
   ofstream m_ofsSlumpDetachTS;
   ofstream m_ofsToppleDetachTS;
   ofstream m_ofsFlowDepositSS;
   ofstream m_ofsSedLostTS;
   ofstream m_ofsSedLoadTS;
   ofstream m_ofsInfiltDepositTS;
   ofstream m_ofsSplashDetachTS;
   ofstream m_ofsSplashDepositTS;
   ofstream m_ofsSplashKETS;
   ofstream m_ofsSoilWaterTS;

   //! Pointer to 2D array of soil cell objects
   CCell** m_Cell;

   //! Pointer to 2D array for weights for soil shear stress spatial distribution, used for slumping
   double** m_SSSWeightQuadrant;

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
   bool bReadSplashAttenuationData(void);
   bool bReadRainfallTimeSeries(void);
   bool bSaveGISFiles(void);
   bool bWriteGISFileFloat(int const, string const*);
   bool bWriteGISFileInt(int const, string const*);
   bool bWritePerIterationResults(void);
   bool bWriteTSFiles(bool const);
   int nWriteFilesAtEnd(void);
   void WriteEndOfSimTotals(void);

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
   void CellMoveWaterAndSediment(int const, int const, int const, int const, double const&, double&);
   double dTimeToCrossCell(int const, int const, int const, double const, double, double const, C2DVec&, double&);
   double dCalcHydraulicRadius(int const, int const);
   static void CalcFlowSpeedManning(double&);
   void CalcFlowSpeedDarcyWeisbach(int const, int const, double const, double const, double&);
   double dCalcLawrenceFrictionFactor(int const, int const, double const, bool const);
   void CalcTransportCapacity(int const, int const, int const, int const, int const, double const, double const, double const, double const, double const, double const);
   void DoCellFlowErosion(int const, int const, int const, int const, int const, double const, double const, double const, double const, double const);
   void DoCellSedLoadDeposition(int const, int const, double const, double const, double const);
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
   void CalcSettlingSpeed(void);

   // Utility
   bool bIsTimeToQuit(void);
   bool bSetUpRainfallIntensity(void);
   void CheckMassBalance(void);
   int nCheckForInstability(void) const;
   void UpdatePerIterGrandTotals(void);
   // void AdjustUnboundedEdges(void);
   static string strGetBuild(void);
   static string strGetComputerName(void);
   void DoCPUClockReset(void);
   void CalcTime(double const);
   void AnnounceProgress(void);
   static string strDispTime(double const, bool const, bool const);
   static char const* pszGetErrorText(int const);
   void WrapLongString(string*);
   void CheckLawrenceFF(void);
   void CheckSplashAttenuation(void) const;
   void InitSplashAttenuation(void);
   void CalcProcessStats(void);
   void CalcEndOfSimDEMChange(void);
#if defined RANDCHECK
   void CheckRand(void) const;
#endif
#if defined _DEBUG
   void DEBUGShowSedLoad(string const);
#endif

   // Output formatting
   static string strTrim(string const*);
   static string strTrimLeft(string const*);
   static string strTrimRight(string const*);
   static string strToLower(string const*);
   static string strRemoveSubstr(string*, string const*);
   static vector<string> VstrSplit(string const*, char const);
   static string strSplitRH(string const*);
   string strDbl(double const, int const);
   string strDblRight(double const, int const, int const, bool);
   string strIntRight(int const, int const);
   string strCentre(string const, int const);
   string strRight(string const, int const);
   string strLeft(string const, int const);
   string strRightPerCent(double const, double const, int const);

   // Random number stuff
   static unsigned long ulGetTausworthe(unsigned long const, unsigned long const, unsigned long const, unsigned long const, unsigned long const);
   void InitRand0(unsigned long);
   void InitRand1(unsigned long);
   unsigned long ulGetRand0(void);
   unsigned long ulGetRand1(void);
   static unsigned long ulGetLCG(unsigned long const);            // used by all generators
   double dGetRand0d1(void);
   // double dGetRand1d1(void);
   int nGetRand0To(int const);
   int nGetRand1To(int const);
   double dGetRand0GaussPos(double const, double const);
   double dGetRand0Gaussian(void);
   static double dGetCGaussianPDF(double const);

public:
   CSimulation(void);
   ~CSimulation(void);

   int nDoSetUpRun(int, char*[]);
   void DoEndRun(int const);

   double dGetTimeStep(void) const;
   double dGetMissingValue(void) const;
   // double dGetCellSide(void) const;
   // double dGetCellSideDiag(void) const;
   void GetPreSimulationValues(double const, double const);
   void AddToEndOfIterRain(double const);
   void AddToEndOfIterRunon(double const);
   void AddToEndOfIterStoredSoilWater(double const);
   void AddToEndOfIterInfiltration(double const);
   void AddToEndOfIterExfiltration(double const);
   void IncrNumWetCells(void);
   void AddToEndOfIterStoredSurfaceWater(double const);
   void AddToEndOfIterSurfaceWaterLost(double const);

   void AddToEndOfIterSedimentOffEdge(double const, double const, double const);
   void AddToEndOfIterClayFlowDetach(double const);
   void AddToEndOfIterSiltFlowDetach(double const);
   void AddToEndOfIterSandFlowDetach(double const);
   void AddToEndOfIterClayFlowDeposit(double const);
   void AddToEndOfIterSiltFlowDeposit(double const);
   void AddToEndOfIterSandFlowDeposit(double const);

   void AddToEndOfIterClaySplashDetach(double const);
   void AddToEndOfIterSiltSplashDetach(double const);
   void AddToEndOfIterSandSplashDetach(double const);
   void AddToEndOfIterClaySplashDeposit(double const);
   void AddToEndOfIterSiltSplashDeposit(double const);
   void AddToEndOfIterSandSplashDeposit(double const);
   void AddToEndOfIterClaySplashOffEdge(double const);
   void AddToEndOfIterSiltSplashOffEdge(double const);
   void AddToEndOfIterSandSplashOffEdge(double const);

   void AddToEndOfIterClaySlumpDetach(double const);
   void AddToEndOfIterSiltSlumpDetach(double const);
   void AddToEndOfIterSandSlumpDetach(double const);
   void AddToEndOfIterClaySlumpDeposit(double const);
   void AddToEndOfIterSiltSlumpDeposit(double const);
   void AddToEndOfIterSandSlumpDeposit(double const);

   void AddToEndOfIterClayToppleDetach(double const);
   void AddToEndOfIterSiltToppleDetach(double const);
   void AddToEndOfIterSandToppleDetach(double const);
   void AddToEndOfIterClayToppleDeposit(double const);
   void AddToEndOfIterSiltToppleDeposit(double const);
   void AddToEndOfIterSandToppleDeposit(double const);

   void AddToEndOfIterClayInfiltDeposit(double const);
   void AddToEndOfIterSiltInfiltDeposit(double const);
   void AddToEndOfIterSandInfiltDeposit(double const);

   void AddToEndOfIterClaySedLoad(double const);
   void AddToEndOfIterSiltSedLoad(double const);
   void AddToEndOfIterSandSedLoad(double const);

   void AddToEndOfIterTotalElev(double const);

   void AddToEndOfIterNetClayDetach(double const);
   void AddToEndOfIterNetSiltDetach(double const);
   void AddToEndOfIterNetSandDetach(double const);

   double dGetRandGaussian(void);
};

#endif                  // __SIMULATION_H__
