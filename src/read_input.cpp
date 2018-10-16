/*========================================================================================================================================

 This is read_input.cpp: it reads the non-GIS input files for RillGrow

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


/*========================================================================================================================================

 The bReadIni member function reads the initialization file

========================================================================================================================================*/
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

   int i = 0;
   string strRec, strErr;
   
   while (getline(InStream, strRec))
   {
      // Trim off leading and trailing whitespace
      strRec = strTrim(&strRec);
      
      // If it is a blank line or a comment then ignore it
      if ((strRec.empty()) || (strRec[0] == QUOTE1) || (strRec[0] == QUOTE2))
         continue;
      
      // It isn't so increment counter
      i++;
      
      // Find the colon: note that lines MUST have a colon separating data from leading description portion
      size_t nPos = strRec.find(':');
      if (nPos == string::npos)
      {
         // Error: badly formatted line (no colon)
         cerr << ERR << "badly formatted line (no ':') in " << strFilePathName << endl << "'" << strRec << "'" << endl;
         return false;
      }
      
      if (nPos == strRec.size()-1)
      {
         // Error: badly formatted line (colon with nothing following)
         cerr << ERR << "badly formatted line (nothing following ':') in " << strFilePathName << endl << "'" << strRec << "'" << endl;
         return false;
      }
      
      // Strip off leading portion (the bit up to and including the colon)
      string strRH = strRec.substr(nPos+1);
      
      // Remove leading whitespace
      strRH = strTrimLeft(&strRH);
      
      // Look for a trailing comment, if found then terminate string at that point and trim off any trailing whitespace
      nPos = strRH.rfind(QUOTE1);
      if (nPos != string::npos)
         strRH = strRH.substr(0, nPos+1);
      
      nPos = strRH.rfind(QUOTE2);
      if (nPos != string::npos)
         strRH = strRH.substr(0, nPos+1);
      
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
            strRH.append(&PATH_SEPARATOR);
         
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
         cerr << ERR << "reading " << strErr << " in " << strFilePathName << endl << "'" << strRec << "'" << endl;
         InStream.close();
         
         return false;
      }
   }

   InStream.close();
   return (true);
}


/*========================================================================================================================================

 The bReadRunData member function reads the run details input file and does some initialization

========================================================================================================================================*/
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

   int i = 0;
   string strRec, strRH, strErr;
   
   while (getline(InStream, strRec))
   {
      // Trim off leading and trailing whitespace
      strRec = strTrim(&strRec);
      
      // If it is a blank line or a comment then ignore it
      if ((strRec.empty()) || (strRec[0] == QUOTE1) || (strRec[0] == QUOTE2))
         continue;
      
      // It isn't so increment counter
      i++;
      
      // Get the RH bit of the string, after the colon
      strRH = strSplitRH(&strRec);
      if (strRH.empty())
      {
         // Error: badly formatted line (no colon)
         cerr << ERR << "badly formatted line (no ':') in " << m_strDataPathName << endl << "'" << strRec << "'" << endl;
         return false;         
      }
      
      int 
         nMultiplier = 0,
         nLen = 0;
      string strTmp;
      vector<string> VstrItems;
      
      switch (i)
      {
      // --------------------------------------------------- Run Information ----------------------------------------------------------
      case 1:
         // Duration of rainfall, then duration of simulation: first convert to lower case
         strRH = strToLower(&strRH);
         
         // Split, space separated
         VstrItems = VstrSplit(&strRH, SPACE);
         
         nLen = VstrItems.size();
         if ((nLen < 2) || (nLen > 3))
         {
            strErr = "simulation duration incorrectly specified";
            break;
         }
         
         if (nLen == 3)
         {
            m_dSimulatedRainDuration = atof(VstrItems[0].c_str());
            m_dSimulationDuration = atof(VstrItems[1].c_str());               
         }
         else
         {
            m_dSimulationDuration = atof(VstrItems[0].c_str());               
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
         nLen = VstrItems.size();
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
            m_dRSaveInterval = atof(VstrItems[0].c_str()) * nMultiplier;
            
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
               double dSave = atof(VstrItems[n].c_str());
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
         nLen = VstrItems.size();
         if (nLen == 0)
         {
            // Just use previously set random number seeds
            break;
         }
         
         // Use user-specified random number seeds
         for (int n = 0; n < NUMBER_OF_RNGS; n++)
         {
            if (n < nLen)
               m_ulRandSeed[n] = atoi(VstrItems[n].c_str());
            else
               m_ulRandSeed[n] = m_ulRandSeed[0];
         }

         break;

      case 4:
         // GIS files to output, convert to lower case filenames
         strRH = strToLower(&strRH);
         if (strRH.find(ALL_CODE) != string::npos)
         {
            m_bRainVarMSave         =
            m_bRunOnSave            =
            m_bElevSave             =
            m_bDetrendElevSave      =
            m_bSlosSave             =
            m_bInitElevSave         =
            m_bInfiltSave           =
            m_bTotInfiltSave        =
            m_bSoilWaterSave        =
            m_bInfiltDepositSave    =
            m_bTotInfiltDepositSave =
            m_bTopSurfaceSave       =
            m_bSplashSave           =
            m_bTotSplashSave        =
            m_bInundationSave       =
            m_bFlowDirSave          =
            m_bStreamPowerSave      =
            m_bShearStressSave      =
            m_bFrictionFactorSave   =
            m_bAvgShearStressSave   =
            m_bReynoldsSave         =
            m_bFroudeSave           =
            m_bTCSave               =
            m_bAvgDepthSave         =
            m_bAvgDWSpdSave         =
            m_bAvgSpdSave           =
            m_bSedConcSave          =
            m_bSedLoadSave          =
            m_bAvgSedLoadSave       =
            m_bTotDepositSave       =
            m_bNetSlumpSave         =
            m_bNetToppleSave        =
            m_bTotNetSlosSave       = true;
#if defined _DEBUG
            m_bLostSave             = true;
#endif
         }
         else
         {
            if (strRH.find(RAIN_VARIATION_CODE) != string::npos)
            {
               m_bRainVarMSave = true;
               strRH = strRemoveSubstr(&strRH, &RAIN_VARIATION_CODE);
            }

            if (strRH.find(RUNON_CODE) != string::npos)
            {
               m_bRunOnSave = true;
               strRH = strRemoveSubstr(&strRH, &RUNON_CODE);
            }

            if (strRH.find(ELEVATION_CODE) != string::npos)
            {
               m_bElevSave = true;
               strRH = strRemoveSubstr(&strRH, &ELEVATION_CODE);
            }

            if (strRH.find(DETRENDED_ELEVATION_CODE) != string::npos)
            {
               m_bDetrendElevSave = true;
               strRH = strRemoveSubstr(&strRH, &DETRENDED_ELEVATION_CODE);
            }

            if (strRH.find(DETACHCODE) != string::npos)
            {
               m_bSlosSave = true;
               strRH = strRemoveSubstr(&strRH, &DETACHCODE);
            }

            if (strRH.find(INITIAL_ELEVATION_CODE) != string::npos)
            {
               m_bInitElevSave = true;
               strRH = strRemoveSubstr(&strRH, &INITIAL_ELEVATION_CODE);
            }

            if (strRH.find(INFILTRATION_CODE) != string::npos)
            {
               m_bTotInfiltSave = true;
               strRH = strRemoveSubstr(&strRH, &INFILTRATION_CODE);
            }

            if (strRH.find(CUMULATIVE_INFILTRATION_CODE) != string::npos)
            {
               m_bTotInfiltSave = true;
               strRH = strRemoveSubstr(&strRH, &CUMULATIVE_INFILTRATION_CODE);
            }

            if (strRH.find(SOIL_WATER_CODE) != string::npos)
            {
               m_bSoilWaterSave = true;
               strRH = strRemoveSubstr(&strRH, &SOIL_WATER_CODE);
            }

            if (strRH.find(TOPSCODE) != string::npos)
            {
               m_bTopSurfaceSave = true;
               strRH = strRemoveSubstr(&strRH, &TOPSCODE);
            }

            if (strRH.find(SPLASHCODE) != string::npos)
            {
               m_bSplashSave = true;
               strRH = strRemoveSubstr(&strRH, &SPLASHCODE);
            }

            if (strRH.find(TOTSPLASHCODE) != string::npos)
            {
               m_bTotSplashSave = true;
               strRH = strRemoveSubstr(&strRH, &TOTSPLASHCODE);
            }

            if (strRH.find(INUNDATIONCODE) != string::npos)
            {
               m_bInundationSave = true;
               strRH = strRemoveSubstr(&strRH, &INUNDATIONCODE);
            }

            if (strRH.find(FLOWDIRCODE) != string::npos)
            {
               m_bFlowDirSave = true;
               strRH = strRemoveSubstr(&strRH, &FLOWDIRCODE);
            }

            if (strRH.find(STREAMPOWERCODE) != string::npos)
            {
               m_bStreamPowerSave = true;
               strRH = strRemoveSubstr(&strRH, &STREAMPOWERCODE);
            }

            if (strRH.find(SHEARSTRESSCODE) != string::npos)
            {
               m_bShearStressSave = true;
               strRH = strRemoveSubstr(&strRH, &SHEARSTRESSCODE);
            }

            if (strRH.find(FRICTIONFACTORCODE) != string::npos)
            {
               m_bFrictionFactorSave = true;
               strRH = strRemoveSubstr(&strRH, &FRICTIONFACTORCODE);
            }

            if (strRH.find(AVGSHEARSTRESSCODE) != string::npos)
            {
               m_bAvgShearStressSave = true;
               strRH = strRemoveSubstr(&strRH, &AVGSHEARSTRESSCODE);
            }

            if (strRH.find(REYNOLDSCODE) != string::npos)
            {
               m_bReynoldsSave = true;
               strRH = strRemoveSubstr(&strRH, &REYNOLDSCODE);
            }

            if (strRH.find(FROUDECODE) != string::npos)
            {
               m_bFroudeSave = true;
               strRH = strRemoveSubstr(&strRH, &FROUDECODE);
            }

            if (strRH.find(TCCODE) != string::npos)
            {
               m_bTCSave = true;
               strRH = strRemoveSubstr(&strRH, &TCCODE);
            }

            if (strRH.find(AVG_SURFACE_WATER_DEPTH_CODE) != string::npos)
            {
               m_bAvgDepthSave = true;
               strRH = strRemoveSubstr(&strRH, &AVG_SURFACE_WATER_DEPTH_CODE);
            }

            if (strRH.find(AVGSPEEDCODE) != string::npos)
            {
               m_bAvgSpdSave = true;
               strRH = strRemoveSubstr(&strRH, &AVGSPEEDCODE);
            }

            if (strRH.find(AVGDWSPEEDCODE) != string::npos)
            {
               m_bAvgDWSpdSave = true;
               strRH = strRemoveSubstr(&strRH, &AVGDWSPEEDCODE);
            }

            if (strRH.find(SEDCONCCODE) != string::npos)
            {
               m_bSedConcSave = true;
               strRH = strRemoveSubstr(&strRH, &SEDCONCCODE);
            }

            if (strRH.find(SEDLOADCODE) != string::npos)
            {
               m_bSedLoadSave = true;
               strRH = strRemoveSubstr(&strRH, &SEDLOADCODE);
            }

            if (strRH.find(AVGSEDLOADCODE) != string::npos)
            {
               m_bAvgSedLoadSave = true;
               strRH = strRemoveSubstr(&strRH, &AVGSEDLOADCODE);
            }

            if (strRH.find(CUMULATIVE_ALL_SIZE_FLOW_DEPOSITION_CODE) != string::npos)
            {
               m_bTotDepositSave = true;
               strRH = strRemoveSubstr(&strRH, &CUMULATIVE_ALL_SIZE_FLOW_DEPOSITION_CODE);
            }

            if (strRH.find(TOT_SURFACE_LOWERING_CODE) != string::npos)
            {
               m_bTotNetSlosSave = true;
               strRH = strRemoveSubstr(&strRH, &TOT_SURFACE_LOWERING_CODE);
            }

#if defined _DEBUG
            if (strRH.find(LOSTCODE) != string::npos)
            {
               m_bLostSave = true;
               strRH = strRemoveSubstr(&strRH, &LOSTCODE);
            }
#endif

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
         if (strRH.find(ALL_CODE) != string::npos)
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
            m_bDoFlowDepositionTS =
            m_bSedLostTS          =
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

            if (strRH.find(INFILTRATION_TIME_SERIES_CODE) != string::npos)
            {
               m_bInfiltTS =
               m_bExfiltTS = true;
               strRH = strRemoveSubstr(&strRH, &INFILTRATION_TIME_SERIES_CODE);
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

            if (strRH.find(FLOW_DETACHMENT_TIME_SERIES_CODE) != string::npos)
            {
               m_bFlowDetachTS = true;
               strRH = strRemoveSubstr(&strRH, &FLOW_DETACHMENT_TIME_SERIES_CODE);
            }

            if (strRH.find(SLUMP_DETACHMENT_TIME_SERIES_CODE) != string::npos)
            {
               m_bSlumpDetachTS = true;
               strRH = strRemoveSubstr(&strRH, &SLUMP_DETACHMENT_TIME_SERIES_CODE);
            }

            if (strRH.find(TOPPLE_DETACHMENT_TIME_SERIES_CODE) != string::npos)
            {
               m_bToppleDetachTS = true;
               strRH = strRemoveSubstr(&strRH, &TOPPLE_DETACHMENT_TIME_SERIES_CODE);
            }

            if (strRH.find(FLOW_DEPOSITION_TIME_SERIES_CODE) != string::npos)
            {
               m_bDoFlowDepositionTS = true;
               strRH = strRemoveSubstr(&strRH, &FLOW_DEPOSITION_TIME_SERIES_CODE);
            }

            if (strRH.find(SEDIMENT_LOST_TIME_SERIES_CODE) != string::npos)
            {
               m_bSedLostTS = true;
               strRH = strRemoveSubstr(&strRH, &SEDIMENT_LOST_TIME_SERIES_CODE);
            }

            if (strRH.find(SEDIMENT_LOAD_TIME_SERIES_CODE) != string::npos)
            {
               m_bSedLoadTS = true;
               strRH = strRemoveSubstr(&strRH, &SEDIMENT_LOAD_TIME_SERIES_CODE);
            }

            if (strRH.find(INFILTRATION_DEPOSIT_TIME_SERIES_CODE) != string::npos)
            {
               m_bInfiltDepositTS = true;
               strRH = strRemoveSubstr(&strRH, &INFILTRATION_DEPOSIT_TIME_SERIES_CODE);
            }

            if (strRH.find(SPLASH_REDISTRIBUTION_TIME_SERIES_CODE) != string::npos)
            {
               m_bSplashRedistTS = true;
               strRH = strRemoveSubstr(&strRH, &SPLASH_REDISTRIBUTION_TIME_SERIES_CODE);
            }

            if (strRH.find(SPLASH_KINETIC_ENERGY_TIME_SERIES_CODE) != string::npos)
            {
               m_bSplashKETS = true;
               strRH = strRemoveSubstr(&strRH, &SPLASH_KINETIC_ENERGY_TIME_SERIES_CODE);
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
         m_dGradient = atof(strRH.c_str());                                           // in per cent
         if (m_dGradient < 0)
            strErr = "slope angle";
         break;

      case 15:
         // Base level e.g. distance below lowest DEM point to flume lip. Negative values mean no baselevel, i.e. can cut down to unerodible basement
         m_dPlotEndDiff = atof(strRH.c_str());
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
         m_nNumSoilLayers = atoi(strRH.c_str());
         if (m_nNumSoilLayers < 1)
            strErr = "need at least one soil layer";
         break;
         
      case 17:
         // Elevation of unerodible basement. This is the same as the elevation of the bottom of the lowest soil layer [Z units]
         m_dBasementElevation = atof(strRH.c_str());
         
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
               }
               
               // Trim off leading and trailing whitespace
               strRec = strTrim(&strRec);
               
               // If this line is a blank line or a comment then ignore it
               if ((strRec.empty()) || (strRec[0] == QUOTE1) || (strRec[0] == QUOTE2))
                  continue;
               
               // It isn't so increment counter
               j++;
               
               strRH = strSplitRH(&strRec);
               if (strRH.empty())
               {
                  // Error: badly formatted line (no colon)
                  cerr << ERR << "badly formatted line (no ':') in " << m_strDataPathName << endl << "'" << strRec << "'" << endl;
                  return false;         
               }
               
               switch (j)
               {
                  case 1:
                     // Layer name
                     m_VstrInputSoilLayerName.push_back(strRH);
                     break;
                     
                  case 2:
                     // Thickness (omit for top layer)
                     m_VdInputSoilLayerThickness.push_back(atof(strRH.c_str()));
                     break;
                     
                  case 3:
                     // % clay
                     dTmp = atof(strRH.c_str());
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
                     dTmp = atof(strRH.c_str());
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
                     dTmp = atof(strRH.c_str());
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
                        if (dPerCentClay == 0)
                           dPerCentClay = (100 - (dPerCentSilt + dPerCentSand));
                        else if (dPerCentSilt == 0)
                           dPerCentSilt = (100 - (dPerCentClay + dPerCentSand));
                        else if (dPerCentSand == 0)
                           dPerCentSand = (100 - (dPerCentClay + dPerCentSilt));
                     }
                     
                     m_VdInputSoilLayerPerCentClay.push_back(dPerCentClay);
                     m_VdInputSoilLayerPerCentSilt.push_back(dPerCentSilt);
                     m_VdInputSoilLayerPerCentSand.push_back(dPerCentSand);

                     break;
                     
                     
                  case 6:
                     // Bulk density (t/m**3 or g/cm**3)
                     dTmp = atof(strRH.c_str());
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
                     dTmp = atof(strRH.c_str());
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
                     dTmp = atof(strRH.c_str());
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
                     dTmp = atof(strRH.c_str());
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
                     dTmp = atof(strRH.c_str());
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
                     dTmp = atof(strRH.c_str());
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
                     dTmp = atof(strRH.c_str());
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
                     dTmp = atof(strRH.c_str());
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
                     dTmp = atof(strRH.c_str());
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
                     dTmp = atof(strRH.c_str());
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
            m_dRainIntensity = atof(strRH.c_str());                                   // in mm/hour
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
         m_dStdRainInt = atof(strRH.c_str());                                                  // in mm/hour
         if (m_dStdRainInt < 0)
            strErr = "standard deviation of rainfall intensity";
         break;

      case 21:
         // Mean raindrop diameter
         m_dDropDiameter = atof(strRH.c_str());                                                // in mm
         if (m_dDropDiameter <= 0)
            strErr = "mean raindrop diameter";
         else
            m_dMeanCellWaterVol = 4 * PI * pow(m_dDropDiameter * 0.5, 3) / 3;         // in mm**3
         break;

      case 22:
         // Standard deviation of raindrop diameter
         m_dStdDropDiameter = atof(strRH.c_str());                                             // in mm
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
         m_dRainSpeed = atof(strRH.c_str());                                                   // in m/sec
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
         // Splash-efficiency data filename
         if (m_bSplash)
         {
            if (strRH.empty())
               strErr = "path and name of splash-efficiency datafile";
            else
            {
               // Now check for leading slash, or leading Unix home dir symbol, or occurrence of a drive letter
               if ((strRH[0] == PATH_SEPARATOR) || (strRH[0] == '~') || (strRH.find(':') != string::npos))
               {
                  // It has an absolute path, so use it 'as is'
                  m_strSplshFile = strRH;
               }
               else
               {
                  // It has a relative path, so prepend the RG6 dir
                  m_strSplshFile = m_strRGDir;
                  m_strSplshFile.append(strRH);
               }
            }
         }
         break;

      case 27:
         // N in splash efficiency equation (sec**2/mm)
         m_VdSplashEffN = atof(strRH.c_str());
         if (m_bSplash && (m_VdSplashEffN < 0))
            strErr = "splash efficiency N";
         break;

      case 28:
         // Splash vertical lowering constant (mm/hr)
         m_dSplashVertical = atof(strRH.c_str());
         if (m_bSplash && (m_dSplashVertical < 0))
            strErr = "splash vertical lowering";
         break;

         // --------------------------------------------------------- Run-on -------------------------------------------------------------
      case 29:
         // Run-on from outside the grid?
         strRH = strToLower(&strRH);
         if (strRH.find('y') != string::npos)
            m_bRunOn = true;
         else if (strRH.find('n') != string::npos)
            m_bRunOn = false;
         else
            strErr = "run-on switch";
         break;

      case 30:
         // Run-on from which edges?
         strRH = strToLower(&strRH);
         if (strRH.find('t') != string::npos)
            m_bRunOnThisEdge[EDGE_TOP] = true;
         if (strRH.find('r') != string::npos)
            m_bRunOnThisEdge[EDGE_RIGHT] = true;
         if (strRH.find('b') != string::npos)
            m_bRunOnThisEdge[EDGE_BOTTOM] = true;
         if (strRH.find('l') != string::npos)
            m_bRunOnThisEdge[EDGE_LEFT] = true;
         break;
         
      case 31:
         // Length of run-on area
         m_dRunOnLen = atof(strRH.c_str());
         if (m_bRunOn && (m_dRunOnLen <= 0))
               strErr = "run-on length";
         break;

      case 32:
         // Rainfall spatial variation multiplier for run-on area
         m_dRunOnRainVarM = atof(strRH.c_str());
         if (m_bRunOn && (m_dRunOnRainVarM <= 0))
            strErr = "rainfall spatial variation multiplier for run-on area";
         break;

      case 33:
         // Flow speed on run-on area
         m_dRunOnSpd = atof(strRH.c_str());
         if (m_bRunOn && (m_dRunOnSpd <= 0))
               strErr = "run-on flow speed";
         break;

      // ----------------------------------------------------------- Infiltration -----------------------------------------------------
      case 34:
         // Simulate infiltration?
         strRH = strToLower(&strRH);
         if (strRH.find('y') != string::npos)
            m_bDoInfiltration = true;
         else if (strRH.find('n') != string::npos)
            m_bDoInfiltration = false;
         else
            strErr = "infiltration switch";
         break;

      case 35:
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
               }
               
               // Trim off leading and trailing whitespace
               strRec = strTrim(&strRec);
               
               // If this line is a blank line or a comment then ignore it
               if ((strRec.empty()) || (strRec[0] == QUOTE1) || (strRec[0] == QUOTE2))
                  continue;
               
               // It isn't so increment counter
               j++;
               
               strRH = strSplitRH(&strRec);
               if (strRH.empty())
               {
                  // Error: badly formatted line (no colon)
                  cerr << ERR << "badly formatted line (no ':') in " << m_strDataPathName << endl << "'" << strRec << "'" << endl;
                  return false;         
               }
               
               switch (j)
               {
                  case 1:
                     // Air entry head, is half of bubbling pressure (cm)
                     if (m_bDoInfiltration)
                     {
                        dTmp = atof(strRH.c_str());
                        if (dTmp == 0)
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
                        dTmp = atof(strRH.c_str());
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
                        dTmp = atof(strRH.c_str());
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
                        dTmp = atof(strRH.c_str());
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
                        dTmp = atof(strRH.c_str());
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
      case 36:
         // In the partially-inundated flow regime, D50 of roughness elements (mm)
         m_dD50 = atof(strRH.c_str());
         if (m_dD50 <= 0)
            strErr = "D50 of roughness elements";
         else
            m_dEpsilon = 0.5 * m_dD50;
         break;

      case 37:
         // In the partially-inundated flow regime, % of surface covered with roughness elements
         m_dPr = atof(strRH.c_str());
         if ((m_dPr < 0) || (m_dPr > 100))
            strErr = "% of surface covered with roughness elements";
         break;

      case 38:
         // In the partially-inundated flow regime, the ratio between the drag of roughness elements and the ideal situation
         m_dCd = atof(strRH.c_str());
         if ((m_dCd < 0) || (m_dCd > 1))
            strErr = "ratio between the drag of roughness elements and the ideal situation";
         break;

      // ------------------------------------------------------------ Flow Erosion- ---------------------------------------------------
      case 39:
         // Simulate flow erosion?
         strRH = strToLower(&strRH);
         if (strRH.find('y') != string::npos)
            m_bFlowErosion = true;
         else if (strRH.find('n') != string::npos)
            m_bFlowErosion = false;
         else
            strErr = "flow erosion switch";
         break;

      case 40:
         // K in detachment equation
         m_dK = atof(strRH.c_str());
         if (m_bFlowErosion && (m_dK <= 0))
            strErr = "constant k for detachment";
         break;

      case 41:
         // T in detachment equation
         m_dT = atof(strRH.c_str());
         if (m_bFlowErosion && (m_dT <= 0))
            strErr = "constant T for detachment";
         break;

      case 42:
         // CV of T in detachment equation
         m_dCVT = atof(strRH.c_str());
         if (m_bFlowErosion)
         {
            if (m_dCVT <= 0)
               strErr = "CV of T for detachment";
         }
         break;

      case 43:
         // CV of tau-b in detachment equation
         m_dCVTaub = atof(strRH.c_str());
         if (m_bFlowErosion && (m_dCVTaub <= 0))
            strErr = "CV of tau-b for detachment";
         break;

      case 44:
         // Critical angle (mm/mm) for source-desination erosion share
         m_dSourceDestCritAngle = atof(strRH.c_str());
         if (m_bFlowErosion && (m_dSourceDestCritAngle <= 0))
            strErr = "critical angle for source-desination erosion share";
         break;

      // -------------------------------------------------------- Transport Capacity --------------------------------------------------
      case 45:
         // Alpha in transport capacity equation
         m_dAlpha = atof(strRH.c_str());
         if (m_bFlowErosion && (m_dAlpha >= 0))
            strErr = "alpha for transport capacity";
         break;

      case 46:
         // Beta in transport capacity equation
         m_dBeta = atof(strRH.c_str());
         if (m_bFlowErosion && (m_dBeta <= 0))
            strErr = "beta for transport capacity";
         break;

      case 47:
         // Gamma in transport capacity equation
         m_dGamma = atof(strRH.c_str());
         if (m_bFlowErosion && (m_dGamma <= 0))
            strErr = "gamma for transport capacity";
         break;

      case 48:
         // Delta in transport capacity equation
         m_dDelta = atof(strRH.c_str());
         if (m_bFlowErosion && (m_dDelta <= 0))
            strErr = "delta for transport capacity";
         break;

      // ----------------------------------------------------------- Deposition -------------------------------------------------------
      case 49:
         // Grain density (kg/m**3)         
         m_dDepositionGrainDensity = atof(strRH.c_str());
         if (m_bFlowErosion && (m_dDepositionGrainDensity <= 0))
            strErr = "grain density, for deposition";
         break;
         
      case 50:
         // Clay minimum size (mm)
         m_dClaySizeMin = atof(strRH.c_str());
         if (m_bFlowErosion && (m_dClaySizeMin < 0))
            strErr = "clay minimum size, for deposition";
         break;
         
      case 51:
         // Clay-silt threshold size (mm)
         m_dClaySiltBoundarySize = atof(strRH.c_str());
         if (m_bFlowErosion && (m_dClaySiltBoundarySize <= m_dClaySizeMin))
            strErr = "clay-silt threshold size, for deposition";
         break;
         
      case 52:
         // Silt-Sand threshold size (mm)
         m_dSiltSandBoundarySize = atof(strRH.c_str());
         if (m_bFlowErosion && (m_dSiltSandBoundarySize <= m_dClaySiltBoundarySize))
            strErr = "silt-sand threshold size, for deposition";
         break;
         
      case 53:
         // Sand maximum size (mm)
         m_dSandSizeMax = atof(strRH.c_str());
         if (m_bFlowErosion && (m_dSandSizeMax <= m_dSiltSandBoundarySize))
            strErr = "sand maximum size, for deposition";
         break;
 
      // -------------------------------------------------------------- Slumping ------------------------------------------------------
      case 54:
         // Simulate slumping?
         strRH = strToLower(&strRH);
         if (strRH.find('y') != string::npos)
            m_bSlumping = true;
         else if (strRH.find('n') != string::npos)
            m_bSlumping = false;
         else
            strErr = "slumping switch";
         break;

      case 55:
         // Threshold shear stress for slumping
         m_dSlumpCritShearStress = atof(strRH.c_str());                    // kg/m s**2 (Pa)
         if (m_bSlumping && (m_dSlumpCritShearStress < 0))
            strErr = "threshold shear stress for slumping";
         break;

      case 56:
         // Angle of rest for saturated slumped soil
         m_dSlumpAngleOfRest = atof(strRH.c_str());                        // in per cent
         if (m_bSlumping && (m_dSlumpAngleOfRest < 0))
            strErr = "angle of rest for slumped soil";
         break;

      case 57:
         // Critical angle for toppling soil (not saturated)
         m_dToppleCriticalAngle = atof(strRH.c_str());                     // in per cent
         if (m_bSlumping)
         {
            if (m_dToppleCriticalAngle < 0)
               strErr = "critical angle for toppling";
         }
         break;

      case 58:
         // Angle of rest for toppled soil (not saturated)
         m_dToppleAngleOfRest = atof(strRH.c_str());                       // in per cent
         if (m_bSlumping)
         {
            if (m_dToppleAngleOfRest == 0)
               m_dToppleAngleOfRest = m_dSlumpAngleOfRest;

            if ((m_dToppleAngleOfRest >= m_dToppleCriticalAngle) || (m_dToppleAngleOfRest < 0))
               strErr = "angle of rest for toppled soil";
         }
         break;

         // --------------------------------------------------- Various Physical Constants -----------------------------------------------
      case 59:
         // Density of water
         m_dRho = atof(strRH.c_str());                                    // kg/m**3
         if (m_dRho <= 0)
            strErr = "density of water";
         break;

      case 60:
         // Viscosity of water
         m_dNu = atof(strRH.c_str());                                     // m**2/sec
         if (m_dNu <= 0)
            strErr = "viscosity of water";
         break;

      case 61:
         // Gravitational acceleration
         m_dG = atof(strRH.c_str());                                      // m/sec**2
         if (m_dG <= 0)
            strErr = "gravitational acceleration";
         break;
      }
   
      // Did an error occur?
      if (! strErr.empty())
      {
         // Error in input to run details file
         cerr << ERR << "reading " << strErr << " in " << m_strDataPathName << endl << "'" << strRec << "'" << endl;
         InStream.close();
         return false;
      }
   }

   // Make some changes now everything has been read: first, don't output runon files if no runon, ditto splash or KE GIS/TS files
   // if no splash
   if (! m_bRunOn)
      m_bRunOnSave = false;

   if (! m_bSplash)
   {
      m_bSplashSave      =
      m_bTotSplashSave   =
      m_bSplashRedistTS  =
      m_bSplashKETS      = false;
   }

   // Similarly, don't output runon TS file if no runon
   if (! m_bRunOn)
      m_bRunOnTS = false;

   // Or infiltration GIS/TS files if no infiltration
   if (! m_bDoInfiltration)
   {
      m_bInfiltSave           =
      m_bTotInfiltSave        =
      m_bSoilWaterSave        =
      m_bInfiltDepositSave    =
      m_bTotInfiltDepositSave =
      m_bInfiltTS             =
      m_bExfiltTS             =
      m_bInfiltDepositTS      = 
      m_bSoilWaterTS          = false;
   }

   // Or slumping or toppling GIS/TS files if no slumping
   if (! m_bSlumping)
   {
      m_bNetSlumpSave   =
      m_bNetToppleSave  =
      m_bSlumpDetachTS  =
      m_bToppleDetachTS = false;
   }

   // Or flow erosion GIS/TS files if no flow erosion
   if (! m_bFlowErosion)
   {
      m_bTCSave         =
      m_bSlosSave       =
      m_bSedConcSave    =
      m_bSedLoadSave    =
      m_bAvgSedLoadSave =
      m_bTotDepositSave =
      m_bTotNetSlosSave = true;
      
      m_bFlowDetachTS       =
      m_bDoFlowDepositionTS =
      m_bSedLostTS          =
      m_bSedLoadTS          = false;
   }

   // Close file
   InStream.close();
   return (true);
}


/*========================================================================================================================================

 Opens the log file

========================================================================================================================================*/
bool CSimulation::bOpenLogFile(void)
{
   // Open in binary mode if just checking random numbers
#if defined RANDCHECK
   LogStream.open(m_strLogFile, ios::out | ios::binary | ios::trunc);
#else
   LogStream.open(m_strLogFile, ios::out | ios::trunc);
#endif

   if (! LogStream)
   {
      // Error, cannot open log file
      cerr << ERR << "cannot open " << m_strLogFile << " for output" << endl;
      return (false);
   }

   // Set default Log output format
   LogStream << setiosflags(ios::scientific);

   return (true);
}


/*========================================================================================================================================

 This member function of CSimulation reads the rainfall intensity time-series file

========================================================================================================================================*/
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
   int i = -2, nMultiplier = 1;

   while (getline(InStream, strRec))
   {
      // Convert to lowercase and trim off leading and trailing whitespace
      strRec = strToLower(&strRec);
      strRec = strTrim(&strRec);

      // If it is not a blank line, but it is a comment line or a blank line, then ignore it
      if ((! strRec.empty()) && (strRec[0] != QUOTE1) && (strRec[0] != QUOTE2))
      {
         // It isn't, so increment counter
         i++;
         
         // Find the colon: note that lines MUST have a colon separating data from leading description portion
         size_t nPos = strRec.find(':');
         if (nPos == string::npos)
         {
            // Error: badly formatted line (no colon)
            cerr << ERR << "badly formatted line (no ':') in " << m_strDataPathName << endl << strRec << endl;
            return false;
         }
         
         // Strip off leading portion (the bit up to and including the colon)
         string strRH = strRec.substr(nPos+1);
         
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
               strRH = strRH.substr(0, nPos);
               bFound = true;
            }
            
            nPos = strRH.rfind(QUOTE2);
            if (nPos != string::npos)
            {
               strRH = strRH.substr(0, nPos);
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
               strErr = "units for rainfall change times ";
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
            double dRainChangeTime = atof(VstrItems[0].c_str()) * nMultiplier;
            
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
            double dRainChangeIntensity = atof(VstrItems[1].c_str());

            // Check for obvious error
            if (dRainChangeIntensity < 0)
            {
               strErr = "rainfall change intensity ";
               strErr.append(to_string(i+1));
               strErr.append(" is invalid: ");
               strErr.append(strRH);
            }

            // All OK, so store
            m_VdRainChangeTime.push_back(dRainChangeTime);            
            m_VdRainChangeIntensity.push_back(dRainChangeIntensity);            
         }

         // Did an error occur?
         if (! strErr.empty())
         {
            // error in input to run details file
            cerr << ERR << strErr << endl;
            InStream.close();
            return (false);
         }
      }
   }

   // All OK, so store number of time/intensity pairs
   m_nRainChangeTimeMax = i;

   // close and return
   InStream.close();
   return (true);
}


/*========================================================================================================================================
 
 Returns the after-colon part of a line read from the text input file
 
========================================================================================================================================*/
string CSimulation::strSplitRH(string const* pstrRec) const
{
   size_t nPos = 0;
   string strRH;
   
   // Find the colon: note that lines MUST have a colon separating data from leading description portion
   nPos = pstrRec->find(':');
   if (nPos == string::npos)
   {
      // Error: badly formatted line (no colon)
      cerr << ERR << "badly formatted line (no ':') in " << m_strDataPathName << endl << *pstrRec << endl;
      return strRH;
   }

   // Strip off leading portion (the bit up to and including the colon)
   strRH = pstrRec->substr(nPos+1);

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
         strRH = strRH.substr(0, nPos);
         bFound = true;
      }
      
      nPos = strRH.rfind(QUOTE2);
      if (nPos != string::npos)
      {
         strRH = strRH.substr(0, nPos);
         bFound = true;
      }
      
      // Trim trailing spaces
      strRH = strTrimRight(&strRH);
   }
   
   return strRH;
}
