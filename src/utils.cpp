/*========================================================================================================================================

This is utils.cpp: utility routines for RillGrow

Copyright (C) 2025 David Favis-Mortlock

=========================================================================================================================================

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public  License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

========================================================================================================================================*/
#include <sstream>
using std::stringstream;

#include <algorithm>
using std::transform;

#include "rg.h"
#include "simulation.h"
#include "cell.h"

//=========================================================================================================================================
//! Handles command-line parameters
//=========================================================================================================================================
int CSimulation::nHandleCommandLineParams(int nArg, char* pcArgv[])
{
   for (int i = 1; i < nArg; i++)
   {
      char* pszArg = pcArgv[i];
      string strArg = pcArgv[i];
      strArg = strToLower(&strArg);

      if (strArg.find("--gdal") != string::npos)
      {
         // User wants to know what GDAL drivers are available
         cout << GDALDRIVERS << endl << endl;

         for (int j = 0; j < GDALGetDriverCount(); j++ )
         {
            GDALDriverH hDriver = GDALGetDriver(j);

            string strDriverShort = GDALGetDriverShortName(hDriver);
            strDriverShort.append("          ");
            string strDriverLong = GDALGetDriverLongName(hDriver);

            cout << strDriverShort << strDriverLong << endl;
         }
         return (RTN_HELPONLY);
      }

      else if (strArg.find("--about") != string::npos)
      {
         // User wants information about RG
         cout << ABOUT << endl << endl;
         cout << THANKS << endl;

         return (RTN_HELPONLY);
      }

      else if (strArg.find("--home") != string::npos)
      {
         // User has specified the full path to the .ini file etc. This is needed if rg6 is run with a default directory other than the
         // one in which the .ini file resides e.g. when debugging in some situations
         vector<string> VstrItems = VstrSplit(&strArg, '=');
         if (VstrItems.size() < 2)
         {
            // Error: badly formatted argument (no equals sign)
            cerr << ERR << "badly formatted command-line parameter: " << pszArg << endl;
            return (RTN_ERR_RGDIR);
         }

         // Assume it is OK after stripping off the leading equals sign
         m_strRGDir = strTrim(&VstrItems[1]);

         // Check for trailing slash in new default RG6 directory name
         if (m_strRGDir.find(PATH_SEPARATOR) == string::npos)
            // Add a trailing slash
            m_strRGDir.append(to_string(PATH_SEPARATOR));         // TODO no idea why this is needed

         // Now attempt to change to this dir
         int nRet = chdir(m_strRGDir.c_str());
         if (nRet == -1)
         {
            // Error, can't change to this dir
            cerr << ERR << "cannot change to " << m_strRGDir << endl;
            return (RTN_ERR_RGDIR);
         }
      }

      else if (strArg.find("--datafile") != string::npos)
      {
         // User has specified the name (and maybe the path) to the main datafile. It overrides whatever will be read from the .ini file
         vector<string> VstrItems = VstrSplit(&strArg, '=');
         if (VstrItems.size() < 2)
         {
            // Error: badly formatted argument (no equals sign)
            cerr << ERR << "badly formatted command-line parameter: " << pszArg << endl;
            return (RTN_ERR_BADPARAM);
         }

         // Assume it is OK after stripping off the leading equals sign
         m_strDataPathName = strTrim(&VstrItems[1]);
      }

      else
      {
         // Display usage information
         cout << USAGE0 << endl;
         cout << USAGE1 << endl;
         cout << USAGE2 << endl;
         cout << USAGE3 << endl;
         cout << USAGE4 << endl;
         cout << USAGE5 << endl;

         return (RTN_HELPONLY);
      }
   }
   return (RTN_OK);
}

//=========================================================================================================================================
//! Tells the user that we have started the simulation
//=========================================================================================================================================
void CSimulation::AnnounceStart(void)
{
   cout << endl << PROGNAME << " for " << PLATFORM << " " << strGetBuild() << endl << endl;
}

//=========================================================================================================================================
//! Starts the clock ticking
//=========================================================================================================================================
void CSimulation::StartClock(void)
{
   // First start the 'CPU time' clock ticking
   if (static_cast<clock_t>(-1) == clock())
   {
      // There's a problem with the clock, but continue anyway
      cerr << WARN << "CPU time not available" << endl;
      m_dCPUClock = -1;
   }
   else
   {
      // All OK, so get the time in m_dClkLast (this is needed to check for clock rollover on long runs)
      m_dClkLast = static_cast<double>(clock());
      m_dClkLast -= CLOCK_T_MIN;       // necessary if clock_t is signed to make m_dClkLast unsigned
   }

   // And now get the actual time we started
   time(&m_tSysStartTime);
}

//=========================================================================================================================================
//! Finds the current directory
//=========================================================================================================================================
bool CSimulation::bFindExeDir(char* pcArg)
{
   string strTmp;
   char szBuf[BUFFER_SIZE] = "";

#ifdef _WIN32
   if (0 != GetModuleFileName(NULL, szBuf, BUFFER_SIZE))
      strTmp = szBuf;
   else
      // It failed, so try another approach
      strTmp = pcArg;
#else
   //   char* pResult = getcwd(szBuf, BUFFER_SIZE);          // Used to use this, but what if cwd is not the same as the RG dir?

   if (-1 != readlink("/proc/self/exe", szBuf, BUFFER_SIZE))
      strTmp = szBuf;
   else
      // It failed, so try another approach
      strTmp = pcArg;
#endif

   // Neither approach has worked, so give up
   if (strTmp.empty())
      return false;

   // It's OK, so trim off the executable's name
   int nPos = static_cast<int>(strTmp.find_last_of(PATH_SEPARATOR));
   m_strRGDir = strTmp.substr(0, nPos+1);            // Note that this must be terminated with a backslash

   return true;
}

//=========================================================================================================================================
//! Tells the user about the licence
//=========================================================================================================================================
void CSimulation::AnnounceLicence(void)
{
   cout << COPYRIGHT << endl << endl;
   cout << LINE << endl;
   cout << DISCLAIMER1 << endl;
   cout << DISCLAIMER2 << endl;
   cout << DISCLAIMER3 << endl;
   cout << DISCLAIMER4 << endl;
   cout << DISCLAIMER5 << endl;
   cout << DISCLAIMER6 << endl;
   cout << LINE << endl << endl;

   // Note endl not needed, ctime() seems to always output a trailing <cr>
   cout << START_NOTICE << strGetComputerName() << " at " << ctime(&m_tSysStartTime);
   cout << INIT_NOTICE << endl;
}

//=========================================================================================================================================
//! Tells the user that we are now reading DEM data
//=========================================================================================================================================
void CSimulation::AnnounceReadDEM(void) const
{
   // Tell the user what is happening
   cout << READ_DEM_FILE << m_strDEMFile << endl;
}

//=========================================================================================================================================
//! Tells the user that we are now allocating memory
//=========================================================================================================================================
void CSimulation::AnnounceAllocateMemory(void)
{
   cout << ALLOCATE_MEMORY << endl;
}

//=========================================================================================================================================
//! Tells the user that we are now reading the rainfall spatial variation file
//=========================================================================================================================================
void CSimulation::AnnounceReadRainVar(void) const
{
   cout << READ_RAIN_VARIATION_FILE << m_strRainVarMFile << endl;
}

//=========================================================================================================================================
//! Tell the user that the simulation is now running
//=========================================================================================================================================
void CSimulation::AnnounceIsRunning(void)
{
   cout << RUN_NOTICE << endl;
}

//=========================================================================================================================================
//! Wraps long lines written to the .out file (TODO is rather crude, improve)
//=========================================================================================================================================
void CSimulation::WrapLongString(string* pstrTemp)
{
   if (static_cast<int>(pstrTemp->size()) > OUTPUT_WIDTH)
   {
      m_ofsOut << *pstrTemp << endl << "                                                        \t: ";
      *pstrTemp = "";
   }
}

//=========================================================================================================================================
//! Returns a string, hopefully giving the name of the computer on which the simulation is running
//=========================================================================================================================================
string CSimulation::strGetComputerName(void)
{
   string strComputerName;

#ifdef _WIN32
   // Being compiled to run under Windows, either by MS VC++, Borland C++, or Cygwin
   strComputerName = getenv("COMPUTERNAME");
#else
   // Being compiled for another platform; assume for Linux-Unix
   char szHostName[BUFFER_SIZE] = "";
   gethostname(szHostName, BUFFER_SIZE);

   strComputerName = szHostName;
   if (strComputerName.empty())
      strComputerName = "Unknown Computer";
#endif

   return strComputerName;
}

//=========================================================================================================================================
//" Resets the CPU clock timer to prevent it 'rolling over', as can happen during long runs. This is a particularly problem under Unix systems where the value returned by clock() is defined in microseconds (for compatibility with systems that have CPU clocks with much higher resolution) i.e. CLOCKS_PER_SEC is 1000000 rather than the more usual 1000. In this case, the value returned from clock() will wrap around after accumulating only 2147 seconds of CPU time (about 36 minutes).
//=========================================================================================================================================
void CSimulation::DoCPUClockReset(void)
{
   if (static_cast<clock_t>(-1) == clock())
   {
      // Error
      m_ofsLog << "CPU time not available" << endl;
      m_dCPUClock = -1;
      return;
   }

   // OK, so carry on
   double dClkThis = static_cast<double>(clock());
   dClkThis -= CLOCK_T_MIN;   // necessary when clock_t is signed, to make dClkThis unsigned

   if (dClkThis < m_dClkLast)
   {
      // Clock has 'rolled over'
      m_dCPUClock += (CLOCK_T_RANGE + 1 - m_dClkLast);   // this elapsed before rollover
      m_dCPUClock += dClkThis;                           // this elapsed after rollover

#if defined CLOCKCHECK
      // For debug purposes
      m_ofsLog << "Rolled over: dClkThis=" << dClkThis << " m_dClkLast=" << m_dClkLast << endl << "\t" << " before rollover=" << (CLOCK_T_RANGE + 1 - m_dClkLast) << endl << "\t" << " after rollover=" << dClkThis << endl << "\t" << " ADDED=" << (CLOCK_T_RANGE + 1 - m_dClkLast + dClkThis) << endl;
#endif
   }
   else
   {
      // No rollover
      m_dCPUClock += (dClkThis - m_dClkLast);

#if defined CLOCKCHECK
      // For debug purposes
      m_ofsLog << "No rollover: dClkThis=" << dClkThis << " m_dClkLast=" << m_dClkLast << " ADDED=" << dClkThis - m_dClkLast << endl;
#endif
   }

   // Reset for next time
   m_dClkLast = dClkThis;
}

//=========================================================================================================================================
//! Announce the end of the simulation
//=========================================================================================================================================
void CSimulation::AnnounceSimEnd(void)
{
   cout << endl << FINAL_OUTPUT << endl;
}

//=========================================================================================================================================
//! Calculates and displays time elapsed in terms of CPU time and real time, also calculates time per iteration in terms of both CPU time and real time TODO re-write this using c++11 chrono see https://www.geeksforgeeks.org/how-to-measure-elapsed-time-in-cpp/
//=========================================================================================================================================
void CSimulation::CalcTime(double const dRunLength)
{
   // Reset CPU count for last time
   DoCPUClockReset();

   if (! bFpEQ(m_dCPUClock, -1.0, TOLERANCE))
   {
      // Calculate CPU time in secs
      double dDuration = m_dCPUClock/CLOCKS_PER_SEC;

      // And write CPU time out to m_ofsOut and m_ofsLog
      m_ofsOut << "CPU time elapsed: " << strDispTime(dDuration, false, true);
      m_ofsLog << "CPU time elapsed: " << strDispTime(dDuration, false, true);

      // Calculate CPU time per iteration
      double dPerIter = dDuration / static_cast<double>(m_ulTotIter);

      // And write CPU time per iteration to m_ofsOut and m_ofsLog
      m_ofsOut << resetiosflags(ios::floatfield) << std::fixed << " (" << setprecision(4) << dPerIter << " per iteration)" << endl;
      m_ofsLog << resetiosflags(ios::floatfield) << std::fixed << " (" << setprecision(4) << dPerIter << " per iteration)" << endl;

      // Calculate ratio of CPU time to time simulated
      m_ofsOut << "In terms of CPU time, this is ";
      m_ofsLog << "In terms of CPU time, this is ";
      if (dDuration > dRunLength)
      {
         m_ofsOut << setprecision(3) << dDuration / dRunLength << " x slower than reality" << endl;
         m_ofsLog << setprecision(3) << dDuration / dRunLength << " x slower than reality" << endl;
      }
      else
      {
         m_ofsOut << setprecision(3) << dRunLength / dDuration << " x faster than reality" << endl;
         m_ofsLog << setprecision(3) << dRunLength / dDuration << " x faster than reality" << endl;
      }
   }

   // Now calculate actual time of run (only really useful if run is a background batch job e.g. under Unix)
   time(&m_tSysEndTime);

   // Calculate run time
   double dDuration = difftime(m_tSysEndTime, m_tSysStartTime);

   // And write run time out to m_ofsOut and m_ofsLog
   m_ofsOut << "Run time elapsed: " << strDispTime(dDuration, false, true);
   m_ofsLog << "Run time elapsed: " << strDispTime(dDuration, false, true);

   // Calculate run time per iteration
   double dPerIter = dDuration / static_cast<double>(m_ulTotIter);

   // And write run time per iteration to m_ofsOut and m_ofsLog
   m_ofsOut << std::fixed << " (" << setprecision(4) << dPerIter << " per iteration)" << endl;
   m_ofsLog << std::fixed << " (" << setprecision(4) << dPerIter << " per iteration)" << endl;

   // Calculate ratio of run time to time simulated
   m_ofsOut << "In terms of run time, this is ";
   m_ofsLog << "In terms of run time, this is ";
   if (dDuration > dRunLength)
   {
      m_ofsOut << setprecision(3) << dDuration / dRunLength << " x slower than reality" << endl;
      m_ofsLog << setprecision(3) << dDuration / dRunLength << " x slower than reality" << endl;
   }
   else
   {
      m_ofsOut << setprecision(3) << dRunLength / dDuration << " x faster than reality" << endl;
      m_ofsLog << setprecision(3) << dRunLength / dDuration << " x faster than reality" << endl;
   }
}

//=========================================================================================================================================
//! This returns a string formatted as ddd:hh:mm:ss given a parameter in seconds, with rounding and fractions of a second if desired
//=========================================================================================================================================
string CSimulation::strDispTime(double const dTimeIn, bool const bRound, bool const bFrac)
{
   // Make sure no negative times
   double dTmpTime = tMax(dTimeIn, 0.0);

   if (bRound)
      dTmpTime = round(dTmpTime);

   string strTime;

   // Display hours
   if (dTmpTime >= 3600)
   {
      unsigned long ulHours = static_cast<unsigned long>(dTmpTime / 3600);
      dTmpTime -= (static_cast<double>(ulHours) * 3600);

      stringstream ststrTmp;
      ststrTmp << setfill('0') << setw(2) << ulHours;
      strTime.append(ststrTmp.str());
      strTime.append("h ");
   }
   else
      strTime.append("00h ");

   // Display minutes
   if (dTmpTime >= 60)
   {
      unsigned long ulMinutes = static_cast<unsigned long>(dTmpTime / 60);
      dTmpTime -= (static_cast<double>(ulMinutes) * 60);

      stringstream ststrTmp;
      ststrTmp << setfill('0') << setw(2) << ulMinutes;
      strTime.append(ststrTmp.str());
      strTime.append("m ");
   }
   else
      strTime.append("00m ");

   // Display seconds
   stringstream ststrTmp;
   ststrTmp << setfill('0') << setw(2) << static_cast<int>(dTmpTime);
   strTime.append(ststrTmp.str());

   if (bFrac)
   {
      // Fractions of a second
      strTime.append(".");
      ststrTmp.clear();
      ststrTmp.str(string());
      ststrTmp << setfill('0') << setw(2) << static_cast<int>(dTmpTime * 100) % 100;
      strTime.append(ststrTmp.str());
   }
   strTime.append("s");

   return strTime;
}

//=========================================================================================================================================
//! Returns the date and time on which the program was compiled
//=========================================================================================================================================
string CSimulation::strGetBuild(void)
{
   string strBuild = "(";
   strBuild.append(__TIME__);
   strBuild.append(" ");
   strBuild.append(__DATE__);
#if defined _DEBUG
   strBuild.append(" DEBUG");
#endif
   strBuild.append(" build)");
   return strBuild;
}

//=========================================================================================================================================
//! Returns the current timestep
//=========================================================================================================================================
double CSimulation::dGetTimeStep(void) const
{
   return m_dTimeStep;
}

//=========================================================================================================================================
//! Wrapper around dGetRand0Gaussian(), needed for cell surface water initialisation
//=========================================================================================================================================
double CSimulation::dGetRandGaussian(void)
{
   return dGetRand0Gaussian();
}

//=========================================================================================================================================
//! Displays information regarding the progress of the simulation
//=========================================================================================================================================
void CSimulation::AnnounceProgress(void)
{
   // Is stdout is connected to a tty?
   if (isatty(1))
   {
      // It isn't, so we are not running as a background job. First get current time
      time_t tNow = time(nullptr);

      // Calculate time elapsed and remaining
      m_dElapsed = difftime(tNow, m_tSysStartTime);
      m_dStillToGo = (m_dElapsed * m_dSimulationDuration / m_dSimulatedTimeElapsed) - m_dElapsed;

      // Tell the user about progress (note need to make several separate calls to cout here, or MS VC++ compiler appears to get confused)
      cout << SIMULATING << strDispTime(m_dSimulatedTimeElapsed, false, true);
      cout << std::fixed << setprecision(3) << setw(9) << 100 * m_dSimulatedTimeElapsed / m_dSimulationDuration;
      cout << "%   (elapsed " << strDispTime(m_dElapsed, true, false) << " remaining ";
      cout << strDispTime(m_dStillToGo, true, false) << ")  ";
      cout.flush();
   }
}

//=========================================================================================================================================
//! This routine checks for instability during the simulation: if any of the per-iteration totals are infeasibly large, the routine return an error code.
//=========================================================================================================================================
int CSimulation::nCheckForInstability(void) const
{
   // If an absurd per-cell average depth of sediment has been detached by flow during this iteration, then abort the run
   if (tAbs(m_dEndOfIterClayFlowDetach + m_dEndOfIterSiltFlowDetach + m_dEndOfIterSandFlowDetach) / static_cast<double>(m_ulNActiveCells) > ERROR_FLOW_DETACH_MAX)
      return (RTN_ERR_FLOWDETACHMAX);

   // If an absurd per-cell average depth of sediment has been deposited by flow during this iteration, then abort the run
   if (tAbs(m_dEndOfIterClayFlowDeposit + m_dEndOfIterSiltFlowDeposit + m_dEndOfIterSandFlowDeposit) / static_cast<double>(m_ulNActiveCells) > ERROR_FLOW_DEPOSIT_MAX)
      return (RTN_ERR_SEDLOADDEPOSITMAX);

   if (m_bSplashThisIter)
   {
      // We are calculating splash, and we are doing so this iteration. If an absurd per-cell average splash detachment has occurred this iteration, then abort the run
      if (tAbs(m_dEndOfIterClaySplashDetach + m_dEndOfIterSiltSplashDetach + m_dEndOfIterSandSplashDetach) / static_cast<double>(m_ulNActiveCells) > ERROR_SPLASH_DETACH_MAX)
         return (RTN_ERR_SPLASHDETMAX);

      // If an absurd per-cell average splash deposition has occurred this iteration, then abort the run
      if (tAbs(m_dEndOfIterClaySplashToSedLoad + m_dEndOfIterSiltSplashToSedLoad + m_dEndOfIterSandSplashToSedLoad) / static_cast<double>(m_ulNActiveCells) > ERROR_SPLASH_DEPOSIT_MAX)
         return (RTN_ERR_SPLASHDEPMAX);
   }

   if (m_bSlumpThisIter)
   {
      // We are calculating slump, and we are doing so this iteration. If an absurd per-cell average slump detachment has occurred this iteration, then abort the run
      if (tAbs(m_dEndOfIterClaySlumpDetach + m_dEndOfIterSiltSlumpDetach + m_dEndOfIterSandSlumpDetach) / static_cast<double>(m_ulNActiveCells) > ERROR_SLUMP_DETACH_MAX)
         return (RTN_ERR_SLUMPDETMAX);

      // If an absurd per-cell average topple detachment has occurred this iteration, then abort the run
      if (tAbs(m_dEndOfIterClayToppleDetach + m_dEndOfIterSiltToppleDetach + m_dEndOfIterSandToppleDetach) / static_cast<double>(m_ulNActiveCells) > ERROR_TOPPLE_DETACH_MAX)
         return (RTN_ERR_TOPPLEDETMAX);
   }

   // All OK
   return (RTN_OK);
}

//=========================================================================================================================================
//! Deterministically (i.e. using a known Z value) returns a probability from a cumulative unit Gaussian (normal) distribution. A numerical approximation to the normal distribution is used, this is from Abramowitz and Stegun's "Handbook of Mathematical Functions", Dover Publications, 1965 (originally published by the US National Bureau of Standards, 1964)
//=========================================================================================================================================
double CSimulation::dGetCGaussianPDF(double const dZ)
{
   double const b1 =  0.31938153;
   double const b2 = -0.356563782;
   double const b3 =  1.781477937;
   double const b4 = -1.821255978;
   double const b5 =  1.330274429;
   double const p  =  0.2316419;
   double const c2 =  0.3989423;

   if (dZ > 6)
      return (1);             // more than 6 SDs into the rh tail, just return 1

   if (dZ < -6)
      return (0);             // more than 6 SDs into the lh tail, just return 0

   double a = tAbs(dZ);
   double t = 1 / (1 + a * p);
   double b = c2 * exp((-dZ) * (dZ/2));
   double fn = ((((b5 * t + b4) * t + b3) * t + b2) * t + b1) * t;
   fn = 1 - b * fn;

   return ((dZ < 0) ? 1 - fn : fn);
}

//=========================================================================================================================================
//! Adds to the whole-grid this-iteration rain value
//=========================================================================================================================================
void CSimulation::AddToEndOfIterRain(double const dRain)
{
   m_dEndOfIterRain += dRain;
}

//=========================================================================================================================================
//! Adds to the whole-grid this-iteration run-on value
//=========================================================================================================================================
void CSimulation::AddToEndOfIterRunon(double const dRunon)
{
   m_dEndOfIterRunOn += dRunon;
}

//=========================================================================================================================================
//! Adds to the whole-grid stored subsurface water, for all cell layers, value
//=========================================================================================================================================
void CSimulation::AddToEndOfIterStoredSoilWater(double const dSoilWater)
{
   m_dEndOfIterTotSoilWater += dSoilWater;
}

//=========================================================================================================================================
//! Increments the whole-grid count of wet cells
//=========================================================================================================================================
void CSimulation::IncrNumWetCells(void)
{
   m_ulNWet++;
}

//=========================================================================================================================================
//! Adds to the whole-grid this-iteration stored surface water value
//=========================================================================================================================================
void CSimulation::AddToEndOfIterStoredSurfaceWater(double const dStoredWater)
{
   m_dEndOfIterTotSurfaceWater += dStoredWater;
}

//=========================================================================================================================================
//! Adds to the whole-grid this-iteration value for surface water lost from the grid
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSurfaceWaterLost(double const dLostWater)
{
   m_dEndOfIterSurfaceWaterOffEdge += dLostWater;
}

//=========================================================================================================================================
//! Adds to the whole-grid this-iteration value for infiltration during this iteration
//=========================================================================================================================================
void CSimulation::AddToEndOfIterInfiltration(double const dInfiltWater)
{
   m_dEndOfIterInfiltration += dInfiltWater;
}

//=========================================================================================================================================
//! Adds to the whole-grid this-iteration value for exfiltration during this iteration
//=========================================================================================================================================
void CSimulation::AddToEndOfIterExfiltration(double const dExfiltWater)
{
   m_dEndOfIterExfiltration += dExfiltWater;
}

//=========================================================================================================================================
//! This member function of CSimulation adds to the end-of-iteration sediment lost from grid values for clay, silt, and sand
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSedimentOffEdge(double const dClaySedOffEdge, double const dSiltSedOffEdge, double const dSandSedOffEdge)
{
   m_dEndOfIterClaySedLoadOffEdge += dClaySedOffEdge;
   m_dEndOfIterSiltSedLoadOffEdge += dSiltSedOffEdge;
   m_dEndOfIterSandSedLoadOffEdge += dSandSedOffEdge;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration flow detachment load value for clay
//=========================================================================================================================================
void CSimulation::AddToEndOfIterClayFlowDetach(double const dClayDiff)
{
   m_dEndOfIterClayFlowDetach += dClayDiff;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration flow detachment value for silt
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSiltFlowDetach(double const dSiltDiff)
{
   m_dEndOfIterSiltFlowDetach += dSiltDiff;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration flow detachment value for sand
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSandFlowDetach(double const dSandDiff)
{
   m_dEndOfIterSandFlowDetach += dSandDiff;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration flow deposition load value for clay
//=========================================================================================================================================
void CSimulation::AddToEndOfIterClayFlowDeposit(double const dClayDiff)
{
   m_dEndOfIterClayFlowDeposit += dClayDiff;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration flow deposition value for silt
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSiltFlowDeposit(double const dSiltDiff)
{
   m_dEndOfIterSiltFlowDeposit += dSiltDiff;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration flow deposition value for sand
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSandFlowDeposit(double const dSandDiff)
{
   m_dEndOfIterSandFlowDeposit += dSandDiff;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration splash detachment load value for clay
//=========================================================================================================================================
void CSimulation::AddToEndOfIterClaySplashDetach(double const dClayDiff)
{
   m_dEndOfIterClaySplashDetach += dClayDiff;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration splash detachment value for silt
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSiltSplashDetach(double const dSiltDiff)
{
   m_dEndOfIterSiltSplashDetach += dSiltDiff;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration splash detachment value for sand
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSandSplashDetach(double const dSandDiff)
{
   m_dEndOfIterSandSplashDetach += dSandDiff;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration splash deposition load value for clay
//=========================================================================================================================================
void CSimulation::AddToEndOfIterClaySplashDeposit(double const dClayDiff)
{
   m_dEndOfIterClaySplashDeposit += dClayDiff;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration splash deposition value for silt
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSiltSplashDeposit(double const dSiltDiff)
{
   m_dEndOfIterSiltSplashDeposit += dSiltDiff;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration splash deposition value for sand
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSandSplashDeposit(double const dSandDiff)
{
   m_dEndOfIterSandSplashDeposit += dSandDiff;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration splash off-edge value for clay
//=========================================================================================================================================
void CSimulation::AddToEndOfIterClaySplashOffEdge(double const dClayDiff)
{
   m_dEndOfIterClaySplashOffEdge += dClayDiff;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration splash off-edge value for silt
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSiltSplashOffEdge(double const dClayDiff)
{
   m_dEndOfIterSiltSplashOffEdge += dClayDiff;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration splash off-edge value for sand
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSandSplashOffEdge(double const dClayDiff)
{
   m_dEndOfIterSandSplashOffEdge += dClayDiff;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration slump detachment value for clay
//=========================================================================================================================================
void CSimulation::AddToEndOfIterClaySlumpDetach(double const dClay)
{
   m_dEndOfIterClaySlumpDetach += dClay;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration slump detachment value for silt
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSiltSlumpDetach(double const dSilt)
{
   m_dEndOfIterSiltSlumpDetach += dSilt;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration slump detachment value for sand
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSandSlumpDetach(double const dSand)
{
   m_dEndOfIterSandSlumpDetach += dSand;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration slump deposition value for clay
//=========================================================================================================================================
void CSimulation::AddToEndOfIterClaySlumpDeposit(double const dClay)
{
   m_dEndOfIterClaySlumpDeposit += dClay;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration slump deposition value for silt
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSiltSlumpDeposit(double const dSilt)
{
   m_dEndOfIterSiltSlumpDeposit += dSilt;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration slump deposition value for sand
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSandSlumpDeposit(double const dSand)
{
   m_dEndOfIterSandSlumpDeposit += dSand;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration topple detachment value for clay
//=========================================================================================================================================
void CSimulation::AddToEndOfIterClayToppleDetach(double const dClay)
{
   m_dEndOfIterClayToppleDetach += dClay;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration topple detachment value for silt
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSiltToppleDetach(double const dSilt)
{
   m_dEndOfIterSiltToppleDetach += dSilt;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration topple detachment value for sand
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSandToppleDetach(double const dSand)
{
   m_dEndOfIterSandToppleDetach += dSand;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration topple deposition value for clay
//=========================================================================================================================================
void CSimulation::AddToEndOfIterClayToppleDeposit(double const dClay)
{
   m_dEndOfIterClayToppleDeposit += dClay;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration topple deposition value for silt
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSiltToppleDeposit(double const dSilt)
{
   m_dEndOfIterSiltToppleDeposit += dSilt;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration topple deposition value for sand
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSandToppleDeposit(double const dSand)
{
   m_dEndOfIterSandToppleDeposit += dSand;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration infiltration deposition value for clay
//=========================================================================================================================================
void CSimulation::AddToEndOfIterClayInfiltDeposit(double const dClay)
{
   m_dEndOfIterClayInfiltDeposit += dClay;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration infiltration deposition value for silt
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSiltInfiltDeposit(double const dSilt)
{
   m_dEndOfIterSiltInfiltDeposit += dSilt;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration infiltration deposition value for sand
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSandInfiltDeposit(double const dSand)
{
   m_dEndOfIterSandInfiltDeposit += dSand;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration net (i.e. all-process detachment - all-process deposition) clay detachment
//=========================================================================================================================================
void CSimulation::AddToEndOfIterNetClayDetach(double const dClay)
{
   m_dEndOfIterNetClayDetachment += dClay;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration net (i.e. all-process detachment - all-process deposition) silt detachment
//=========================================================================================================================================
void CSimulation::AddToEndOfIterNetSiltDetach(double const dSilt)
{
   m_dEndOfIterNetSiltDetachment += dSilt;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration net (i.e. all-process detachment - all-process deposition) sand detachment
//=========================================================================================================================================
void CSimulation::AddToEndOfIterNetSandDetach(double const dSand)
{
   m_dEndOfIterNetSandDetachment += dSand;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration sediment load for clay
//=========================================================================================================================================
void CSimulation::AddToEndOfIterClaySedLoad(double const dClay)
{
   // TEST Kahan summation
   double y = dClay - m_dEndOfIterClaySedLoadKahanCorrection;
   volatile double t = m_dEndOfIterClaySedLoad + y;
   volatile double z = t - m_dEndOfIterClaySedLoad;
   m_dEndOfIterClaySedLoadKahanCorrection = z - y;
   m_dEndOfIterClaySedLoad = t;

   //m_dEndOfIterClaySedLoad += dClay;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration sediment load for silt
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSiltSedLoad(double const dSilt)
{
   // TEST Kahan summation
   double y = dSilt - m_dEndOfIterSiltSedLoadKahanCorrection;
   volatile double t = m_dEndOfIterSiltSedLoad + y;
   volatile double z = t - m_dEndOfIterSiltSedLoad;
   m_dEndOfIterSiltSedLoadKahanCorrection = z - y;
   m_dEndOfIterSiltSedLoad = t;

   //m_dEndOfIterSiltSedLoad += dSilt;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration sediment load for sand
//=========================================================================================================================================
void CSimulation::AddToEndOfIterSandSedLoad(double const dSand)
{
   // TEST Kahan summation
   double y = dSand - m_dEndOfIterSandSedLoadKahanCorrection;
   volatile double t = m_dEndOfIterSandSedLoad + y;
   volatile double z = t - m_dEndOfIterSandSedLoad;
   m_dEndOfIterSandSedLoadKahanCorrection = z - y;
   m_dEndOfIterSandSedLoad = t;

   //m_dEndOfIterSandSedLoad += dSand;
}

//=========================================================================================================================================
//! Adds to the end-of-iteration total elevation
//=========================================================================================================================================
void CSimulation::AddToEndOfIterTotalElev(double const dElev)
{
   m_dEndOfIterTotElev += dElev;
}

//========================================================================================================================================
//! Adds to the pre-simulation whole-grid values for elevation and soil water
//=========================================================================================================================================
void CSimulation::GetPreSimulationValues(double const dElev,double const dSoilWater)
{
   m_dStartOfIterTotElev         += dElev;
   m_dStartOfIterTotSoilWater    += dSoilWater;
}

//=========================================================================================================================================
//!  This member function of CSimulation outputs Lawrence's (1997) lambda - friction factor (calculated as a cubic spline) for checking purposes
//=========================================================================================================================================
void CSimulation::CheckLawrenceFF(void)
{
   // Put together file name for CSV friction factor check file
   string strFilePathName = m_strOutputPath;
   strFilePathName.append(FRICTION_FACTOR_CHECK);
   strFilePathName.append(CSV_EXT);

   // Create an ofstream object then try to open file for output
   ofstream ofsFF;
   ofsFF.open(strFilePathName.c_str(), ios::out | ios::trunc);

   // Did it open OK?
   if (! ofsFF.is_open())
   {
      // Error: cannot open friction factor check file for output
      cerr << ERR << "cannot open " << strFilePathName << " for output" << endl;
      return;
   }

   ofsFF << std::scientific;
   ofsFF << setprecision(6);

   ofsFF << "lambda\t,\tFF" << endl;
   double dLambda = 1e-6;                                             // Arbitrarily small
   while (dLambda <= 1000)                                            // Arbitrarily large
   {
      double dFF = dCalcLawrenceFrictionFactor(0, 0, dLambda, true);
      ofsFF << dLambda << ",\t" << dFF << endl;
      dLambda *= 1.1;
   }

   // Close file
   ofsFF.close();
}

//=========================================================================================================================================
//! This calculates and displays process statistics
//=========================================================================================================================================
void CSimulation::CalcProcessStats(void)
{
   string const NA = "Not available";

   m_ofsOut << endl;
   m_ofsOut << "Process statistics" << endl;
   m_ofsOut << "------------------" << endl;

// #if defined _MSC_VER
//    // First, find out which version of Windows we are running under
//    OSVERSIONINFOEX osvi;
//    BOOL bOsVersionInfoEx;
//
//    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));              // fill this much memory with zeros
//    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
//
//    if (! (bOsVersionInfoEx = GetVersionEx(static_cast<OSVERSIONINFO*<(&osvi))))
//    {
//       // OSVERSIONINFOEX didn't work so try OSVERSIONINFO instead
//       osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
//
//       if (! GetVersionEx(static_cast<OSVERSIONINFO*<(&osvi)))
//       {
//          // That didn't work either, too risky to proceed so give up
//          m_ofsOut << NA << endl;
//          return;
//       }
//    }
//
//    // OK, we have Windows version so display it
//    m_ofsOut << "Running under                                \t: ";
//    switch (osvi.dwPlatformId)
//    {
//       case VER_PLATFORM_WIN32_NT:
//          if (osvi.dwMajorVersion <= 4)
//             m_ofsOut << "Windows NT ";
//          else if (5 == osvi.dwMajorVersion && 0 == osvi.dwMinorVersion)
//             m_ofsOut << "Windows 2000 ";
//          else if (5 == osvi.dwMajorVersion && 1 == osvi.dwMinorVersion)
//             m_ofsOut << "Windows XP ";
//          else if (6 == osvi.dwMajorVersion && 0 == osvi.dwMinorVersion)
//             m_ofsOut << "Windows Vista ";
//          // TODO add info for other Windows version
//          else
//             m_ofsOut << "unknown Windows version ";
//
//          // Display version, service pack (if any), and build number
//          if (osvi.dwMajorVersion <= 4)
//             // TODO does this still work on 64-bit platforms?
//             m_ofsOut << "version " << osvi.dwMajorVersion << "." << osvi.dwMinorVersion << " " << osvi.szCSDVersion << " (Build " << (osvi.dwBuildNumber & 0xFFFF) << ")" << endl;
//          else
//             // TODO does this still work on 64-bit platforms?
//             m_ofsOut << osvi.szCSDVersion << " (Build " << (osvi.dwBuildNumber & 0xFFFF) << ")" << endl;
//          break;
//
//       case VER_PLATFORM_WIN32_WINDOWS:
//          if (4 == osvi.dwMajorVersion && 0 == osvi.dwMinorVersion)
//          {
//              m_ofsOut << "Windows 95";
//              if ('C' == osvi.szCSDVersion[1] || 'B' == osvi.szCSDVersion[1])
//                 m_ofsOut << " OSR2";
//              m_ofsOut << endl;
//          }
//          else if (4 == osvi.dwMajorVersion && 10 == osvi.dwMinorVersion)
//          {
//              m_ofsOut << "Windows 98";
//              if ('A' == osvi.szCSDVersion[1])
//                 m_ofsOut << "SE";
//              m_ofsOut << endl;
//          }
//          else if (4 == osvi.dwMajorVersion && 90 == osvi.dwMinorVersion)
//              m_ofsOut << "Windows Me" << endl;
//          else
//              m_ofsOut << "unknown 16-bit Windows version " << endl;
//          break;
//
//       case VER_PLATFORM_WIN32s:
//          m_ofsOut << "Win32s" << endl;
//          break;
//    }
//
//    // Now get process timimgs: this only works under 32-bit windows
//    if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId )
//    {
//       FILETIME ftCreate, ftExit, ftKernel, ftUser;
//       if (GetProcessTimes(GetCurrentProcess(), &ftCreate, &ftExit, &ftKernel, &ftUser))
//       {
//          ULARGE_INTEGER ul;
//          ul.LowPart = ftUser.dwLowDateTime;
//          ul.HighPart = ftUser.dwHighDateTime;
//          m_ofsOut << "Time spent executing user code               \t: " << strDispTime(static_cast<double>(ul.QuadPart) * 1e-7, false, true) << endl;
//          ul.LowPart = ftKernel.dwLowDateTime;
//          ul.HighPart = ftKernel.dwHighDateTime;
//          m_ofsOut << "Time spent executing kernel code             \t: " << strDispTime(static_cast<double>(ul.QuadPart) * 1e-7, false, true) << endl;
//       }
//    }
//    else
//       m_ofsOut << "Process timings                              \t: " << NA << endl;
//
//    // Finally get more process statistics: this needs psapi.dll, so only proceed if it is present on this system
//    HINSTANCE hDLL = LoadLibrary("psapi.dll");
//    if (hDLL != NULL)
//    {
//       // The dll has been found
//       typedef BOOL (__stdcall* DLLPROC) (HANDLE, PPROCESS_MEMORY_COUNTERS, DWORD);
//       DLLPROC ProcAdd;
//
//       // Try to get the address of the function we will call
//       ProcAdd = static_cast<DLLPROC>(GetProcAddress(hDLL, "GetProcessMemoryInfo"));
//       if (ProcAdd)
//       {
//          // Address was found
//          PROCESS_MEMORY_COUNTERS pmc;
//
//          // Now call the function
//          if ((ProcAdd) (GetCurrentProcess(), &pmc, sizeof(pmc)))
//          {
//             m_ofsOut << "Peak working set size                        \t: " << pmc.PeakWorkingSetSize / (1024.0 * 1024.0) << " Mb" << endl;
//             m_ofsOut << "Current working set size                     \t: " << pmc.WorkingSetSize / (1024.0 * 1024.0) << " Mb" << endl;
//             m_ofsOut << "Peak paged pool usage                        \t: " << pmc.QuotaPeakPagedPoolUsage / (1024.0 * 1024.0) << " Mb" << endl;
//             m_ofsOut << "Current paged pool usage                     \t: " << pmc.QuotaPagedPoolUsage / (1024.0 * 1024.0) << " Mb" << endl;
//             m_ofsOut << "Peak non-paged pool usage                    \t: " << pmc.QuotaPeakNonPagedPoolUsage / (1024.0 * 1024.0) << " Mb" << endl;
//             m_ofsOut << "Current non-paged pool usage                 \t: " << pmc.QuotaNonPagedPoolUsage / (1024.0 * 1024.0) << " Mb" << endl;
//             m_ofsOut << "Peak pagefile usage                          \t: " << pmc.PeakPagefileUsage / (1024.0 * 1024.0) << " Mb" << endl;
//             m_ofsOut << "Current pagefile usage                       \t: " << pmc.PagefileUsage / (1024.0 * 1024.0) << " Mb" << endl;
//             m_ofsOut << "No. of page faults                           \t: " << pmc.PageFaultCount << endl;
//          }
//       }
//
//       // Free the memory used by the dll
//       FreeLibrary(hDLL);
//    }

#if defined __GNUG__
   rusage ru;
   if (getrusage(RUSAGE_SELF, &ru) >= 0)
   {
      m_ofsOut << "Time spent executing user code               \t: "  << strDispTime(static_cast<double>(ru.ru_utime.tv_sec), false, true) << endl;
//      m_ofsOut << "ru_utime.tv_usec                             \t: " << ru.ru_utime.tv_usec << endl;
      m_ofsOut << "Time spent executing kernel code             \t: " << strDispTime(static_cast<double>(ru.ru_stime.tv_sec), false, true) << endl;
//      m_ofsOut << "ru_stime.tv_usec                             \t: " << ru.ru_stime.tv_usec << endl;
//      m_ofsOut << "Maximum resident set size                    \t: " << ru.ru_maxrss/1024.0 << " Mb" << endl;
//      m_ofsOut << "ixrss (???)                                  \t: " << ru.ru_ixrss << endl;
//      m_ofsOut << "Sum of rm_asrss (???)                        \t: " << ru.ru_idrss << endl;
//      m_ofsOut << "isrss (???)                                  \t: " << ru.ru_isrss << endl;
      m_ofsOut << "No. of page faults not requiring physical I/O\t: " << ru.ru_minflt << endl;
      m_ofsOut << "No. of page faults requiring physical I/O    \t: " << ru.ru_majflt << endl;
//      m_ofsOut << "No. of times swapped out of main memory      \t: " << ru.ru_nswap << endl;
//      m_ofsOut << "No. of times performed input (read request)  \t: " << ru.ru_inblock << endl;
//      m_ofsOut << "No. of times performed output (write request)\t: " << ru.ru_oublock << endl;
//      m_ofsOut << "No. of signals received                      \t: " << ru.ru_nsignals << endl;
      m_ofsOut << "No. of voluntary context switches            \t: " << ru.ru_nvcsw << endl;
      m_ofsOut << "No. of involuntary context switches          \t: " << ru.ru_nivcsw << endl;
   }
   else
      m_ofsOut << NA << endl;
#else
   m_ofsOut << NA << endl;
#endif

}

//=========================================================================================================================================
//! Returns an error message given an error code
//=========================================================================================================================================
char const* CSimulation::pszGetErrorText(int const nErr)
{
   // Note this means that what is pointed to is const, though the pointer itself may change
   static char const* strErr;

   switch (nErr)
   {
   case RTN_USERABORT:
      strErr = "aborted by user";
      break;
   case RTN_ERR_BADPARAM:
      strErr = "error in command-line parameter";
      break;
   case RTN_ERR_INI:
      strErr = "error reading initialization file";
      break;
   case RTN_ERR_RUNDATA:
      strErr = "error reading run details file";
      break;
   case RTN_ERR_LOGFILE:
      strErr = "error creating log file";
      break;
   case RTN_ERR_TSFILE:
      strErr = "error creating time series file";
      break;
   case RTN_ERR_DEMFILE:
      strErr = "error reading DEM file";
      break;
   case RTN_ERR_MEMALLOC:
      strErr = "error allocating memory";
      break;
   case RTN_ERR_GISOUTPUTFORMAT:
      strErr = "problem with GIS output format";
      break;
   case RTN_ERR_RAIN_VARIATION_FILE:
      strErr = "error reading rain variation file";
      break;
   case RTN_ERR_TEXTFILEWRITE:
      strErr = "error writing text output file";
      break;
   case RTN_ERR_GISFILEWRITE:
      strErr = "error writing GIS output file";
      break;
   case RTN_ERR_TSFILEWRITE:
      strErr = "error writing time series output file";
      break;
   case RTN_ERR_SPLASH_ATTENUATION:
      strErr = "error reading water depth-splash attenuation file";
      break;
   case RTN_ERR_OUTFILE:
      strErr = "error creating text output file";
      break;
   case RTN_ERR_RGDIR:
      strErr = "error in directory name";
      break;
   case RTN_ERR_FLOWDETACHMAX:
      strErr = "unstable simulation, unrealistically high flow detachment";
      break;
   case RTN_ERR_SEDLOADDEPOSITMAX:
      strErr = "unstable simulation, unrealistically high flow deposition";
      break;
   case RTN_ERR_SPLASHDETMAX:
      strErr = "unstable simulation, unrealistically high splash detachment";
      break;
   case RTN_ERR_SPLASHDEPMAX:
      strErr = "unstable simulation, unrealistically high splash deposition";
      break;
   case RTN_ERR_SLUMPDETMAX:
      strErr = "unstable simulation, unrealistically high slump detachment";
      break;
   case RTN_ERR_TOPPLEDETMAX:
      strErr = "unstable simulation, unrealistically high toppling detachment";
      break;
   case RTN_ERR_RAINFALLTSFILE:
      strErr = "error in rainfall time-series file";
      break;
   default:
      // should never get here
      strErr = "unknown error";
   }

   return (strErr);
}

//=========================================================================================================================================
//! Notifies the user that the simulation has ended, asks for keypress if necessary, and if compiled under GNU can send an email
//=========================================================================================================================================
void CSimulation::DoEndRun(int const nRtn)
{
   switch (nRtn)
   {
   case (RTN_OK):
      // Normal ending
      cout << RUN_END_NOTICE << ctime(&m_tSysEndTime);
      break;

   case (RTN_HELPONLY):
      cout << "End of help-only run" << endl;
      return;

   case (RTN_CHECKONLY):
      cout << "End of check-only run" << endl;
      return;

   default:
      // Aborting because of some error
      time(&m_tSysEndTime);
      cout << ERROR_NOTICE << nRtn << " (" << pszGetErrorText(nRtn) << ") at " << ctime(&m_tSysEndTime);

      if (m_ofsLog && m_ofsLog.is_open())
      {
         m_ofsLog << ERR << "run aborted (error code " << nRtn << "): " << pszGetErrorText(nRtn) << " at " << ctime(&m_tSysEndTime);
         m_ofsLog.flush();
      }

      if (m_ofsOut && m_ofsOut.is_open())
      {
         m_ofsOut << ERR << "run aborted (error code " << nRtn << "): " << pszGetErrorText(nRtn) << " at " << ctime(&m_tSysEndTime);
         m_ofsOut.flush();
      }
   }

#if defined __GNUG__
   if (isatty(1))
   {
      // Stdout is connected to a tty, so not running as a background job
      cout << endl << PRESS_KEY;
      cout.flush();
      getchar();
   }
   else
   {
      // Stdout is not connected to a tty, so must be running in the background; if we have something entered for the email address, then send an email
      if (! m_strMailAddress.empty())
      {
         cout << SEND_EMAIL << m_strMailAddress << endl;

         string strCmd = "echo \"";
         time_t tNow;
         time(&tNow);

         if (RTN_OK == nRtn)
         {
            // Finished normally
            strCmd.append("Simulation ");
            strCmd.append(m_strRunName);
            strCmd.append(", running on ");
            strCmd.append(strGetComputerName());
            strCmd.append(", completed normally on ");
            strCmd.append(ctime(&tNow));
            strCmd.append("\" | mail -s \"");
            strCmd.append(PROGNAME);
            strCmd.append(": normal completion\" ");
            strCmd.append(m_strMailAddress);
         }
         else
         {
            // Error, so give some information to help debugging
            strCmd.append("Simulation ");
            strCmd.append(m_strRunName);
            strCmd.append(", running on ");
            strCmd.append(strGetComputerName());
            strCmd.append(", aborted with error code ");
            strCmd.append(to_string(nRtn));
            strCmd.append(": ");
            strCmd.append(pszGetErrorText(nRtn));
            strCmd.append(" at timestep ");
            strCmd.append(to_string(m_ulIter));
            strCmd.append(" (");
            strCmd.append(strDispTime(m_dSimulatedTimeElapsed, true, false));
            strCmd.append("). This message sent ");
            strCmd.append(ctime(&tNow));
            strCmd.append("\" | mail -s \"");
            strCmd.append(PROGNAME);
            strCmd.append(": ERROR\" ");
            strCmd.append(m_strMailAddress);
         }

         int nTries = 5;
         int nWaitBetweenTries = 5;
         for (int n = 0; n < nTries; n++)
         {
            // Send an email using Linux/Unix mail command
            int nRet = system(strCmd.c_str());

            // Did it send OK?
            if (WEXITSTATUS(nRet) == 0)
               break;
            else
            {
               cout << ERR << EMAIL_ERROR << endl;
               sleep(nWaitBetweenTries);
            }
         }
      }
   }
#endif
}

//=========================================================================================================================================
//! Returns the simulation-wide missing-value value
//=========================================================================================================================================
double CSimulation::dGetMissingValue(void) const
{
   return m_dMissingValue;
}

// //=========================================================================================================================================
// //! Returns the simulation-wide cell side length
// //=========================================================================================================================================
// double CSimulation::dGetCellSide(void) const
// {
//    return m_dCellSide;
// }
//
// //=========================================================================================================================================
// //! Returns the simulation-wide cell diagonal length
// //=========================================================================================================================================
// double CSimulation::dGetCellSideDiag(void) const
// {
//    return m_dCellDiag;
// }

//=========================================================================================================================================
//! Trims whitespace from the left side of a string, does not change the original string
//=========================================================================================================================================
string CSimulation::strTrimLeft(string const* pStrIn)
{
   if (pStrIn->empty())
      return "";

   size_t nStartpos = pStrIn->find_first_not_of(" \t");
   if (nStartpos == string::npos)
      // The string consists only of whitespace
      return "";
   else
      return pStrIn->substr(nStartpos);
}

//=========================================================================================================================================
//! Trims whitespace from the right side of a string, does not change the original string
//=========================================================================================================================================
string CSimulation::strTrimRight(string const* pStrIn)
{
   if (pStrIn->empty())
      return "";

   size_t nEndpos = pStrIn->find_last_not_of(" \t");
   if (nEndpos == string::npos)
      return *pStrIn;
   else
      return pStrIn->substr(0, nEndpos+1);
}

//=========================================================================================================================================
//! Trims whitespace from both sides of a string, does not change the original string
//=========================================================================================================================================
string CSimulation::strTrim(string const* pStrIn)
{
   if (pStrIn->empty())
      return "";

   string strTmp = *pStrIn;

   // Trim leading spaces
   size_t nFoundPos = strTmp.find_first_not_of(" \t");

   if (nFoundPos == string::npos)
      // The string consists only of whitespace
      return "";
   else
      strTmp = strTmp.substr(nFoundPos);

   // Trim trailing spaces
   nFoundPos = strTmp.find_last_not_of(" \t");

   if (nFoundPos != string::npos)
      strTmp.resize(nFoundPos+1);

   return strTmp;
}

//=========================================================================================================================================
//! Returns the lower case version of an string, leaving the original unchanged
//=========================================================================================================================================
string CSimulation::strToLower(string const* strIn)
{
   string strOut = *strIn;
   transform(strIn->begin(), strIn->end(), strOut.begin(), tolower);
   return strOut;
}

//=========================================================================================================================================
//! Returns a string with a substring removed
//=========================================================================================================================================
string CSimulation::strRemoveSubstr(string* strIn, string const* strSub)
{
   size_t nPos = strIn->find(*strSub);

   // If not found, return the string unchanged
   if (nPos != string::npos)
      strIn->replace(nPos, strSub->size(), "");

   return *strIn;
}

//=========================================================================================================================================
//! This function implements (approximately) Python's split() function, it is based upon an example at http://stackoverflow.com/questions/236129/split-a-string-in-c It 
//=========================================================================================================================================
vector<string> CSimulation::VstrSplit(string const* s, char const delim)
{
   vector<string> elems;
   stringstream ss(*s);
   string item;
   
   while (getline(ss, item, delim))
   {
      if (! item.empty())
         elems.push_back(item);
   }
   
   return elems;
}

//===============================================================================================================================
//! Converts double to string with specified number of places after the decimal. From https://stackoverflow.com/questions/14765155/how-can-i-easily-format-my-data-table-in-c
//===============================================================================================================================
string CSimulation::strDbl(double const dX, int const nDigits)
{
   stringstream ss;
   ss << std::fixed;
   ss.precision(nDigits);      // Set the number of places after decimal
   ss << dX;
   return ss.str();
}

//===============================================================================================================================
//! Converts double to string with specified number of decimal places, within a field of given width, pads with blank spaces to enforce right alignment. Modified from https://stackoverflow.com/questions/14765155/how-can-i-easily-format-my-data-table-in-c
//===============================================================================================================================
string CSimulation::strDblRight(double const dX, int const nDigits, int const nWidth, bool bShowZero)
{
   stringstream ss;
   ss << std::fixed << std::right;
   ss.fill(' ');
   ss.width(nWidth-1);

   if (bFpEQ(dX, 0.0, TOLERANCE) && (! bShowZero))
   {
      ss << "-";
   }
   else
   {
      ss.precision(nDigits);  // Set number of places after decimal
      ss << dX;
   }

   ss << " ";                 // Add a final space
   return ss.str();
}

//===============================================================================================================================
//! Converts int to string within a field of given width, pads with blank spaces to enforce alignment.. From https://stackoverflow.com/questions/14765155/how-can-i-easily-format-my-data-table-in-c
//===============================================================================================================================
string CSimulation::strIntRight(int const nX, int const nWidth)
{
   stringstream ss;
   ss << std::fixed << std::right;
   ss.fill(' ');              // Fill space around displayed number
   ss.width(nWidth-1);        // Set width around displayed number
   ss << nX;
   ss << " ";                 // Add a final space
   return ss.str();
}

//===============================================================================================================================
//! Centre-aligns string or char within a field of given width, pads with blank spaces to enforce alignment. From https://stackoverflow.com/questions/14765155/how-can-i-easily-format-my-data-table-in-c
//===============================================================================================================================
string CSimulation::strCentre(string const strIn, int const nWidth)
{
   stringstream ss, spaces;
   int nPadding = nWidth - static_cast<int>(strIn.size());

   for (int i = 0; i < nPadding / 2; ++i)
      spaces << " ";

   ss << spaces.str() << strIn << spaces.str();

   if (nPadding > 0 && nPadding % 2 != 0)       // If odd number, add one space
      ss << " ";

   return ss.str();
}

//===============================================================================================================================
//! Right-aligns string within a field of given width, pads with blank spaces to enforce alignment. From https://stackoverflow.com/questions/14765155/how-can-i-easily-format-my-data-table-in-c
//===============================================================================================================================
string CSimulation::strRight(string const strIn, int const nWidth)
{
   stringstream ss, spaces;
   int nPadding = nWidth - static_cast<int>(strIn.size()) - 1;
   for (int i = 0; i < nPadding; ++i)
      spaces << " ";
   ss << spaces.str() << strIn;
   ss << " ";
   return ss.str();
}

//===============================================================================================================================
//! Left-aligns string within a field of given width, pads with blank spaces to enforce alignment. From https://stackoverflow.com/questions/14765155/how-can-i-easily-format-my-data-table-in-c
//===============================================================================================================================
string CSimulation::strLeft(string const strIn, int const nWidth)
{
   stringstream ss, spaces;
   int nPadding = nWidth - static_cast<int>(strIn.size());
   for (int i = 0; i < nPadding; ++i)
      spaces << " ";
   ss << strIn << spaces.str();
   return ss.str();
}

//===============================================================================================================================
//! Calculates a percentage from two numbers then, if the result is non-zero, right-aligns the result as a string within a field of given width, pads with blank spaces to enforce alignment. From https://stackoverflow.com/questions/14765155/how-can-i-easily-format-my-data-table-in-c
//===============================================================================================================================
string CSimulation::strRightPerCent(double const d1, double const d2, int const nWidth)
{
   // Are either of the inputs zero?
   if (bFpEQ(d1, 0.0, TOLERANCE))
      return string(nWidth, SPACE);

   if (bFpEQ(d2, 0.0, TOLERANCE))
      return string(nWidth, SPACE);

   // Non-zero, so calculate the percentage
   double dResult = 100 * d1 / d2;

   char cBuffer[10];
   sprintf(cBuffer, "%+.2f", dResult);
   string strResult = cBuffer;

   stringstream ss, spaces;
   int nPadding = nWidth - static_cast<int>(strResult.size()) - 4;
   for (int i = 0; i < nPadding; ++i)
      spaces << " ";
   ss << spaces.str();
   ss << "(";
   ss << strResult;
   ss << "%)";
   ss << " ";
   return ss.str();
}

//=========================================================================================================================================
//! Creates the soil layers, note that this must be done before any overall gradient is imposed on the DEM
//=========================================================================================================================================
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

//=========================================================================================================================================
//! Calculates the timestep for the next iteration
//=========================================================================================================================================
void CSimulation::CalcTimestep(void)
{
   if (bFpEQ(m_dPossMaxSpeedNextIter, 0.0, TOLERANCE))
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

//=========================================================================================================================================
//! Checks to see if the simulation has gone on too long, also make sure that the clock() function does not roll over
//=========================================================================================================================================
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
   m_ulTotIter = static_cast<unsigned long>(round(m_dSimulationDuration / m_dTimeStep));

   // Check to see if we have done CLOCK_CHECK_ITERATIONS iterations: if so, it is time to reset the CPU time running total in case the clock()
   // function later rolls over
   if (0 == m_ulIter % CLOCK_CHECK_ITERATIONS)
      DoCPUClockReset();

   // Not yet time to quit
   return false;
}

//=========================================================================================================================================
//! Update grand totals at the end of each iteration (these are all volumes)
//=========================================================================================================================================
void CSimulation::UpdatePerIterGrandTotals(void)
{
   m_ldGTotRain         += (m_dEndOfIterRain * m_dCellSquare);
   m_ldGTotRunOn        += (m_dEndOfIterRunOn * m_dCellSquare);
   m_ldGTotWaterOffEdge += (m_dEndOfIterSurfaceWaterOffEdge * m_dCellSquare);
   m_ldGTotFlowDetach   += (m_dEndOfIterClayFlowDetach + m_dEndOfIterSiltFlowDetach + m_dEndOfIterSandFlowDetach) * m_dCellSquare;
   m_ldGTotFlowDeposit  += (m_dEndOfIterClayFlowDeposit + m_dEndOfIterSiltFlowDeposit + m_dEndOfIterSandFlowDeposit) * m_dCellSquare;
   m_ldGTotSedLoad      += (m_dEndOfIterClaySedLoad + m_dEndOfIterSiltSedLoad + m_dEndOfIterSandSedLoad) * m_dCellSquare;
   m_ldGTotFlowSedLoadOffEdge += (m_dEndOfIterClaySedLoadOffEdge + m_dEndOfIterSiltSedLoadOffEdge + m_dEndOfIterSandSedLoadOffEdge) * m_dCellSquare;

   if (m_bInfiltThisIter)
   {
      m_ldGTotInfilt        += (m_dEndOfIterInfiltration * m_dCellSquare);
      m_ldGTotExfilt        += (m_dEndOfIterExfiltration * m_dCellSquare);
      m_ldGTotInfiltDeposit += (m_dEndOfIterClayInfiltDeposit + m_dEndOfIterSiltInfiltDeposit + m_dEndOfIterSandInfiltDeposit) * m_dCellSquare;
   }

   if (m_bSplashThisIter)
   {
      m_ldGTotSplashDetach    += (m_dEndOfIterClaySplashDetach + m_dEndOfIterSiltSplashDetach + m_dEndOfIterSandSplashDetach) * m_dCellSquare;
      m_ldGTotSplashDeposit   += (m_dEndOfIterClaySplashDeposit + m_dEndOfIterSiltSplashDeposit + m_dEndOfIterSandSplashDeposit) * m_dCellSquare;
      m_ldGTotSplashToSedLoad += (m_dEndOfIterClaySplashToSedLoad + m_dEndOfIterSiltSplashToSedLoad + m_dEndOfIterSandSplashToSedLoad) * m_dCellSquare;
      m_ldGTotSplashOffEdge   += (m_dEndOfIterClaySplashOffEdge + m_dEndOfIterSiltSplashOffEdge + m_dEndOfIterSandSplashOffEdge) * m_dCellSquare;
   }

   if (m_bSlumpThisIter)
   {
      m_ldGTotSlumpDetach     += (m_dEndOfIterClaySlumpDetach + m_dEndOfIterSiltSlumpDetach + m_dEndOfIterSandSlumpDetach) * m_dCellSquare;
      m_ldGTotSlumpDeposit    += (m_dEndOfIterClaySlumpDeposit + m_dEndOfIterSiltSlumpDeposit + m_dEndOfIterSandSlumpDeposit) * m_dCellSquare;
      m_ldGTotSlumpToSedLoad  += (m_dEndOfIterClaySlumpToSedLoad + m_dEndOfIterSiltSlumpToSedLoad + m_dEndOfIterSandSlumpToSedLoad) * m_dCellSquare;

      m_ldGTotToppleDetach    += (m_dEndOfIterClayToppleDetach + m_dEndOfIterSiltToppleDetach + m_dEndOfIterSandToppleDetach) * m_dCellSquare;
      m_ldGTotToppleDeposit   += (m_dEndOfIterClayToppleDeposit + m_dEndOfIterSiltToppleDeposit + m_dEndOfIterSandToppleDeposit) * m_dCellSquare;
      m_ldGTotToppleToSedLoad += (m_dEndOfIterClayToppleToSedLoad + m_dEndOfIterSiltToppleToSedLoad + m_dEndOfIterSandToppleToSedLoad) * m_dCellSquare;
   }

   if (m_bHeadcutRetreatThisIter)
   {
      m_ldGTotHeadcutRetreatDetach  += (m_dEndOfIterClayHeadcutDetach + m_dEndOfIterSiltHeadcutDetach + m_dEndOfIterSandHeadcutDetach) * m_dCellSquare;
      m_ldGTotHeadcutRetreatDeposit += (m_dEndOfIterClayHeadcutDeposit + m_dEndOfIterSiltHeadcutDeposit + m_dEndOfIterSandHeadcutDeposit) * m_dCellSquare;
      m_ldGTotHeadcutRetreatToSedLoad += (m_dEndOfIterClayHeadcutToSedLoad + m_dEndOfIterSiltHeadcutToSedLoad + m_dEndOfIterSandHeadcutToSedLoad) * m_dCellSquare;
   }
}

//===============================================================================================================================
//! Calculates the change in DEM volume at the end of the simulation
//===============================================================================================================================
void CSimulation::CalcEndOfSimDEMChange(void)
{
   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      for (int nY = 0; nY < m_nYGridMax; nY++)
      {
         double dElevNow = m_Cell[nX][nY].pGetSoil()->dGetSoilSurfaceElevation();
         double dElevInit = m_Cell[nX][nY].dGetInitialSoilSurfaceElevation();

         m_dEndOfSimElevChange += (dElevNow - dElevInit);
      }
   }
}

#if defined _DEBUG
void CSimulation::DEBUGShowSedLoad(string const strIn)
{
   double dClaySedLoad = 0;
   double dSiltSedLoad = 0;
   double dSandSedLoad = 0;
   double dChangeInClaySedLoad = 0;
   double dChangeInSiltSedLoad = 0;
   double dChangeInSandSedLoad = 0;

   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      for (int nY = 0; nY < m_nYGridMax; nY++)
      {
         if (m_Cell[nX][nY].bIsMissingValue())
            continue;

         dClaySedLoad += (m_Cell[nX][nY].pGetSedLoad()->dGetLastIterClaySedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetClaySplashSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetClayFlowSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetClaySlumpSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetClayToppleSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetClayHeadcutRetreatSedLoad());

         dChangeInClaySedLoad += (m_Cell[nX][nY].pGetSedLoad()->dGetClaySplashSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetClayFlowSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetClaySlumpSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetClayToppleSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetClayHeadcutRetreatSedLoad());

         dSiltSedLoad += (m_Cell[nX][nY].pGetSedLoad()->dGetLastIterSiltSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetSiltSplashSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetSiltFlowSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetSiltSlumpSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetSiltToppleSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetSiltHeadcutRetreatSedLoad());

         dChangeInSiltSedLoad += (m_Cell[nX][nY].pGetSedLoad()->dGetSiltSplashSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetSiltFlowSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetSiltSlumpSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetSiltToppleSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetSiltHeadcutRetreatSedLoad());

         dSandSedLoad += (m_Cell[nX][nY].pGetSedLoad()->dGetLastIterSandSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetSandSplashSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetSandFlowSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetSandSlumpSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetSandToppleSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetSandHeadcutRetreatSedLoad());

         dChangeInSandSedLoad += (m_Cell[nX][nY].pGetSedLoad()->dGetSandSplashSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetSandFlowSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetSandSlumpSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetSandToppleSedLoad() + m_Cell[nX][nY].pGetSedLoad()->dGetSandHeadcutRetreatSedLoad());

         // if (! bFpEQ(m_Cell[nX][nY].pGetSedLoad()->dGetSandFlowSedLoad(), 0.0, 0.00000001))
         //    m_ofsLog << std::fixed << setprecision(15) << "[" << nX << "][" << nY << "] dGetSandFlowSedLoad() = " << m_Cell[nX][nY].pGetSedLoad()->dGetSandFlowSedLoad() << endl;

      }
   }

   m_ofsLog << "Sediment load " << strIn << endl;
   m_ofsLog << std::fixed << setprecision(0);
   m_ofsLog << "dClaySedLoad = " << dClaySedLoad * m_dCellSquare << " dSiltSedLoad = " << dSiltSedLoad * m_dCellSquare << " dSandSedLoad = " << dSandSedLoad * m_dCellSquare << endl;
   m_ofsLog << "dChangeInClaySedLoad = " << dChangeInClaySedLoad * m_dCellSquare << " dChangeInSiltSedLoad = " << dChangeInSiltSedLoad * m_dCellSquare << " dChangeInSandSedLoad = " << dChangeInSandSedLoad * m_dCellSquare << endl << endl;
}
#endif
