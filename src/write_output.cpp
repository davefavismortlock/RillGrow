/*========================================================================================================================================

 This is write_output.cpp: writes non-GIS output for RillGrow

 Copyright (C) 2018 David Favis-Mortlock

 =========================================================================================================================================

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public  License as published
 by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation,
 Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

========================================================================================================================================*/
#include "rg.h"
#include "simulation.h"
#include "cell.h"


/*========================================================================================================================================

 Writes run details to Out and Log files

========================================================================================================================================*/
void CSimulation::WriteRunDetails(void)
{
   // Set default output format to be fixed point
   OutStream << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(2);

   OutStream << PROGNAME << " for " << PLATFORM << " " << strGetBuild() << " on " << strGetComputerName() << endl << endl;

   LogStream << PROGNAME << " for " << PLATFORM << " " << strGetBuild() << " on " << strGetComputerName() << endl << endl;

   // --------------------------------------------------------- Run Information ----------------------------------------------------------
   OutStream << "RUN DETAILS" << endl;
   OutStream << " Name                                                   \t: " << m_strRunName << endl;
   OutStream << " Started on                                             \t: " << ctime(&m_tSysStartTime);   //  << endl;

   // Same info. for Log file
   LogStream << m_strRunName << " run started on " << ctime(&m_tSysStartTime) << endl;

   // Contine with Out file
   OutStream << " Initialization file                                    \t: " << m_strRGIni << endl;
   OutStream << " Data read from                                         \t: " << m_strDataPathName << endl;
   if (m_bSplash)
      OutStream << " Splash efficiency parameters read from                 \t: " << m_strSplshFile << endl;
   OutStream << " Duration of simulation                                 \t: ";
   OutStream << strDispTime(m_dSimulationDuration, true, false) << endl;
   if (! bFPIsEqual(m_dSimulationDuration, m_dSimulatedRainDuration, 0.1))
   {
      OutStream << " Duration of rainfall                                   \t: ";
      OutStream << strDispTime(m_dSimulatedRainDuration, true, false) << endl;
   }
   if (m_bSaveRegular)
   {
      // Saves at regular intervals
      OutStream << " Time between saves                                     \t: ";
      OutStream << strDispTime(m_dRSaveInterval, true, false) << endl;
   }
   else
   {
      // Saves at user-defined intervals
      OutStream << " Saves at                                               \t: ";
      string strTmp;
      for (int i = 0; i < m_nUSave; i++)
      {
         strTmp.append(strDispTime(m_VdSaveTime[i], true, false));
         strTmp.append(" ");

         WrapLongString(&strTmp);
      }

      // Also at end of run
      strTmp.append(strDispTime(m_dSimulationDuration, true, false));
      OutStream << strTmp << endl;
   }

   OutStream << " Random number seeds                                    \t: ";
   {
      for (int i = 0; i < NUMBER_OF_RNGS; i++)
         OutStream << m_ulRandSeed[i] << '\t';
   }
   OutStream << endl;

   // Note must modify if more RNGs added
   OutStream << "*First random numbers generated                         \t: " << ulGetRand0() << '\t' << ulGetRand1() << endl;

   OutStream << " GIS output format                                      \t: " << m_strGDALOutputDriverLongname << endl;
   OutStream << " Optional GIS files saved                               \t: ";

   string strTmp;
   if (m_bRainVarMSave)
   {
      strTmp.append(RAIN_VARIATION_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bRunOnSave)
   {
      strTmp.append(RUNON_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bInitElevSave)
   {
      strTmp.append(INITIAL_ELEVATION_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bElevSave)
   {
      strTmp.append(ELEVATION_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bDetrendElevSave)
   {
      strTmp.append(DETRENDED_ELEVATION_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSlosSave)
   {
      strTmp.append(DETACHCODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bInfiltSave)
   {
      strTmp.append(INFILTRATION_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bTotInfiltSave)
   {
      strTmp.append(CUMULATIVE_INFILTRATION_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSoilWaterSave)
   {
      strTmp.append(SOIL_WATER_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bInfiltDepositSave)
   {
      strTmp.append(INFILTRATION_DEPOSITION_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bTotInfiltDepositSave)
   {
      strTmp.append(CUMULATIVE_INFILTRATION_DEPOSITION_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bTopSurfaceSave)
   {
      strTmp.append(TOPSCODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSplashSave)
   {
      strTmp.append(SPLASHCODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bTotSplashSave)
   {
      strTmp.append(TOTSPLASHCODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bInundationSave)
   {
      strTmp.append(INUNDATIONCODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bFlowDirSave)
   {
      strTmp.append(FLOWDIRCODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bStreamPowerSave)
   {
      strTmp.append(STREAMPOWERCODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bShearStressSave)
   {
      strTmp.append(SHEARSTRESSCODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bFrictionFactorSave)
   {
      strTmp.append(FRICTIONFACTORCODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bAvgShearStressSave)
   {
      strTmp.append(AVGSHEARSTRESSCODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bReynoldsSave)
   {
      strTmp.append(REYNOLDSCODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bFroudeSave)
   {
      strTmp.append(FROUDECODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bTCSave)
   {
      strTmp.append(TCCODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

#if defined _DEBUG
   if (m_bLostSave)
   {
      strTmp.append(LOSTCODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }
#endif

   if (m_bAvgDWSpdSave)
   {
      strTmp.append(AVGDWSPEEDCODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bAvgSpdSave)
   {
      strTmp.append(AVGSPEEDCODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bAvgDepthSave)
   {
      strTmp.append(AVG_SURFACE_WATER_DEPTH_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSedConcSave)
   {
      strTmp.append(SEDCONCCODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSedLoadSave)
   {
      strTmp.append(SEDLOADCODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bTotDepositSave)
   {
      strTmp.append(CUMULATIVE_ALL_SIZE_FLOW_DEPOSITION_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bNetSlumpSave)
   {
      strTmp.append(NET_SLUMPING_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bNetToppleSave)
   {
      strTmp.append(NET_TOPPLING_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bTotNetSlosSave)
   {
      strTmp.append(TOT_SURFACE_LOWERING_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   OutStream << strTmp << endl;
   OutStream << " GIS output format                                      \t: " << m_strGISOutFormat << endl;
   OutStream << " Output file (this file)                                \t: " << m_strOutputPath << endl;
   OutStream << " Log file                                               \t: " << m_strLogFile << endl;

   OutStream << " Optional time series files saved                       \t: ";

   if (m_bTimeStepTS)
   {
      strTmp.append(TIMESTEP_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bAreaWetTS)
   {
      strTmp.append(AREA_WET_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bRainTS)
   {
      strTmp.append(RAIN_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bInfiltTS)
   {
      strTmp.append(INFILTRATION_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bExfiltTS)
   {
      strTmp.append(EXFILTRATION_TIME_SERIES_CODE);
      strTmp.append(" ");
      
      WrapLongString(&strTmp);
   }
   
   if (m_bRunOnTS)
   {
      strTmp.append(RUNON_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSurfaceWaterTS)
   {
      strTmp.append(SURFACE_WATER_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSurfaceWaterLostTS)
   {
      strTmp.append(WATER_LOST_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bFlowDetachTS)
   {
      strTmp.append(FLOW_DETACHMENT_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSlumpDetachTS)
   {
      strTmp.append(SLUMP_DETACHMENT_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bToppleDetachTS)
   {
      strTmp.append(TOPPLE_DETACHMENT_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bDoFlowDepositionTS)
   {
      strTmp.append(FLOW_DEPOSITION_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSedLostTS)
   {
      strTmp.append(SEDIMENT_LOST_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSedLoadTS)
   {
      strTmp.append(SEDIMENT_LOAD_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bInfiltDepositTS)
   {
      strTmp.append(INFILTRATION_DEPOSIT_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSplashRedistTS)
   {
      strTmp.append(SPLASH_REDISTRIBUTION_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSplashKETS)
   {
      strTmp.append(SPLASH_KINETIC_ENERGY_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   OutStream << strTmp << endl << endl;

   // --------------------------------------------------------- Microtopography ----------------------------------------------------------
   OutStream << "MICROTOPOGRAPHY" << endl;
   OutStream << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(2);
   OutStream << " Input microtopography (DEM) file                       \t: " << m_strDEMFile << endl;
   OutStream << " Z (vertical) units                                     \t: ";
   if (m_nZUnits == Z_UNIT_MM)
      OutStream << "mm";
   else if (m_nZUnits == Z_UNIT_CM)
      OutStream << "cm"; 
   else if (m_nZUnits == Z_UNIT_M)
      OutStream << "m"; 
   OutStream << endl;
   OutStream << " GDAL DEM driver code                                   \t: " << m_strGDALDEMDriverCode << endl;
   OutStream << " GDAL DEM driver description                            \t: " << m_strGDALDEMDriverDesc << endl;
   OutStream << " GDAL DEM projection                                    \t: " << m_strGDALDEMProjection << endl;
   OutStream << " GDAL DEM data type                                     \t: " << m_strGDALDEMDataType << endl;
   OutStream << " DEM grid size (x-y)                                    \t: " << m_nXGridMax << " x " << m_nYGridMax << endl;
   OutStream << " Number of cells                                        \t: " << m_nXGridMax * m_nYGridMax << endl;
   OutStream << " Number of NODATA cells                                 \t: " << m_ulMissingValueCells << endl;
   OutStream << " Number of valid cells                                  \t: " << m_ulNActiveCells << endl;
   OutStream << setprecision(2);
   OutStream << "*DEM X co-ordinates                                     \t: " << m_dMinX << " - " << m_dMaxX << endl;
   OutStream << "*DEM Y co-ordinates                                     \t: " << m_dMinY << " - " << m_dMaxY << endl;
   OutStream << "*DEM area (including NODATA cells)                      \t: " << m_nXGridMax * m_nYGridMax * m_dCellSquare << " mm2" << endl;
   OutStream << "*DEM area (including NODATA cells)                      \t: " << m_nXGridMax * m_nYGridMax * m_dCellSquare * 1e-6 << " m2" << endl;
   OutStream << "*DEM area (only active cells)                           \t: " << m_ulNActiveCells * m_dCellSquare << " mm2" << endl;
   OutStream << "*DEM area (only active cells)                           \t: " << m_ulNActiveCells * m_dCellSquare * 1e-6 << " m2" << endl;
   OutStream << "*DEM cell size                                          \t: " << m_dCellSide << " mm" << endl;
   OutStream << " DEM bounded edges                                      \t: ";
   if (m_bClosedThisEdge[EDGE_TOP])
      OutStream << "t";
   if (m_bClosedThisEdge[EDGE_RIGHT])
      OutStream << "r";
   if (m_bClosedThisEdge[EDGE_BOTTOM])
      OutStream << "b";
   if (m_bClosedThisEdge[EDGE_LEFT])
      OutStream << "l";
   OutStream << endl;
   OutStream << "*Average DEM elevation (inc datum, pre slope imposed)   \t: " << m_dAvgElev << " mm" << endl;
   OutStream << "*Minimum DEM elevation (inc datum, pre slope imposed)   \t: " << m_dMinElev << " mm" << endl;
   OutStream << " DEM slope increased by                                 \t: " << m_dGradient << " %" << endl;
   OutStream << "*DEM slope increased by                                 \t: ";
   if (m_dGradient > 0)
      OutStream << atan2(m_dGradient, 100) * 180 / PI;
   else
      OutStream << "0";
   OutStream << " degrees" << endl;
   OutStream << "*DEM maximum elevation (after slope imposed)            \t: " << m_dPlotElevMax << " mm" << endl;
   OutStream << "*DEM minimum elevation (after slope imposed)            \t: " << m_dPlotElevMin << " mm" << endl;
   OutStream << " Difference in plot end elev (below lowest point)       \t: ";
   if (m_bHaveBaseLevel)
      OutStream << m_dPlotEndDiff << " mm" << endl;
   else
      OutStream << "none" << endl;
   OutStream << "*DEM base level (after slope imposed)                   \t: ";
   if (m_bHaveBaseLevel)
      OutStream << m_dBaseLevel << " mm" << endl;
   else
      OutStream << "none" << endl;
   OutStream << endl;

   // -------------------------------------------------------------- Soil ----------------------------------------------------------------   
   OutStream << "SOIL" << endl;
   OutStream << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(1);
   OutStream << "Number of soil layers                                   \t: " << m_nNumSoilLayers << endl;
   OutStream << "Elevation of base of lowest layer                       \t: " << m_dBasementElevation << " ";
   if (m_nZUnits == Z_UNIT_MM)
      OutStream << "mm";
   else if (m_nZUnits == Z_UNIT_CM)
      OutStream << "cm"; 
   else if (m_nZUnits == Z_UNIT_M)
      OutStream << "m"; 
   OutStream << endl;
   
   double dBelowTopLayerThickness = 0;
   for (int nLayer = 0; nLayer < m_nNumSoilLayers; nLayer++)
   {
      if (nLayer > 0)
         dBelowTopLayerThickness += m_VdInputSoilLayerThickness[nLayer];
   }
   
   double dTopLayerThickness = (m_dAvgElev - m_dBasementElevation) - dBelowTopLayerThickness;
      
   for (int nLayer = 0; nLayer < m_nNumSoilLayers; nLayer++)
   {
      OutStream << "\tName of soil layer " << nLayer+1 << "                                 \t: " << m_VstrInputSoilLayerName[nLayer] << endl;

      double dThickness;
      if (nLayer == 0)
         dThickness = dTopLayerThickness;
      else
         dThickness = m_VdInputSoilLayerThickness[nLayer];
      
      OutStream << "*\tClay component of soil layer " << nLayer+1 << ", equivalent thickness\t: " << dThickness * m_VdInputSoilLayerPerCentClay[nLayer] / 100 << " mm" << endl;
      OutStream << "*\tSilt component of soil layer " << nLayer+1 << ", equivalent thickness\t: " << dThickness * m_VdInputSoilLayerPerCentSilt[nLayer] / 100 << " mm" << endl;
      OutStream << "*\tSand component of soil layer " << nLayer+1 << ", equivalent thickness\t: " << dThickness * m_VdInputSoilLayerPerCentSand[nLayer] / 100 << " mm" << endl;
      OutStream << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(2);
      
      OutStream << "\tBulk density of soil layer " << nLayer+1 << "                        \t: " << m_VdInputSoilLayerBulkDensity[nLayer] / 1000 << " t/m**3" << endl;
      OutStream << "\tFlow erodibility of clay in soil layer " << nLayer+1 << "            \t: " << m_VdInputSoilLayerClayFlowErodibility[nLayer] << endl;
      OutStream << "\tFlow erodibility of silt in soil layer " << nLayer+1 << "            \t: " << m_VdInputSoilLayerSiltFlowErodibility[nLayer] << endl;
      OutStream << "\tFlow erodibility of sand in soil layer " << nLayer+1 << "            \t: " << m_VdInputSoilLayerSandFlowErodibility[nLayer] << endl;
      OutStream << "\tSplash erodibility of clay in soil layer " << nLayer+1 << "          \t: " << m_VdInputSoilLayerClaySplashErodibility[nLayer] << endl;
      OutStream << "\tSplash erodibility of silt in soil layer " << nLayer+1 << "          \t: " << m_VdInputSoilLayerSiltSplashErodibility[nLayer] << endl;
      OutStream << "\tSplash erodibility of sand in soil layer " << nLayer+1 << "          \t: " << m_VdInputSoilLayerSandSplashErodibility[nLayer] << endl;
      OutStream << "\tSlump erodibility of clay in soil layer " << nLayer+1 << "           \t: " << m_VdInputSoilLayerClaySlumpErodibility[nLayer] << endl;
      OutStream << "\tSlump erodibility of silt in soil layer " << nLayer+1 << "           \t: " << m_VdInputSoilLayerSiltSlumpErodibility[nLayer] << endl;
      OutStream << "\tSlump erodibility of sand in soil layer " << nLayer+1 << "           \t: " << m_VdInputSoilLayerSandSlumpErodibility[nLayer] << endl;
   }
   OutStream << endl;
   
   // ------------------------------------------------------------- Rainfall -------------------------------------------------------------
   OutStream << "RAINFALL" << endl;
   if (m_bTimeVaryingRain)
   {
      OutStream << " Rainfall intensities (mm/hr) during run                \t: ";
      for (int i = 0; i <= m_nRainChangeTimeMax; i++)
      {
         OutStream << m_VdRainChangeIntensity[i] << " at " << strDispTime(m_VdRainChangeTime[i], true, false) << "\t";
         if (3 == i)
            OutStream << endl << "                                                    \t: ";
      }
      OutStream << endl;
   }
   else
   {
      OutStream << " Mean rainfall rate                                     \t: " << m_dSpecifiedRainIntensity << " mm/hr" << endl;
   }
   OutStream << " SD of rainfall rate                                    \t: " << m_dStdRainInt << " mm/hr" << endl;
   OutStream << " Mean raindrop diameter                                 \t: " << m_dDropDiameter << " mm" << endl;
   OutStream << " SD of raindrop diameter                                \t: " << m_dStdDropDiameter << " mm" << endl;
   OutStream << "*Mean raindrop volume                                   \t: " << m_dMeanCellWaterVol << " mm**3" << endl;
   OutStream << setprecision(4);
   OutStream << "*SD of raindrop volume                                  \t: " << m_dStdCellWaterVol << " mm**3" << endl;
   OutStream << "*Mean initial water depth " << (m_bDoInfiltration ?  "(pre-infiltration)            \t: " : "                              \t: ") << m_dMeanCellWaterVol * m_dInvCellSquare << " mm" << endl;
   OutStream << setprecision(2);
   OutStream << " Spatial mask of rainfall variation multipliers         \t: " << m_strRainVarMFile << endl;
if (! m_strRainVarMFile.empty())
   {
      OutStream << "*Mean value of rainfall variation multipliers           \t: " << m_dRainVarMFileMean << endl;
      OutStream << " GDAL rainfall variation driver code                    \t: " << m_strGDALRainVarDriverCode << endl;
      OutStream << " GDAL rainfall variation driver description             \t: " << m_strGDALRainVarDriverDesc << endl;
      OutStream << " GDAL rainfall variation projection                     \t: " << m_strGDALRainVarProjection << endl;
      OutStream << " GDAL rainfall variation data type                      \t: " << m_strGDALRainVarDataType << endl;
   }
   if (m_bSplash)
      OutStream << " Raindrop fall velocity at soil surface                 \t: " << m_dRainSpeed << " m/sec" << endl;
   OutStream << endl;

   // ----------------------------------------------------- Splash redistribution --------------------------------------------------------
   OutStream << "SPLASH REDISTRIBUTION" << endl;
   OutStream << " Simulate splash redistribution?                        \t: " << (m_bSplash ? "y" : "n") << endl;
   if (m_bSplash)
   {
      OutStream << " Water depth (x raindrop diameter)                      \t: ";
      int nLen = m_VdSplashDepth.size();;
      for (int i = 0; i < nLen; i++)
         OutStream << m_VdSplashDepth[i] / m_dDropDiameter << ((i < nLen-1) ? "\t" : "");
      OutStream << endl;
      OutStream << "*Water depth                                            \t: ";
      for (int i = 0; i < nLen; i++)
         OutStream << m_VdSplashDepth[i] << ((i < nLen-1) ? "\t" : "");
      OutStream << " mm" << endl;
      OutStream << " Normalized splash efficiency parameter (0-1)           \t: ";
      for (int i = 0; i < nLen; i++)
         OutStream << m_VdSplashEff[i] << ((i < nLen-1) ? "\t" : "");
      OutStream << endl;
      OutStream << setprecision(1);
      OutStream << " Normalised N for splash efficiency                     \t: " << m_VdSplashEffN << " sec**2/mm" << endl;
      OutStream << setprecision(2);
      OutStream << "*Grid size correction                                   \t: " << m_dCellSizeKC * m_dCellSquare << endl;
      OutStream << " Vertical splash lowering                               \t: " << m_dSplashVertical * 3600 << " mm/hr" << endl;
   }
   OutStream << endl;

   // ------------------------------------------------------------ Run-on ----------------------------------------------------------------
   OutStream << "RUN ON" << endl;
   OutStream << " Upslope run-on area?                                   \t: " << (m_bRunOn ? "y" : "n") << endl;
   if (m_bRunOn)
   {
      OutStream << " Run-on from these edges                                \t: ";
      if (m_bRunOnThisEdge[EDGE_TOP])
         OutStream << "t";
      if (m_bRunOnThisEdge[EDGE_RIGHT])
         OutStream << "r";
      if (m_bRunOnThisEdge[EDGE_BOTTOM])
         OutStream << "b";
      if (m_bRunOnThisEdge[EDGE_LEFT])
         OutStream << "l";
      OutStream << endl;
      OutStream << " Length of upslope run-on area                          \t: " << m_dRunOnLen << " mm" << endl;
      OutStream << " Rain spatial variation multiplier for run-on area      \t: " << m_dRunOnRainVarM << endl;
      OutStream << " Flow speed on run-on area                              \t: " << m_dRunOnSpd << " mm/sec" << endl;
   }
   OutStream << endl;

   // ---------------------------------------------------------- Infiltration ------------------------------------------------------------
   OutStream << "INFILTRATION" << endl;
   OutStream << " Infiltration considered?                               \t: " << (m_bDoInfiltration ? "y" : "n") << endl;
   if (m_bDoInfiltration)
   {
      for (int nLayer = 0; nLayer < m_nNumSoilLayers; nLayer++)
      {
         OutStream << "\tName of soil layer " << nLayer+1 << "                                 \t: " << m_VstrInputSoilLayerName[nLayer] << endl;
         OutStream << "\tAir exit head                                          \t: " << m_VdInputSoilLayerInfiltAirHead[nLayer] << " cm" << endl;
         OutStream << " Exponent of Brooks-Corey water retention equation      \t: " << m_VdInputSoilLayerInfiltLambda[nLayer] << endl;
         OutStream << " Saturated volumetric water content                     \t: " << m_VdInputSoilLayerInfiltSatWater[nLayer] << " cm**3/cm**3" << endl;
         OutStream << " Initial volumetric water content                       \t: " << m_VdInputSoilLayerInfiltInitWater[nLayer] << " cm**3/cm**3" << endl;
         OutStream << " Saturated hydraulic conductivity                       \t: " << m_VdInputSoilLayerInfiltKSat[nLayer] << " cm/h" << endl;
      }
   }
   OutStream << endl;

   // ---------------------------------------------------------- Overland Flow -----------------------------------------------------------
   OutStream << "OVERLAND FLOW" << endl;
   OutStream << " Roughness scaling                                      \t: " << m_dRoughnessScaling << endl;
   OutStream << " In the partially-inundated flow regime:" << endl;
   OutStream << "  D50 of within-cell roughness elements                 \t: " << m_dD50 << " mm" << endl;
   OutStream << "* Epsilon (0.5 * D50)                                   \t: " << m_dEpsilon << endl;
   OutStream << "  % of surface covered with roughness elements          \t: " << m_dPr << " %" << endl;
   OutStream << "  Ratio roughness element drag : ideal situation        \t: " << m_dCd << endl;
   OutStream << endl;

   // ---------------------------------------------------------- Flow detachment ---------------------------------------------------------
   OutStream << "FLOW DETACHMENT" << endl;
   OutStream << " Simulate flow erosion?                                 \t: " << (m_bFlowErosion ? "y" : "n") << endl;
   if (m_bFlowErosion)
   {
      OutStream << setprecision(3);
      OutStream << " Constant k for flow detachment                         \t: " << m_dK << " kg/m**3" << endl;
      OutStream << setprecision(2);
      OutStream << " Mean value of T for flow detachment                    \t: " << m_dT << " Pascal" << endl;
      OutStream << " Coefficient of variation of T for flow detachment      \t: " << m_dCVT << endl;
      OutStream << " Coefficient of variation of tau-b for detachment       \t: " << m_dCVTaub << endl;
      OutStream << " Critical angle for source-destination erosion share    \t: " << m_dSourceDestCritAngle << " mm/mm" << endl;
   }
   OutStream << endl;

   // --------------------------------------------------------- Transport Capacity -------------------------------------------------------
   OutStream << "TRANSPORT CAPACITY" << endl;
   if (m_bFlowErosion)
   {
      OutStream << " Alpha for transport capacity                           \t: " << m_dAlpha << endl;
      OutStream << " Beta for transport capacity                            \t: " << m_dBeta << endl;
      OutStream << " Gamma for transport capacity                           \t: " << m_dGamma << endl;
      OutStream << " Delta for transport capacity                           \t: " << m_dDelta << endl;
   }
   OutStream << endl;

   // ------------------------------------------------------------- Deposition -----------------------------------------------------------
   OutStream << "DEPOSITION" << endl;
   if (m_bFlowErosion)
   {
      OutStream << " Grain density                                          \t: " << m_dDepositionGrainDensity << " kg/m**3" << endl;
      OutStream << " Clay, minimum size                                     \t: " << m_dClaySizeMin << " mm" << endl;
      OutStream << " Clay-silt threshold size                               \t: " << m_dClaySiltBoundarySize << " mm" << endl;
      OutStream << " Silt-Sand threshold size                               \t: " << m_dSiltSandBoundarySize << " mm" << endl;
      OutStream << " Sand, maximum size                                     \t: " << m_dSandSizeMax << " mm" << endl;
   }
   OutStream << endl;

   // ----------------------------------------------------------- Slumping ---------------------------------------------------------------
   OutStream << "SLUMPING" << endl;
   OutStream << " Simulate slumping?                                     \t: " << (m_bSlumping ? "y" : "n") << endl;
   if (m_bSlumping)
   {
      OutStream << " Threshold shear stress for slumping                    \t: " << m_dSlumpCritShearStress << " kg/m s**2 (Pa)" << endl;
      OutStream << " Angle of rest for slumped soil                         \t: " << m_dSlumpAngleOfRest << " %" << endl;
      OutStream << " Critical angle for toppled soil                        \t: " << m_dToppleCriticalAngle << " %" << endl;
      OutStream << " Angle of rest for toppled soil                         \t: " << m_dToppleAngleOfRest << " %" << endl;
   }
   OutStream << endl;

   // --------------------------------------------------- Various Physical Constants -----------------------------------------------------
   OutStream << "VARIOUS CONSTANTS" << endl;
   OutStream << " Density of water                                       \t: " << m_dRho << " kg/m**3" << endl;
   OutStream << setprecision(6);
   OutStream << " Kinematic viscosity of water                           \t: " << m_dNu << " m**2/sec" << endl;
   OutStream << setprecision(2);
   OutStream << " Gravitational acceleration                             \t: " << m_dG << " m/sec**2" << endl;
   OutStream << endl;

   OutStream << "*=calculated value" << endl;
   OutStream << endl;

   // Write per-iteration headers to .out file
   OutStream << PERITERHEAD << endl;
   OutStream << "Units: volumes in mm**3, times in sec." << endl;
   OutStream << "Markers: dRain = change in rain intensity, ";
   OutStream << "GISn = GIS files saved as <filename>n." << endl;
   OutStream << endl;

   OutStream << PERITERHEAD1 << endl;
   OutStream << PERITERHEAD2 << endl;
}


int CSimulation::nWriteFilesAtEnd(void)
{
   // Write files which are always written
   if (! bWriteFileFloat(PLOT_CUMULATIVE_ALL_SIZE_FLOW_DETACHMENT, &PLOT_CUMULATIVE_ALL_SIZE_FLOW_DETACHMENT_TITLE))   // also increments filename count
      return (RTN_ERR_GISFILEWRITE);

   if (! bWriteFileFloat(PLOT_TOTAL_RAIN, &PLOT_TOTAL_RAIN_TITLE))
      return (RTN_ERR_GISFILEWRITE);

   if (! bWriteFileFloat(PLOT_TOTAL_SPLASH, &PLOT_TOTAL_SPLASH_TITLE))
      return (RTN_ERR_GISFILEWRITE);

   if (! bWriteFileFloat(PLOT_SURFACE_WATER_DEPTH, &PLOT_SURFACE_WATER_DEPTH_TITLE))
      return (RTN_ERR_GISFILEWRITE);

   if (! bWriteFileFloat(PLOT_SURFACE_WATER_DW_FLOW_SPEED, &PLOT_SURFACE_WATER_DW_FLOW_SPEED_TITLE))
      return (RTN_ERR_GISFILEWRITE);

   if (! bWriteFileFloat(PLOT_SURFACE_WATER_FLOW_SPEED, &PLOT_SURFACE_WATER_FLOW_SPEED_TITLE))
      return (RTN_ERR_GISFILEWRITE);

   // Maybe write optional files
   if (m_bElevSave)
      if (! bWriteFileFloat(PLOT_ELEVATION, &PLOT_ELEVATION_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bDetrendElevSave)
      if (! bWriteFileFloat(PLOT_DETRENDED_ELEVATION, &PLOT_DETRENDED_ELEVATION_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bRunOnSave)
      if (! bWriteFileFloat(PLOT_TOTAL_RUNON, &PLOT_TOTAL_RUNON_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bSplashSave)
      if (! bWriteFileFloat(PLOT_SPLASH, &PLOT_SPLASH_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bSlosSave)
      if (! bWriteFileFloat(PLOT_ALL_SIZE_FLOW_DETACHMENT, &PLOT_ALL_SIZE_FLOW_DETACHMENT_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bInundationSave)
      if (! bWriteFileInt(PLOT_INUNDATION_REGIME, &PLOT_INUNDATION_REGIME_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bFlowDirSave)
      if (! bWriteFileInt(PLOT_SURFACE_WATER_FLOW_DIRECTION, &PLOT_SURFACE_WATER_FLOW_DIRECTION_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bInfiltSave)
      if (! bWriteFileFloat(PLOT_INFILTRATION, &PLOT_INFILTRATION_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bTotInfiltSave)
      if (! bWriteFileFloat(PLOT_CUMULATIVE_INFILTRATION_, &PLOT_CUMULATIVE_INFILTRATION__TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bSoilWaterSave)
      if (! bWriteFileFloat(PLOT_SOIL_WATER, &PLOT_SOIL_WATER_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bInfiltDepositSave)
      if (! bWriteFileFloat(PLOT_INFILTRATION_DEPOSIT, &PLOT_INFILTRATION_DEPOSIT_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bTotInfiltDepositSave)
      if (! bWriteFileFloat(PLOT_CUMULATIVE_INFILTRATION_DEPOSIT, &PLOT_CUMULATIVE_INFILTRATION_DEPOSIT_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bTopSurfaceSave)
      if (! bWriteFileFloat(PLOT_TOP, &PLOT_TOP_TITLE))
         return (RTN_ERR_GISFILEWRITE);

#if defined _DEBUG
   if (m_bLostSave)
      if (! bWriteFileFloat(PLOT_LOST_FROM_EDGES, &PLOT_LOST_FROM_EDGES_TITLE))
         return (RTN_ERR_GISFILEWRITE);
#endif

   if (m_bStreamPowerSave)
      if (! bWriteFileFloat(PLOT_STREAMPOWER, &PLOT_STREAMPOWER_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bShearStressSave)
      if (! bWriteFileFloat(PLOT_SHEAR_STRESS, &PLOT_SHEAR_STRESS_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bFrictionFactorSave)
      if (! bWriteFileFloat(PLOT_FRICTION_FACTOR, &PLOT_FRICTION_FACTOR_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bAvgShearStressSave)
      if (! bWriteFileFloat(PLOT_AVG_SHEAR_STRESS, &PLOT_AVG_SHEAR_STRESS_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bReynoldsSave)
      if (! bWriteFileFloat(PLOT_REYNOLDS_NUMBER, &PLOT_REYNOLDS_NUMBER_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bFroudeSave)
      if (! bWriteFileFloat(PLOT_FROUDE_NUMBER, &PLOT_FROUDE_NUMBER_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bTCSave)
      if (! bWriteFileFloat(PLOT_TRANSPORT_CAPACITY, &PLOT_TRANSPORT_CAPACITY_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bAvgDepthSave)
      if (! bWriteFileFloat(PLOT_AVG_SURFACE_WATER_DEPTH, &PLOT_AVG_SURFACE_WATER_DEPTH_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bAvgDWSpdSave)
      if (! bWriteFileFloat(PLOT_SURFACE_WATER_AVG_DW_FLOW_SPEED, &PLOT_SURFACE_WATER_AVG_DW_FLOW_SPEED_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bAvgSpdSave)
      if (! bWriteFileFloat(PLOT_SURFACE_WATER_AVG_FLOW_SPEED, &PLOT_SURFACE_WATER_AVG_FLOW_SPEED_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bSedConcSave)
      if (! bWriteFileFloat(PLOT_SEDIMENT_CONCENTRATION, &PLOT_SEDIMENT_CONCENTRATION_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bSedConcSave)
      if (! bWriteFileFloat(PLOT_SEDIMENT_CONCENTRATION, &PLOT_SEDIMENT_CONCENTRATION_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bSedLoadSave)
      if (! bWriteFileFloat(PLOT_SEDIMENT_LOAD, &PLOT_SEDIMENT_LOAD_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bAvgSedLoadSave)
      if (! bWriteFileFloat(PLOT_AVG_SEDIMENT_LOAD, &PLOT_AVG_SEDIMENT_LOAD_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bNetSlumpSave)
      if (! bWriteFileFloat(PLOT_NET_SLUMP, &PLOT_NET_SLUMP_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bNetToppleSave)
      if (! bWriteFileFloat(PLOT_NET_TOPPLE, &PLOT_NET_TOPPLE_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bTotDepositSave)
      if (! bWriteFileFloat(PLOT_CUMULATIVE_ALL_SIZE_FLOW_DEPOSITION, &PLOT_CUMULATIVE_ALL_SIZE_FLOW_DEPOSITION_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bTotNetSlosSave)
      if (! bWriteFileFloat(PLOT_TOT_SURFACE_LOWERING, &PLOT_TOT_SURFACE_LOWERING_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   return RTN_OK;
}


void CSimulation::WriteGrandTotals(void)
{
   // Print out run totals etc.
   LogStream << "End of run: files written" << endl << endl;

   OutStream << PERITERHEAD1 << endl;
   OutStream << PERITERHEAD2 << endl;

   OutStream << setprecision(2);
   OutStream << setiosflags(ios::fixed);
   OutStream << endl << endl;

   // Write out rainfall grand totals
   OutStream << ENDRAINHEAD << endl;

   // Start calculating final totals etc., first calculate total area in mm**2 (excluding NODATA cells)
   double fTotArea = m_ulNActiveCells * m_dCellSquare;
   if (m_bTimeVaryingRain)
   {
      OutStream << "Mean rainfall intensity for time-varying rain = " << 3600 * m_ldGTotRain / (m_dSimulatedRainDuration * fTotArea) << " mm/hr" << endl;
   }
   else
   {
      OutStream << "Mean rainfall intensity = " << 3600 * m_ldGTotRain / (m_dSimulatedRainDuration * fTotArea) << " mm/hr, should be " << m_dSpecifiedRainIntensity * m_dRainVarMFileMean << " mm/hr" << endl;
   }
   OutStream << "Mean raindrop volume    = " << m_ldGTotRain / m_ldGTotDrops << " mm**3, should be " << m_dMeanCellWaterVol * m_dRainVarMFileMean << " mm**3" << endl;
   if (! m_strRainVarMFile.empty())
      OutStream << "Note: a rainfall variation correction of " << m_dRainVarMFileMean << "x was applied, from " << m_strRainVarMFile << endl;
   OutStream << endl;

   // Run-on?
   if (m_bRunOn)
   {
      OutStream << "Mean run-on rainfall intensity = " << 3600 * m_ldGTotRunOnWater  / (m_dSimulatedRainDuration * m_dRunOnLen * m_nXGridMax * m_dCellSide) << " mm/hr, should be " << m_dSpecifiedRainIntensity * m_dRunOnRainVarM << " mm/hr" << endl;
      OutStream << "Mean run-on raindrop volume    = " << m_ldGTotRunOnWater / m_ldGTotRunOnDrops << " mm**3, should be " << m_dMeanCellWaterVol * m_dRunOnRainVarM << " mm**3" << endl;
      if (! m_strRainVarMFile.empty())
         OutStream << "Note: a rainfall variation correction of " << m_dRunOnRainVarM << "x was applied, from " << m_strRainVarMFile << endl;
      OutStream << endl;
   }

   if (! bFPIsEqual(m_dSimulationDuration, m_dSimulatedRainDuration, 0.1))
   {
      OutStream << "Note: these are calculated for the period of rainfall (" << m_dSimulatedRainDuration / 60 << " mins) only." << endl;
   }
   OutStream << endl << endl;

   // Write out infiltration grand totals
   OutStream << ENDINFILTRATIONHEAD << endl;
   OutStream << "Mean infiltration during period of rainfall (" << m_dSimulatedRainDuration / 60 << " mins) = " << 3600 * m_ldGTotInfilt / (m_dSimulatedRainDuration * fTotArea) << " mm/hr" << endl;
   OutStream << "Mean infiltration during whole simulation (" << m_dSimulationDuration / 60 << " mins) = " << 3600 * m_ldGTotInfilt / (m_dSimulationDuration * fTotArea) << " mm/hr" << endl;
   OutStream << endl << endl;


   // Next, write out hydrology grand totals
   OutStream << ENDHYDHEAD << endl;
   OutStream << "Totals lost from edge(s)" << endl;
   OutStream << "mm depth                    " << setw(15) << m_ldGTotWaterLost / fTotArea << endl;
   OutStream << "mm**3                       " << setw(15) << m_ldGTotWaterLost << endl;
   OutStream << "m**3                        " << setw(15) << m_ldGTotWaterLost * 1e-9 << endl;
   OutStream << "litre                       " << setw(15) << m_ldGTotWaterLost * 1e-6 << endl;
   OutStream << "litre/s                     " << setw(15) << m_ldGTotWaterLost * 1e-6 / m_dSimulatedRainDuration << endl;
   if (! bFPIsEqual(m_dSimulationDuration, m_dSimulatedRainDuration, 0.1))
      OutStream << endl << "Note: this is the rate during the period of rainfall (" << m_dSimulatedRainDuration / 60 << " mins)" << endl;
   OutStream << endl;

   double dTmpTot = 0, dTmp;
   OutStream << "Water Balance (% of total rainfall" << (m_bRunOn ? " + run-on" : "") << ")" << endl;

   dTmp = 100 * (m_ldGTotInfilt - m_ldGTotExfilt) / (m_ldGTotRain + m_ldGTotRunOnWater);
   dTmpTot += dTmp;
   OutStream << "Infiltration - exfiltration " << setw(10) << dTmp << endl;

   dTmp = 100 * m_dThisIterSurfaceWaterStored * m_dCellSquare / (m_ldGTotRain + m_ldGTotRunOnWater);
   dTmpTot += dTmp;
   OutStream << "Storage at end              " << setw(10) << dTmp << endl;

   dTmp = 100 * m_ldGTotWaterLost / (m_ldGTotRain + m_ldGTotRunOnWater);
   dTmpTot += dTmp;
   OutStream << "Off edge                    " << setw(10) << dTmp << endl;

   OutStream << "TOTAL                       " << setw(10) << dTmpTot << endl;
   OutStream << endl << endl;

   // Now write out sediment delivery grand totals
   OutStream << ENDSEDDELHEAD << endl;
   OutStream << "Totals lost from edge(s)    " << endl;
   OutStream << "mm depth                    " << setw(15) << m_ldGTotSedLost / fTotArea << endl;
   OutStream << "mm**3                       " << setw(15) << m_ldGTotSedLost << endl;
   OutStream << "m**3                        " << setw(15) << m_ldGTotSedLost * 1e-9 << endl;
   OutStream << "g                           " << setw(15) << m_ldGTotSedLost * m_dBulkDensityForOutputCalcs * 1e-6 << endl;
   OutStream << "kg                          " << setw(15) << m_ldGTotSedLost * m_dBulkDensityForOutputCalcs * 1e-9 << endl;
   OutStream << "kg/m**2                     " << setw(15) << m_ldGTotSedLost * m_dBulkDensityForOutputCalcs * 1e-3 / fTotArea << endl;
   OutStream << "m**3/ha                     " << setw(15) << 10 * m_ldGTotSedLost / fTotArea << endl;
   m_dBulkDensityForOutputCalcs /= 1000;                   // convert back from kg/m**3 to t/m**3
   OutStream << "t/ha                        " << setw(15) << (10 * m_ldGTotSedLost * m_dBulkDensityForOutputCalcs) / fTotArea << endl;
   OutStream << endl;

   OutStream << "Sediment Balance (% of total detached by flow" << (m_bSplash ? " + splash" : "") << (m_bSlumping ? " + slumping + toppling" : "") << ")" << endl;

   long double ldGTotEroded = m_ldGTotFlowDetach + m_ldGTotSplashDetach + m_ldGTotSlumpDetach + m_ldGTotToppleDetach;
   long double ldGTotNetDeposited = ldGTotEroded - (((m_dThisIterClaySedLoad + m_dThisIterSiltSedLoad + m_dThisIterSandSedimentLoad) * m_dCellSquare) + m_ldGTotSedLost);
   dTmpTot = 0;

   dTmp = 100 * ldGTotNetDeposited / ldGTotEroded;
   dTmpTot += dTmp;
   OutStream << "Deposition                  " << setw(10) << dTmp << endl;

   dTmp = 100 * ((m_dThisIterClaySedLoad + m_dThisIterSiltSedLoad + m_dThisIterSandSedimentLoad) * m_dCellSquare) / ldGTotEroded;
   dTmpTot += dTmp;
   OutStream << "Storage at end              " << setw(10) << dTmp << endl;

   dTmp = 100 * m_ldGTotSedLost / ldGTotEroded;
   dTmpTot += dTmp;
   OutStream << "Off edge                    " << setw(10) << dTmp << endl;

   OutStream << "TOTAL                       " << setw(10) << dTmpTot << endl;

   OutStream << endl;
   OutStream << "Sediment delivery ratio     " << setw(10) << m_ldGTotSedLost / ldGTotEroded << endl;
   OutStream << endl << endl;

   // Show grand totals for detachment and deposition
   OutStream << DETDEPHEAD << endl;

   // Flow detachment
   OutStream << "Detachment due to flow" << endl;
   OutStream << "Average = " << m_ldGTotFlowDetach / fTotArea << " mm" << endl;
   OutStream << "        = " << 10 * m_ldGTotFlowDetach / fTotArea << " m**3/ha" << endl;         // convert mm3/mm2 to m3/ha
   OutStream << "        = " << 10 * m_ldGTotFlowDetach * m_dBulkDensityForOutputCalcs / fTotArea << " t/ha" << endl;
   OutStream << endl;

   // Flow deposition
   OutStream << "Deposition (including subsequently re-eroded sediment) due to flow" << endl;
   OutStream << "Average = " << m_ldGTotFlowDeposition / fTotArea << " mm" << endl;
   OutStream << "        = " << 10 * m_ldGTotFlowDeposition / fTotArea  << " m**3/ha" << endl;       // convert mm3/mm2 to m3/ha
   OutStream << "        = " << 10 * m_ldGTotFlowDeposition * m_dBulkDensityForOutputCalcs / fTotArea << " t/ha" << endl;
   OutStream << endl;

   // Splash detachment
   OutStream << "Elevation decrease due to splash" << endl;
   OutStream << "Average = " << m_ldGTotSplashDetach / fTotArea << " mm" << endl;
   OutStream << "        = " << 10 * m_ldGTotSplashDetach / fTotArea << " m**3/ha" << endl;         // convert mm3/mm2 to m3/ha

   OutStream << "        = " << 10 * m_ldGTotSplashDetach * m_dBulkDensityForOutputCalcs / fTotArea << " t/ha" << endl;
   OutStream << endl;

   // Splash deposition
   OutStream << "Elevation increase (including subsequently re-eroded sediment) due to splash" << endl;
   OutStream << "Average = " << m_ldGTotSplashDeposit / fTotArea << " mm" << endl;
   OutStream << "        = " << 10 * m_ldGTotSplashDeposit / fTotArea << " m**3/ha" << endl;      // convert mm3/mm2 to m3/ha
   OutStream << "        = " << 10 * m_ldGTotSplashDeposit * m_dBulkDensityForOutputCalcs / fTotArea << " t/ha" << endl;
   OutStream << endl;

   // Slumping detachment
   OutStream << "Elevation decrease due to slumping" << endl;
   OutStream << "Average = " << m_ldGTotSlumpDetach / fTotArea << " mm" << endl;
   OutStream << "        = " << 10 * m_ldGTotSlumpDetach / fTotArea << " m**3/ha" << endl;        // convert mm3/mm2 to m3/ha
   OutStream << "        = " << 10 * m_ldGTotSlumpDetach * m_dBulkDensityForOutputCalcs / fTotArea << " t/ha" << endl;
   OutStream << endl;

   // Toppling detachment
   OutStream << "Elevation decrease due to toppling" << endl;
   OutStream << "Average = " << m_ldGTotToppleDetach / fTotArea << " mm" << endl;
   OutStream << "        = " << 10 * m_ldGTotToppleDetach / fTotArea << " m**3/ha" << endl;        // convert mm3/mm2 to m3/ha
   OutStream << "        = " << 10 * m_ldGTotToppleDetach * m_dBulkDensityForOutputCalcs / fTotArea << " t/ha" << endl;
   OutStream << endl;

   // Toppling deposition
   OutStream << "Elevation increase (including subsequently re-eroded sediment) due to toppling" << endl;
   OutStream << "Average = " << m_ldGTotToppleDeposit / fTotArea << " mm" << endl;
   OutStream << "        = " << 10 * m_ldGTotToppleDeposit / fTotArea << " m**3/ha" << endl;       // convert mm3/mm2 to m3/ha
   OutStream << "        = " << 10 * m_ldGTotToppleDeposit * m_dBulkDensityForOutputCalcs / fTotArea << " t/ha" << endl;
   OutStream << endl;

   // Infiltration deposition
   OutStream << "Elevation increase (including subsequently re-eroded sediment) due to infiltration deposition" << endl;
   OutStream << "Average = " << m_ldGTotInfiltDeposit / fTotArea << " mm" << endl;
   OutStream << "        = " << 10 * m_ldGTotInfiltDeposit / fTotArea << " m**3/ha" << endl;       // convert mm3/mm2 to m3/ha
   OutStream << "        = " << 10 * m_ldGTotInfiltDeposit * m_dBulkDensityForOutputCalcs / fTotArea << " t/ha" << endl;
   OutStream << endl << endl;

   // Show relative contributions of each detachment process to total sediment lost
   OutStream << RELCONTRIBLOSTHEAD << endl;

   OutStream << "Detachment due to flow (%)            " << setw(10) << 100 * m_ldGTotFlowDetach / m_ldGTotSedLost << endl;
   OutStream << "Elevation decrease due to splash (%)  " << setw(10) << 100 * m_ldGTotSplashDetach / m_ldGTotSedLost << endl;
   OutStream << "Elevation decrease due to slumping (%)" << setw(10) << 100 * m_ldGTotSlumpDetach / m_ldGTotSedLost << endl;
   OutStream << "Elevation decrease due to toppling (%)" << setw(10) << 100 * m_ldGTotToppleDeposit / m_ldGTotSedLost << endl;
   OutStream << endl << "Note: potential double-counting here, due to subsequent deposition and re-detachment." << endl;
   OutStream << endl << endl;

   // Show relative contributions of each detachment process to total soil detached
   OutStream << RELCONTRIBDETACHHEAD << endl;

   OutStream << "Detachment due to flow (%)            " << setw(10) << 100 * m_ldGTotFlowDetach / ldGTotEroded << endl;
   OutStream << "Elevation decrease due to splash (%)  " << setw(10) << 100 * m_ldGTotSplashDetach / ldGTotEroded << endl;
   OutStream << "Elevation decrease due to slumping (%)" << setw(10) << 100 * m_ldGTotSlumpDetach / ldGTotEroded << endl;
   OutStream << "Elevation decrease due to toppling (%)" << setw(10) << 100 * m_ldGTotToppleDeposit / ldGTotEroded << endl;
   OutStream << endl << endl;

}


/*========================================================================================================================================

 The bSetUpTSFiles member function sets up the time series files

========================================================================================================================================*/
bool CSimulation::bSetUpTSFiles(void)
{
   string strTSFile;

   if (m_bTimeStepTS)
   {
      // First, do timestep TS
      strTSFile = m_strOutputPath;
      strTSFile.append(TIMESTEP_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open timestep time-series CSV file
      TimeStepTSStream.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! TimeStepTSStream)
      {
         // Error, cannot open timestep time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }
      
      // Write header line
      TimeStepTSStream << "'Elapsed'" << "\t,\t" << "'Since last'" << "\t,\t" << "'Timestep'" << endl;
   }

   if (m_bAreaWetTS)
   {
      // Next, do area wet TS
      strTSFile = m_strOutputPath;
      strTSFile.append(AREA_WET_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open area wet time-series CSV file
      AreaWetTSStream.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! AreaWetTSStream)
      {
         // Error, cannot open area wet time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }
      
      // Write header line
      AreaWetTSStream << "'Elapsed'" << "\t,\t" << "'Since last'" << "\t,\t" << "'Percent wet cells'" << endl;
   }

   if (m_bRainTS)
   {
      // Now rainfall TS
      strTSFile = m_strOutputPath;
      strTSFile.append(RAIN_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open rain time-series CSV file
      RainTSStream.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! RainTSStream)
      {
         // Error, cannot open rain time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      RainTSStream << "'Elapsed'" << "\t,\t" << "'Since last'" << "\t,\t" << "'Rain depth (mm)'" << endl;
   }

   if (m_bRunOnTS)
   {
      // Now run-on
      strTSFile = m_strOutputPath;
      strTSFile.append(RUNON_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);
      
      // Open run-on time-series CSV file
      RunOnTSStream.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! RunOnTSStream)
      {
         // Error, cannot open run-on time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      RunOnTSStream << "'Elapsed'" << "\t,\t" << "'Since last'" << "\t,\t" << "'Runon (l)'" << endl;      
   }
   
   if (m_bSurfaceWaterTS)
   {
      // Now surface water
      strTSFile = m_strOutputPath;
      strTSFile.append(SURFACE_WATER_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);
      
      // Open stored water time-series CSV file
      SurfaceWaterTSStream.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! SurfaceWaterTSStream)
      {
         // Error, cannot open stored water time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      SurfaceWaterTSStream << "'Elapsed'" << "\t,\t" << "'Since last'" << "\t,\t" << "'Surface water (l)'" << endl;      
   }
   
   if (m_bSurfaceWaterLostTS)
   {
      // Then surface water lost
      strTSFile = m_strOutputPath;
      strTSFile.append(WATER_LOST_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);
      
      // Open water lost time-series CSV file
      SurfaceWaterLostTSStream.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! SurfaceWaterLostTSStream)
      {
         // Error, cannot open water lost time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }
      
      // Write header line
      SurfaceWaterLostTSStream << "'Elapsed'" << "\t,\t" << "'Since last'" << "\t,\t" << "'Discharge (l)'" << endl;      
   }
   
   if (m_bFlowDetachTS)
   {
      // Flow detachment
      strTSFile = m_strOutputPath;
      strTSFile.append(FLOW_DETACHMENT_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);
      
      // Open flow detachment time-series CSV file
      FlowDetachmentTSStream.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! FlowDetachmentTSStream)
      {
         // Error, cannot open flow detachment time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      FlowDetachmentTSStream << "'Elapsed'" << "\t,\t" << "'Since last'" << "\t,\t" << "'Clay flow detachment (g)'" << "\t,\t" << "'Silt flow detachment (g)'" << "\t,\t" << "'Sand flow detachment (g)'" << endl;      
   }
   
   if (m_bDoFlowDepositionTS)
   {
      // Flow deposition
      strTSFile = m_strOutputPath;
      strTSFile.append(FLOW_DEPOSITION_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);
      
      // Open flow deposition time-series CSV file
      FlowDepositionTSStream.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! FlowDepositionTSStream)
      {
         // Error, cannot open flow deposition time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }
      
      // Write header line
      FlowDepositionTSStream << "'Elapsed'" << "\t,\t" << "'Since last'" << "\t,\t" << "'Clay flow deposition (g)'" << "\t,\t" << "'Silt flow deposition (g)'" << "\t,\t" << "'Sand flow deposition (g)'" << endl;      
   }
   
   if (m_bSedLostTS)
   {
      // Sediment loss
      strTSFile = m_strOutputPath;
      strTSFile.append(SEDIMENT_LOST_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);
      
      // Open sediment loss time-series CSV file
      SedimentLostTSStream.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! SedimentLostTSStream)
      {
         // Error, cannot open sediment loss time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }
      
      // Write header line
      SedimentLostTSStream << "'Elapsed'" << "\t,\t" << "'Since last'" << "\t,\t" << "'Clay sediment lost (g)'" << "\t,\t" << "'Silt sediment lost (g)'" << "\t,\t" << "'Sand sediment lost (g)'" << endl;      
   }
   
   if (m_bSedLoadTS)
   {
      // Sediment load
      strTSFile = m_strOutputPath;
      strTSFile.append(SEDIMENT_LOAD_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);
      
      // Open sediment load time-series CSV file
      SedimentLoadTSStream.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! SedimentLoadTSStream)
      {
         // Error, cannot open sediment load time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }
      
      // Write header line
      SedimentLoadTSStream << "'Elapsed'" << "\t,\t" << "'Since last'" << "\t,\t" << "'Clay sediment load (g)'" << "\t,\t" << "'Silt sediment load (g)'" << "\t,\t" << "'Sand sediment load (g)'" << endl;      
   }
   
   if (m_bInfiltTS)
   {
      // Now infiltration
      strTSFile = m_strOutputPath;
      strTSFile.append(INFILTRATION_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open infiltration time-series CSV file
      InfiltrationTSStream.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! InfiltrationTSStream)
      {
         // Error, cannot open infiltration time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      InfiltrationTSStream << "'Elapsed'" << "\t,\t" << "'Since last'" << "\t,\t" << "'Infiltration (l)'" << endl;      
   }

   if (m_bExfiltTS)
   {
      // Now exfiltration
      strTSFile = m_strOutputPath;
      strTSFile.append(EXFILTRATION_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);
      
      // Open exfiltration time-series CSV file
      ExfiltrationTSStream.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! ExfiltrationTSStream)
      {
         // Error, cannot open exfiltration time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }
      
      // Write header line
      ExfiltrationTSStream << "'Elapsed'" << "\t,\t" << "'Since last'" << "\t,\t" << "'Exfiltration (l)'" << endl;      
   }
   
   if (m_bInfiltDepositTS)
   {
      // Deposition resulting from infiltration
      strTSFile = m_strOutputPath;
      strTSFile.append(INFILTRATION_DEPOSIT_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);
      
      // Open deposition due to infiltration time-series CSV file
      InfiltrationDepositionTSStream.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! InfiltrationDepositionTSStream)
      {
         // Error, cannot open deposition due to infiltration time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }
      
      // Write header line
      InfiltrationDepositionTSStream << "'Elapsed'" << "\t,\t" << "'Since last'" << "\t,\t" << "'Clay deposition from infiltration (g)'" << "\t,\t" << "'Silt deposition from infiltration (g)'" << "\t,\t" << "'Sand deposition from infiltration (g)'" << endl;      
   }
   
   if (m_bSplashRedistTS)
   {
      // Splash detachment (not linked to other totals)
      strTSFile = m_strOutputPath;
      strTSFile.append(SPLASH_REDISTRIBUTION_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);
      
      // Open splash detachment time-series CSV file
      SplashDetachmentTSStream.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! SplashDetachmentTSStream)
      {
         // Error, cannot splash detachment time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }
      
      // Write header line
      SplashDetachmentTSStream << "'Elapsed'" << "\t,\t" << "'Since last'" << "\t,\t" << "'Clay splash detachment (g)'" << "\t,\t" << "'Silt splash detachment (g)'" << "\t,\t" << "'Sand splash detachment (g)'" << endl;      
   }
   
   if (m_bSplashKETS)
   {
      // Splash kinetic energy (in Joules)
      strTSFile = m_strOutputPath;
      strTSFile.append(SPLASH_KINETIC_ENERGY_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);
      
      // Open splash kinetic energy time-series CSV file
      SplashKineticEnergyTSStream.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! SplashKineticEnergyTSStream)
      {
         // Error, cannot splash kinetic energy time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }
      
      // Write header line
      SplashKineticEnergyTSStream << "'Elapsed'" << "\t,\t" << "'Since last'" << "\t,\t" << "'Splash KE (J)'" << endl;      
   }
   
   
   if (m_bSlumpDetachTS)
   {
      // Slump detachment (same as slump deposition)
      strTSFile = m_strOutputPath;
      strTSFile.append(SLUMP_DETACHMENT_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open slump detachment time-series CSV file
      SlumpDetachmentTSStream.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! SlumpDetachmentTSStream)
      {
         // Error, cannot open slump detachment time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }
      
      // Write header line
      SlumpDetachmentTSStream << "'Elapsed'" << "\t,\t" << "'Since last'" << "\t,\t" << "'Clay slump detachment (g)'" << "\t,\t" << "'Silt slump detachment (g)'" << "\t,\t" << "'Sand slump detachment (g)'" << endl;      
   }

   if (m_bToppleDetachTS)
   {
      // Toppling detachment (same as toppling deposition)
      strTSFile = m_strOutputPath;
      strTSFile.append(TOPPLE_DETACHMENT_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open toppling detachment time-series CSV file
      ToppleDetachmentTSStream.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! ToppleDetachmentTSStream)
      {
         // Error, cannot open toppling detachment time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }
      
      // Write header line
      ToppleDetachmentTSStream << "'Elapsed'" << "\t,\t" << "'Since last'" << "\t,\t" << "'Clay toppling detachment (g)'" << "\t,\t" << "'Silt toppling detachment (g)'" << "\t,\t" << "'Sand toppling detachment (g)'" << endl;      
   }

   if (m_bSoilWaterTS)
   {
      // Soil water (per layer, mm total)
      strTSFile = m_strOutputPath;
      strTSFile.append(SOIL_WATER_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);
      
      // Open soil water time-series CSV file
      SoilWaterTSStream.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! SoilWaterTSStream)
      {
         // Error, cannot soil water time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }
      
      // Write header line
      SoilWaterTSStream << "'Elapsed'" << "\t,\t" << "'Since last'";
      for (int nLayer = 0; nLayer < m_nNumSoilLayers; nLayer++)
         SoilWaterTSStream << "\t,\t" << "'Soil water for layer " << nLayer+1 << " (l)'";
      SoilWaterTSStream << endl;      
   }
   
   return (true);
}


/*========================================================================================================================================

 Write the results for this iteration to the .out file

========================================================================================================================================*/
bool CSimulation::bWritePerIterationResults(void)
{
   // Output per-iteration hydrology
   OutStream << setw(7)  << m_ulIter;
   OutStream << setprecision(4);
   OutStream << setw(11) << m_dSimulatedTimeElapsed;

   // All these displayed as volumes (mm3)
   OutStream << setprecision(0);
   OutStream << setw(7)  << m_dThisIterRain * m_dCellSquare;
   if (m_bRunOn)
      OutStream << setw(6)  << m_dThisIterRunOn * m_dCellSquare;
   else
      OutStream << "      -";
   if (m_bInfiltThisIter)
      OutStream << setw(7)  << m_dThisIterInfiltration * m_dCellSquare;
   else
      OutStream << "      -";
   OutStream << setw(11) << m_dThisIterSurfaceWaterStored * m_dCellSquare;
   OutStream << setw(8)  << m_dThisIterWaterLost * m_dCellSquare;
   OutStream << " ";

   // Output per-iteration flow erosion details, all displayed as volumes (mm3)
   OutStream << setw(8)  << (m_dThisIterClayFlowDetach + m_dThisIterSiltFlowDetach + m_dThisIterSandFlowDetach) * m_dCellSquare;
   OutStream << setw(10) << (m_dThisIterClaySedLoad + m_dThisIterSiltSedLoad + m_dThisIterSandSedimentLoad) * m_dCellSquare;
   OutStream << setw(8)  << (m_dThisIterClayFlowDeposit + m_dThisIterSiltFlowDeposit + m_dThisIterSandFlowDeposit) * m_dCellSquare;
   OutStream << setw(8)  << (m_dThisIterClaySedLost + m_dThisIterSiltSedLost + m_dThisIterSandSedLost) * m_dCellSquare;
   OutStream << " ";

   if (m_bSplashThisIter)
   {
      // OK, we are calculating splash, and we are doing so this iteration, so output per-iteration splash redistribution, all as volumes (mm3)
//       OutStream << setprecision(1);
      OutStream << setw(8) << (m_dThisIterClaySplashDetach + m_dThisIterSiltSplashDetach + m_dThisIterSandSplashDetach) * m_dCellSquare;
//       OutStream << setw(8) << (m_dThisIterClaySplashDeposit + m_dThisIterSiltSplashDeposit + m_dThisIterSandSplashDeposit) * m_dCellSquare;
   }
   else
      OutStream << "       -       - ";

   if (m_bSlumpThisIter)
   {
      // OK, we are calculating slump, and we are doing so this iteration, so output per-iteration slumping and toppling, all as volumes (mm3)
//       OutStream << setprecision(0);
      OutStream << setw(10) << (m_dThisIterClaySlumpDetach + m_dThisIterSiltSlumpDetach + m_dThisIterSandSlumpDetach) * m_dCellSquare;
      OutStream << setw(10) << (m_dThisIterClaySlumpDeposit + m_dThisIterSiltSlumpDeposit + m_dThisIterSandSlumpDeposit) * m_dCellSquare;
      OutStream << setw(10) << (m_dThisIterClayToppleDetach + m_dThisIterSiltToppleDetach + m_dThisIterSandToppleDetach) * m_dCellSquare;
      OutStream << setw(10) << (m_dThisIterClayToppleDeposit + m_dThisIterSiltToppleDeposit + m_dThisIterSandToppleDeposit) * m_dCellSquare;
   }
   else
      OutStream << "         -         -         -         -";

   if (m_bInfiltThisIter)
   {
//       OutStream << setprecision(1);
      OutStream << setw(10) << (m_dThisIterClayInfiltDeposit + m_dThisIterSiltInfiltDeposit + m_dThisIterSandInfiltDeposit) * m_dCellSquare;
   }
   else
      OutStream << "         -";

   // Finally, set "markers" for events (rainfall change, files saves) that have occurred this iteration
   if (m_bThisIterRainChange)
      OutStream << " dRain";

   if (m_bSaveGISThisIter)
      OutStream << " GIS" << m_nGISSave;

   OutStream << endl;

   // Did a text file write error occur?
   if (OutStream.fail())
      return (false);

   return (true);
}


/*========================================================================================================================================

 Write the results for this iteration to the time series CSV files

========================================================================================================================================*/
bool CSimulation::bWriteTSFiles(bool const bIsLastIter)
{
   static double sdLastSimulatedTimeElapsed = 0;
   
   /// First do the ones which are output every iteration   
   if (m_bTimeStepTS)
   {
      // Just output the timestep, in sec
      TimeStepTSStream << m_dSimulatedTimeElapsed << "\t,\t" << m_dSimulatedTimeElapsed - sdLastSimulatedTimeElapsed << "\t,\t" << m_dTimeStep << endl;

      // Did a timestep time series file write error occur?
      if (TimeStepTSStream.fail())
         return (false);
   }

   if (m_bAreaWetTS)
   {
      // Output as a percentage of the total area
      AreaWetTSStream << m_dSimulatedTimeElapsed << "\t,\t" << m_dSimulatedTimeElapsed - sdLastSimulatedTimeElapsed << "\t,\t" << 100.0 * static_cast<double>(m_ulNWet) / static_cast<double>(m_ulNActiveCells) << endl;

      // Did a rainfall time series file write error occur?
      if (AreaWetTSStream.fail())
         return (false);
   }

   if (m_bRainTS)
   {
      // Output as a depth in mm
      RainTSStream << m_dSimulatedTimeElapsed << "\t,\t" << m_dSimulatedTimeElapsed - sdLastSimulatedTimeElapsed << "\t,\t" << m_dThisIterRain * m_dCellSquare << endl;

      // Did a rainfall time series file write error occur?
      if (RainTSStream.fail())
         return (false);
   }

   if (m_bRunOnTS)
   {
      // Convert run-on water in mm3 to litres
      RunOnTSStream << m_dSimulatedTimeElapsed << "\t,\t" << m_dSimulatedTimeElapsed - sdLastSimulatedTimeElapsed << "\t,\t" << m_dThisIterRunOn * m_dCellSquare * 1e-6 << endl;

      // Did a run-on time series file write error occur?
      if (RunOnTSStream.fail())
         return (false);
   }
   
   if (m_bSurfaceWaterTS)
   {
      // Convert surface water in mm3 to litres
      SurfaceWaterTSStream << m_dSimulatedTimeElapsed << "\t,\t" << m_dThisIterSurfaceWaterStored * m_dCellSquare * 1e-6 << endl;
      
      // Did a surface water time series file write error occur?
      if (SurfaceWaterTSStream.fail())
         return (false);
   }
   
   if (m_bSurfaceWaterLostTS)
   {
      // Convert water lost (i.e. discharge) in mm3 to litres
      SurfaceWaterLostTSStream << m_dSimulatedTimeElapsed << "\t,\t" << m_dSimulatedTimeElapsed - sdLastSimulatedTimeElapsed << "\t,\t" << m_dThisIterWaterLost * m_dCellSquare * 1e-6 << endl;
      
      // Did a surface water lost time series file write error occur?
      if (SurfaceWaterLostTSStream.fail())
         return (false);
   }
   
   if (m_bFlowDetachTS)
   {
      // Output flow detachment for each size class, convert from mm3 to g (bulk density is in kg/m3, so multiply by 1e6 to get it into g/mm3)
      FlowDetachmentTSStream << m_dSimulatedTimeElapsed << "\t,\t" << m_dSimulatedTimeElapsed - sdLastSimulatedTimeElapsed << "\t,\t" << m_dThisIterClayFlowDetach * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << "\t,\t" << m_dThisIterSiltFlowDetach * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << "\t,\t" << m_dThisIterSandFlowDetach * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << endl;
      
      // Did a flow detachment time series file write error occur?
      if (FlowDetachmentTSStream.fail())
         return (false);
   }
   
   if (m_bDoFlowDepositionTS)
   {
      // Output flow deposition for each size class, convert from mm3 to g (bulk density is in kg/m3, so multiply by 1e6 to get it into g/mm3)
      FlowDepositionTSStream << m_dSimulatedTimeElapsed << "\t,\t" << m_dSimulatedTimeElapsed - sdLastSimulatedTimeElapsed << "\t,\t" << m_dThisIterClayFlowDeposit * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << "\t,\t" << m_dThisIterSiltFlowDeposit * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << "\t,\t" << m_dThisIterSandFlowDeposit * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << endl;
      
      // Did a flow deposition time series file write error occur?
      if (FlowDepositionTSStream.fail())
         return (false);
   }
   
   if (m_bSedLostTS)
   {
      // Output sediment lost for each size class, convert from mm3 to g (bulk density is in kg/m3, so multiply by 1e6 to get it into g/mm3)
      SedimentLostTSStream << m_dSimulatedTimeElapsed << "\t,\t" << m_dSimulatedTimeElapsed - sdLastSimulatedTimeElapsed << "\t,\t" << m_dThisIterClaySedLost * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << "\t,\t" << m_dThisIterSiltSedLost * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << "\t,\t" << m_dThisIterSandSedLost * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << endl;
      
      // Did a sediment lost time series file write error occur?
      if (SedimentLostTSStream.fail())
         return (false);
   }
   
   if (m_bSedLoadTS)
   {
      // Output sediment load for each size class, convert from mm3 to g (bulk density is in kg/m3, so multiply by 1e6 to get it into g/mm3)
      SedimentLoadTSStream << m_dSimulatedTimeElapsed << "\t,\t" << m_dSimulatedTimeElapsed - sdLastSimulatedTimeElapsed << "\t,\t" << m_dThisIterClaySedLoad * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << "\t,\t" << m_dThisIterSiltSedLoad * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << "\t,\t" << m_dThisIterSandSedimentLoad * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << endl;
      
      // Did a sediment load time series file write error occur?
      if (SedimentLoadTSStream.fail())
         return (false);
   }
   
   // Now do the ones which are output less frequently
   static double sdLastInfiltSimulatedTimeElapsed = 0;
   
   if (m_bInfiltTS && (m_bInfiltThisIter || bIsLastIter))
   {
      // Convert surface water lost to soil water by infiltration in mm3 to litres
      InfiltrationTSStream << m_dSimulatedTimeElapsed << "\t,\t" << m_dSimulatedTimeElapsed - sdLastInfiltSimulatedTimeElapsed << "\t,\t" << m_dSinceLastTSInfiltration * m_dCellSquare * 1e-6 << endl;
      
      m_dSinceLastTSInfiltration = 0;

      // Did an infiltration time series file write error occur?
      if (InfiltrationTSStream.fail())
         return (false);
   }

   if (m_bExfiltTS && (m_bInfiltThisIter || bIsLastIter))
   {
      // Convert soil water returned to surface water by exfiltration in mm3 to litres
      ExfiltrationTSStream << m_dSimulatedTimeElapsed << "\t,\t" << m_dSimulatedTimeElapsed - sdLastInfiltSimulatedTimeElapsed << "\t,\t" << m_dSinceLastTSExfiltration * m_dCellSquare * 1e-6 << endl;
      
      m_dSinceLastTSInfiltration = 0;
      
      // Did an exfiltration time series file write error occur?
      if (ExfiltrationTSStream.fail())
         return (false);
   }
   
   if (m_bInfiltDepositTS && (m_bInfiltThisIter || bIsLastIter))
   {
      // Output infiltration deposition for each size class, convert from mm3 to g (bulk density is in kg/m3, so multiply by 1e6 to get it into g/mm3)
      InfiltrationDepositionTSStream << m_dSimulatedTimeElapsed << "\t,\t" << m_dSimulatedTimeElapsed - sdLastInfiltSimulatedTimeElapsed << "\t,\t" << m_dSinceLastTSClayInfiltDeposit * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << "\t,\t" << m_dSinceLastTSSiltInfiltDeposit * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << "\t,\t" << m_dSinceLastTSSandInfiltDeposit * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << endl;
      
      m_dSinceLastTSClayInfiltDeposit =
      m_dSinceLastTSSiltInfiltDeposit =
      m_dSinceLastTSSandInfiltDeposit = 0;

      // Did an infiltration deposition time series file write error occur?
      if (InfiltrationDepositionTSStream.fail())
         return (false);
   }

   static double sdLastSplashSimulatedTimeElapsed = 0;
   
   if (m_bSplashRedistTS && (m_bSplashThisIter || bIsLastIter))
   {
      // Output splash redistribution for each size class, convert from mm3 to g (bulk density is in kg/m3, so multiply by 1e6 to get it into g/mm3)
      SplashDetachmentTSStream << m_dSimulatedTimeElapsed << "\t,\t" << m_dSimulatedTimeElapsed - sdLastSplashSimulatedTimeElapsed << "\t,\t" << m_dSinceLastTSClaySplashRedist * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << "\t,\t" << m_dSinceLastTSSiltSplashRedist * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << "\t,\t" << m_dSinceLastTSSandSplashRedist * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << endl;
      
      m_dSinceLastTSClaySplashRedist =
      m_dSinceLastTSSiltSplashRedist =
      m_dSinceLastTSSandSplashRedist = 0;      
      
      // Did a splash redistribution time series file write error occur?
      if (SplashDetachmentTSStream.fail())
         return (false);
   }

   if (m_bSplashKETS && (m_bSplashThisIter || bIsLastIter))
   {
      // Write out rainfall kinetic energy in Joules
      SplashKineticEnergyTSStream << m_dSimulatedTimeElapsed << "\t,\t" << m_dSimulatedTimeElapsed - sdLastSplashSimulatedTimeElapsed << "\t,\t" << m_dSinceLastTSKE << endl;
      
      m_dSinceLastTSKE = 0;

      // Did a splash deposition time series file write error occur?
      if (SplashKineticEnergyTSStream.fail())
         return (false);
   }

   static double sdLastSlumpSimulatedTimeElapsed = 0;
   
   if (m_bSlumpDetachTS && (m_bSlumpThisIter || bIsLastIter))
   {
      // Output slump detachment for each size class, convert from mm3 to g (bulk density is in kg/m3, so multiply by 1e6 to get it into g/mm3)
      SlumpDetachmentTSStream << m_dSimulatedTimeElapsed << "\t,\t" << m_dSimulatedTimeElapsed - sdLastSlumpSimulatedTimeElapsed << "\t,\t" << m_dSinceLastTSClaySlumpDetach * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << "\t,\t" << m_dSinceLastTSSiltSlumpDetach * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << "\t,\t" << m_dSinceLastTSSandSlumpDetach * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << endl;
      
      m_dSinceLastTSClaySlumpDetach =
      m_dSinceLastTSSiltSlumpDetach =
      m_dSinceLastTSSandSlumpDetach = 0;      

      // Did a slumping time series file write error occur?
      if (SlumpDetachmentTSStream.fail())
         return (false);
   }

   if (m_bToppleDetachTS && (m_bSlumpThisIter || bIsLastIter))
   {
      // Output toppling detachment for each size class, convert from mm3 to g (bulk density is in kg/m3, so multiply by 1e6 to get it into g/mm3)
      ToppleDetachmentTSStream << m_dSimulatedTimeElapsed << "\t,\t" << m_dSimulatedTimeElapsed - sdLastSlumpSimulatedTimeElapsed << "\t,\t" << m_dSinceLastTSClayToppleDetach * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << "\t,\t" << m_dSinceLastTSSiltToppleDetach * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << "\t,\t" << m_dSinceLastTSSandToppleDetach * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 << endl;

      m_dSinceLastTSClayToppleDetach =
      m_dSinceLastTSSiltToppleDetach =
      m_dSinceLastTSSandToppleDetach = 0;      
      
      // Did a toppling time series file write error occur?
      if (ToppleDetachmentTSStream.fail())
         return (false);
   }

   static double sdLastSoilWaterSimulatedTimeElapsed = 0;

   if (m_bSoilWaterTS && (m_bInfiltThisIter || bIsLastIter))
   {
      // Output soil water content for each soil layer, in litres
      SoilWaterTSStream << m_dSimulatedTimeElapsed << "\t,\t" << m_dSimulatedTimeElapsed - sdLastSoilWaterSimulatedTimeElapsed;
      for (int nLayer = 0; nLayer < m_nNumSoilLayers; nLayer++)
      {
         SoilWaterTSStream << "\t,\t" << m_VdSinceLastTSSoilWater[nLayer] * m_dCellSquare * 1e-6;
         m_VdSinceLastTSSoilWater[nLayer] = 0;
      }
      SoilWaterTSStream << endl;
      
      // Did a soil water time series file write error occur?
      if (SoilWaterTSStream .fail())
         return (false);
   }
   
   // Update for next time
   sdLastSimulatedTimeElapsed          = 
   sdLastInfiltSimulatedTimeElapsed    =
   sdLastSplashSimulatedTimeElapsed    = 
   sdLastSlumpSimulatedTimeElapsed     = 
   sdLastSoilWaterSimulatedTimeElapsed = m_dSimulatedTimeElapsed;
   
   return (true);
}


