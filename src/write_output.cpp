/*========================================================================================================================================

This is write_output.cpp: writes non-GIS output for RillGrow

Copyright (C) 2025 David Favis-Mortlock

=========================================================================================================================================

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public  License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

========================================================================================================================================*/
#include "rg.h"
#include "simulation.h"
#include "cell.h"

//=========================================================================================================================================
//! Writes run details to Out and Log files
//=========================================================================================================================================
void CSimulation::WriteRunDetails(void)
{
   // Set default output format to be fixed point
   m_ofsOut << resetiosflags(ios::floatfield) << std::fixed << setprecision(2);

   m_ofsOut << PROGNAME << " for " << PLATFORM << " " << strGetBuild() << " on " << strGetComputerName() << endl << endl;

   m_ofsLog << PROGNAME << " for " << PLATFORM << " " << strGetBuild() << " on " << strGetComputerName() << endl << endl;

   // --------------------------------------------------------- Run Information ----------------------------------------------------------
   m_ofsOut << "RUN DETAILS" << endl;
   m_ofsOut << " Name                                                   \t: " << m_strRunName << endl;
   m_ofsOut << " Started at                                             \t: " << ctime(&m_tSysStartTime);   //  << endl;

   // Same info. for Log file
   m_ofsLog << m_strRunName << " run started at " << ctime(&m_tSysStartTime) << endl;

   // Contine with Out file
   m_ofsOut << " Initialization file                                    \t: " << m_strRGIni << endl;
   m_ofsOut << " Data read from                                         \t: " << m_strDataPathName << endl;
   if (m_bSplash)
      m_ofsOut << " Splash efficiency parameters read from                 \t: " << m_strSplashAttenuationFile << endl;
   m_ofsOut << " Duration of simulation                                 \t: ";
   m_ofsOut << strDispTime(m_dSimulationDuration, true, false) << endl;
   if (! bFpEQ(m_dSimulationDuration, m_dSimulatedRainDuration, 0.1))
   {
      m_ofsOut << " Duration of rainfall                                   \t: ";
      m_ofsOut << strDispTime(m_dSimulatedRainDuration, true, false) << endl;
   }
   if (m_bSaveRegular)
   {
      // Saves at regular intervals
      m_ofsOut << " Time between saves                                     \t: ";
      m_ofsOut << strDispTime(m_dRSaveInterval, true, false) << endl;
   }
   else
   {
      // Saves at user-defined intervals
      m_ofsOut << " Saves at                                               \t: ";
      string strTmp;
      for (int i = 0; i < m_nUSave; i++)
      {
         strTmp.append(strDispTime(m_VdSaveTime[i], true, false));
         strTmp.append(" ");

         WrapLongString(&strTmp);
      }

      // Also at end of run
      strTmp.append(strDispTime(m_dSimulationDuration, true, false));
      m_ofsOut << strTmp << endl;
   }

   m_ofsOut << " Random number seeds                                    \t: ";
   {
      for (int i = 0; i < NUMBER_OF_RNGS; i++)
         m_ofsOut << m_ulRandSeed[i] << '\t';
   }
   m_ofsOut << endl;

   // Note must modify if more RNGs added
   m_ofsOut << "*First random numbers generated                         \t: " << ulGetRand0() << '\t' << ulGetRand1() << endl;

   m_ofsOut << " GIS output format                                      \t: " << m_strGDALOutputDriverLongname << endl;
   m_ofsOut << " Optional GIS files saved                               \t: ";

   string strTmp;
   if (m_bRainVarMSave)
   {
      strTmp.append(GIS_RAIN_VARIATION_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bCumulRunOnSave)
   {
      strTmp.append(GIS_CUMUL_RUNON_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bInitElevSave)
   {
      strTmp.append(GIS_INITIAL_ELEVATION_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bElevSave)
   {
      strTmp.append(GIS_ELEVATION_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bDetrendElevSave)
   {
      strTmp.append(GIS_DETREND_ELEVATION_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bFlowDetachSave)
   {
      strTmp.append(GIS_ALL_SIZE_FLOW_DETACH_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bInfiltSave)
   {
      strTmp.append(GIS_INFILT_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bCumulInfiltSave)
   {
      strTmp.append(GIS_CUMUL_INFILT_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSoilWaterSave)
   {
      strTmp.append(GIS_SOIL_WATER_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bInfiltDepositSave)
   {
      strTmp.append(GIS_INFILT_DEPOSIT_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bCumulInfiltDepositSave)
   {
      strTmp.append(GIS_CUMUL_INFILT_DEPOSIT_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bTopSurfaceSave)
   {
      strTmp.append(GIS_TOP_SURFACE_DETREND_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSplashSave)
   {
      strTmp.append(GIS_SPLASH_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bCumulSplashSave)
   {
      strTmp.append(GIS_CUMUL_SPLASH_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bInundationSave)
   {
      strTmp.append(GIS_INUNDATION_REGIME_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bFlowDirSave)
   {
      strTmp.append(GIS_SURFACE_WATER_DIRECTION_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bStreamPowerSave)
   {
      strTmp.append(GIS_STREAMPOWER_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bShearStressSave)
   {
      strTmp.append(GIS_SHEAR_STRESS_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bFrictionFactorSave)
   {
      strTmp.append(GIS_FRICTION_FACTOR_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bCumulAvgShearStressSave)
   {
      strTmp.append(GIS_AVG_SHEAR_STRESS_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bReynoldsSave)
   {
      strTmp.append(GIS_REYNOLDS_NUMBER_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bFroudeSave)
   {
      strTmp.append(GIS_FROUDE_NUMBER_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bTCSave)
   {
      strTmp.append(GIS_TRANSPORT_CAPACITY_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bLostSave)
   {
      strTmp.append(GIS_AVG_SURFACE_WATER_FROM_EDGES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bCumulAvgDWSpdSave)
   {
      strTmp.append(GIS_AVG_SURFACE_WATER_DW_SPEED_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bCumulAvgSpdSave)
   {
      strTmp.append(GIS_AVG_SURFACE_WATER_SPEED_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bCumulAvgDepthSave)
   {
      strTmp.append(GIS_AVG_SURFACE_WATER_DEPTH_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSedConcSave)
   {
      strTmp.append(GIS_SEDIMENT_CONCENTRATION_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSedLoadSave)
   {
      strTmp.append(GIS_SEDIMENT_LOAD_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bCumulFlowDepositSave)
   {
      strTmp.append(GIS_CUMUL_ALL_SIZE_FLOW_DEPOSIT_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSlumpSave)
   {
      strTmp.append(GIS_CUMUL_SLUMP_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bToppleSave)
   {
      strTmp.append(GIS_CUMUL_TOPPLE_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bCumulLoweringSave)
   {
      strTmp.append(GIS_CUMUL_ALL_PROC_SURF_LOWER_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   m_ofsOut << strTmp << endl;
   m_ofsOut << " GIS output format                                      \t: " << m_strGISOutFormat << endl;
   m_ofsOut << " Output file (this file)                                \t: " << m_strOutputPath << endl;
   m_ofsOut << " Log file                                               \t: " << m_strLogFile << endl;

   m_ofsOut << " Optional time series files saved                       \t: ";

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
      strTmp.append(INFILT_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bExfiltTS)
   {
      strTmp.append(EXFILT_TIME_SERIES_CODE);
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
      strTmp.append(FLOW_DETACH_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSlumpDetachTS)
   {
      strTmp.append(SLUMP_DETACH_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bToppleDetachTS)
   {
      strTmp.append(TOPPLE_DETACH_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bDoSedLoadDepositTS)
   {
      strTmp.append(SEDLOAD_DEPOSIT_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSedOffEdgeTS)
   {
      strTmp.append(SEDLOAD_LOST_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSedLoadTS)
   {
      strTmp.append(SEDLOAD_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bInfiltDepositTS)
   {
      strTmp.append(INFILT_DEPOSIT_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSplashRedistTS)
   {
      strTmp.append(SPLASH_REDIST_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   if (m_bSplashKETS)
   {
      strTmp.append(SPLASH_KE_TIME_SERIES_CODE);
      strTmp.append(" ");

      WrapLongString(&strTmp);
   }

   m_ofsOut << strTmp << endl << endl;

   // --------------------------------------------------------- Microtopography ----------------------------------------------------------
   m_ofsOut << "MICROTOPOGRAPHY" << endl;
   m_ofsOut << resetiosflags(ios::floatfield) << std::fixed << setprecision(2);
   m_ofsOut << " Input microtopography (DEM) file                       \t: " << m_strDEMFile << endl;
   m_ofsOut << " Z (vertical) units                                     \t: ";
   if (m_nZUnits == Z_UNIT_MM)
      m_ofsOut << "mm";
   else if (m_nZUnits == Z_UNIT_CM)
      m_ofsOut << "cm";
   else if (m_nZUnits == Z_UNIT_M)
      m_ofsOut << "m";
   m_ofsOut << endl;
   m_ofsOut << " GDAL DEM driver code                                   \t: " << m_strGDALDEMDriverCode << endl;
   m_ofsOut << " GDAL DEM driver description                            \t: " << m_strGDALDEMDriverDesc << endl;
   m_ofsOut << " GDAL DEM projection                                    \t: " << m_strGDALDEMProjection << endl;
   m_ofsOut << " GDAL DEM data type                                     \t: " << m_strGDALDEMDataType << endl;
   m_ofsOut << " DEM grid size (x-y)                                    \t: " << m_nXGridMax << " x " << m_nYGridMax << endl;
   m_ofsOut << " Number of cells                                        \t: " << m_nXGridMax * m_nYGridMax << endl;
   m_ofsOut << " Number of NODATA cells                                 \t: " << m_ulMissingValueCells << endl;
   m_ofsOut << " Number of valid cells                                  \t: " << m_ulNActiveCells << endl;
   m_ofsOut << setprecision(2);
   m_ofsOut << "*DEM X co-ordinates                                     \t: " << m_dMinX << " - " << m_dMaxX << endl;
   m_ofsOut << "*DEM Y co-ordinates                                     \t: " << m_dMinY << " - " << m_dMaxY << endl;
   m_ofsOut << "*DEM area (including NODATA cells)                      \t: " << m_nXGridMax * m_nYGridMax * m_dCellSquare << " mm2" << endl;
   m_ofsOut << "*DEM area (including NODATA cells)                      \t: " << m_nXGridMax * m_nYGridMax * m_dCellSquare * 1e-6 << " m2" << endl;
   m_ofsOut << "*DEM area (only active cells)                           \t: " << static_cast<double>(m_ulNActiveCells) * m_dCellSquare << " mm2" << endl;
   m_ofsOut << "*DEM area (only active cells)                           \t: " << static_cast<double>(m_ulNActiveCells) * m_dCellSquare * 1e-6 << " m2" << endl;
   m_ofsOut << "*DEM cell size                                          \t: " << m_dCellSide << " mm" << endl;
   m_ofsOut << " DEM bounded edges                                      \t: ";
   if (m_bClosedThisEdge[EDGE_TOP])
      m_ofsOut << "t";
   if (m_bClosedThisEdge[EDGE_RIGHT])
      m_ofsOut << "r";
   if (m_bClosedThisEdge[EDGE_BOTTOM])
      m_ofsOut << "b";
   if (m_bClosedThisEdge[EDGE_LEFT])
      m_ofsOut << "l";
   m_ofsOut << endl;
   m_ofsOut << "*Average DEM elevation (inc datum, pre slope imposed)   \t: " << m_dAvgElev << " mm" << endl;
   m_ofsOut << "*Minimum DEM elevation (inc datum, pre slope imposed)   \t: " << m_dMinElev << " mm" << endl;
   m_ofsOut << " DEM slope increased by                                 \t: " << m_dGradient << " %" << endl;
   m_ofsOut << "*DEM slope increased by                                 \t: ";
   if (m_dGradient > 0)
      m_ofsOut << atan2(m_dGradient, 100) * 180 / PI;
   else
      m_ofsOut << "0";
   m_ofsOut << " degrees" << endl;
   m_ofsOut << "*DEM maximum elevation (after slope imposed)            \t: " << m_dPlotElevMax << " mm" << endl;
   m_ofsOut << "*DEM minimum elevation (after slope imposed)            \t: " << m_dPlotElevMin << " mm" << endl;
   m_ofsOut << " Difference in plot end elev (below lowest point)       \t: ";
   if (m_bHaveBaseLevel)
      m_ofsOut << m_dPlotEndDiff << " mm" << endl;
   else
      m_ofsOut << "none" << endl;
   m_ofsOut << "*DEM base level (after slope imposed)                   \t: ";
   if (m_bHaveBaseLevel)
      m_ofsOut << m_dBaseLevel << " mm" << endl;
   else
      m_ofsOut << "none" << endl;
   m_ofsOut << endl;

   // -------------------------------------------------------------- Soil ----------------------------------------------------------------
   m_ofsOut << "SOIL" << endl;
   m_ofsOut << resetiosflags(ios::floatfield) << std::fixed << setprecision(1);
   m_ofsOut << "Number of soil layers                                   \t: " << m_nNumSoilLayers << endl;
   m_ofsOut << "Elevation of base of lowest layer                       \t: " << m_dBasementElevation << " ";
   if (m_nZUnits == Z_UNIT_MM)
      m_ofsOut << "mm";
   else if (m_nZUnits == Z_UNIT_CM)
      m_ofsOut << "cm";
   else if (m_nZUnits == Z_UNIT_M)
      m_ofsOut << "m";
   m_ofsOut << endl;

   double dBelowTopLayerThickness = 0;
   for (int nLayer = 0; nLayer < m_nNumSoilLayers; nLayer++)
   {
      if (nLayer > 0)
         dBelowTopLayerThickness += m_VdInputSoilLayerThickness[nLayer];
   }

   double dTopLayerThickness = (m_dAvgElev - m_dBasementElevation) - dBelowTopLayerThickness;

   for (int nLayer = 0; nLayer < m_nNumSoilLayers; nLayer++)
   {
      m_ofsOut << "\tName of soil layer " << nLayer+1 << "                                 \t: " << m_VstrInputSoilLayerName[nLayer] << endl;

      double dThickness;
      if (nLayer == 0)
         dThickness = dTopLayerThickness;
      else
         dThickness = m_VdInputSoilLayerThickness[nLayer];

      m_ofsOut << "*\tClay component of soil layer " << nLayer+1 << ", equivalent thickness\t: " << dThickness * m_VdInputSoilLayerPerCentClay[nLayer] / 100 << " mm" << endl;
      m_ofsOut << "*\tSilt component of soil layer " << nLayer+1 << ", equivalent thickness\t: " << dThickness * m_VdInputSoilLayerPerCentSilt[nLayer] / 100 << " mm" << endl;
      m_ofsOut << "*\tSand component of soil layer " << nLayer+1 << ", equivalent thickness\t: " << dThickness * m_VdInputSoilLayerPerCentSand[nLayer] / 100 << " mm" << endl;
      m_ofsOut << resetiosflags(ios::floatfield) << std::fixed << setprecision(2);

      m_ofsOut << "\tBulk density of soil layer " << nLayer+1 << "                        \t: " << m_VdInputSoilLayerBulkDensity[nLayer] / 1000 << " t/m**3" << endl;
      m_ofsOut << "\tFlow erodibility of clay in soil layer " << nLayer+1 << "            \t: " << m_VdInputSoilLayerClayFlowErodibility[nLayer] << endl;
      m_ofsOut << "\tFlow erodibility of silt in soil layer " << nLayer+1 << "            \t: " << m_VdInputSoilLayerSiltFlowErodibility[nLayer] << endl;
      m_ofsOut << "\tFlow erodibility of sand in soil layer " << nLayer+1 << "            \t: " << m_VdInputSoilLayerSandFlowErodibility[nLayer] << endl;
      m_ofsOut << "\tSplash erodibility of clay in soil layer " << nLayer+1 << "          \t: " << m_VdInputSoilLayerClaySplashErodibility[nLayer] << endl;
      m_ofsOut << "\tSplash erodibility of silt in soil layer " << nLayer+1 << "          \t: " << m_VdInputSoilLayerSiltSplashErodibility[nLayer] << endl;
      m_ofsOut << "\tSplash erodibility of sand in soil layer " << nLayer+1 << "          \t: " << m_VdInputSoilLayerSandSplashErodibility[nLayer] << endl;
      m_ofsOut << "\tSlump erodibility of clay in soil layer " << nLayer+1 << "           \t: " << m_VdInputSoilLayerClaySlumpErodibility[nLayer] << endl;
      m_ofsOut << "\tSlump erodibility of silt in soil layer " << nLayer+1 << "           \t: " << m_VdInputSoilLayerSiltSlumpErodibility[nLayer] << endl;
      m_ofsOut << "\tSlump erodibility of sand in soil layer " << nLayer+1 << "           \t: " << m_VdInputSoilLayerSandSlumpErodibility[nLayer] << endl;
   }
   m_ofsOut << endl;

   // ------------------------------------------------------------- Rainfall -------------------------------------------------------------
   m_ofsOut << "RAINFALL" << endl;
   if (m_bTimeVaryingRain)
   {
      m_ofsOut << " Rainfall intensities (mm/hr) during run                \t: ";
      for (int i = 0; i <= m_nRainChangeTimeMax; i++)
      {
         m_ofsOut << m_VdRainChangeIntensity[i] << " at " << strDispTime(m_VdRainChangeTime[i], true, false) << "\t";
         if (3 == i)
            m_ofsOut << endl << "                                                    \t: ";
      }
      m_ofsOut << endl;
   }
   else
   {
      m_ofsOut << " Mean rainfall rate                                     \t: " << m_dSpecifiedRainIntensity << " mm/hr" << endl;
   }
   m_ofsOut << " SD of rainfall rate                                    \t: " << m_dStdRainInt << " mm/hr" << endl;
   m_ofsOut << " Mean raindrop diameter                                 \t: " << m_dDropDiameter << " mm" << endl;
   m_ofsOut << " SD of raindrop diameter                                \t: " << m_dStdDropDiameter << " mm" << endl;
   m_ofsOut << "*Mean raindrop volume                                   \t: " << m_dMeanCellWaterVol << " mm**3" << endl;
   m_ofsOut << setprecision(4);
   m_ofsOut << "*SD of raindrop volume                                  \t: " << m_dStdCellWaterVol << " mm**3" << endl;
   m_ofsOut << "*Mean initial water depth " << (m_bDoInfiltration ?  "(pre-infilt)            \t: " : "                              \t: ") << m_dMeanCellWaterVol * m_dInvCellSquare << " mm" << endl;
   m_ofsOut << setprecision(2);
   m_ofsOut << " Spatial mask of rainfall variation multipliers         \t: " << m_strRainVarMFile << endl;
if (! m_strRainVarMFile.empty())
   {
      m_ofsOut << "*Mean value of rainfall variation multipliers           \t: " << m_dRainVarMFileMean << endl;
      m_ofsOut << " GDAL rainfall variation driver code                    \t: " << m_strGDALRainVarDriverCode << endl;
      m_ofsOut << " GDAL rainfall variation driver description             \t: " << m_strGDALRainVarDriverDesc << endl;
      m_ofsOut << " GDAL rainfall variation projection                     \t: " << m_strGDALRainVarProjection << endl;
      m_ofsOut << " GDAL rainfall variation data type                      \t: " << m_strGDALRainVarDataType << endl;
   }
   if (m_bSplash)
      m_ofsOut << " Raindrop fall speed                                    \t: " << m_dRainSpeed << " m/sec" << endl;
   m_ofsOut << endl;

   // ----------------------------------------------------- Splash redistribution --------------------------------------------------------
   m_ofsOut << "SPLASH REDISTRIBUTION" << endl;
   m_ofsOut << " Simulate splash redistribution?                        \t: " << (m_bSplash ? "y" : "n") << endl;
   if (m_bSplash)
   {
      m_ofsOut << " Water depth (x raindrop diameter)                      \t: ";
      int nLen = static_cast<int>(m_VdSplashDepth.size());
      for (int i = 0; i < nLen; i++)
         m_ofsOut << m_VdSplashDepth[i] / m_dDropDiameter << ((i < nLen-1) ? "\t" : "");
      m_ofsOut << endl;
      m_ofsOut << "*Water depth                                            \t: ";
      for (int i = 0; i < nLen; i++)
         m_ofsOut << m_VdSplashDepth[i] << ((i < nLen-1) ? "\t" : "");
      m_ofsOut << " mm" << endl;
      m_ofsOut << " Normalized splash efficiency (0-1)                     \t: ";
      for (int i = 0; i < nLen; i++)
         m_ofsOut << m_VdSplashEff[i] << ((i < nLen-1) ? "\t" : "");
      m_ofsOut << endl;
      m_ofsOut << "*Normalized splash attenuation (0-1)                    \t: ";
      for (int i = 0; i < nLen; i++)
         m_ofsOut << 1 - m_VdSplashEff[i] << ((i < nLen-1) ? "\t" : "");
      m_ofsOut << endl;
      m_ofsOut << setprecision(1);


      m_ofsOut << " Splash constant                                        \t: " << m_dSplashConstant << " sec**2/kg.m" << endl;
      m_ofsOut << "*Normalised splash constant                             \t: " << m_dSplashConstantNormalized << " sec**2/mm" << endl;
      m_ofsOut << setprecision(2);

      if (m_bPoesenSplashEqn)
      {
         m_ofsOut << "Using Poesen splash equation" << endl;
         m_ofsOut << " Constant in Poesen splash equation                     \t: " << m_dPoesenSplashConstant << endl;
      }
      if (m_bPlanchonSplashEqn)
      {
         m_ofsOut << "Using Planchon et al. splash equation" << endl;
         m_ofsOut << "*Grid size correction to Laplacian                      \t: " << m_dPlanchonCellSizeKC * m_dCellSquare << endl;
      }
   }
   m_ofsOut << endl;

   // --------------------------------------------------------- Run-on and run-off -------------------------------------------------------
   m_ofsOut << "RUN-ON AND RUN-OFF" << endl;
   m_ofsOut << " Upslope run-on area?                                   \t: " << (m_bRunOn ? "y" : "n") << endl;
   if (m_bRunOn)
   {
      m_ofsOut << " Run-on from these edges                                \t: ";
      if (m_bRunOnThisEdge[EDGE_TOP])
         m_ofsOut << "t";
      if (m_bRunOnThisEdge[EDGE_RIGHT])
         m_ofsOut << "r";
      if (m_bRunOnThisEdge[EDGE_BOTTOM])
         m_ofsOut << "b";
      if (m_bRunOnThisEdge[EDGE_LEFT])
         m_ofsOut << "l";
      m_ofsOut << endl;
      m_ofsOut << " Length of upslope run-on area                          \t: " << m_dRunOnLen << " mm" << endl;
      m_ofsOut << " Rain spatial variation multiplier for run-on area      \t: " << m_dRunOnRainVarM << endl;
      m_ofsOut << " Flow speed on run-on area                              \t: " << m_dRunOnSpd << " mm/sec" << endl;
   }

   m_ofsOut << " Off-edge parameter A                                  \t: " << setprecision(4) << m_dOffEdgeParamA << endl;
   m_ofsOut << " Off-edge parameter B                                  \t: " << m_dOffEdgeParamB << endl;
   m_ofsOut << "*Off-edge head constant                                \t: " << m_dOffEdgeHeadConst << endl;
   m_ofsOut << setprecision(2);
   m_ofsOut << endl;

   // ---------------------------------------------------------- Infiltration ------------------------------------------------------------
   m_ofsOut << "INFILT" << endl;
   m_ofsOut << " Infiltration considered?                               \t: " << (m_bDoInfiltration ? "y" : "n") << endl;
   if (m_bDoInfiltration)
   {
      for (int nLayer = 0; nLayer < m_nNumSoilLayers; nLayer++)
      {
         m_ofsOut << "\tName of soil layer " << nLayer+1 << "                                 \t: " << m_VstrInputSoilLayerName[nLayer] << endl;
         m_ofsOut << "\tAir exit head                                       \t: " << m_VdInputSoilLayerInfiltAirHead[nLayer] << " cm" << endl;
         m_ofsOut << " Exponent of Brooks-Corey water retention equation      \t: " << m_VdInputSoilLayerInfiltLambda[nLayer] << endl;
         m_ofsOut << " Saturated volumetric water content                     \t: " << m_VdInputSoilLayerInfiltSatWater[nLayer] << " cm**3/cm**3" << endl;
         m_ofsOut << " Initial volumetric water content                       \t: " << m_VdInputSoilLayerInfiltInitWater[nLayer] << " cm**3/cm**3" << endl;
         m_ofsOut << " Saturated hydraulic conductivity                       \t: " << m_VdInputSoilLayerInfiltKSat[nLayer] << " cm/h" << endl;
      }
   }
   m_ofsOut << endl;

   // ---------------------------------------------------------- Overland Flow -----------------------------------------------------------
   m_ofsOut << "OVERLAND FLOW" << endl;
   if (m_bManningFlowSpeedEqn)
   {
      m_ofsOut << " Using Manning-type flow speed equation" << endl;
      m_ofsOut << " Parameter A                                               \t: " << m_dManningParamA << endl;
      m_ofsOut << " Parameter B                                               \t: " << m_dManningParamB << endl;
   }
   if (m_bDarcyWeisbachFlowSpeedEqn)
   {
      m_ofsOut << " Using Darcy-Weisbach flow speed equation, with ";
      if (m_bFrictionFactorConstant)
      {
         m_ofsOut << "constant friction factor" << endl;
         m_ofsOut << " Friction factor                                        \t: " << m_dFFConstant << endl;
      }

      if (m_bFrictionFactorReynolds)
      {
         m_ofsOut << "Reynolds' number-controlled friction factor" << endl;
         m_ofsOut << " Parameter A                                            \t: " << m_dFFReynoldsParamA << endl;
         m_ofsOut << " Parameter B                                            \t: " << m_dFFReynoldsParamB << endl;
      }

      if (m_bFrictionFactorLawrence)
      {
         m_ofsOut << "friction factor approach from Lawrence (1997)" << endl;
         m_ofsOut << " In the partially-inundated flow regime:" << endl;
         m_ofsOut << "  D50 of within-cell roughness elements                 \t: " << m_dFFLawrenceD50 << " mm" << endl;
         m_ofsOut << "* Epsilon (0.5 * D50)                                   \t: " << m_dFFLawrenceEpsilon << " mm" << endl;
         m_ofsOut << "  % of surface covered with roughness elements          \t: " << m_dFFLawrencePr << " %" << endl;
         m_ofsOut << "  Ratio roughness element drag : ideal situation        \t: " << m_dFFLawrenceCd << endl;
      }

      if (m_bFrictionFactorCheng)
      {
         m_ofsOut << "friction factor approach from Cheng (2008)" << endl;
         m_ofsOut << " Effective roughness height                             \t: " << m_dChengRoughnessHeight << " mm" << endl;

      }
   }
   m_ofsOut << " Maximum flow speed                                     \t: " << m_dMaxFlowSpeed << " mm/sec" << endl;
   m_ofsOut << endl;

   // ---------------------------------------------------------- Flow detachment ---------------------------------------------------------
   m_ofsOut << "FLOW DETACHMENT" << endl;
   m_ofsOut << " Simulate flow erosion?                                 \t: " << (m_bFlowErosion ? "y" : "n") << endl;
   if (m_bFlowErosion)
   {
      m_ofsOut << setprecision(3);
      m_ofsOut << " Constant k for flow detachment                         \t: " << m_dK << " kg/m**3" << endl;
      m_ofsOut << setprecision(2);
      m_ofsOut << " Mean value of T for flow detachment                    \t: " << m_dT << " Pascal" << endl;
      m_ofsOut << " Coefficient of variation of T for flow detachment      \t: " << m_dCVT << endl;
      m_ofsOut << " Coefficient of variation of tau-b for detachment       \t: " << m_dCVTaub << endl;
   }
   m_ofsOut << endl;

   // --------------------------------------------------------- Transport Capacity -------------------------------------------------------
   m_ofsOut << "TRANSPORT CAPACITY" << endl;
   if (m_bFlowErosion)
   {
      m_ofsOut << " Alpha for transport capacity                           \t: " << m_dAlpha << endl;
      m_ofsOut << " Beta for transport capacity                            \t: " << m_dBeta << endl;
      m_ofsOut << " Gamma for transport capacity                           \t: " << m_dGamma << endl;
      m_ofsOut << " Delta for transport capacity                           \t: " << m_dDelta << endl;
   }
   m_ofsOut << endl;

   // ------------------------------------------------------------- Deposition -----------------------------------------------------------
   m_ofsOut << "DEPOSITION" << endl;
   if (m_bFlowErosion)
   {
      m_ofsOut << " Deposition equation                                    \t: ";
      if (m_bSettlingEqnCheng)
         m_ofsOut << "Cheng" << endl;
      if (m_bSettlingEqnStokesBudryckRittinger)
         m_ofsOut << "Stokes-Budryck-Rittinger" << endl;
      if (m_bSettlingEqnFergusonChurch)
         m_ofsOut << "Ferguson and Church" << endl;
      m_ofsOut << " Grain density                                          \t: " << m_dDepositionGrainDensity << " kg/m**3" << endl;
      m_ofsOut << resetiosflags(ios::floatfield) << std::scientific << setprecision(2);
      m_ofsOut << " Clay, minimum diameter                                 \t: " << m_dClaySizeMin << " mm" << endl;
      m_ofsOut << resetiosflags(ios::floatfield) << std::fixed << setprecision(3);
      m_ofsOut << " Clay-silt threshold diameter                           \t: " << m_dClaySiltBoundarySize << " mm" << endl;
      m_ofsOut << " Silt-sand threshold diameter                           \t: " << m_dSiltSandBoundarySize << " mm" << endl;
      m_ofsOut << " Sand, maximum diameter                                 \t: " << m_dSandSizeMax << " mm" << endl;
      m_ofsOut << "*Mean diameter of clay-sized sediment                   \t: " << setprecision(4) << m_dClayDiameter << " mm" << endl;
      m_ofsOut << "*Mean diameter of silt-sized sediment                   \t: " << setprecision(2) << m_dSiltDiameter << " mm" << endl;
      m_ofsOut << "*Mean diameter of sand-sized sediment                   \t: " << m_dSandDiameter << " mm" << endl;
      m_ofsOut << resetiosflags(ios::floatfield) << std::fixed << setprecision(2);
      m_ofsOut << "*Settling speed of clay-sized sediment                  \t: " << setprecision(4) << m_dClaySettlingSpeed << " mm/sec" << endl;
      m_ofsOut << "*Settling speed of silt-sized sediment                  \t: " << setprecision(2) << m_dSiltSettlingSpeed << " mm/sec" << endl;
      m_ofsOut << "*Settling speed of sand-sized sediment                  \t: " << m_dSandSettlingSpeed << " mm/sec" << endl;
   }
   m_ofsOut << endl;

   // ----------------------------------------------------------- Slumping ---------------------------------------------------------------
   m_ofsOut << "SLUMPING" << endl;
   m_ofsOut << " Simulate slumping?                                     \t: " << (m_bSlumping ? "y" : "n") << endl;
   if (m_bSlumping)
   {
      m_ofsOut << " Radius of soil shear stress 'patch'                    \t: " << m_dSSSPatchSize << " mm" << endl;
      m_ofsOut << " Threshold shear stress for slumping                    \t: " << m_dCritSSSForSlump << " kg/m s**2 (Pa)" << endl;
      m_ofsOut << " Angle of rest for slumped soil                         \t: " << m_dSlumpAngleOfRest << " %" << endl;
      m_ofsOut << " Critical angle for toppled soil                        \t: " << m_dToppleCriticalAngle << " %" << endl;
      m_ofsOut << " Angle of rest for toppled soil                         \t: " << m_dToppleAngleOfRest << " %" << endl;
   }
   m_ofsOut << endl;

   // -------------------------------------------------------- Headcut Retreat -----------------------------------------------------------
   m_ofsOut << "HEADCUT RETREAT" << endl;
   m_ofsOut << " Simulate headcut retreat?                              \t: " << (m_bHeadcutRetreat ? "y" : "n") << endl;
   if (m_bHeadcutRetreat)
   {
      m_ofsOut << " Headcut retreat constant                               \t: " << m_dHeadcutRetreatConst << endl;
   }
   m_ofsOut << endl;

   // --------------------------------------------------- Various Physical Constants -----------------------------------------------------
   m_ofsOut << "OTHER CONSTANTS" << endl;
   m_ofsOut << " Density of water                                       \t: " << m_dRho << " kg/m**3" << endl;
   m_ofsOut << setprecision(6);
   m_ofsOut << " Kinematic viscosity of water                           \t: " << m_dNu << " m**2/sec" << endl;
   m_ofsOut << setprecision(2);
   m_ofsOut << " Gravitational acceleration                             \t: " << m_dG << " m/sec**2" << endl;
   m_ofsOut << endl;

   m_ofsOut << "*=calculated value" << endl;
   m_ofsOut << endl;

   // Write per-iteration headers to .out file
   m_ofsOut << PERITERHEAD << endl;
   m_ofsOut << "Units: volumes in mm**3, times in sec." << endl;
   m_ofsOut << "Markers: dRain = change in rain intensity, ";
   m_ofsOut << "GISn = GIS files saved as <filename>n." << endl;
   m_ofsOut << endl;

   m_ofsOut << PERITERHEAD1 << endl;
   m_ofsOut << PERITERHEAD2 << endl;
}

//=========================================================================================================================================
//! Writes files at the end of the simulation
//=========================================================================================================================================
int CSimulation::nWriteFilesAtEnd(void)
{
   // Write files which are always written
   if (! bWriteGISFileFloat(GIS_CUMUL_ALL_SIZE_FLOW_DETACH, &GIS_CUMUL_ALL_SIZE_FLOW_DETACH_TITLE))   // also increments filename count
      return (RTN_ERR_GISFILEWRITE);

   if (! bWriteGISFileFloat(GIS_CUMUL_RAIN, &GIS_CUMUL_RAIN_TITLE))
      return (RTN_ERR_GISFILEWRITE);

   if (! bWriteGISFileFloat(GIS_CUMUL_SPLASH, &GIS_CUMUL_SPLASH_TITLE))
      return (RTN_ERR_GISFILEWRITE);

   if (! bWriteGISFileFloat(GIS_SURFACE_WATER_DEPTH, &GIS_SURFACE_WATER_DEPTH_TITLE))
      return (RTN_ERR_GISFILEWRITE);

   if (! bWriteGISFileFloat(GIS_SURFACE_WATER_DW_SPEED, &GIS_SURFACE_WATER_DW_SPEED_TITLE))
      return (RTN_ERR_GISFILEWRITE);

   if (! bWriteGISFileFloat(GIS_SURFACE_WATER_SPEED, &GIS_SURFACE_WATER_SPEED_TITLE))
      return (RTN_ERR_GISFILEWRITE);

   // Maybe write optional files
   if (m_bElevSave)
      if (! bWriteGISFileFloat(GIS_ELEVATION, &GIS_ELEVATION_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bDetrendElevSave)
      if (! bWriteGISFileFloat(GIS_DETREND_ELEVATION, &GIS_DETREND_ELEVATION_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bCumulRunOnSave)
      if (! bWriteGISFileFloat(GIS_CUMUL_RUNON, &GIS_CUMUL_RUNON_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bSplashSave)
      if (! bWriteGISFileFloat(GIS_SPLASH, &GIS_SPLASH_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bFlowDetachSave)
      if (! bWriteGISFileFloat(GIS_ALL_SIZE_FLOW_DETACH, &GIS_CUMUL_ALL_SIZE_FLOW_DETACH_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bInundationSave)
      if (! bWriteGISFileInt(GIS_INUNDATION_REGIME, &GIS_INUNDATION_REGIME_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bFlowDirSave)
      if (! bWriteGISFileInt(GIS_SURFACE_WATER_DIRECTION, &GIS_SURFACE_WATER_DIRECTION_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bInfiltSave)
      if (! bWriteGISFileFloat(GIS_INFILT, &GIS_INFILT_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bCumulInfiltSave)
      if (! bWriteGISFileFloat(GIS_CUMUL_INFILT, &GIS_CUMUL_INFILT_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bSoilWaterSave)
      if (! bWriteGISFileFloat(GIS_SOIL_WATER, &GIS_SOIL_WATER_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bInfiltDepositSave)
      if (! bWriteGISFileFloat(GIS_INFILT_DEPOSIT, &GIS_INFILT_DEPOSIT_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bCumulInfiltDepositSave)
      if (! bWriteGISFileFloat(GIS_CUMUL_INFILT_DEPOSIT, &GIS_CUMUL_INFILT_DEPOSIT_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bTopSurfaceSave)
      if (! bWriteGISFileFloat(GIS_TOP_SURFACE_DETREND, &GIS_TOP_SURFACE_DETREND_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bLostSave)
      if (! bWriteGISFileFloat(GIS_AVG_SURFACE_WATER_FROM_EDGES, &GIS_AVG_SURFACE_WATER_FROM_EDGES_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bStreamPowerSave)
      if (! bWriteGISFileFloat(GIS_STREAMPOWER, &GIS_STREAMPOWER_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bShearStressSave)
      if (! bWriteGISFileFloat(GIS_SHEAR_STRESS, &GIS_SHEAR_STRESS_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bFrictionFactorSave)
      if (! bWriteGISFileFloat(GIS_FRICTION_FACTOR, &GIS_FRICTION_FACTOR_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bCumulAvgShearStressSave)
      if (! bWriteGISFileFloat(GIS_AVG_SHEAR_STRESS, &GIS_AVG_SHEAR_STRESS_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bReynoldsSave)
      if (! bWriteGISFileFloat(GIS_REYNOLDS_NUMBER, &GIS_REYNOLDS_NUMBER_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bFroudeSave)
      if (! bWriteGISFileFloat(GIS_FROUDE_NUMBER, &GIS_FROUDE_NUMBER_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bTCSave)
      if (! bWriteGISFileFloat(GIS_TRANSPORT_CAPACITY, &GIS_TRANSPORT_CAPACITY_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bCumulAvgDepthSave)
      if (! bWriteGISFileFloat(GIS_AVG_SURFACE_WATER_DEPTH, &GIS_AVG_SURFACE_WATER_DEPTH_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bCumulAvgDWSpdSave)
      if (! bWriteGISFileFloat(GIS_AVG_SURFACE_WATER_DW_SPEED, &GIS_AVG_SURFACE_WATER_DW_SPEED_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bCumulAvgSpdSave)
      if (! bWriteGISFileFloat(GIS_AVG_SURFACE_WATER_SPEED, &GIS_AVG_SURFACE_WATER_SPEED_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bSedConcSave)
      if (! bWriteGISFileFloat(GIS_SEDIMENT_CONCENTRATION, &GIS_SEDIMENT_CONCENTRATION_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bSedLoadSave)
      if (! bWriteGISFileFloat(GIS_SEDIMENT_LOAD, &GIS_SEDIMENT_LOAD_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bAvgSedLoadSave)
      if (! bWriteGISFileFloat(GIS_AVG_SEDIMENT_LOAD, &GIS_AVG_SEDIMENT_LOAD_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bSlumpSave)
   {
      if (! bWriteGISFileFloat(GIS_CUMUL_SLUMP_DETACH, &GIS_CUMUL_SLUMP_DETACH_TITLE))
         return (RTN_ERR_GISFILEWRITE);

      if (! bWriteGISFileFloat(GIS_CUMUL_SLUMP_DEPOSIT, &GIS_CUMUL_SLUMP_DEPOSIT_TITLE))
         return (RTN_ERR_GISFILEWRITE);
   }

   if (m_bToppleSave)
   {
      if (! bWriteGISFileFloat(GIS_CUMUL_TOPPLE_DETACH, &GIS_CUMUL_TOPPLE_DETACH_TITLE))
         return (RTN_ERR_GISFILEWRITE);

      if (! bWriteGISFileFloat(GIS_CUMUL_TOPPLE_DEPOSIT, &GIS_CUMUL_TOPPLE_DEPOSIT_TITLE))
         return (RTN_ERR_GISFILEWRITE);
   }

   if (m_bCumulFlowDepositSave)
      if (! bWriteGISFileFloat(GIS_CUMUL_ALL_SIZE_FLOW_DEPOSIT, &GIS_CUMUL_ALL_SIZE_FLOW_DEPOSIT_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bCumulLoweringSave)
      if (! bWriteGISFileFloat(GIS_CUMUL_ALL_PROC_SURF_LOWER, &GIS_CUMUL_ALL_PROC_SURF_LOWER_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   if (m_bHeadcutRetreat)
      if (! bWriteGISFileInt(GIS_CUMUL_BINARY_HEADCUT_RETREAT, &GIS_CUMUL_BINARY_HEADCUT_RETREAT_TITLE))
         return (RTN_ERR_GISFILEWRITE);

   return RTN_OK;
}

//=========================================================================================================================================
//! Writes grand totals
//=========================================================================================================================================
void CSimulation::WriteEndOfSimTotals(void)
{
   // Print out run totals etc.
   m_ofsLog << "End of run: files written" << endl << endl;

   m_ofsOut << PERITERHEAD1 << endl;
   m_ofsOut << PERITERHEAD2 << endl;

   m_ofsOut << setprecision(2);
   m_ofsOut << std::fixed;
   m_ofsOut << endl << endl;

   // Write out rainfall grand totals
   m_ofsOut << ENDRAINHEAD << endl;

   // Start calculating final totals etc., first calculate total area in mm**2 (excluding NODATA cells)
   double dTotArea = static_cast<double>(m_ulNActiveCells) * m_dCellSquare;
   if (m_bTimeVaryingRain)
   {
      m_ofsOut << "Mean rainfall intensity for time-varying rain = " << 3600 * m_ldGTotRain / (m_dSimulatedRainDuration * dTotArea) << " mm/hr" << endl;
   }
   else
   {
      m_ofsOut << "Mean rainfall intensity = " << 3600 * m_ldGTotRain / (m_dSimulatedRainDuration * dTotArea) << " mm/hr, should be " << m_dSpecifiedRainIntensity * m_dRainVarMFileMean << " mm/hr" << endl;
   }
   m_ofsOut << "Mean raindrop volume    = " << m_ldGTotRain / m_ldGTotDrops << " mm**3, should be " << m_dMeanCellWaterVol * m_dRainVarMFileMean << " mm**3" << endl;
   if (! m_strRainVarMFile.empty())
      m_ofsOut << "Note: a rainfall variation correction of " << m_dRainVarMFileMean << "x was applied, from " << m_strRainVarMFile << endl;
   m_ofsOut << endl;

   // Run-on?
   if (m_bRunOn)
   {
      m_ofsOut << "Mean run-on rainfall intensity = " << 3600 * m_ldGTotRunOn  / (m_dSimulatedRainDuration * m_dRunOnLen * m_nXGridMax * m_dCellSide) << " mm/hr, should be " << m_dSpecifiedRainIntensity * m_dRunOnRainVarM << " mm/hr" << endl;
      m_ofsOut << "Mean run-on raindrop volume    = " << m_ldGTotRunOn / m_ldGTotRunOnDrops << " mm**3, should be " << m_dMeanCellWaterVol * m_dRunOnRainVarM << " mm**3" << endl;
      if (! m_strRainVarMFile.empty())
         m_ofsOut << "Note: a rainfall variation correction of " << m_dRunOnRainVarM << "x was applied, from " << m_strRainVarMFile << endl;
      m_ofsOut << endl;
   }

   if (! bFpEQ(m_dSimulationDuration, m_dSimulatedRainDuration, 0.1))
   {
      m_ofsOut << "Note: these are calculated for the period of rainfall (" << m_dSimulatedRainDuration / 60 << " mins) only." << endl;
   }
   m_ofsOut << endl << endl;

   // Write out infilt grand totals
   m_ofsOut << ENDINFILTHEAD << endl;
   m_ofsOut << "Mean infiltration during period of rainfall (" << m_dSimulatedRainDuration / 60 << " mins) = " << 3600 * m_ldGTotInfilt / (m_dSimulatedRainDuration * dTotArea) << " mm/hr" << endl;
   m_ofsOut << "Mean infiltration during whole simulation (" << m_dSimulationDuration / 60 << " mins) = " << 3600 * m_ldGTotInfilt / (m_dSimulationDuration * dTotArea) << " mm/hr" << endl;
   m_ofsOut << endl << endl;

   // Next, write out hydrology grand totals
   m_ofsOut << ENDHYDHEAD << endl;
   m_ofsOut << "Totals lost from edge(s)" << endl;
   m_ofsOut << "mm depth                    " << setw(15) << m_ldGTotWaterOffEdge / dTotArea << endl;
   m_ofsOut << "mm**3                       " << setw(15) << m_ldGTotWaterOffEdge << endl;
   m_ofsOut << "m**3                        " << setw(15) << m_ldGTotWaterOffEdge * 1e-9 << endl;
   m_ofsOut << "litre                       " << setw(15) << m_ldGTotWaterOffEdge * 1e-6 << endl;
   m_ofsOut << "litre/s                     " << setw(15) << m_ldGTotWaterOffEdge * 1e-6 / m_dSimulatedRainDuration << endl;
   if (! bFpEQ(m_dSimulationDuration, m_dSimulatedRainDuration, 0.1))
      m_ofsOut << endl << "Note: this is the rate during the period of rainfall (" << m_dSimulatedRainDuration / 60 << " mins)" << endl;
   m_ofsOut << endl;

   double
      dTmpTot = 0,
      dTmp,
      dWaterIn = static_cast<double>(m_ldGTotRain + m_ldGTotRunOn);
   m_ofsOut << "Water Balance (% of total rainfall" << (m_bRunOn ? " + run-on" : "") << ")" << endl;

   dTmp = (dWaterIn > 0 ? 100 * static_cast<double>(m_ldGTotInfilt - m_ldGTotExfilt) / dWaterIn : 0);
   dTmpTot += dTmp;
   m_ofsOut << "Infiltration - exfiltration " << setw(10) << dTmp << endl;

   dTmp = (dWaterIn > 0 ? 100 * m_dEndOfIterTotSurfaceWater * m_dCellSquare / dWaterIn : 0);
   dTmpTot += dTmp;
   m_ofsOut << "Storage at end              " << setw(10) << dTmp << endl;

   dTmp = (dWaterIn > 0 ? 100 * static_cast<double>(m_ldGTotWaterOffEdge) / dWaterIn : 0);
   dTmpTot += dTmp;
   m_ofsOut << "Off edge                    " << setw(10) << dTmp << endl;

   m_ofsOut << "TOTAL                       " << setw(10) << dTmpTot << endl;
   m_ofsOut << endl << endl;

   // Now write out sediment delivery grand totals
   m_ofsOut << ENDSEDDELHEAD << endl;
   m_ofsOut << "Totals lost from edge(s)    " << endl;
   m_ofsOut << "mm depth                    " << setw(15) << (m_ldGTotFlowSedLoadOffEdge + m_ldGTotSplashOffEdge) / dTotArea << endl;
   m_ofsOut << "mm**3                       " << setw(15) << (m_ldGTotFlowSedLoadOffEdge + m_ldGTotSplashOffEdge) << endl;
   m_ofsOut << "m**3                        " << setw(15) << (m_ldGTotFlowSedLoadOffEdge + m_ldGTotSplashOffEdge) * 1e-9 << endl;
   m_ofsOut << "g                           " << setw(15) << (m_ldGTotFlowSedLoadOffEdge + m_ldGTotSplashOffEdge) * m_dBulkDensityForOutputCalcs * 1e-6 << endl;
   m_ofsOut << "kg                          " << setw(15) << (m_ldGTotFlowSedLoadOffEdge + m_ldGTotSplashOffEdge) * m_dBulkDensityForOutputCalcs * 1e-9 << endl;
   m_ofsOut << "kg/m**2                     " << setw(15) << (m_ldGTotFlowSedLoadOffEdge + m_ldGTotSplashOffEdge) * m_dBulkDensityForOutputCalcs * 1e-3 / dTotArea << endl;
   m_ofsOut << "m**3/ha                     " << setw(15) << 10 * (m_ldGTotFlowSedLoadOffEdge + m_ldGTotSplashOffEdge) / dTotArea << endl;
   m_dBulkDensityForOutputCalcs /= 1000;                   // convert back from kg/m**3 to t/m**3
   m_ofsOut << "t/ha                        " << setw(15) << (10 * (m_ldGTotFlowSedLoadOffEdge + m_ldGTotSplashOffEdge) * m_dBulkDensityForOutputCalcs) / dTotArea << endl;
   m_ofsOut << endl;

   m_ofsOut << "Sediment Balance (% of total detached by flow" << (m_bSplash ? " + splash" : "") << (m_bSlumping ? " + slumping + toppling" : "") << ")" << endl;

   long double ldGTotEroded = m_ldGTotFlowDetach + m_ldGTotSplashDetach + m_ldGTotSlumpDetach + m_ldGTotToppleDetach + m_ldGTotHeadcutRetreatDetach;
   long double ldGTotNetDeposited = m_ldGTotFlowDeposit + m_ldGTotSplashDeposit + m_ldGTotSlumpDeposit + m_ldGTotToppleDeposit + m_ldGTotInfiltDeposit + m_ldGTotHeadcutRetreatDeposit;
   dTmpTot = 0;

   dTmp = (ldGTotEroded > 0 ? 100 * static_cast<double>(ldGTotNetDeposited / ldGTotEroded) : 0);
   dTmpTot += dTmp;
   m_ofsOut << "Deposition                  " << setw(10) << dTmp << endl;

   dTmp = (ldGTotEroded > 0 ? 100 * ((m_dEndOfIterClaySedLoad + m_dEndOfIterSiltSedLoad + m_dEndOfIterSandSedLoad) * m_dCellSquare) / static_cast<double>(ldGTotEroded) : 0);
   dTmpTot += dTmp;
   m_ofsOut << "Storage at end              " << setw(10) << dTmp << endl;

   dTmp = (ldGTotEroded > 0 ? 100 * static_cast<double>((m_ldGTotFlowSedLoadOffEdge + m_ldGTotSplashOffEdge) / ldGTotEroded) : 0);
   dTmpTot += dTmp;
   m_ofsOut << "Off edge                    " << setw(10) << dTmp << endl;

   m_ofsOut << "TOTAL                       " << setw(10) << dTmpTot << endl;

   m_ofsOut << endl;
   m_ofsOut << "Sediment delivery ratio     " << setw(10) << (ldGTotEroded > 0 ? (m_ldGTotFlowSedLoadOffEdge + m_ldGTotSplashOffEdge) / ldGTotEroded : 0) << endl;
   m_ofsOut << endl << endl;

   // Show grand totals for detachment and deposition
   m_ofsOut << DETDEPHEAD << endl;

   // Flow detachment
   m_ofsOut << "Detachment due to flow" << endl;
   m_ofsOut << "Average = " << m_ldGTotFlowDetach / dTotArea << " mm" << endl;
   m_ofsOut << "        = " << 10 * m_ldGTotFlowDetach / dTotArea << " m**3/ha" << endl;         // convert mm3/mm2 to m3/ha
   m_ofsOut << "        = " << 10 * m_ldGTotFlowDetach * m_dBulkDensityForOutputCalcs / dTotArea << " t/ha" << endl;
   m_ofsOut << endl;

   // Flow deposition
   m_ofsOut << "Deposition (including subsequently re-eroded sediment)" << endl;
   m_ofsOut << "Average = " << m_ldGTotFlowDeposit / dTotArea << " mm" << endl;
   m_ofsOut << "        = " << 10 * m_ldGTotFlowDeposit / dTotArea  << " m**3/ha" << endl;       // convert mm3/mm2 to m3/ha
   m_ofsOut << "        = " << 10 * m_ldGTotFlowDeposit * m_dBulkDensityForOutputCalcs / dTotArea << " t/ha" << endl;
   m_ofsOut << endl;

   // Splash detachment
   m_ofsOut << "Elevation decrease due to splash" << endl;
   m_ofsOut << "Average = " << m_ldGTotSplashDetach / dTotArea << " mm" << endl;
   m_ofsOut << "        = " << 10 * m_ldGTotSplashDetach / dTotArea << " m**3/ha" << endl;         // convert mm3/mm2 to m3/ha

   m_ofsOut << "        = " << 10 * m_ldGTotSplashDetach * m_dBulkDensityForOutputCalcs / dTotArea << " t/ha" << endl;
   m_ofsOut << endl;

   // Splash deposition
   m_ofsOut << "Elevation increase (including subsequently re-eroded sediment) due to splash" << endl;
   m_ofsOut << "Average = " << m_ldGTotSplashDeposit / dTotArea << " mm" << endl;
   m_ofsOut << "        = " << 10 * m_ldGTotSplashDeposit / dTotArea << " m**3/ha" << endl;      // convert mm3/mm2 to m3/ha
   m_ofsOut << "        = " << 10 * m_ldGTotSplashDeposit * m_dBulkDensityForOutputCalcs / dTotArea << " t/ha" << endl;
   m_ofsOut << endl;

   // Splash to sed load
   m_ofsOut << "Contribution to sediment load from splash" << endl;
   m_ofsOut << "Average = " << m_ldGTotSplashToSedLoad / dTotArea << " mm" << endl;
   m_ofsOut << "        = " << 10 * m_ldGTotSplashToSedLoad / dTotArea << " m**3/ha" << endl;      // convert mm3/mm2 to m3/ha
   m_ofsOut << "        = " << 10 * m_ldGTotSplashToSedLoad * m_dBulkDensityForOutputCalcs / dTotArea << " t/ha" << endl;
   m_ofsOut << endl;

   // Slumping detachment
   m_ofsOut << "Elevation decrease due to slumping" << endl;
   m_ofsOut << "Average = " << m_ldGTotSlumpDetach / dTotArea << " mm" << endl;
   m_ofsOut << "        = " << 10 * m_ldGTotSlumpDetach / dTotArea << " m**3/ha" << endl;        // convert mm3/mm2 to m3/ha
   m_ofsOut << "        = " << 10 * m_ldGTotSlumpDetach * m_dBulkDensityForOutputCalcs / dTotArea << " t/ha" << endl;
   m_ofsOut << endl;

   // Slumping deposition
   m_ofsOut << "Elevation increase due to slumping" << endl;
   m_ofsOut << "Average = " << m_ldGTotSlumpDeposit / dTotArea << " mm" << endl;
   m_ofsOut << "        = " << 10 * m_ldGTotSlumpDeposit / dTotArea << " m**3/ha" << endl;        // convert mm3/mm2 to m3/ha
   m_ofsOut << "        = " << 10 * m_ldGTotSlumpDeposit * m_dBulkDensityForOutputCalcs / dTotArea << " t/ha" << endl;
   m_ofsOut << endl;

   // Slumping to sed load
   m_ofsOut << "Contribution to sediment load from slumping" << endl;
   m_ofsOut << "Average = " << m_ldGTotSlumpToSedLoad / dTotArea << " mm" << endl;
   m_ofsOut << "        = " << 10 * m_ldGTotSlumpToSedLoad / dTotArea << " m**3/ha" << endl;      // convert mm3/mm2 to m3/ha
   m_ofsOut << "        = " << 10 * m_ldGTotSlumpToSedLoad * m_dBulkDensityForOutputCalcs / dTotArea << " t/ha" << endl;
   m_ofsOut << endl;

   // Toppling detachment
   m_ofsOut << "Elevation decrease due to toppling" << endl;
   m_ofsOut << "Average = " << m_ldGTotToppleDetach / dTotArea << " mm" << endl;
   m_ofsOut << "        = " << 10 * m_ldGTotToppleDetach / dTotArea << " m**3/ha" << endl;        // convert mm3/mm2 to m3/ha
   m_ofsOut << "        = " << 10 * m_ldGTotToppleDetach * m_dBulkDensityForOutputCalcs / dTotArea << " t/ha" << endl;
   m_ofsOut << endl;

   // Toppling deposition
   m_ofsOut << "Elevation increase (including subsequently re-eroded sediment) due to toppling" << endl;
   m_ofsOut << "Average = " << m_ldGTotToppleDeposit / dTotArea << " mm" << endl;
   m_ofsOut << "        = " << 10 * m_ldGTotToppleDeposit / dTotArea << " m**3/ha" << endl;       // convert mm3/mm2 to m3/ha
   m_ofsOut << "        = " << 10 * m_ldGTotToppleDeposit * m_dBulkDensityForOutputCalcs / dTotArea << " t/ha" << endl;
   m_ofsOut << endl;

   // Toppling to sed load
   m_ofsOut << "Contribution to sediment load from toppling" << endl;
   m_ofsOut << "Average = " << m_ldGTotToppleToSedLoad / dTotArea << " mm" << endl;
   m_ofsOut << "        = " << 10 * m_ldGTotToppleToSedLoad / dTotArea << " m**3/ha" << endl;      // convert mm3/mm2 to m3/ha
   m_ofsOut << "        = " << 10 * m_ldGTotToppleToSedLoad * m_dBulkDensityForOutputCalcs / dTotArea << " t/ha" << endl;
   m_ofsOut << endl;

   // Infiltration deposition
   m_ofsOut << "Elevation increase (including subsequently re-eroded sediment) due to infiltration deposition" << endl;
   m_ofsOut << "Average = " << m_ldGTotInfiltDeposit / dTotArea << " mm" << endl;
   m_ofsOut << "        = " << 10 * m_ldGTotInfiltDeposit / dTotArea << " m**3/ha" << endl;       // convert mm3/mm2 to m3/ha
   m_ofsOut << "        = " << 10 * m_ldGTotInfiltDeposit * m_dBulkDensityForOutputCalcs / dTotArea << " t/ha" << endl;
   m_ofsOut << endl;

   // Headcut retreat detachment
   m_ofsOut << "Elevation decrease due to headcut retreat" << endl;
   m_ofsOut << "Average = " << m_ldGTotHeadcutRetreatDetach / dTotArea << " mm" << endl;
   m_ofsOut << "        = " << 10 * m_ldGTotHeadcutRetreatDetach / dTotArea << " m**3/ha" << endl;        // convert mm3/mm2 to m3/ha
   m_ofsOut << "        = " << 10 * m_ldGTotHeadcutRetreatDetach * m_dBulkDensityForOutputCalcs / dTotArea << " t/ha" << endl;
   m_ofsOut << endl;

   // Headcut retreat deposition
   m_ofsOut << "Elevation increase (including subsequently re-eroded sediment) due to headcut retreat" << endl;
   m_ofsOut << "Average = " << m_ldGTotHeadcutRetreatDeposit / dTotArea << " mm" << endl;
   m_ofsOut << "        = " << 10 * m_ldGTotHeadcutRetreatDeposit / dTotArea << " m**3/ha" << endl;       // convert mm3/mm2 to m3/ha
   m_ofsOut << "        = " << 10 * m_ldGTotHeadcutRetreatDeposit * m_dBulkDensityForOutputCalcs / dTotArea << " t/ha" << endl;
   m_ofsOut << endl;

   // Headcut retreat to sed load
   m_ofsOut << "Contribution to sediment load from headcut retreat" << endl;
   m_ofsOut << "Average = " << m_ldGTotHeadcutRetreatToSedLoad / dTotArea << " mm" << endl;
   m_ofsOut << "        = " << 10 * m_ldGTotHeadcutRetreatToSedLoad / dTotArea << " m**3/ha" << endl;      // convert mm3/mm2 to m3/ha
   m_ofsOut << "        = " << 10 * m_ldGTotHeadcutRetreatToSedLoad * m_dBulkDensityForOutputCalcs / dTotArea << " t/ha" << endl;
   m_ofsOut << endl << endl;

   // Show relative contributions of each detachment process to total sediment lost
   m_ofsOut << RELCONTRIBLOSTHEAD << endl;

   m_ofsOut << "Detachment due to flow (%)                   " << setw(10) << ((m_ldGTotFlowSedLoadOffEdge + m_ldGTotSplashOffEdge) > 0 ? 100 * m_ldGTotFlowDetach / (m_ldGTotFlowSedLoadOffEdge + m_ldGTotSplashOffEdge) : 0) << endl;
   m_ofsOut << "Elevation decrease due to splash (%)         " << setw(10) << ((m_ldGTotFlowSedLoadOffEdge + m_ldGTotSplashOffEdge) > 0 ? 100 * m_ldGTotSplashDetach / (m_ldGTotFlowSedLoadOffEdge + m_ldGTotSplashOffEdge) : 0)  << endl;
   m_ofsOut << "Elevation decrease due to slumping (%)       " << setw(10) << ((m_ldGTotFlowSedLoadOffEdge + m_ldGTotSplashOffEdge) > 0 ? 100 * m_ldGTotSlumpDetach / (m_ldGTotFlowSedLoadOffEdge + m_ldGTotSplashOffEdge) : 0) << endl;
   m_ofsOut << "Elevation decrease due to toppling (%)       " << setw(10) << ((m_ldGTotFlowSedLoadOffEdge + m_ldGTotSplashOffEdge) > 0 ? 100 * m_ldGTotToppleDetach / (m_ldGTotFlowSedLoadOffEdge + m_ldGTotSplashOffEdge) : 0) << endl;
   m_ofsOut << "Elevation decrease due to headcut retreat (%)" << setw(10) << ((m_ldGTotFlowSedLoadOffEdge + m_ldGTotSplashOffEdge) > 0 ? 100 * m_ldGTotHeadcutRetreatDetach / (m_ldGTotFlowSedLoadOffEdge + m_ldGTotSplashOffEdge) : 0) << endl;
   if ((m_ldGTotFlowDetach + m_ldGTotSplashDetach +  m_ldGTotSlumpDetach + m_ldGTotToppleDetach + m_ldGTotHeadcutRetreatDetach) > 100)
      m_ofsOut << endl << "Note: double counting here, due to subsequent deposition and re-detachment." << endl;
   m_ofsOut << endl << endl;

   // Show relative contributions of each detachment process to total soil detached
   m_ofsOut << RELCONTRIBDETACHHEAD << endl;

   m_ofsOut << "Detachment due to flow (%)            " << setw(10) << (ldGTotEroded > 0 ? 100 * m_ldGTotFlowDetach / ldGTotEroded : 0) << endl;
   m_ofsOut << "Elevation decrease due to splash (%)  " << setw(10) << (ldGTotEroded > 0 ? 100 * m_ldGTotSplashDetach / ldGTotEroded : 0) << endl;
   m_ofsOut << "Elevation decrease due to slumping (%)" << setw(10) << (ldGTotEroded > 0 ? 100 * m_ldGTotSlumpDetach / ldGTotEroded : 0) << endl;
   m_ofsOut << "Elevation decrease due to toppling (%)" << setw(10) << (ldGTotEroded > 0 ? 100 * m_ldGTotToppleDeposit / ldGTotEroded : 0) << endl;
   m_ofsOut << endl << endl;
}

//=========================================================================================================================================
//! This member function sets up the time series files
//=========================================================================================================================================
bool CSimulation::bSetUpTSFiles(void)
{
   string strTSFile;

   // First the error TS file
   strTSFile = m_strOutputPath;
   strTSFile.append(ERROR_TIME_SERIES_NAME);
   strTSFile.append(CSV_EXT);

   // Open error time-series CSV file
   m_ofsErrorTS.open(strTSFile.c_str(), ios::out | ios::trunc);
   if (! m_ofsErrorTS)
   {
      // Error, cannot open error time-series file
      cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
      return (false);
   }

   // Write header line
   m_ofsErrorTS << "'Iteration'" << ",\t" << "'Elapsed'" << ",\t" << "'Timestep (sec)'" << ",\t" << "Water error" << ",\t" << "Splash error" << ",\t" << "Slump error" << ",\t" << "Topple error" << ",\t" << "Headcut error" << ",\t" << "Flow error" << endl;

   if (m_bTimeStepTS)
   {
      // Next do timestep TS
      strTSFile = m_strOutputPath;
      strTSFile.append(TIMESTEP_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open timestep time-series CSV file
      m_ofsTimestepTS.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! m_ofsTimestepTS)
      {
         // Error, cannot open timestep time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      m_ofsTimestepTS << "'Iteration'" << ",\t" << "'Elapsed'" << ",\t" << "'Timestep (sec)'" << ",\t" << "Poss. max flow speed (mm/sec)" << endl;
   }

   if (m_bAreaWetTS)
   {
      // Next, do area wet TS
      strTSFile = m_strOutputPath;
      strTSFile.append(AREA_WET_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open area wet time-series CSV file
      m_ofsAreaWetTS.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! m_ofsAreaWetTS)
      {
         // Error, cannot open area wet time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      m_ofsAreaWetTS << "'Elapsed'" << ",\t" << "'Percent wet cells'" << endl;
   }

   if (m_bRainTS)
   {
      // Now rainfall TS
      strTSFile = m_strOutputPath;
      strTSFile.append(RAIN_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open rain time-series CSV file
      m_ofsRainTS.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! m_ofsRainTS)
      {
         // Error, cannot open rain time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      m_ofsRainTS << "'Elapsed'" << ",\t" << "'Rain depth (mm/sec)'" << endl;
   }

   if (m_bRunOnTS)
   {
      // Now run-on
      strTSFile = m_strOutputPath;
      strTSFile.append(RUNON_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open run-on time-series CSV file
      m_ofsRunOnTS.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! m_ofsRunOnTS)
      {
         // Error, cannot open run-on time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      m_ofsRunOnTS << "'Elapsed'" << ",\t" << "'Runon (l/sec)'" << endl;
   }

   if (m_bSurfaceWaterTS)
   {
      // Now surface water
      strTSFile = m_strOutputPath;
      strTSFile.append(SURFACE_WATER_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open stored water time-series CSV file
      m_ofsSurfaceWaterTS.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! m_ofsSurfaceWaterTS)
      {
         // Error, cannot open stored water time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      m_ofsSurfaceWaterTS << "'Elapsed'" << ",\t" << "'Surface water (l/sec)'" << endl;
   }

   if (m_bSurfaceWaterLostTS)
   {
      // Then surface water lost
      strTSFile = m_strOutputPath;
      strTSFile.append(WATER_LOST_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open water lost time-series CSV file
      m_ofsSurfaceWaterLostTS.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! m_ofsSurfaceWaterLostTS)
      {
         // Error, cannot open water lost time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      m_ofsSurfaceWaterLostTS << "'Elapsed'" << ",\t" << "'Discharge (l/sec)'" << endl;
   }

   if (m_bFlowDetachTS)
   {
      // Flow detachment
      strTSFile = m_strOutputPath;
      strTSFile.append(FLOW_DETACH_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open flow detachment time-series CSV file
      m_ofsFlowDetachTS.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! m_ofsFlowDetachTS)
      {
         // Error, cannot open flow detachment time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      m_ofsFlowDetachTS << "'Elapsed'" << ",\t" << "'Clay flow detachment (g/sec)'" << ",\t" << "'Silt flow detachment (g/sec)'" << ",\t" << "'Sand flow detachment (g/sec)'" << endl;
   }

   if (m_bDoSedLoadDepositTS)
   {
      // Flow deposition
      strTSFile = m_strOutputPath;
      strTSFile.append(SEDLOAD_DEPOSIT_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open flow deposition time-series CSV file
      m_ofsFlowDepositSS.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! m_ofsFlowDepositSS)
      {
         // Error, cannot open flow deposition time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      m_ofsFlowDepositSS << "'Elapsed'" << ",\t" << "'Clay flow deposition (g/sec)'" << ",\t" << "'Silt flow deposition (g/sec)'" << ",\t" << "'Sand flow deposition (g/sec)'" << endl;
   }

   if (m_bSedOffEdgeTS)
   {
      // Sediment loss
      strTSFile = m_strOutputPath;
      strTSFile.append(SEDLOAD_LOST_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open sediment loss time-series CSV file
      m_ofsSedLostTS.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! m_ofsSedLostTS)
      {
         // Error, cannot open sediment loss time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      m_ofsSedLostTS << "'Elapsed'" << ",\t" << "'Clay sediment lost (g/sec)'" << ",\t" << "'Silt sediment lost (g/sec)'" << ",\t" << "'Sand sediment lost (g/sec)'" << endl;
   }

   if (m_bSedLoadTS)
   {
      // Sediment load
      strTSFile = m_strOutputPath;
      strTSFile.append(SEDLOAD_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open sediment load time-series CSV file
      m_ofsSedLoadTS.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! m_ofsSedLoadTS)
      {
         // Error, cannot open sediment load time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      m_ofsSedLoadTS << "'Elapsed'" << ",\t" << "'Clay sediment load (g/sec)'" << ",\t" << "'Silt sediment load (g/sec)'" << ",\t" << "'Sand sediment load (g/sec)'" << endl;
   }

   if (m_bInfiltTS)
   {
      // Now infilt
      strTSFile = m_strOutputPath;
      strTSFile.append(INFILT_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open infilt time-series CSV file
      m_ofsInfiltTS.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! m_ofsInfiltTS)
      {
         // Error, cannot open infilt time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      m_ofsInfiltTS << "'Elapsed'" << ",\t" << "'Infiltration (l/sec)'" << endl;
   }

   if (m_bExfiltTS)
   {
      // Now exfilt
      strTSFile = m_strOutputPath;
      strTSFile.append(EXFILT_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open exfilt time-series CSV file
      m_ofsExfiltTS.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! m_ofsExfiltTS)
      {
         // Error, cannot open exfilt time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      m_ofsExfiltTS << "'Elapsed'" << ",\t" << "'Exfiltration (l/sec)'" << endl;
   }

   if (m_bInfiltDepositTS)
   {
      // Deposition resulting from infilt
      strTSFile = m_strOutputPath;
      strTSFile.append(INFILT_DEPOSIT_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open deposition due to infilt time-series CSV file
      m_ofsInfiltDepositTS.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! m_ofsInfiltDepositTS)
      {
         // Error, cannot open deposition due to infilt time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      m_ofsInfiltDepositTS << "'Elapsed'" << ",\t" << "'Clay deposition from infilt (g/sec)'" << ",\t" << "'Silt deposition from infilt (g/sec)'" << ",\t" << "'Sand deposition from infilt (g/sec)'" << endl;
   }

   if (m_bSplashRedistTS)
   {
      // Splash detachment (not linked to other totals)
      strTSFile = m_strOutputPath;
      strTSFile.append(SPLASH_REDIST_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open splash detachment time-series CSV file
      m_ofsSplashDetachTS.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! m_ofsSplashDetachTS)
      {
         // Error, cannot splash detachment time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      m_ofsSplashDetachTS << "'Elapsed'" << ",\t" << "'Clay splash detachment (g/sec)'" << ",\t" << "'Silt splash detachment (g/sec)'" << ",\t" << "'Sand splash detachment (g/sec)'" << endl;
   }

   if (m_bSplashKETS)
   {
      // Splash kinetic energy (in Joules)
      strTSFile = m_strOutputPath;
      strTSFile.append(SPLASH_KE_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open splash kinetic energy time-series CSV file
      m_ofsSplashKETS.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! m_ofsSplashKETS)
      {
         // Error, cannot splash kinetic energy time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      m_ofsSplashKETS << "'Elapsed'" << ",\t" << "'Splash KE (J/sec)'" << endl;
   }


   if (m_bSlumpDetachTS)
   {
      // Slump detachment (same as slump deposition)
      strTSFile = m_strOutputPath;
      strTSFile.append(SLUMP_DETACH_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open slump detachment time-series CSV file
      m_ofsSlumpDetachTS.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! m_ofsSlumpDetachTS)
      {
         // Error, cannot open slump detachment time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      m_ofsSlumpDetachTS << "'Elapsed'" << ",\t" << "'Clay slump detachment (g/sec)'" << ",\t" << "'Silt slump detachment (g/sec)'" << ",\t" << "'Sand slump detachment (g/sec)'" << endl;
   }

   if (m_bToppleDetachTS)
   {
      // Toppling detachment (same as toppling deposition)
      strTSFile = m_strOutputPath;
      strTSFile.append(TOPPLE_DETACH_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open toppling detachment time-series CSV file
      m_ofsToppleDetachTS.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! m_ofsToppleDetachTS)
      {
         // Error, cannot open toppling detachment time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      m_ofsToppleDetachTS << "'Elapsed'" << ",\t" << "'Clay toppling detachment (g/sec)'" << ",\t" << "'Silt toppling detachment (g/sec)'" << ",\t" << "'Sand toppling detachment (g/sec)'" << endl;
   }

   if (m_bSoilWaterTS)
   {
      // Soil water (per layer, mm total)
      strTSFile = m_strOutputPath;
      strTSFile.append(SOIL_WATER_TIME_SERIES_NAME);
      strTSFile.append(CSV_EXT);

      // Open soil water time-series CSV file
      m_ofsSoilWaterTS.open(strTSFile.c_str(), ios::out | ios::trunc);
      if (! m_ofsSoilWaterTS)
      {
         // Error, cannot soil water time-series file
         cerr << ERR << "cannot open " << strTSFile << " for output" << endl;
         return (false);
      }

      // Write header line
      m_ofsSoilWaterTS << "'Elapsed'";
      for (int nLayer = 0; nLayer < m_nNumSoilLayers; nLayer++)
         m_ofsSoilWaterTS << ",\t" << "'Soil water for layer " << nLayer+1 << " (l)'";
      m_ofsSoilWaterTS << endl;
   }

   return (true);
}

//=========================================================================================================================================
//! Writes the results for this iteration to the .out file
//=========================================================================================================================================
bool CSimulation::bWritePerIterationResults(void)
{
   // TODO make this a user setting
   if (0 == m_ulIter % 20)
   {
      m_ofsOut << PERITERHEAD1 << endl;
      m_ofsOut << PERITERHEAD2 << endl;
   }

   // Output per-iteration hydrology
   m_ofsOut << setw(7) << m_ulIter;
   m_ofsOut << setprecision(4);
   m_ofsOut << setw(11) << m_dSimulatedTimeElapsed;

   // All these displayed as volumes (mm3)
   m_ofsOut << setprecision(0);
   m_ofsOut << setw(8) << m_dEndOfIterRain * m_dCellSquare;
   if (m_bRunOn)
      m_ofsOut << setw(6)  << m_dEndOfIterRunOn * m_dCellSquare;
   else
      m_ofsOut << "      -";
   if (m_bInfiltThisIter)
      m_ofsOut << setw(7) << m_dEndOfIterInfiltration * m_dCellSquare;
   else
      m_ofsOut << "      -";
   m_ofsOut << setw(8)  << m_dEndOfIterSurfaceWaterOffEdge * m_dCellSquare;
   m_ofsOut << setw(12) << m_dEndOfIterTotSurfaceWater * m_dCellSquare;
   m_ofsOut << " ";

   // Output per-iteration flow erosion details, all displayed as volumes (mm3)
   m_ofsOut << setw(8)  << (m_dEndOfIterClayFlowDetach + m_dEndOfIterSiltFlowDetach + m_dEndOfIterSandFlowDetach) * m_dCellSquare;
   m_ofsOut << setw(8)  << (m_dEndOfIterClayFlowDeposit + m_dEndOfIterSiltFlowDeposit + m_dEndOfIterSandFlowDeposit) * m_dCellSquare;
   m_ofsOut << setw(8)  << (m_dEndOfIterClaySedLoadOffEdge + m_dEndOfIterSiltSedLoadOffEdge + m_dEndOfIterSandSedLoadOffEdge) * m_dCellSquare;
   m_ofsOut << " ";

   if (m_bSplashThisIter)
   {
      // OK, we are calculating splash, and we are doing so this iteration, so output per-iteration splash redistribution, all as volumes (mm3)
//       m_ofsOut << setprecision(1);
      m_ofsOut << setw(8) << (m_dEndOfIterClaySplashDetach + m_dEndOfIterSiltSplashDetach + m_dEndOfIterSandSplashDetach) * m_dCellSquare;
      m_ofsOut << setw(8) << (m_dEndOfIterClaySplashDeposit + m_dEndOfIterSiltSplashDeposit + m_dEndOfIterSandSplashDeposit) * m_dCellSquare;
   }
   else
      m_ofsOut << "       -       -";

   m_ofsOut << setw(11) << (m_dEndOfIterClaySedLoad + m_dEndOfIterSiltSedLoad + m_dEndOfIterSandSedLoad) * m_dCellSquare;

   if (m_bSlumpThisIter)
   {
      // OK, we are calculating slump, and we are doing so this iteration, so output per-iteration slumping and toppling, all as volumes (mm3)
//       m_ofsOut << setprecision(0);
      m_ofsOut << setw(10) << (m_dEndOfIterClaySlumpDetach + m_dEndOfIterSiltSlumpDetach + m_dEndOfIterSandSlumpDetach) * m_dCellSquare;
      m_ofsOut << setw(10) << (m_dEndOfIterClayToppleDetach + m_dEndOfIterSiltToppleDetach + m_dEndOfIterSandToppleDetach) * m_dCellSquare;
   }
   else
      m_ofsOut << "         -         -";

   if (m_bInfiltThisIter)
   {
//       m_ofsOut << setprecision(1);
      m_ofsOut << setw(10) << (m_dEndOfIterClayInfiltDeposit + m_dEndOfIterSiltInfiltDeposit + m_dEndOfIterSandInfiltDeposit) * m_dCellSquare;
   }
   else
      m_ofsOut << "         -";

   if (m_bHeadcutRetreatThisIter)
   {
      m_ofsOut << setw(10) << (m_dEndOfIterClayHeadcutDetach + m_dEndOfIterSiltHeadcutDetach + m_dEndOfIterSandHeadcutDetach) * m_dCellSquare;
   }
   else
      m_ofsOut << "         -";

   // Finally, set "markers" for events (rainfall change, file saves) that have occurred this iteration
   if (m_bThisIterRainChange)
      m_ofsOut << " ΔRain";

   if (m_bSaveGISThisIter)
      m_ofsOut << " GIS" << m_nGISSave;

   m_ofsOut << endl;

   // Did a text file write error occur?
   if (m_ofsOut.fail())
      return (false);

   return (true);
}

//=========================================================================================================================================
//! Writes the results for this iteration to the time series CSV files
//=========================================================================================================================================
bool CSimulation::bWriteTSFiles(bool const bIsLastIter)
{
   static double sdLastSimulatedTimeElapsed = 0;

   // First do imprecision errors (always written): output the iteration and the timestep (in sec)
   m_ofsErrorTS << m_ulIter << ",\t" << m_dSimulatedTimeElapsed << ",\t" << m_dTimeStep << ",\t" << m_dWaterErrorLast << ",\t" << m_dSplashErrorLast << ",\t" << m_dSlumpErrorLast << ",\t" << m_dToppleErrorLast << ",\t" << m_dHeadcutErrorLast << ",\t" << m_dFlowErrorLast << endl;

   // Did an imprecision errors time series file write error occur?
   if (m_ofsErrorTS.fail())
      return (false);

   if (m_bTimeStepTS)
   {
      // Output the iteration and the timestep (in sec)
      m_ofsTimestepTS << m_ulIter << ",\t" << m_dSimulatedTimeElapsed << ",\t" << m_dTimeStep << ",\t" << m_dPossMaxSpeedNextIter << endl;

      // Did a timestep time series file write error occur?
      if (m_ofsTimestepTS.fail())
         return (false);
   }

   if (m_bAreaWetTS)
   {
      // Output as a percentage of the total area
      m_ofsAreaWetTS << m_dSimulatedTimeElapsed << ",\t" << 100.0 * static_cast<double>(m_ulNWet) / static_cast<double>(m_ulNActiveCells) << endl;

      // Did a rainfall time series file write error occur?
      if (m_ofsAreaWetTS.fail())
         return (false);
   }

   if (m_bRainTS)
   {
      // Output as a depth in mm
      m_ofsRainTS << m_dSimulatedTimeElapsed << ",\t" << m_dEndOfIterRain * m_dCellSquare << endl;

      // Did a rainfall time series file write error occur?
      if (m_ofsRainTS.fail())
         return (false);
   }

   if (m_bRunOnTS)
   {
      // Convert run-on water in mm3 to litres/sec
      m_ofsRunOnTS << m_dSimulatedTimeElapsed << ",\t" << m_dEndOfIterRunOn * m_dCellSquare * 1e-6 / m_dTimeStep << endl;

      // Did a run-on time series file write error occur?
      if (m_ofsRunOnTS.fail())
         return (false);
   }

   if (m_bSurfaceWaterTS)
   {
      // Convert surface water in mm3 to litres/sec
      m_ofsSurfaceWaterTS << m_dSimulatedTimeElapsed << ",\t" << m_dEndOfIterTotSurfaceWater * m_dCellSquare * 1e-6 / m_dTimeStep << endl;

      // Did a surface water time series file write error occur?
      if (m_ofsSurfaceWaterTS.fail())
         return (false);
   }

   if (m_bSurfaceWaterLostTS)
   {
      // Convert water lost (i.e. discharge) in mm3 to litres/sec
      m_ofsSurfaceWaterLostTS << m_dSimulatedTimeElapsed << ",\t" << m_dEndOfIterSurfaceWaterOffEdge * m_dCellSquare * 1e-6 / m_dTimeStep << endl;

      // Did a surface water lost time series file write error occur?
      if (m_ofsSurfaceWaterLostTS.fail())
         return (false);
   }

   if (m_bFlowDetachTS)
   {
      // Output flow detachment for each size class, convert from mm3 to g (bulk density is in kg/m3, so multiply by 1e6 to get it into g/mm3) then into g/sec
      m_ofsFlowDetachTS << m_dSimulatedTimeElapsed << ",\t" << m_dEndOfIterClayFlowDetach * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << ",\t" << m_dEndOfIterSiltFlowDetach * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << ",\t" << m_dEndOfIterSandFlowDetach * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << endl;

      // Did a flow detachment time series file write error occur?
      if (m_ofsFlowDetachTS.fail())
         return (false);
   }

   if (m_bDoSedLoadDepositTS)
   {
      // Output flow deposition for each size class, convert from mm3 to g (bulk density is in kg/m3, so multiply by 1e6 to get it into g/mm3) then into g/sec
      m_ofsFlowDepositSS << m_dSimulatedTimeElapsed << ",\t" << m_dEndOfIterClayFlowDeposit * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << ",\t" << m_dEndOfIterSiltFlowDeposit * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << ",\t" << m_dEndOfIterSandFlowDeposit * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << endl;

      // Did a flow deposition time series file write error occur?
      if (m_ofsFlowDepositSS.fail())
         return (false);
   }

   if (m_bSedOffEdgeTS)
   {
      // Output sediment lost for each size class, convert from mm3 to g (bulk density is in kg/m3, so multiply by 1e6 to get it into g/mm3) then into g/sec
      m_ofsSedLostTS << m_dSimulatedTimeElapsed << ",\t" << m_dSimulatedTimeElapsed - sdLastSimulatedTimeElapsed << ",\t" << m_dEndOfIterClaySedLoadOffEdge * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << ",\t" << m_dEndOfIterSiltSedLoadOffEdge * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << ",\t" << m_dEndOfIterSandSedLoadOffEdge * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << endl;

      // Did a sediment lost time series file write error occur?
      if (m_ofsSedLostTS.fail())
         return (false);
   }

   if (m_bSedLoadTS)
   {
      // Output sediment load for each size class, convert from mm3 to g (bulk density is in kg/m3, so multiply by 1e6 to get it into g/mm3) then into g/sec
      m_ofsSedLoadTS << m_dSimulatedTimeElapsed << ",\t" << m_dEndOfIterClaySedLoad * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << ",\t" << m_dEndOfIterSiltSedLoad * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << ",\t" << m_dEndOfIterSandSedLoad * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << endl;

      // Did a sediment load time series file write error occur?
      if (m_ofsSedLoadTS.fail())
         return (false);
   }

   // Now do the ones which are output less frequently
   static double sdLastInfiltSimulatedTimeElapsed = 0;

   if (m_bInfiltTS && (m_bInfiltThisIter || bIsLastIter))
   {
      // Convert surface water lost to soil water by infilt in mm3 to litres/sec
      m_ofsInfiltTS << m_dSimulatedTimeElapsed << ",\t" << m_dEndOfIterTSWriteInfiltration * m_dCellSquare * 1e-6 / m_dTimeStep << endl;

      m_dEndOfIterTSWriteInfiltration = 0;

      // Did an infilt time series file write error occur?
      if (m_ofsInfiltTS.fail())
         return (false);
   }

   if (m_bExfiltTS && (m_bInfiltThisIter || bIsLastIter))
   {
      // Convert soil water returned to surface water by exfilt in mm3 to litres/sec
      m_ofsExfiltTS << m_dSimulatedTimeElapsed << ",\t" << m_dSimulatedTimeElapsed - sdLastInfiltSimulatedTimeElapsed << ",\t" << m_dEndOfIterTSWriteExfiltration * m_dCellSquare * 1e-6 / m_dTimeStep << endl;

      m_dEndOfIterTSWriteInfiltration = 0;

      // Did an exfilt time series file write error occur?
      if (m_ofsExfiltTS.fail())
         return (false);
   }

   if (m_bInfiltDepositTS && (m_bInfiltThisIter || bIsLastIter))
   {
      // Output infilt deposition for each size class, convert from mm3 to g (bulk density is in kg/m3, so multiply by 1e6 to get it into g/mm3) then into g/sec
      m_ofsInfiltDepositTS << m_dSimulatedTimeElapsed << ",\t" << m_dEndOfIterTSWriteClayInfiltDeposit * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << ",\t" << m_dEndOfIterTSWriteSiltInfiltDeposit * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << ",\t" << m_dEndOfIterTSWriteSandInfiltDeposit * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << endl;

      m_dEndOfIterTSWriteClayInfiltDeposit =
      m_dEndOfIterTSWriteSiltInfiltDeposit =
      m_dEndOfIterTSWriteSandInfiltDeposit = 0;

      // Did an infilt deposition time series file write error occur?
      if (m_ofsInfiltDepositTS.fail())
         return (false);
   }

   static double sdLastSplashSimulatedTimeElapsed = 0;

   if (m_bSplashRedistTS && (m_bSplashThisIter || bIsLastIter))
   {
      // Output splash redistribution for each size class, convert from mm3 to g (bulk density is in kg/m3, so multiply by 1e6 to get it into g/mm3) then into g/sec
      m_ofsSplashDetachTS << m_dSimulatedTimeElapsed << ",\t" << m_dEndOfIterTSWriteClaySplashRedist * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << ",\t" << m_dEndOfIterTSWriteSiltSplashRedist * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << ",\t" << m_dEndOfIterTSWriteSandSplashRedist * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << endl;

      m_dEndOfIterTSWriteClaySplashRedist =
      m_dEndOfIterTSWriteSiltSplashRedist =
      m_dEndOfIterTSWriteSandSplashRedist = 0;

      // Did a splash redistribution time series file write error occur?
      if (m_ofsSplashDetachTS.fail())
         return (false);
   }

   if (m_bSplashKETS && (m_bSplashThisIter || bIsLastIter))
   {
      // Write out rainfall kinetic energy in Joules/sec
      m_ofsSplashKETS << m_dSimulatedTimeElapsed << ",\t" << m_dSimulatedTimeElapsed - sdLastSplashSimulatedTimeElapsed << ",\t" << m_dEndOfIterTSWriteKE / m_dTimeStep << endl;

      m_dEndOfIterTSWriteKE = 0;

      // Did a splash deposition time series file write error occur?
      if (m_ofsSplashKETS.fail())
         return (false);
   }

   static double sdLastSlumpSimulatedTimeElapsed = 0;

   if (m_bSlumpDetachTS && (m_bSlumpThisIter || bIsLastIter))
   {
      // Output slump detachment for each size class, convert from mm3 to g (bulk density is in kg/m3, so multiply by 1e6 to get it into g/mm3) then into g/sec
      m_ofsSlumpDetachTS << m_dSimulatedTimeElapsed << ",\t" << m_dEndOfIterTSWriteClaySlumpDetach * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << ",\t" << m_dEndOfIterTSWriteSiltSlumpDetach * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << ",\t" << m_dEndOfIterTSWriteSandSlumpDetach * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << endl;

      m_dEndOfIterTSWriteClaySlumpDetach =
      m_dEndOfIterTSWriteSiltSlumpDetach =
      m_dEndOfIterTSWriteSandSlumpDetach = 0;

      // Did a slumping time series file write error occur?
      if (m_ofsSlumpDetachTS.fail())
         return (false);
   }

   if (m_bToppleDetachTS && (m_bSlumpThisIter || bIsLastIter))
   {
      // Output toppling detachment for each size class, convert from mm3 to g (bulk density is in kg/m3, so multiply by 1e6 to get it into g/mm3) then into g/sec
      m_ofsToppleDetachTS << m_dSimulatedTimeElapsed << ",\t" << m_dEndOfIterTSWriteClayToppleDetach * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << ",\t" << m_dEndOfIterTSWriteSiltToppleDetach * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << ",\t" << m_dEndOfIterTSWriteSandToppleDetach * m_dCellSquare * m_dBulkDensityForOutputCalcs * 1e-6 / m_dTimeStep << endl;

      m_dEndOfIterTSWriteClayToppleDetach =
      m_dEndOfIterTSWriteSiltToppleDetach =
      m_dEndOfIterTSWriteSandToppleDetach = 0;

      // Did a toppling time series file write error occur?
      if (m_ofsToppleDetachTS.fail())
         return (false);
   }

   static double sdLastSoilWaterSimulatedTimeElapsed = 0;

   if (m_bSoilWaterTS && (m_bInfiltThisIter || bIsLastIter))
   {
      // Output soil water content for each soil layer, in litres
      m_ofsSoilWaterTS << m_dSimulatedTimeElapsed;
      for (int nLayer = 0; nLayer < m_nNumSoilLayers; nLayer++)
      {
         m_ofsSoilWaterTS << ",\t" << m_VdSinceLastTSSoilWater[nLayer] * m_dCellSquare * 1e-6;
         m_VdSinceLastTSSoilWater[nLayer] = 0;
      }
      m_ofsSoilWaterTS << endl;

      // Did a soil water time series file write error occur?
      if (m_ofsSoilWaterTS .fail())
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

//=========================================================================================================================================
//! Calculate the mass balance of water and of sediment for this iteration. Note that there is a potential problem re. rounding errors. These accumulate when many small values are summed using finite-precision arithmetic (see e.g. "Floating-Point Summation" http://www.ddj.com/cpp/184403224#REF1)
//=========================================================================================================================================
void CSimulation::CheckMassBalance(void)
{
   int const nWide = 12;

   m_ofsLog << string(110, '=') << endl << endl;

   m_ofsLog << std::fixed << setprecision(3) << "Iteration " << m_ulIter << ". Iteration duration: " << m_dTimeStep << "s. End-of-timestep simulated elapsed time: " << strDispTime(m_dSimulatedTimeElapsed, false, true) << " (" << 100 * m_dSimulatedTimeElapsed / m_dSimulationDuration << "%). Real-world elapsed time: " << strDispTime(m_dElapsed, true, false) << endl << endl;

   m_ofsLog << std::fixed << setprecision(0);

   m_ofsLog << "WATER (all volumes, mm**3)" << endl;
   m_ofsLog << "|" << string(58, '-') << "|" << string(nWide, '-') << "|" << endl;
   m_ofsLog << "|" << strLeft("Surface water at start of iteration", 58) << "|" << strDblRight(m_dStartOfIterTotSurfaceWater * m_dCellSquare, 0, nWide, true) << "|" << endl;
   m_ofsLog << "|" << strLeft("Subsurface water at start of iteration", 58) << "|" << strDblRight(m_dStartOfIterTotSoilWater * m_dCellSquare, 0, nWide, true) << "|" << endl;
   m_ofsLog << "|" << strLeft("Rain", 58) << "|" << strDblRight(m_dEndOfIterRain * m_dCellSquare, 0, nWide, true) << "|" << endl;
   m_ofsLog << "|" << strLeft("Run-on", 58) << "|" << strDblRight(m_dEndOfIterRunOn * m_dCellSquare, 0, nWide, true) << "|" << endl;   m_ofsLog << "|" << string(58, '-') << "|" << string(nWide, '-') << "|" << endl;
   double dTotWaterAtStart = m_dEndOfIterRain + m_dEndOfIterRunOn + m_dStartOfIterTotSurfaceWater + m_dStartOfIterTotSoilWater;
   m_ofsLog << "|" << strLeft("TOTAL", 58) << "|" << strDblRight(dTotWaterAtStart * m_dCellSquare, 0, nWide, true) << "|" << endl;
   m_ofsLog << "|" << string(58, '-') << "|" << string(nWide, '-') << "|" << endl;
   // m_ofsLog << "|" << strLeft("Infiltration", 58) << "|" << strDblRight(m_dEndOfIterInfiltration * m_dCellSquare, 0, nWide, true) << "|" << endl;
   // m_ofsLog << "|" << strLeft("Exfiltration", 58) << "|" << strDblRight(m_dEndOfIterExfiltration * m_dCellSquare, 0, nWide, true) << "|" << endl;
   m_ofsLog << "|" << strLeft("Surface water at end of simulation", 58) << "|" << strDblRight(m_dEndOfIterTotSurfaceWater * m_dCellSquare, 0, nWide, true) << "|" << endl;
   m_ofsLog << "|" << strLeft("Subsurface water at end of simulation", 58) << "|" << strDblRight(m_dEndOfIterTotSoilWater * m_dCellSquare, 0, nWide, true) << "|" << endl;
   m_ofsLog << "|" << strLeft("Surface water lost from grid", 58) << "|" << strDblRight(m_dEndOfIterSurfaceWaterOffEdge * m_dCellSquare, 0, nWide, true) << "|" << endl;
   m_ofsLog << "|" << string(58, '-') << "|" << string(nWide, '-') << "|" << endl;
   double dTotWaterAtEnd = m_dEndOfIterTotSurfaceWater + m_dEndOfIterTotSoilWater + m_dEndOfIterSurfaceWaterOffEdge;
   m_ofsLog << "|" << strLeft("TOTAL", 58) << "|" << strDblRight(dTotWaterAtEnd * m_dCellSquare, 0, nWide, true) << "|" << endl;
   m_ofsLog << "|" << string(58, '-') << "|" << string(nWide, '-') << "|" << endl << endl;

   m_ofsLog << "SEDIMENT (all volumes, mm**3)" << endl;
   m_ofsLog << "|" << string(58, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|"<< endl;
   m_ofsLog << "|" << string(58, ' ') << "|" << strCentre("Clay", nWide) << "|" << strCentre("Silt", nWide) << "|" << strCentre("Sand", nWide) << "|" << strCentre("ALL", nWide) << "|" << endl;
   m_ofsLog << "|" << string(58, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|"<< endl;

   double dThisIterFlowDetach = m_dEndOfIterClayFlowDetach + m_dEndOfIterSiltFlowDetach + m_dEndOfIterSandFlowDetach;
   m_ofsLog << "|" << strLeft("Flow detachment", 58) << "|" << strDblRight(m_dEndOfIterClayFlowDetach * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSiltFlowDetach * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSandFlowDetach * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dThisIterFlowDetach * m_dCellSquare, 0, nWide, true) << "|" << endl;

   double dThisIterSplashDetach = m_dEndOfIterClaySplashDetach + m_dEndOfIterSiltSplashDetach + m_dEndOfIterSandSplashDetach;
   m_ofsLog << "|" << strLeft("Splash detachment", 58) << "|" << strDblRight(m_dEndOfIterClaySplashDetach * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSiltSplashDetach * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSandSplashDetach * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dThisIterSplashDetach * m_dCellSquare, 0, nWide, true) << "|" << endl;

   double dThisIterSlumpDetach = m_dEndOfIterClaySlumpDetach + m_dEndOfIterSiltSlumpDetach  + m_dEndOfIterSandSlumpDetach;
   m_ofsLog << "|" << strLeft("Slump detachment", 58) << "|" << strDblRight(m_dEndOfIterClaySlumpDetach * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSiltSlumpDetach * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSandSlumpDetach * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dThisIterSlumpDetach * m_dCellSquare, 0, nWide, true) << "|" << endl;

   double dThisIterToppleDetach = m_dEndOfIterClayToppleDetach + m_dEndOfIterSiltToppleDetach  + m_dEndOfIterSandToppleDetach;
   m_ofsLog << "|" << strLeft("Toppling detachment", 58) << "|" << strDblRight(m_dEndOfIterClayToppleDetach * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSiltToppleDetach * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSandToppleDetach * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dThisIterToppleDetach * m_dCellSquare, 0, nWide, true) << "|" << endl;

   double dThisIterHeadcutRetreatDetach = m_dEndOfIterClayHeadcutDetach + m_dEndOfIterSiltHeadcutDetach  + m_dEndOfIterSandHeadcutDetach;
   m_ofsLog << "|" << strLeft("Headcut retreat detachment", 58) << "|" << strDblRight(m_dEndOfIterClayHeadcutDetach * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSiltHeadcutDetach * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSandHeadcutDetach * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dThisIterHeadcutRetreatDetach * m_dCellSquare, 0, nWide, true) << "|" << endl;

   m_ofsLog << "|" << string(58, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|"<< endl;

   double dThisIterClayDetach = m_dEndOfIterClayFlowDetach + m_dEndOfIterClaySplashDetach + m_dEndOfIterClaySlumpDetach + m_dEndOfIterClayToppleDetach + m_dEndOfIterClayHeadcutDetach;
   double dThisIterSiltDetach = m_dEndOfIterSiltFlowDetach + m_dEndOfIterSiltSplashDetach + m_dEndOfIterSiltSlumpDetach + m_dEndOfIterSiltToppleDetach + m_dEndOfIterSiltHeadcutDetach;
   double dThisIterSandDetach = m_dEndOfIterSandFlowDetach + m_dEndOfIterSandSplashDetach + m_dEndOfIterSandSlumpDetach + m_dEndOfIterSandToppleDetach + m_dEndOfIterSandHeadcutDetach;
   double dThisIterTotDetach = dThisIterSplashDetach + dThisIterFlowDetach + dThisIterSlumpDetach + dThisIterToppleDetach + dThisIterHeadcutRetreatDetach;
   m_ofsLog << "|" << strLeft("TOTAL DETACHMENT *", 58) << "|" << strDblRight(dThisIterClayDetach * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dThisIterSiltDetach * m_dCellSquare, 0, nWide, false) << "|" <<strDblRight(dThisIterSandDetach * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dThisIterTotDetach * m_dCellSquare, 0, nWide, true) << "|" << endl;

   m_ofsLog << "|" << string(58, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|"<< endl;

   double dThisIterFlowDeposit = m_dEndOfIterClayFlowDeposit + m_dEndOfIterSiltFlowDeposit + m_dEndOfIterSandFlowDeposit;
   m_ofsLog << "|" << strLeft("Deposition from flow", 58) << "|" << strDblRight(m_dEndOfIterClayFlowDeposit * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSiltFlowDeposit * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSandFlowDeposit * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dThisIterFlowDeposit * m_dCellSquare, 0, nWide, true) << "|" << endl;

   double dThisIterInfiltDeposit = m_dEndOfIterClayInfiltDeposit + m_dEndOfIterSiltInfiltDeposit + m_dEndOfIterSandInfiltDeposit;
   m_ofsLog << "|" << strLeft("Deposition due to infiltration", 58) << "|" << strDblRight(m_dEndOfIterClayInfiltDeposit * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSiltInfiltDeposit * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSandInfiltDeposit * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dThisIterInfiltDeposit * m_dCellSquare, 0, nWide, true) << "|" << endl;

   double dThisIterSplashDeposit = m_dEndOfIterClaySplashDeposit + m_dEndOfIterSiltSplashDeposit + m_dEndOfIterSandSplashDeposit;
   m_ofsLog << "|" << strLeft("Deposition from splash", 58) << "|" << strDblRight(m_dEndOfIterClaySplashDeposit * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSiltSplashDeposit * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSandSplashDeposit * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dThisIterSplashDeposit * m_dCellSquare, 0, nWide, true) << "|" << endl;

   double dThisIterSlumpDeposit = m_dEndOfIterClaySlumpDeposit + m_dEndOfIterSiltSlumpDeposit  + m_dEndOfIterSandSlumpDeposit;
   m_ofsLog << "|" << strLeft("Deposition from slump", 58) << "|" << strDblRight(m_dEndOfIterClaySlumpDeposit * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSiltSlumpDeposit * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSandSlumpDeposit * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dThisIterSlumpDeposit * m_dCellSquare, 0, nWide, true) << "|" << endl;

   double dThisIterToppleDeposit = m_dEndOfIterClayToppleDeposit + m_dEndOfIterSiltToppleDeposit  + m_dEndOfIterSandToppleDeposit;
   m_ofsLog << "|" << strLeft("Deposition from toppling", 58) << "|" << strDblRight(m_dEndOfIterClayToppleDeposit * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSiltToppleDeposit * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSandToppleDeposit * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dThisIterToppleDeposit * m_dCellSquare, 0, nWide, true) << "|" << endl;

   double dThisIterHeadcutRetreatDeposit = m_dEndOfIterClayHeadcutDeposit + m_dEndOfIterSiltHeadcutDeposit  + m_dEndOfIterSandHeadcutDeposit;
   m_ofsLog << "|" << strLeft("Deposition from headcut retreat", 58) << "|" << strDblRight(m_dEndOfIterClayHeadcutDeposit * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSiltHeadcutDeposit * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSandHeadcutDeposit * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dThisIterHeadcutRetreatDeposit * m_dCellSquare, 0, nWide, true) << "|" << endl;

   m_ofsLog << "|" << string(58, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|"<< endl;

   double dThisIterClayDeposit = m_dEndOfIterClayFlowDeposit + m_dEndOfIterClayInfiltDeposit + m_dEndOfIterClaySplashDeposit + m_dEndOfIterClaySlumpDeposit + m_dEndOfIterClayToppleDeposit + m_dEndOfIterClayHeadcutDeposit;
   double dThisIterSiltDeposit = m_dEndOfIterSiltFlowDeposit + m_dEndOfIterSiltInfiltDeposit + m_dEndOfIterSiltSplashDeposit + m_dEndOfIterSiltSlumpDeposit + m_dEndOfIterSiltToppleDeposit + m_dEndOfIterSiltHeadcutDeposit;
   double dThisIterSandDeposit = m_dEndOfIterSandFlowDeposit + m_dEndOfIterSandInfiltDeposit + m_dEndOfIterSandSplashDeposit + m_dEndOfIterSandSlumpDeposit + m_dEndOfIterSandToppleDeposit + m_dEndOfIterSandHeadcutDeposit;
   double dThisIterTotDeposit = dThisIterSplashDeposit + dThisIterFlowDeposit + dThisIterSlumpDeposit + dThisIterToppleDeposit + dThisIterHeadcutRetreatDeposit;
   m_ofsLog << "|" << strLeft("TOTAL DEPOSITION *", 58) << "|" << strDblRight(dThisIterClayDeposit * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dThisIterSiltDeposit * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dThisIterSandDeposit * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dThisIterTotDeposit * m_dCellSquare, 0, nWide, true) << "|" << endl;

   m_ofsLog << "|" << string(58, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|"<< endl;

   double dTotNetDetachment = m_dEndOfIterNetClayDetachment + m_dEndOfIterNetSiltDetachment + m_dEndOfIterNetSandDetachment;
   m_ofsLog << "|" << strLeft("TOTAL PER CELL DETACHMENT - DEPOSITION", 58) << "|" << strDblRight(m_dEndOfIterNetClayDetachment * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterNetSiltDetachment * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterNetSandDetachment * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dTotNetDetachment * m_dCellSquare, 0, nWide, true) << "|" << endl;

   double dThisIterElevChange = m_dStartOfIterTotElev - m_dEndOfIterTotElev;
   m_ofsLog << "|" << strLeft("Volumetric change in DEM", 58) << "|" << string(nWide, ' ') << "|" << string(nWide, ' ') << "|" << string(nWide, ' ') << "|" << strDblRight(dThisIterElevChange * m_dCellSquare, 0, nWide, true) << "|" << endl;

   m_ofsLog << "|" << string(58, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|"<< endl;

   double dStartOfIterTotSedLoad = m_dStartOfIterTotClaySedLoad + m_dStartOfIterTotSiltSedLoad + m_dStartOfIterTotSandSedLoad;
   m_ofsLog << "|" << strLeft("Sediment load at start of iteration", 58) << "|" << strDblRight(m_dStartOfIterTotClaySedLoad * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dStartOfIterTotSiltSedLoad * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dStartOfIterTotSandSedLoad * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dStartOfIterTotSedLoad * m_dCellSquare, 0, nWide, true) << "|" << endl;

   double dEndOfIterTotSedLoad = m_dEndOfIterClaySedLoad + m_dEndOfIterSiltSedLoad + m_dEndOfIterSandSedLoad;
   m_ofsLog << "|" << strLeft("Sediment load at end of iteration", 58) << "|" << strDblRight(m_dEndOfIterClaySedLoad * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSiltSedLoad * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSandSedLoad * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dEndOfIterTotSedLoad * m_dCellSquare, 0, nWide, true) << "|" << endl;

   m_ofsLog << "|" << string(58, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|"<< endl;

   m_ofsLog << "|" << strLeft("TOTAL CHANGE IN SEDIMENT LOAD", 58) << "|" << strDblRight((m_dEndOfIterClaySedLoad - m_dStartOfIterTotClaySedLoad) * m_dCellSquare, 0, nWide, false) << "|" << strDblRight((m_dEndOfIterSiltSedLoad - m_dStartOfIterTotSiltSedLoad) * m_dCellSquare, 0, nWide, false) << "|" << strDblRight((m_dEndOfIterSandSedLoad - m_dStartOfIterTotSandSedLoad) * m_dCellSquare, 0, nWide, false) << "|" << strDblRight((dEndOfIterTotSedLoad - dStartOfIterTotSedLoad) * m_dCellSquare, 0, nWide, true) << "|" << endl;

   m_ofsLog << "|" << string(58, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|"<< endl;

   double dFlowOffEdge = m_dEndOfIterClaySedLoadOffEdge + m_dEndOfIterSiltSedLoadOffEdge + m_dEndOfIterSandSedLoadOffEdge;
   m_ofsLog << "|" << strLeft("Sediment load moved off grid edge", 58) << "|" << strDblRight(m_dEndOfIterClaySedLoadOffEdge * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSiltSedLoadOffEdge * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSandSedLoadOffEdge * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dFlowOffEdge * m_dCellSquare, 0, nWide, true) << "|" << endl;

   double dSplashOffEdge = m_dEndOfIterClaySplashOffEdge + m_dEndOfIterSiltSplashOffEdge + m_dEndOfIterSandSplashOffEdge;
   m_ofsLog << "|" << strLeft("Splashed sediment moved off grid edge", 58) << "|" << strDblRight(m_dEndOfIterClaySplashOffEdge * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSiltSplashOffEdge * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(m_dEndOfIterSandSplashOffEdge * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dSplashOffEdge * m_dCellSquare, 0, nWide, true) << "|" << endl;

   m_ofsLog << "|" << string(58, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|"<< endl;

   double dClayOffEdge = m_dEndOfIterClaySedLoadOffEdge + m_dEndOfIterClaySplashOffEdge;
   double dSiltOffEdge = m_dEndOfIterSiltSedLoadOffEdge + m_dEndOfIterSiltSplashOffEdge;
   double dSandOffEdge = m_dEndOfIterSandSedLoadOffEdge + m_dEndOfIterSandSplashOffEdge;
   m_ofsLog << "|" << strLeft("TOTAL OFF EDGE", 58) << "|" << strDblRight(dClayOffEdge * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dSiltOffEdge * m_dCellSquare, 0, nWide, false) << "|" << strDblRight(dSandOffEdge * m_dCellSquare, 0, nWide, false) << "|" << strDblRight((dClayOffEdge + dSiltOffEdge + dSandOffEdge) * m_dCellSquare, 0, nWide, true) << "|" << endl;

   m_ofsLog << "|" << string(58, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|" << string(nWide, '-') << "|"<< endl;

   m_ofsLog << "* Total detachment - total deposition usually differs from total per-cell detachment - deposition, due to re-detachment of previously deposited sediment." << endl << endl;
}


