/*========================================================================================================================================

This is read_input.cpp: it reads the non-GIS input files for RillGrow

Copyright (C) 2025 David Favis-Mortlock

=========================================================================================================================================

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public  License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

========================================================================================================================================*/
#include "rg.h"
#include "simulation.h"

//=========================================================================================================================================
//! The bReadIni member function reads the initialization file
//=========================================================================================================================================
bool CSimulation::bReadIni(void)
{
   m_strRGIni = m_strRGDir;
   m_strRGIni.append(RG_INI);

   // The .ini file is assumed to be in the RG executable's directory
   string strFilePathName(m_strRGIni);

   // Tell the user what is happening
   cout << READ_FILE_LOCATIONS << strFilePathName << endl;

   // Create an ifstream object
   ifstream InStream;

   // Try to open .ini file for input
   InStream.open(strFilePathName, ios::in);

   // Did it open OK?
   if (! InStream.is_open())
   {
      // Error: cannot open .ini file for input
      cerr << ERR << "cannot open " << strFilePathName << " for input" << endl;
      return (false);
   }

   int i = 0, nLine = 0;
   string strRec, strErr;

   while (getline(InStream, strRec))
   {
      nLine++;

      // Trim off leading and trailing whitespace
      strRec = strTrim(&strRec);

      // If it is a blank line or a comment then ignore it
      if ((strRec.empty()) || (strRec[0] == QUOTE1) || (strRec[0] == QUOTE2))
         continue;

      size_t nPos = strRec.find(':');
      if (nPos == string::npos)
      {
         // Error: badly formatted line (no colon)
         cerr << ERR << "badly formatted line " << nLine << " (no ':') in " << strFilePathName << endl << "'" << strRec << "'" << endl;
         return (false);
      }

      // It is OK, so increment item counter
      i++;

      // Strip off leading portion (the bit up to and including the colon). Note that strRH could be empty for un-needed data items
      string strRH = strSplitRH(&strRec);

      // Remove leading whitespace
      strRH = strTrimLeft(&strRH);

      // Look for a trailing comment, if found then terminate string at that point and trim off any trailing whitespace
      nPos = strRH.rfind(QUOTE1);
      if (nPos != string::npos)
         strRH.resize(nPos+1);

      nPos = strRH.rfind(QUOTE2);
      if (nPos != string::npos)
         strRH.resize(nPos+1);

      // Remove trailing whitespace
      strRH = strTrimRight(&strRH);

      switch (i)
      {
      case 1:
         // The main input run-data filename
         if (strRH.empty())
         {
            strErr = "path and name of main datafile";
            break;
         }

         // Check that we don't already have an input run-data filename, e.g. one entered on the command-line
         if (m_strDataPathName.empty())
         {
            // We don't: so first check for leading slash, or leading Unix home dir symbol, or occurrence of a drive letter
            if ((strRH.find(PATH_SEPARATOR) != string::npos) || (strRH.find('~') != string::npos) || (strRH.find(':') != string::npos))
               // It has an absolute path, so use it 'as is'
               m_strDataPathName = strRH;
            else
            {
               // It has a relative path, so prepend the RG dir
               m_strDataPathName = m_strRGDir;
               m_strDataPathName.append(strRH);
            }
         }

         break;

      case 2:
         // Path for RG output
         if (strRH.empty())
         {
            strErr = "path for RillGrow's output";
            break;
         }

         // Check for trailing slash on RG output directory name (is vital)
         if (strRH[strRH.size()-1] != PATH_SEPARATOR)
            strRH.push_back(PATH_SEPARATOR);

         // Now check for leading slash, or leading Unix home dir symbol, or occurrence of a drive letter
         if ((strRH[0] == PATH_SEPARATOR) || (strRH[0] == '~') || (strRH[1] == ':'))
            // It is an absolute path, so use it 'as is'
            m_strOutputPath = strRH;
         else
         {
            // It is a relative path, so prepend the RG dir
            m_strOutputPath = m_strRGDir;
            m_strOutputPath.append(strRH);
         }

         break;

      case 3:
         // Email address, only useful if running under Linux/Unix
         if (! strRH.empty())
         {
            // Something was entered, do rudimentary check for valid email address
            if (strRH.find('@') == string::npos)
            {
               strErr = "email address for messages";
               break;
            }
            else
               m_strMailAddress = strRH;
         }
         break;
      }

      // Did an error occur?
      if (! strErr.empty())
      {
         // Error in input to initialisation file
         cerr << ERR << "reading " << strErr << " on line " << nLine << " of " << strFilePathName << endl << "'" << strRec << "'" << endl;
         InStream.close();

         return false;
      }
   }

   InStream.close();
   return (true);
}

//=========================================================================================================================================
//! The bReadRunData member function reads the run details input file and does some initialization
//=========================================================================================================================================
bool CSimulation::bReadRunData(void)
{
   // Tell the user what is happening
   cout << READ_RUN_DATA_FILE << m_strDataPathName << endl;

   // Create an ifstream object
   ifstream InStream;

   // Try to open run details file for input
   InStream.open(m_strDataPathName, ios::in);

   // Did it open OK?
   if (! InStream.is_open())
   {
      // Error: cannot open run details file for input
      cerr << ERR << "cannot open " << m_strDataPathName << " for input" << endl;
      return false;
   }

   int i = 0, nLine = 0;
   string strRec, strRH, strErr;

   while (getline(InStream, strRec))
   {
      nLine++;

      // Trim off leading and trailing whitespace
      strRec = strTrim(&strRec);

      // If it is a blank line or a comment then ignore it
      if ((strRec.empty()) || (strRec[0] == QUOTE1) || (strRec[0] == QUOTE2))
         continue;

      size_t nPos = strRec.find(':');
      if (nPos == string::npos)
      {
         // Error: badly formatted line (no colon)
         cerr << ERR << "badly formatted line " << nLine << " (no ':') in " << m_strDataPathName << endl << "'" << strRec << "'" << endl;
         return (false);
      }

      // It is OK, so increment item counter
      i++;

      // Get the RH bit of the string, after the colon. Note that this might be an empty string for un-needed data items
      strRH = strSplitRH(&strRec);

      int
         nMultiplier = 0,
         nLen = 0;
      vector<string> VstrItems;

      switch (i)
      {
      // --------------------------------------------------- Run Information ----------------------------------------------------------
      case 1:
         // Duration of rainfall, then duration of simulation: first convert to lower case
         strRH = strToLower(&strRH);

         // Split, space separated
         VstrItems = VstrSplit(&strRH, SPACE);

         nLen = static_cast<int>(VstrItems.size());
         if ((nLen < 2) || (nLen > 3))
         {
            strErr = "simulation duration incorrectly specified";
            break;
         }

         if (nLen == 3)
         {
            m_dSimulatedRainDuration = stod(VstrItems[0]);
            m_dSimulationDuration = stod(VstrItems[1]);
         }
         else
         {
            m_dSimulationDuration = stod(VstrItems[0]);
            m_dSimulatedRainDuration = m_dSimulationDuration;
         }

         if (m_dSimulationDuration <= 0)
         {
            strErr = "simulation duration must be greater than zero";
            break;
         }

         // Now work out what units this is in
         if (VstrItems[nLen-1].find("m") != string::npos)
            // Specified in minutes
            nMultiplier = 60;
         else if (VstrItems[nLen-1].find("h") != string::npos)
            // Specified in hours
            nMultiplier = 3600;
         else if (VstrItems[nLen-1].find("d") != string::npos)
            // Specified in days
            nMultiplier = 3600 * 24;
         else if (VstrItems[nLen-1].find("s") != string::npos)
            // Specifed in seconds
            nMultiplier = 1;
         else
         {
            strErr = "units of simulation duration";
            break;
         }

         // Convert the simulation duration and rain duration to seconds
         m_dSimulationDuration *= nMultiplier;
         m_dSimulatedRainDuration *= nMultiplier;
         break;

      case 2:
         // Save interval(s): first convert to lower case
         strRH = strToLower(&strRH);

         // Split, space separated
         VstrItems = VstrSplit(&strRH, SPACE);
         nLen = static_cast<int>(VstrItems.size());
         if (nLen == 0)
         {
            // No save intervals specified, just output at end
            break;
         }

         // Now work out what units this is in
         if (VstrItems[nLen-1].find("m") != string::npos)
            // Specified in minutes
            nMultiplier = 60;
         else if (VstrItems[nLen-1].find("h") != string::npos)
            // Specified in hours
            nMultiplier = 3600;
         else if (VstrItems[nLen-1].find("d") != string::npos)
            // Specified in days
            nMultiplier = 3600 * 24;
         else if (VstrItems[nLen-1].find("s") != string::npos)
            // Specifed in seconds
            nMultiplier = 1;
         else
         {
            strErr = "units for save intervals";
            break;
         }

         if (nLen == 2)
         {
            // Regular save intervals
            m_bSaveRegular = true;
            m_dRSaveInterval = stod(VstrItems[0]) * nMultiplier;

            if (m_dRSaveTime <= 0)
            {
               strErr = "save interval must be greater than zero";
               break;
            }

            // Set up for first save
            m_dRSaveTime = m_dRSaveInterval;
         }
         else
         {
            // Irregular save intervals
            m_bSaveRegular = false;

            m_nUSave = nLen-1;
            for (int n = 0; n < m_nUSave; n++)
            {
               double dSave = stod(VstrItems[n]);
               dSave *= nMultiplier;

               m_VdSaveTime.push_back(dSave);
            }

            // Put a dummy save interval as the last entry in the array: this is needed to stop problems at end of run
            m_VdSaveTime.push_back(m_dSimulationDuration + 1);
         }
         break;

      case 3:
         // Random number seed(s)
         VstrItems = VstrSplit(&strRH, SPACE);
         nLen = static_cast<int>(VstrItems.size());
         if (nLen == 0)
         {
            // Just use previously set random number seeds
            break;
         }

         // Use user-specified random number seeds
         for (int n = 0; n < NUMBER_OF_RNGS; n++)
         {
            if (n < nLen)
               m_ulRandSeed[n] = stoi(VstrItems[n]);
            else
               m_ulRandSeed[n] = m_ulRandSeed[0];
         }
         break;

      case 4:
         // GIS files to output, convert to lower case filenames
         strRH = strToLower(&strRH);
         if (strRH.find(GIS_ALL_CODE) != string::npos)
         {
            m_bRainVarMSave            =
            m_bCumulRunOnSave          =
            m_bElevSave                =
            m_bDetrendElevSave         =
            m_bFlowDetachSave          =
            m_bInitElevSave            =
            m_bInfiltSave              =
            m_bCumulInfiltSave         =
            m_bSoilWaterSave           =
            m_bInfiltDepositSave       =
            m_bCumulInfiltDepositSave  =
            m_bTopSurfaceSave          =
            m_bSplashSave              =
            m_bCumulSplashSave         =
            m_bFlowDirSave             =
            m_bStreamPowerSave         =
            m_bShearStressSave         =
            m_bFrictionFactorSave      =
            m_bCumulAvgShearStressSave =
            m_bReynoldsSave            =
            m_bFroudeSave              =
            m_bTCSave                  =
            m_bCumulAvgDepthSave       =
            m_bCumulAvgDWSpdSave       =
            m_bCumulAvgSpdSave         =
            m_bSedConcSave             =
            m_bSedLoadSave             =
            m_bAvgSedLoadSave          =
            m_bCumulFlowDepositSave    =
            m_bSlumpSave               =
            m_bToppleSave              =
            m_bCumulLoweringSave       =
            m_bLostSave                = true;
         }
         else
         {
            if (strRH.find(GIS_RAIN_VARIATION_CODE) != string::npos)
            {
               m_bRainVarMSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_RAIN_VARIATION_CODE);
            }

            if (strRH.find(GIS_CUMUL_RUNON_CODE) != string::npos)
            {
               m_bCumulRunOnSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_CUMUL_RUNON_CODE);
            }

            if (strRH.find(GIS_ELEVATION_CODE) != string::npos)
            {
               m_bElevSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_ELEVATION_CODE);
            }

            if (strRH.find(GIS_DETREND_ELEVATION_CODE) != string::npos)
            {
               m_bDetrendElevSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_DETREND_ELEVATION_CODE);
            }

            if (strRH.find(GIS_ALL_SIZE_FLOW_DETACH_CODE) != string::npos)
            {
               m_bFlowDetachSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_ALL_SIZE_FLOW_DETACH_CODE);
            }

            if (strRH.find(GIS_INITIAL_ELEVATION_CODE) != string::npos)
            {
               m_bInitElevSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_INITIAL_ELEVATION_CODE);
            }

            if (strRH.find(GIS_INFILT_CODE) != string::npos)
            {
               m_bCumulInfiltSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_INFILT_CODE);
            }

            if (strRH.find(GIS_CUMUL_INFILT_CODE) != string::npos)
            {
               m_bCumulInfiltSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_CUMUL_INFILT_CODE);
            }

            if (strRH.find(GIS_SOIL_WATER_CODE) != string::npos)
            {
               m_bSoilWaterSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_SOIL_WATER_CODE);
            }

            if (strRH.find(GIS_TOP_SURFACE_DETREND_CODE) != string::npos)
            {
               m_bTopSurfaceSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_TOP_SURFACE_DETREND_CODE);
            }

            if (strRH.find(GIS_SPLASH_CODE) != string::npos)
            {
               m_bSplashSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_SPLASH_CODE);
            }

            if (strRH.find(GIS_CUMUL_SPLASH_CODE) != string::npos)
            {
               m_bCumulSplashSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_CUMUL_SPLASH_CODE);
            }

            if (strRH.find(GIS_INUNDATION_REGIME_CODE) != string::npos)
            {
               m_bInundationSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_INUNDATION_REGIME_CODE);
            }

            if (strRH.find(GIS_SURFACE_WATER_DIRECTION_CODE) != string::npos)
            {
               m_bFlowDirSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_SURFACE_WATER_DIRECTION_CODE);
            }

            if (strRH.find(GIS_STREAMPOWER_CODE) != string::npos)
            {
               m_bStreamPowerSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_STREAMPOWER_CODE);
            }

            if (strRH.find(GIS_SHEAR_STRESS_CODE) != string::npos)
            {
               m_bShearStressSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_SHEAR_STRESS_CODE);
            }

            if (strRH.find(GIS_FRICTION_FACTOR_CODE) != string::npos)
            {
               m_bFrictionFactorSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_FRICTION_FACTOR_CODE);
            }

            if (strRH.find(GIS_AVG_SHEAR_STRESS_CODE) != string::npos)
            {
               m_bCumulAvgShearStressSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_AVG_SHEAR_STRESS_CODE);
            }

            if (strRH.find(GIS_REYNOLDS_NUMBER_CODE) != string::npos)
            {
               m_bReynoldsSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_REYNOLDS_NUMBER_CODE);
            }

            if (strRH.find(GIS_FROUDE_NUMBER_CODE) != string::npos)
            {
               m_bFroudeSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_FROUDE_NUMBER_CODE);
            }

            if (strRH.find(GIS_TRANSPORT_CAPACITY_CODE) != string::npos)
            {
               m_bTCSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_TRANSPORT_CAPACITY_CODE);
            }

            if (strRH.find(GIS_AVG_SURFACE_WATER_DEPTH_CODE) != string::npos)
            {
               m_bCumulAvgDepthSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_AVG_SURFACE_WATER_DEPTH_CODE);
            }

            if (strRH.find(GIS_AVG_SURFACE_WATER_SPEED_CODE) != string::npos)
            {
               m_bCumulAvgSpdSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_AVG_SURFACE_WATER_SPEED_CODE);
            }

            if (strRH.find(GIS_AVG_SURFACE_WATER_DW_SPEED_CODE) != string::npos)
            {
               m_bCumulAvgDWSpdSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_AVG_SURFACE_WATER_DW_SPEED_CODE);
            }

            if (strRH.find(GIS_SEDIMENT_CONCENTRATION_CODE) != string::npos)
            {
               m_bSedConcSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_SEDIMENT_CONCENTRATION_CODE);
            }

            if (strRH.find(GIS_SEDIMENT_LOAD_CODE) != string::npos)
            {
               m_bSedLoadSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_SEDIMENT_LOAD_CODE);
            }

            if (strRH.find(GIS_AVG_SEDIMENT_LOAD_CODE) != string::npos)
            {
               m_bAvgSedLoadSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_AVG_SEDIMENT_LOAD_CODE);
            }

            if (strRH.find(GIS_CUMUL_ALL_SIZE_FLOW_DEPOSIT_CODE) != string::npos)
            {
               m_bCumulFlowDepositSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_CUMUL_ALL_SIZE_FLOW_DEPOSIT_CODE);
            }

            if (strRH.find(GIS_CUMUL_ALL_PROC_SURF_LOWER_CODE) != string::npos)
            {
               m_bCumulLoweringSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_CUMUL_ALL_PROC_SURF_LOWER_CODE);
            }

            if (strRH.find(GIS_AVG_SURFACE_WATER_FROM_EDGES_CODE) != string::npos)
            {
               m_bLostSave = true;
               strRH = strRemoveSubstr(&strRH, &GIS_AVG_SURFACE_WATER_FROM_EDGES_CODE);
            }

            // Check to see if all codes have been removed
            strRH = strTrimLeft(&strRH);
            if (! strRH.empty())
               strErr = "GIS output file list";
         }
         break;

      case 5:
         // GIS output format: blank means use same format as input DEM file (if possible)
         strRH = strToLower(&strRH);
         m_strGISOutFormat = strRH;
         break;

      case 6:
         // Text output file names, don't change case
         if (strRH.empty())
         {
            strErr = "output file names";
            break;
         }

         m_strRunName = strRH;

         m_strOutFile = m_strOutputPath;
         m_strOutFile.append(strRH);
         m_strOutFile.append(OUT_EXT);

         m_strLogFile = m_strOutputPath;
         m_strLogFile.append(strRH);
         m_strLogFile.append(LOG_EXT);
         break;

      case 7:
         // Time series files to output, convert to lower case filenames
         strRH = strToLower(&strRH);
         if (strRH.find(GIS_ALL_CODE) != string::npos)
         {
            m_bTimeStepTS         =
            m_bAreaWetTS          =
            m_bRainTS             =
            m_bInfiltTS           =
            m_bExfiltTS           =
            m_bRunOnTS            =
            m_bSurfaceWaterTS     =
            m_bSurfaceWaterLostTS =
            m_bFlowDetachTS       =
            m_bSlumpDetachTS      =
            m_bToppleDetachTS     =
            m_bDoSedLoadDepositTS =
            m_bSedOffEdgeTS      =
            m_bSedLoadTS          =
            m_bInfiltDepositTS    =
            m_bSplashRedistTS     =
            m_bSplashKETS         =
            m_bSoilWaterTS        = true;
         }
         else
         {
            if (strRH.find(TIMESTEP_TIME_SERIES_CODE) != string::npos)
            {
               m_bTimeStepTS = true;
               strRH = strRemoveSubstr(&strRH, &TIMESTEP_TIME_SERIES_CODE);
            }

            if (strRH.find(AREA_WET_TIME_SERIES_CODE) != string::npos)
            {
               m_bAreaWetTS = true;
               strRH = strRemoveSubstr(&strRH, &AREA_WET_TIME_SERIES_CODE);
            }

            if (strRH.find(RAIN_TIME_SERIES_CODE) != string::npos)
            {
               m_bRainTS = true;
               strRH = strRemoveSubstr(&strRH, &RAIN_TIME_SERIES_CODE);
            }

            if (strRH.find(INFILT_TIME_SERIES_CODE) != string::npos)
            {
               m_bInfiltTS =
               m_bExfiltTS = true;
               strRH = strRemoveSubstr(&strRH, &INFILT_TIME_SERIES_CODE);
            }

            if (strRH.find(RUNON_TIME_SERIES_CODE) != string::npos)
            {
               m_bRunOnTS = true;
               strRH = strRemoveSubstr(&strRH, &RUNON_TIME_SERIES_CODE);
            }

            if (strRH.find(SURFACE_WATER_TIME_SERIES_CODE) != string::npos)
            {
               m_bSurfaceWaterTS = true;
               strRH = strRemoveSubstr(&strRH, &SURFACE_WATER_TIME_SERIES_CODE);
            }

            if (strRH.find(WATER_LOST_TIME_SERIES_CODE) != string::npos)
            {
               m_bSurfaceWaterLostTS = true;
               strRH = strRemoveSubstr(&strRH, &WATER_LOST_TIME_SERIES_CODE);
            }

            if (strRH.find(FLOW_DETACH_TIME_SERIES_CODE) != string::npos)
            {
               m_bFlowDetachTS = true;
               strRH = strRemoveSubstr(&strRH, &FLOW_DETACH_TIME_SERIES_CODE);
            }

            if (strRH.find(SLUMP_DETACH_TIME_SERIES_CODE) != string::npos)
            {
               m_bSlumpDetachTS = true;
               strRH = strRemoveSubstr(&strRH, &SLUMP_DETACH_TIME_SERIES_CODE);
            }

            if (strRH.find(TOPPLE_DETACH_TIME_SERIES_CODE) != string::npos)
            {
               m_bToppleDetachTS = true;
               strRH = strRemoveSubstr(&strRH, &TOPPLE_DETACH_TIME_SERIES_CODE);
            }

            if (strRH.find(SEDLOAD_DEPOSIT_TIME_SERIES_CODE) != string::npos)
            {
               m_bDoSedLoadDepositTS = true;
               strRH = strRemoveSubstr(&strRH, &SEDLOAD_DEPOSIT_TIME_SERIES_CODE);
            }

            if (strRH.find(SEDLOAD_LOST_TIME_SERIES_CODE) != string::npos)
            {
               m_bSedOffEdgeTS = true;
               strRH = strRemoveSubstr(&strRH, &SEDLOAD_LOST_TIME_SERIES_CODE);
            }

            if (strRH.find(SEDLOAD_TIME_SERIES_CODE) != string::npos)
            {
               m_bSedLoadTS = true;
               strRH = strRemoveSubstr(&strRH, &SEDLOAD_TIME_SERIES_CODE);
            }

            if (strRH.find(INFILT_DEPOSIT_TIME_SERIES_CODE) != string::npos)
            {
               m_bInfiltDepositTS = true;
               strRH = strRemoveSubstr(&strRH, &INFILT_DEPOSIT_TIME_SERIES_CODE);
            }

            if (strRH.find(SPLASH_REDIST_TIME_SERIES_CODE) != string::npos)
            {
               m_bSplashRedistTS = true;
               strRH = strRemoveSubstr(&strRH, &SPLASH_REDIST_TIME_SERIES_CODE);
            }

            if (strRH.find(SPLASH_KE_TIME_SERIES_CODE) != string::npos)
            {
               m_bSplashKETS = true;
               strRH = strRemoveSubstr(&strRH, &SPLASH_KE_TIME_SERIES_CODE);
            }

            if (strRH.find(SOIL_WATER_TIME_SERIES_CODE) != string::npos)
            {
               m_bSoilWaterTS = true;
               strRH = strRemoveSubstr(&strRH, &SOIL_WATER_TIME_SERIES_CODE);
            }

            // Check to see if all codes have been removed
            strRH = strTrimLeft(&strRH);
            if (! strRH.empty())
               strErr = "time-series output file list";
         }
         break;

      case 8:
         // Output splash efficiency check file?
         strRH = strToLower(&strRH);
         if (strRH.find('y') != string::npos)
            m_bSplashCheck = true;
         else if (strRH.find('n') != string::npos)
            m_bSplashCheck = false;
         else
            strErr = "splash efficiency check output switch";
         break;

      case 9:
         // Output depth-friction factor check file?
         strRH = strToLower(&strRH);
         if (strRH.find('y') != string::npos)
            m_bFFCheck = true;
         else if (strRH.find('n') != string::npos)
            m_bFFCheck = false;
         else
            strErr = "friction factor check output switch";
         break;

      // ------------------------------------------------------- Microtopography ------------------------------------------------------
      case 10:
         // Microtopography file
         if (strRH.empty())
         {
            strErr = "microtopography input file";
            break;
         }

         // Check for leading slash, or leading Unix home dir symbol, or occurrence of a drive letter
         if ((strRH[0] == PATH_SEPARATOR) || (strRH[0] == '~') || (strRH.find(':') != string::npos))
         {
            // It has an absolute path, so use it 'as is'
            m_strDEMFile = strRH;
         }
         else
         {
            // It has a relative path, so prepend the RG6 dir
            m_strDEMFile = m_strRGDir;
            m_strDEMFile.append(strRH);
         }
         break;

      case 11:
         // Z (vertical) units, this is used if the input DEM does not specify Z units
         strRH = strToLower(&strRH);
         if (strRH.find("mm") != string::npos)
            m_nZUnits = Z_UNIT_MM;
         else if (strRH.find("cm") != string::npos)
            m_nZUnits = Z_UNIT_CM;
         else if (strRH.find("m") != string::npos)
            m_nZUnits = Z_UNIT_M;
         else
            // Anything else
            m_nZUnits = Z_UNIT_NONE;
         break;

      case 12:
         // Output DEMs using Z units from input DEM?
         strRH = strToLower(&strRH);
         if (strRH.find('y') != string::npos)
            m_bOutDEMsUsingInputZUnits = true;
         else if (strRH.find('n') != string::npos)
            m_bOutDEMsUsingInputZUnits = false;
         else
            strErr = "output Z units using units from input DEM switch";
         break;

      case 13:
         // Which edges are bounded?
         strRH = strToLower(&strRH);
         if (strRH.find('t') != string::npos)
            m_bClosedThisEdge[EDGE_TOP] = true;
         if (strRH.find('r') != string::npos)
            m_bClosedThisEdge[EDGE_RIGHT] = true;
         if (strRH.find('b') != string::npos)
            m_bClosedThisEdge[EDGE_BOTTOM] = true;
         if (strRH.find('l') != string::npos)
            m_bClosedThisEdge[EDGE_LEFT] = true;
         break;

      case 14:
         // Gradient to add
         m_dGradient = stod(strRH);                                           // In per cent
         if (m_dGradient < 0)
            strErr = "slope angle";
         break;

      case 15:
         // Base level e.g. distance below lowest DEM point to flume lip. Negative values mean no baselevel, i.e. can cut down to unerodible basement
         m_dPlotEndDiff = stod(strRH);
         m_bHaveBaseLevel = true;

         if (m_dPlotEndDiff < 0)
         {
            m_bHaveBaseLevel = false;
            m_dPlotEndDiff = 0;
         }
         break;

         // ------------------------------------------------------------- Soil -----------------------------------------------------------
      case 16:
         // Number of layers
         m_nNumSoilLayers = stoi(strRH);
         if (m_nNumSoilLayers < 1)
            strErr = "need at least one soil layer";
         break;

      case 17:
         // Elevation of unerodible basement. This is the same as the elevation of the bottom of the lowest soil layer [Z units]
         m_dBasementElevation = stod(strRH);

         if (m_nZUnits == Z_UNIT_M)
            m_dBasementElevation /= 1e3;
         else if (m_nZUnits == Z_UNIT_CM)
            m_dBasementElevation /= 1e2;

         break;

      case 18:
         // Repeat for number of layers
         for (int nLayer = 0; nLayer < m_nNumSoilLayers; nLayer++)
         {
            int j = 0;
            double
               dTmp,
               dPerCentClay = 0,
               dPerCentSilt = 0,
               dPerCentSand = 0,
               dTotal = 0;

            while (j < 15)
            {
               if ((nLayer != 0) || (j != 0))
               {
                  // Only need to read a line from the file for layers after the first (have already read the line for the first layer)
                  getline(InStream, strRec);
                  nLine++;
               }

               // Trim off leading and trailing whitespace
               strRec = strTrim(&strRec);

               // If this line is a blank line or a comment then ignore it
               if ((strRec.empty()) || (strRec[0] == QUOTE1) || (strRec[0] == QUOTE2))
                  continue;

               nPos = strRec.find(':');
               if (nPos == string::npos)
               {
                  // Error: badly formatted line (no colon)
                  cerr << ERR << "badly formatted line " << nLine << " (no ':') in " << m_strDataPathName << endl << "'" << strRec << "'" << endl;
                  return (false);
               }

               // It is OK, so increment counter
               j++;

               strRH = strSplitRH(&strRec);
               if (strRH.empty())
               {
                  // Error: badly formatted line (nothing after colon)
                  cerr << ERR << "badly formatted line " << nLine << " (nothing after ':') in " << m_strDataPathName << endl << "'" << strRec << "'" << endl;
                  return (false);
               }

               switch (j)
               {
                  case 1:
                     // Layer name
                     m_VstrInputSoilLayerName.push_back(strRH);
                     break;

                  case 2:
                     // Thickness (omit for top layer)
                     m_VdInputSoilLayerThickness.push_back(stod(strRH));
                     break;

                  case 3:
                     // % clay
                     dTmp = stod(strRH);
                     if (dTmp < 0)
                     {
                        strErr = "invalid clay % (";
                        strErr.append(to_string(dTmp));
                        strErr.append(") for soil layer ");
                        strErr.append(to_string(nLayer+1));
                        break;
                     }
                     dPerCentClay = dTmp;
                     break;

                  case 4:
                     // % silt
                     dTmp = stod(strRH);
                     if (dTmp < 0)
                     {
                        strErr = "invalid silt % (";
                        strErr.append(to_string(dTmp));
                        strErr.append(") for soil layer ");
                        strErr.append(to_string(nLayer+1));
                        break;
                     }
                     dPerCentSilt = dTmp;
                     break;

                  case 5:
                     // % sand
                     dTmp = stod(strRH);
                     if (dTmp < 0)
                     {
                        strErr = "invalid sand % (";
                        strErr.append(to_string(dTmp));
                        strErr.append(") for soil layer ");
                        strErr.append(to_string(nLayer+1));
                        break;
                     }
                     dPerCentSand = dTmp;

                     dTotal = dPerCentClay + dPerCentSand + dPerCentSilt;
                     if (dTotal > 100)
                     {
                        strErr = "sum of clay, silt and sand % must not exceed 100 %";
                        break;
                     }
                     else if (dTotal < 100)
                     {
                        // Total is less than 100%
                        if (bFpEQ(dPerCentClay, 0.0, TOLERANCE))
                           dPerCentClay = (100 - (dPerCentSilt + dPerCentSand));
                        else if (bFpEQ(dPerCentSilt, 0.0, TOLERANCE))
                           dPerCentSilt = (100 - (dPerCentClay + dPerCentSand));
                        else if (bFpEQ(dPerCentSand, 0.0, TOLERANCE))
                           dPerCentSand = (100 - (dPerCentClay + dPerCentSilt));
                     }

                     m_VdInputSoilLayerPerCentClay.push_back(dPerCentClay);
                     m_VdInputSoilLayerPerCentSilt.push_back(dPerCentSilt);
                     m_VdInputSoilLayerPerCentSand.push_back(dPerCentSand);
                     break;

                  case 6:
                     // Bulk density (t/m**3 or g/cm**3)
                     dTmp = stod(strRH);
                     if (dTmp <= 0)
                     {
                        strErr = "bulk density must be greater than zero for soil layer ";
                        strErr.append(to_string(nLayer+1));;
                        break;
                     }

                     // Convert to kg/m**3
                     dTmp *= 1000;

                     m_VdInputSoilLayerBulkDensity.push_back(dTmp);
                     break;

                  case 7:
                     // Clay flow erodibility [0-1]
                     dTmp = stod(strRH);
                     if ((dTmp < 0) || (dTmp > 1))
                     {
                        strErr = "invalid clay flow erodibility (";
                        strErr.append(to_string(dTmp));
                        strErr.append(") for soil layer ");
                        strErr.append(to_string(nLayer+1));
                        break;
                     }
                     m_VdInputSoilLayerClayFlowErodibility.push_back(dTmp);
                     break;

                  case 8:
                     // Silt flow erodibility [0-1]
                     dTmp = stod(strRH);
                     if ((dTmp < 0) || (dTmp > 1))
                     {
                        strErr = "invalid silt flow erodibility (";
                        strErr.append(to_string(dTmp));
                        strErr.append(") for soil layer ");
                        strErr.append(to_string(nLayer+1));
                        break;
                     }
                     m_VdInputSoilLayerSiltFlowErodibility.push_back(dTmp);
                     break;

                  case 9:
                     // Sand flow erodibility [0-1]
                     dTmp = stod(strRH);
                     if ((dTmp < 0) || (dTmp > 1))
                     {
                        strErr = "invalid sand flow erodibility (";
                        strErr.append(to_string(dTmp));
                        strErr.append(") for soil layer ");
                        strErr.append(to_string(nLayer+1));
                        break;
                     }
                     m_VdInputSoilLayerSandFlowErodibility.push_back(dTmp);
                     break;

                  case 10:
                     // Clay splash erodibility [0-1]
                     dTmp = stod(strRH);
                     if ((dTmp < 0) || (dTmp > 1))
                     {
                        strErr = "invalid clay splash erodibility (";
                        strErr.append(to_string(dTmp));
                        strErr.append(") for soil layer ");
                        strErr.append(to_string(nLayer+1));
                        break;
                     }
                     m_VdInputSoilLayerClaySplashErodibility.push_back(dTmp);
                     break;

                  case 11:
                     // Silt splash erodibility [0-1]
                     dTmp = stod(strRH);
                     if ((dTmp < 0) || (dTmp > 1))
                     {
                        strErr = "invalid silt splash erodibility (";
                        strErr.append(to_string(dTmp));
                        strErr.append(") for soil layer ");
                        strErr.append(to_string(nLayer+1));
                        break;
                     }
                     m_VdInputSoilLayerSiltSplashErodibility.push_back(dTmp);
                     break;

                  case 12:
                     // Sand splash erodibility [0-1]
                     dTmp = stod(strRH);
                     if ((dTmp < 0) || (dTmp > 1))
                     {
                        strErr = "invalid sand splash erodibility (";
                        strErr.append(to_string(dTmp));
                        strErr.append(") for soil layer ");
                        strErr.append(to_string(nLayer+1));
                        break;
                     }
                     m_VdInputSoilLayerSandSplashErodibility.push_back(dTmp);
                     break;

                  case 13:
                     // Clay slump erodibility [0-1]
                     dTmp = stod(strRH);
                     if ((dTmp < 0) || (dTmp > 1))
                     {
                        strErr = "invalid clay slump erodibility (";
                        strErr.append(to_string(dTmp));
                        strErr.append(") for soil layer ");
                        strErr.append(to_string(nLayer+1));
                        break;
                     }
                     m_VdInputSoilLayerClaySlumpErodibility.push_back(dTmp);
                     break;

                  case 14:
                     // Silt slump erodibility [0-1]
                     dTmp = stod(strRH);
                     if ((dTmp < 0) || (dTmp > 1))
                     {
                        strErr = "invalid silt slump erodibility (";
                        strErr.append(to_string(dTmp));
                        strErr.append(") for soil layer ");
                        strErr.append(to_string(nLayer+1));
                        break;
                     }
                     m_VdInputSoilLayerSiltSlumpErodibility.push_back(dTmp);
                     break;

                  case 15:
                     // Sand slump erodibility [0-1]
                     dTmp = stod(strRH);
                     if ((dTmp < 0) || (dTmp > 1))
                     {
                        strErr = "invalid sand slump erodibility (";
                        strErr.append(to_string(dTmp));
                        strErr.append(") for soil layer ");
                        strErr.append(to_string(nLayer+1));
                        break;
                     }
                     m_VdInputSoilLayerSandSlumpErodibility.push_back(dTmp);
                     break;
               }

               if (! strErr.empty())
                  break;
            }
         }
         break;

         // ---------------------------------------------------------------- Rainfall ----------------------------------------------------
      case 19:
         // Mean rainfall intensity or rainfall intensity time-series file
         if (! strRH.empty())
         {
            // First check if it is a floating-point number
            m_dRainIntensity = stod(strRH);                                   // in mm/hour
            m_dSpecifiedRainIntensity = m_dRainIntensity;

            if (m_dRainIntensity <= 0)
            {
               m_bTimeVaryingRain = true;
               m_nRainChangeTimeMax = 0;

               // Not a viable floating-point-number, so see if it is a filename string; first check for a leading slash, or leading Unix home dir symbol, or occurrence of a drive letter
               if ((strRH[0] == PATH_SEPARATOR) || (strRH[0] == '~') || (strRH.find(':') != string::npos))
               {
                  // It has an absolute path, so use it 'as is'
                  m_strRainTSFile = strRH;
               }
               else
               {
                  // It has a relative path, so prepend the RG6 dir
                  m_strRainTSFile = m_strRGDir;
                  m_strRainTSFile.append(strRH);
               }
            }
         }
         else
            strErr = "rainfall intensity";
         break;

      case 20:
         // Standard deviation of rainfall intensity
         m_dStdRainInt = stod(strRH);                                                  // in mm/hour
         if (m_dStdRainInt < 0)
            strErr = "standard deviation of rainfall intensity";
         break;

      case 21:
         // Mean raindrop diameter
         m_dDropDiameter = stod(strRH);                                                // in mm
         if (m_dDropDiameter <= 0)
            strErr = "mean raindrop diameter";
         else
            m_dMeanCellWaterVol = 4 * PI * pow(m_dDropDiameter * 0.5, 3) / 3;         // in mm**3
         break;

      case 22:
         // Standard deviation of raindrop diameter
         m_dStdDropDiameter = stod(strRH);                                             // in mm
         if (m_dStdDropDiameter < 0)
            strErr = "standard deviation of raindrop diameter";
         else
            m_dStdCellWaterVol = 4 * PI * pow(m_dStdDropDiameter * 0.5, 3) / 3;       // in mm**3
         break;

      case 23:
         // Spatial mask of rainfall variation on soil area
         if (! strRH.empty())
         {
            // Now check for leading slash, or leading Unix home dir symbol, or occurrence of a drive letter
            if ((strRH[0] == PATH_SEPARATOR) || (strRH[0] == '~') || (strRH.find(':') != string::npos))
            {
               // It has an absolute path, so use it 'as is'
               m_strRainVarMFile = strRH;
            }
            else
            {
               // It has a relative path, so prepend the RG6 dir
               m_strRainVarMFile = m_strRGDir;
               m_strRainVarMFile.append(strRH);
            }
         }
         break;

      case 24:
         // Raindrop fall velocity at soil surface
         m_dRainSpeed = stod(strRH);                                                   // in m/sec
         if (m_bSplash)
         {
            if (m_dRainSpeed <= 0)
               strErr = "raindrop fall velocity";
         }
         break;

      // --------------------------------------------------------- Splash redistribution ----------------------------------------------
      case 25:
         // Simulate splash redistribution?
         strRH = strToLower(&strRH);
         if (strRH.find('y') != string::npos)
            m_bSplash = true;
         else if (strRH.find('n') != string::npos)
            m_bSplash = false;
         else
            strErr = "splash redistribution switch";
         break;

      case 26:
         // Water depth-splash attenuation data filename
         if (m_bSplash)
         {
            if (strRH.empty())
               strErr = "path and name of splash-attenuation datafile";
            else
            {
               // Now check for leading slash, or leading Unix home dir symbol, or occurrence of a drive letter
               if ((strRH[0] == PATH_SEPARATOR) || (strRH[0] == '~') || (strRH.find(':') != string::npos))
               {
                  // It has an absolute path, so use it 'as is'
                  m_strSplashAttenuationFile = strRH;
               }
               else
               {
                  // It has a relative path, so prepend the RG6 dir
                  m_strSplashAttenuationFile = m_strRGDir;
                  m_strSplashAttenuationFile.append(strRH);
               }
            }
         }
         break;

      case 27:
         if (m_bSplash)
         {
            // Constant in splash equation (sec**2/mm)
            m_dSplashConstant = stod(strRH);
            if (m_dSplashConstant <= 0)
               strErr = "splash constant";
         }
         break;

      case 28:
         if (m_bSplash)
         {
            // Poesen (p) or Planchon et al. (o) splash equation?
            strRH = strToLower(&strRH);

            if (strRH.find('p') != string::npos)
               m_bPoesenSplashEqn = true;
            else if (strRH.find('o') != string::npos)
               m_bPlanchonSplashEqn = true;
            else
               strErr = "must choose either Poesen or Planchon splash equation";
         }
         break;

      case 29:
         if (m_bSplash && m_bPoesenSplashEqn)
         {
            // Constant in Poesen splash equation
            m_dPoesenSplashConstant = stod(strRH);
            if (bFpEQ(m_dPoesenSplashConstant, 0.0, TOLERANCE))
               strErr = "constant for Poesen splash requation";
         }
         break;

      // ---------------------------------------------------- Run-on and run-off ------------------------------------------------------
      case 30:
         // Run-on from outside the grid?
         strRH = strToLower(&strRH);
         if (strRH.find('y') != string::npos)
            m_bRunOn = true;
         else if (strRH.find('n') != string::npos)
            m_bRunOn = false;
         else
            strErr = "run-on switch";
         break;

      case 31:
         // Run-on from which edges?
         if (m_bRunOn)
         {
            strRH = strToLower(&strRH);
            if (strRH.find('t') != string::npos)
            {
               m_bRunOnThisEdge[EDGE_TOP]  =
               m_bClosedThisEdge[EDGE_TOP] = true;
            }
            if (strRH.find('r') != string::npos)
            {
               m_bRunOnThisEdge[EDGE_RIGHT]  =
               m_bClosedThisEdge[EDGE_RIGHT] = true;
            }
            if (strRH.find('b') != string::npos)
            {
               m_bRunOnThisEdge[EDGE_BOTTOM]  =
               m_bClosedThisEdge[EDGE_BOTTOM] = true;
            }
            if (strRH.find('l') != string::npos)
            {
               m_bRunOnThisEdge[EDGE_LEFT]  =
               m_bClosedThisEdge[EDGE_LEFT] = true;
            }
         }
         break;

      case 32:
         // Length of run-on area
         if (m_bRunOn)
         {
            m_dRunOnLen = stod(strRH);
            if (m_dRunOnLen <= 0)
                  strErr = "run-on length";
         }
         break;

      case 33:
         // Rainfall spatial variation multiplier for run-on area
         if (m_bRunOn)
         {
            m_dRunOnRainVarM = stod(strRH);
            if (m_dRunOnRainVarM <= 0)
               strErr = "rainfall spatial variation multiplier for run-on area";
         }
         break;

      case 34:
         // Flow speed on run-on area
         if (m_bRunOn)
         {
            m_dRunOnSpd = stod(strRH);
            if (m_dRunOnSpd <= 0)
               strErr = "run-on flow speed";
         }
         break;

      case 35:
         // Off-edge param A
         m_dOffEdgeParamA = stod(strRH);
         if (m_dOffEdgeParamA <= 0)
            strErr = "off-edge parameter A";
         break;

      case 36:
         // Off-edge param B
         m_dOffEdgeParamB = stod(strRH);
         if (bFpEQ(m_dOffEdgeParamB, 0.0, TOLERANCE))
            strErr = "off-edge parameter B";
         break;

      // ----------------------------------------------------------- Infiltration -----------------------------------------------------
      case 37:
         // Simulate infilt?
         strRH = strToLower(&strRH);
         if (strRH.find('y') != string::npos)
            m_bDoInfiltration = true;
         else if (strRH.find('n') != string::npos)
            m_bDoInfiltration = false;
         else
            strErr = "infilt switch";
         break;

      case 38:
         // Repeat for number of layers
         for (int nLayer = 0; nLayer < m_nNumSoilLayers; nLayer++)
         {
            int j = 0;
            double dTmp;

            while (j < 5)
            {
               if ((nLayer != 0) || (j != 0))
               {
                  // Only need to read a line from the file for layers after the first (have already read the line for the first layer)
                  getline(InStream, strRec);
                  nLine++;
               }

               // Trim off leading and trailing whitespace
               strRec = strTrim(&strRec);

               // If this line is a blank line or a comment then ignore it
               if ((strRec.empty()) || (strRec[0] == QUOTE1) || (strRec[0] == QUOTE2))
                  continue;

               if (m_bDoInfiltration)
               {
                  nPos = strRec.find(':');
                  if (nPos == string::npos)
                  {
                     // Error: badly formatted line (no colon)
                     cerr << ERR << "badly formatted line " << nLine << " (no ':') in " << m_strDataPathName << endl << "'" << strRec << "'" << endl;
                     return (false);
                  }
               }

               // It isn't so increment counter
               j++;

               if (m_bDoInfiltration)
               {
                  strRH = strSplitRH(&strRec);
                  if (strRH.empty())
                  {
                     // Error: badly formatted line (no colon or nothing after colon)
                     cerr << ERR << "badly formatted line " << nLine << " (nothing after ':') in " << m_strDataPathName << endl << "'" << strRec << "'" << endl;
                     return (false);
                  }
               }

               switch (j)
               {
                  case 1:
                     // Air entry head, is half of bubbling pressure (cm)
                     if (m_bDoInfiltration)
                     {
                        dTmp = stod(strRH);
                        if (bFpEQ(dTmp, 0.0, TOLERANCE))
                        {
                           strErr = "air entry head";
                           break;
                        }

                        m_VdInputSoilLayerInfiltAirHead.push_back(dTmp);
                     }
                     break;

                  case 2:
                     // Exponent of Brooks-Corey water retention equation
                     if (m_bDoInfiltration)
                     {
                        dTmp = stod(strRH);
                        if (dTmp <= 0)
                        {
                           strErr = "exponent of Brooks-Corey water retention equation";
                           break;
                        }

                        m_VdInputSoilLayerInfiltLambda.push_back(dTmp);
                     }
                     break;

                  case 3:
                     // Saturated volumetric water content (cm3/cm3 or mm/mm3)
                     if (m_bDoInfiltration)
                     {
                        dTmp = stod(strRH);
                        if (dTmp < 0)
                        {
                           strErr = "saturated volumetric water content";
                           break;
                        }

                        m_VdInputSoilLayerInfiltSatWater.push_back(dTmp);
                     }
                     break;

                  case 4:
                     // Initial volumetric water content (cm3/cm3 or mm3/mm3)
                     if (m_bDoInfiltration)
                     {
                        dTmp = stod(strRH);
                        if ((dTmp < 0) || (dTmp > m_VdInputSoilLayerInfiltSatWater.back()))
                        {
                           strErr = "initial volumetric water content";
                           break;
                        }

                        m_VdInputSoilLayerInfiltInitWater.push_back(dTmp);
                     }
                     break;

                  case 5:
                     // Saturated hydraulic conductivity (cm/h)
                     if (m_bDoInfiltration)
                     {
                        dTmp = stod(strRH);
                        if (dTmp < 0)
                        {
                           strErr = "saturated hydraulic conductivity";
                           break;
                        }

                        m_VdInputSoilLayerInfiltKSat.push_back(dTmp);
                     }
                     break;
               }

               if (! strErr.empty())
                  break;
            }
         }

         break;

      // ----------------------------------------------------------- Overland Flow ----------------------------------------------------
      case 39:
         // Manning-type (m) or Darcy-Weisbach (d) flow velocity equation?
         strRH = strToLower(&strRH);

         if (strRH.find('m') != string::npos)
            m_bManningFlowSpeedEqn = true;
         else if (strRH.find('d') != string::npos)
            m_bDarcyWeisbachFlowSpeedEqn = true;
         else
            strErr = "must choose either Manning or Darcy-Weisbach equation";
         break;

      case 40:
         // Parameter A if using Manning-type flow velocity equation
         if (m_bManningFlowSpeedEqn)
         {
            m_dManningParamA = stod(strRH);

            if (m_dManningParamA <= 0)
               strErr = "Manning parameter A must be > 0";
         }
         break;

      case 41:
         // Parameter B if using Manning-type flow velocity equation
         if (m_bManningFlowSpeedEqn)
         {
            m_dManningParamB = stod(strRH);

            if (m_dManningParamB <= 0)
               strErr = "Manning parameter B must be > 0";
         }
         break;

      case 42:
         // If using Darcy-Weisbach flow velocity equation, then friction factor is a constant (c), is based on Reynolds' number (r), or is calculated using the Lawrence (1997) formulation (l)
         if (m_bDarcyWeisbachFlowSpeedEqn)
         {
            strRH = strToLower(&strRH);

            if (strRH.find('k') != string::npos)
               m_bFrictionFactorConstant = true;
            else if (strRH.find('r') != string::npos)
               m_bFrictionFactorReynolds = true;
            else if (strRH.find('l') != string::npos)
            {
               m_bFrictionFactorLawrence = true;
               m_bInundationSave         = true;
            }
            else if (strRH.find('c') != string::npos)
               m_bFrictionFactorCheng = true;
            else
               strErr = "Must choose one way of calculating the Darcy-Weisbach friction factor";
         }
         break;

      case 43:
         // Constant friction factor for Darcy-Weisbach equation, if using constant friction factor approach
         if (m_bDarcyWeisbachFlowSpeedEqn && m_bFrictionFactorConstant)
         {
            m_dFFConstant = stod(strRH);

            if (m_dFFConstant <= 0)
               strErr = "Constant friction factor for Darcy-Weisbach equation must be > 0";
         }
         break;

      case 44:
         // Parameter A if using Reynolds' number-based friction factor
         if (m_bDarcyWeisbachFlowSpeedEqn && m_bFrictionFactorReynolds)
         {
            m_dFFReynoldsParamA = stod(strRH);

            if (m_dFFReynoldsParamA <= 0)
               strErr = "Parameter A for Reynolds' number-based friction factor for Darcy-Weisbach equation must be > 0";
         }
         break;

      case 45:
         // Parameter B if using Reynolds' number-based friction factor
         if (m_bDarcyWeisbachFlowSpeedEqn && m_bFrictionFactorReynolds)
         {
            m_dFFReynoldsParamB = stod(strRH);

            if (bFpEQ(m_dFFReynoldsParamB, 0.0, TOLERANCE))
               strErr = "Parameter B for Reynolds' number-based friction factor for Darcy-Weisbach equation cannot be 0";
         }
         break;

      case 46:
         // If using the Lawrence (1997) friction factor approach: D50 of roughness elements (mm) in the partially-inundated flow regime
         if (m_bDarcyWeisbachFlowSpeedEqn && m_bFrictionFactorLawrence)
         {
            m_dFFLawrenceD50 = stod(strRH);

            if (m_dFFLawrenceD50 <= 0)
               strErr = "D50 of roughness elements for Lawrence (1997) friction factor for Darcy-Weisbach equation must be > 0";
            else
               m_dFFLawrenceEpsilon = 0.5 * m_dFFLawrenceD50;
         }
         break;

      case 47:
         // If using the Lawrence (1997) friction factor approach: % of surface covered with roughness elements in the partially-inundated flow regime
         if (m_bDarcyWeisbachFlowSpeedEqn && m_bFrictionFactorLawrence)
         {
            m_dFFLawrencePr = stod(strRH);

            if ((m_dFFLawrencePr < 0) || (m_dFFLawrencePr > 100))
               strErr = "% of surface covered with roughness elements for Lawrence (1997) friction factor for Darcy-Weisbach equation must be between 0 and 100";
         }
         break;

      case 48:
         // If using the Lawrence (1997) friction factor approach: the ratio between the drag of roughness elements and the ideal situation in the partially-inundated flow regime
         if (m_bDarcyWeisbachFlowSpeedEqn && m_bFrictionFactorLawrence)
         {
            m_dFFLawrenceCd = stod(strRH);

            if ((m_dFFLawrenceCd < 0) || (m_dFFLawrenceCd > 1))
               strErr = "ratio between the drag of roughness elements and the ideal situation for Lawrence (1997) friction factor for Darcy-Weisbach equation must be between 0 and 1";
         }
         break;

      case 49:
         // If using the Cheng friction factor, effective roughness height (mm)
         if (m_bDarcyWeisbachFlowSpeedEqn && m_bFrictionFactorCheng)
         {
            m_dChengRoughnessHeight = stod(strRH);
            if (m_dChengRoughnessHeight <= 0)
               strErr = "Cheng effective roughness height must be greater than zero";
         }
         break;

      case 50:
         // Maximum flow speed (mm/sec)
         m_dMaxFlowSpeed = stod(strRH);
         if (m_dMaxFlowSpeed <= 0)
            strErr = "maximum flow speed must be greater than zero";
         break;

         // ------------------------------------------------------------ Flow Erosion- ---------------------------------------------------
      case 51:
         // Simulate flow erosion? Note that if flow erosion is not considered, then tranport capacity is not considered either, which means that deposition is ignored. This is obviousy unrealistic if we are considering another form of erosion such as splash or slumping. So this switch is really only useful for debugging
         strRH = strToLower(&strRH);

         if (strRH.find('y') != string::npos)
            m_bFlowErosion = true;
         else if (strRH.find('n') != string::npos)
            m_bFlowErosion = false;
         else
            strErr = "flow erosion switch";
         break;

      case 52:
         // K in detachment equation
         m_dK = stod(strRH);
         if (m_bFlowErosion && (m_dK <= 0))
            strErr = "constant k for detachment";
         break;

      case 53:
         // T in detachment equation
         m_dT = stod(strRH);
         if (m_bFlowErosion && (m_dT <= 0))
            strErr = "constant T for detachment";
         break;

      case 54:
         // CV of T in detachment equation
         m_dCVT = stod(strRH);
         if (m_bFlowErosion)
         {
            if (m_dCVT <= 0)
               strErr = "CV of T for detachment";
         }
         break;

      case 55:
         // CV of tau-b in detachment equation
         m_dCVTaub = stod(strRH);
         if (m_bFlowErosion && (m_dCVTaub <= 0))
            strErr = "CV of tau-b for detachment";
         break;

      // -------------------------------------------------------- Transport Capacity --------------------------------------------------
      case 56:
         // Alpha in transport capacity equation
         m_dAlpha = stod(strRH);
         if (m_bFlowErosion && (m_dAlpha >= 0))
            strErr = "alpha for transport capacity";
         break;

      case 57:
         // Beta in transport capacity equation
         m_dBeta = stod(strRH);
         if (m_bFlowErosion && (m_dBeta <= 0))
            strErr = "beta for transport capacity";
         break;

      case 58:
         // Gamma in transport capacity equation
         m_dGamma = stod(strRH);
         if (m_bFlowErosion && (m_dGamma <= 0))
            strErr = "gamma for transport capacity";
         break;

      case 59:
         // Delta in transport capacity equation
         m_dDelta = stod(strRH);
         if (m_bFlowErosion && (m_dDelta <= 0))
            strErr = "delta for transport capacity";
         break;

      // ----------------------------------------------------------- Deposition -------------------------------------------------------
      // TODO Move this section, needs to be after slumping since still get deposition if only process operation is slumping
      case 60:
         // Cheng (C), Stokes-Budryck-Rittinger (S), or Ferguson-Church (F) deposition equation? [csf]
         strRH = strToLower(&strRH);
         if (strRH.find('c') != string::npos)
            m_bSettlingEqnCheng = true;
         else if (strRH.find('s') != string::npos)
           m_bSettlingEqnStokesBudryckRittinger = true;
         else if (strRH.find('f') != string::npos)
           m_bSettlingEqnFergusonChurch = true;
         else
            strErr = "Must choose a deposition equation";
         break;

      case 61:
         // Grain density (kg/m**3)
         m_dDepositionGrainDensity = stod(strRH);
         if ((m_bFlowErosion || m_bSplash) && (m_dDepositionGrainDensity <= 0))
            strErr = "sediment grain density, for deposition";
         break;

      case 62:
         // Clay minimum size (mm)
         m_dClaySizeMin = stod(strRH);
         if ((m_bFlowErosion || m_bSplash) && (m_dClaySizeMin < 0))
            strErr = "clay minimum size, for deposition";
         break;

      case 63:
         // Clay-silt threshold size (mm)
         m_dClaySiltBoundarySize = stod(strRH);
         if ((m_bFlowErosion || m_bSplash) && (m_dClaySiltBoundarySize <= m_dClaySizeMin))
            strErr = "clay-silt threshold size, for deposition";
         break;

      case 64:
         // Silt-Sand threshold size (mm)
         m_dSiltSandBoundarySize = stod(strRH);
         if ((m_bFlowErosion || m_bSplash) && (m_dSiltSandBoundarySize <= m_dClaySiltBoundarySize))
            strErr = "silt-sand threshold size, for deposition";
         break;

      case 65:
         // Sand maximum size (mm)
         m_dSandSizeMax = stod(strRH);
         if ((m_bFlowErosion || m_bSplash) && (m_dSandSizeMax <= m_dSiltSandBoundarySize))
            strErr = "sand maximum size, for deposition";
         break;

      // -------------------------------------------------------------- Slumping ------------------------------------------------------
      case 66:
         // Simulate slumping?
         strRH = strToLower(&strRH);
         if (strRH.find('y') != string::npos)
            m_bSlumping = true;
         else if (strRH.find('n') != string::npos)
            m_bSlumping = false;
         else
            strErr = "slumping switch";
         break;

      case 67:
         // Radius of soil shear stress 'patch'
         m_dSSSPatchSize = stod(strRH);                    // mm
         if (m_bSlumping && (m_dSSSPatchSize <= 0))
            strErr = "radius of soil shear stress 'patch', for slumping";
         break;

      case 68:
         // Threshold shear stress for slumping
         m_dCritSSSForSlump = stod(strRH);                    // kg/m s**2 (Pa)
         if (m_bSlumping && (m_dCritSSSForSlump < 0))
            strErr = "threshold shear stress for slumping";
         break;

      case 69:
         // Angle of rest for saturated slumped soil
         m_dSlumpAngleOfRest = stod(strRH);                        // in per cent
         if (m_bSlumping && (m_dSlumpAngleOfRest < 0))
            strErr = "angle of rest for slumped soil";
         break;

      case 70:
         // Critical angle for toppling soil (not saturated)
         m_dToppleCriticalAngle = stod(strRH);                     // in per cent
         if (m_bSlumping)
         {
            if (m_dToppleCriticalAngle < 0)
               strErr = "critical angle for toppling";
         }
         break;

      case 71:
         // Angle of rest for toppled soil (not saturated)
         m_dToppleAngleOfRest = stod(strRH);                       // in per cent
         if (m_bSlumping)
         {
            if (bFpEQ(m_dToppleAngleOfRest, 0.0, TOLERANCE))
               m_dToppleAngleOfRest = m_dSlumpAngleOfRest;

            if ((m_dToppleAngleOfRest >= m_dToppleCriticalAngle) || (m_dToppleAngleOfRest < 0))
               strErr = "angle of rest for toppled soil";
         }
         break;

         // ---------------------------------------------------------- Headcut Retreat ---------------------------------------------------
         // TODO can't have headcut retreat if no flow erosion
      case 72:
         // Simulate headcut retreat?
         strRH = strToLower(&strRH);
         if (strRH.find('y') != string::npos)
            m_bHeadcutRetreat = true;
         else if (strRH.find('n') != string::npos)
            m_bHeadcutRetreat = false;
         else
            strErr = "headcut retreat switch";
         break;

      case 73:
         // Headcut retreat constant
         m_dHeadcutRetreatConst = stod(strRH);
         if (m_dHeadcutRetreatConst <= 0)
            strErr = "headcut retreat constant";
         break;

         // --------------------------------------------------- Various Physical Constants -----------------------------------------------
      case 74:
         // Density of water
         m_dRho = stod(strRH);                                    // kg/m**3
         if (m_dRho <= 0)
            strErr = "density of water";
         break;

      case 75:
         // Viscosity of water
         m_dNu = stod(strRH);                                     // m**2/sec
         if (m_dNu <= 0)
            strErr = "viscosity of water";
         break;

      case 76:
         // Gravitational acceleration
         m_dG = stod(strRH);                                      // m/sec**2
         if (m_dG <= 0)
            strErr = "gravitational acceleration";
         break;
      }

      // Did an error occur?
      if (! strErr.empty())
      {
         // Error in input to run details file
         cerr << ERR << "reading " << strErr << " on line " << nLine << " of " << m_strDataPathName << endl << "'" << strRec << "'" << endl;
         InStream.close();
         return false;
      }
   }

   // Close the input file
   InStream.close();

   // Make some changes now everything has been read: first, don't output runon files if no runon
   if (! m_bRunOn)
      m_bCumulRunOnSave = false;

   // Don't output splash or KE GIS/TS files if no splash
   if (! m_bSplash)
   {
      m_bSplashSave      =
      m_bCumulSplashSave =
      m_bSplashRedistTS  =
      m_bSplashKETS      = false;
   }

   // Don't output runon TS file if no runon
   if (! m_bRunOn)
      m_bRunOnTS = false;

   // Don't output infilt GIS/TS files if no infilt
   if (! m_bDoInfiltration)
   {
      m_bInfiltSave              =
      m_bCumulInfiltSave         =
      m_bSoilWaterSave           =
      m_bInfiltDepositSave       =
      m_bCumulInfiltDepositSave  =
      m_bInfiltTS                =
      m_bExfiltTS                =
      m_bInfiltDepositTS         =
      m_bSoilWaterTS             = false;
   }

   // Don't output slumping or toppling GIS/TS files if no slumping
   if (! m_bSlumping)
   {
      m_bSlumpSave      =
      m_bToppleSave     =
      m_bSlumpDetachTS  =
      m_bToppleDetachTS = false;
   }

   // Don't output flow erosion GIS/TS files if no flow erosion
   if (! m_bFlowErosion)
   {
      m_bFlowDetachTS       =
      m_bDoSedLoadDepositTS =
      m_bSedOffEdgeTS      =
      m_bSedLoadTS          = false;
   }

   return (true);
}

//=========================================================================================================================================
//!  Opens the log file
//=========================================================================================================================================
bool CSimulation::bOpenLogFile(void)
{
   // Open in binary mode if just checking random numbers
#if defined RANDCHECK
   m_ofsLog.open(m_strLogFile, ios::out | ios::binary | ios::trunc);
#else
   m_ofsLog.open(m_strLogFile, ios::out | ios::trunc);
#endif

   if (! m_ofsLog)
   {
      // Error, cannot open log file
      cerr << ERR << "cannot open " << m_strLogFile << " for output" << endl;
      return (false);
   }

   // Set default Log output format
   m_ofsLog << std::scientific;

   return (true);
}

//=========================================================================================================================================
//! This member function of CSimulation reads the rainfall intensity time-series file
//=========================================================================================================================================
bool CSimulation::bReadRainfallTimeSeries(void)
{
   // Put together file name
   string strFilePathName = m_strRainTSFile;

   // Create ifstream object
   ifstream InStream;

   // Try to open rainfall time series file for input
   InStream.open(strFilePathName, ios::in);
   if (! InStream.is_open())
   {
      // Error: cannot open rainfall time series file for input
      cerr << ERR << "cannot open " << strFilePathName << " for input" << endl;
      return (false);
   }

   string strRec, strErr;
   int i = -2, nMultiplier = 1, nLine = 0;

   while (getline(InStream, strRec))
   {
      nLine++;

      // Convert to lowercase and trim off leading and trailing whitespace
      strRec = strToLower(&strRec);
      strRec = strTrim(&strRec);

      // If it is not a blank line, but it is a comment line or a blank line, then ignore it
      if ((! strRec.empty()) && (strRec[0] != QUOTE1) && (strRec[0] != QUOTE2))
      {
         size_t nPos = strRec.find(':');
         if (nPos == string::npos)
         {
            // Error: badly formatted line (no colon)
            strErr = "badly formatted line " + to_string(nLine) + " (no ':') in " + strFilePathName + "\n'" + strRec + "'\n";
            break;
         }

         // It is OK, so increment item counter
         i++;

         // Strip off leading portion (the bit up to and including the colon)
         string strRH = strSplitRH(&strRec);
         if (strRH.empty())
         {
            // Error: badly formatted line (nothing after colon)
            strErr = "badly formatted line " + to_string(nLine) + " (nothing after ':') in " + strFilePathName + "\n'" + strRec + "'\n";
            break;
         }

         // Remove leading whitespace after the colon
         strRH = strTrimLeft(&strRH);

         // Look for trailing comments, if found then terminate string at that point and trim off any trailing whitespace
         bool bFound = true;
         while (bFound)
         {
            bFound = false;

            nPos = strRH.rfind(QUOTE1);
            if (nPos != string::npos)
            {
               strRH.resize(nPos);
               bFound = true;
            }

            nPos = strRH.rfind(QUOTE2);
            if (nPos != string::npos)
            {
               strRH.resize(nPos);
               bFound = true;
            }

            // Trim trailing spaces
            strRH = strTrimRight(&strRH);
         }

         if (i == -1)
         {
            // Rainfall change times in minutes or seconds?
            strRH = strToLower(&strRH);
            if (strRH.find('m') != string::npos)
               nMultiplier = 60;          // time in mins
            else if (strRH.find('s') != string::npos)
               nMultiplier = 1;           // time in secs
            else
            {
               strErr = "units for rainfall change times: ";
               strErr.append(strRH);
               break;
            }
         }

         else
         {
            // Should be a time/intensity pair, so split by space
            vector<string> VstrItems = VstrSplit(&strRH, SPACE);
            if (VstrItems.size() < 2)
            {
               strErr = "rainfall change time/intensity ";
               strErr.append(to_string(i+1));
               strErr.append(" is invalid: ");
               strErr.append(strRH);
               break;
            }

            // There are two items, so read in rainfall change time (convert to seconds, if necessary)
            double dRainChangeTime = stod(VstrItems[0]) * nMultiplier;

            // Check for obvious error
            if ((dRainChangeTime <= 0) || (dRainChangeTime > m_dSimulatedRainDuration))
            {
               strErr = "rainfall change time must be greater than zero: ";
               strErr.append(strRH);
               break;
            }

            // Also check that time is later than last one
            if (i > 0)
            {
               if (dRainChangeTime <= m_VdRainChangeTime[i-1])
               {
                  strErr = "rainfall change times ";
                  strErr.append(to_string(i));
                  strErr.append(" & ");
                  strErr.append(to_string(i-1));
                  strErr.append(" are out of sequence");
                  break;
               }
            }

            // Now read in rainfall intensity
            double dRainChangeIntensity = stod(VstrItems[1]);

            // Check for obvious error
            if (dRainChangeIntensity < 0)
            {
               strErr = "rainfall change intensity ";
               strErr.append(to_string(i+1));
               strErr.append(" is invalid: ");
               strErr.append(strRH);
               break;
            }

            // All OK, so store
            m_VdRainChangeTime.push_back(dRainChangeTime);
            m_VdRainChangeIntensity.push_back(dRainChangeIntensity);
         }

      }
   }

   // Did an error occur?
   if (! strErr.empty())
   {
      // error in input to run details file
      cerr << ERR << " on line " << nLine << " of " << strFilePathName << endl << "'" << strErr << "'" << endl;
      InStream.close();
      return (false);
   }

   // All OK, so store number of time/intensity pairs
   m_nRainChangeTimeMax = i;

   // close and return
   InStream.close();
   return (true);
}

//=========================================================================================================================================
//! Returns the after-colon part of a line read from the text input file
//=========================================================================================================================================
string CSimulation::strSplitRH(string const* pstrRec)
{
   size_t nPos = 0;
   string strRH;

   // Find the colon: note that lines MUST have a colon separating data from leading description portion
   nPos = pstrRec->find(':');
   if (nPos == string::npos)
      // Error: badly formatted line (no colon)
      return "";

   // Strip off leading portion (the bit up to and including the colon)
   strRH = pstrRec->substr(nPos+1);

   // Remove leading whitespace after the colon
   strRH = strTrimLeft(&strRH);

   if (strRH.empty())
      // There was nothing after the colon
      return "";

   // OK so far, so look for trailing comments, if found then terminate string at that point and trim off any trailing whitespace
   bool bFound = true;
   while (bFound)
   {
      bFound = false;

      nPos = strRH.rfind(QUOTE1);
      if (nPos != string::npos)
      {
         strRH.resize(nPos);
         bFound = true;
      }

      nPos = strRH.rfind(QUOTE2);
      if (nPos != string::npos)
      {
         strRH.resize(nPos);
         bFound = true;
      }

      // Trim trailing spaces
      strRH = strTrimRight(&strRH);
   }

   return strRH;
}
