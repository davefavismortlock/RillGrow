/*========================================================================================================================================

 This is utils.cpp: utility routines for RillGrow

 Copyright (C) 2020 David Favis-Mortlock

 =========================================================================================================================================

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public  License as published
 by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation,
 Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

========================================================================================================================================*/
#include <sstream>
using std::stringstream;

#include <algorithm>
using std::transform;

#include "rg.h"
#include "simulation.h"
#include "cell.h"


/*========================================================================================================================================

 Handles command-line parameters

========================================================================================================================================*/
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

         string strDriverLong, strDriverShort;
         for (int i = 0; i < GDALGetDriverCount(); i++ )
         {
            GDALDriverH hDriver = GDALGetDriver(i);

            strDriverShort = GDALGetDriverShortName(hDriver);
            strDriverShort.append("          ");
            strDriverLong = GDALGetDriverLongName(hDriver);

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


/*========================================================================================================================================

 Tells the user that we have started the simulation

========================================================================================================================================*/
void CSimulation::AnnounceStart(void)
{
   cout << endl << PROGNAME << " for " << PLATFORM << " " << strGetBuild() << endl << endl;
}


/*========================================================================================================================================

 Starts the clock ticking

========================================================================================================================================*/
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


/*========================================================================================================================================

 Finds the current directory

========================================================================================================================================*/
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
   //   char* pResult = getcwd(szBuf, BUFFER_SIZE);          // Used to use this, but what if cwd is not the same as the CoastalME dir?

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
   int nPos = strTmp.find_last_of(PATH_SEPARATOR);
   m_strRGDir = strTmp.substr(0, nPos+1);            // Note that this must be terminated with a backslash

   return true;
}


/*========================================================================================================================================

 Tells the user about the licence

========================================================================================================================================*/
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
   cout << START_NOTICE << strGetComputerName() << " on " << ctime(&m_tSysStartTime);
   cout << INIT_NOTICE << endl;
}


/*========================================================================================================================================

 Tells the user that we are now reading DEM data

========================================================================================================================================*/
void CSimulation::AnnounceReadDEM(void) const
{
   // Tell the user what is happening
   cout << READ_DEM_FILE << m_strDEMFile << endl;
}


/*========================================================================================================================================

 Tells the user that we are now allocating memory

========================================================================================================================================*/
void CSimulation::AnnounceAllocateMemory(void)
{
   cout << ALLOCATE_MEMORY << endl;
}


/*========================================================================================================================================

 Tells the user that we are now reading the rainfall spatial variation file

========================================================================================================================================*/
void CSimulation::AnnounceReadRainVar(void) const
{
   cout << READ_RAIN_VARIATION_FILE << m_strRainVarMFile << endl;
}


/*========================================================================================================================================

 Tell the user that the simulation is now running

========================================================================================================================================*/
void CSimulation::AnnounceIsRunning(void)
{
   cout << RUN_NOTICE << endl;
}


/*========================================================================================================================================

 Wraps long lines written to the .out file (TODO is rather crude, improve)

========================================================================================================================================*/
void CSimulation::WrapLongString(string* pstrTemp)
{
   if (static_cast<int>(pstrTemp->size()) > OUTPUT_WIDTH)
   {
      m_ofsOut << *pstrTemp << endl << "                                                        \t: ";
      *pstrTemp = "";
   }
}


/*========================================================================================================================================

 Returns a string, hopefully giving the name of the computer on which the simulation is running

========================================================================================================================================*/
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


/*========================================================================================================================================

 Resets the CPU clock timer to prevent it 'rolling over', as can happen during long runs. This is a particularly problem under Unix systems where the value returned by clock() is defined in microseconds (for compatibility with systems that have CPU clocks with much higher resolution) i.e. CLOCKS_PER_SEC is 1000000 rather than the more usual 1000. In this case, the value returned from clock() will wrap around after accumulating only 2147 seconds of CPU time (about 36 minutes).

========================================================================================================================================*/
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


/*========================================================================================================================================

 Announce the end of the simulation

========================================================================================================================================*/
void CSimulation::AnnounceSimEnd(void)
{
   cout << endl << FINAL_OUTPUT << endl;
}


/*========================================================================================================================================

 Calculates and displays time elapsed in terms of CPU time and real time, also calculates time per iteration in terms of both CPU time
 and real time

========================================================================================================================================*/
void CSimulation::CalcTime(double const dRunLength)
{
   // Reset CPU count for last time
   DoCPUClockReset();

   if (m_dCPUClock != -1)
   {
      // Calculate CPU time in secs
      double dDuration = m_dCPUClock/CLOCKS_PER_SEC;

      // And write CPU time out to m_ofsOut and m_ofsLog
      m_ofsOut << "CPU time elapsed: " << strDispTime(dDuration, false, true);
      m_ofsLog << "CPU time elapsed: " << strDispTime(dDuration, false, true);

      // Calculate CPU time per iteration
      double dPerIter = dDuration / m_ulTotIter;

      // And write CPU time per iteration to m_ofsOut and m_ofsLog
      m_ofsOut << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << " (" << setprecision(4) << dPerIter << " per iteration)" << endl;
      m_ofsLog << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << " (" << setprecision(4) << dPerIter << " per iteration)" << endl;

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
   double dPerIter = dDuration / m_ulTotIter;

   // And write run time per iteration to m_ofsOut and m_ofsLog
   m_ofsOut << setiosflags(ios::fixed) << " (" << setprecision(4) << dPerIter << " per iteration)" << endl;
   m_ofsLog << setiosflags(ios::fixed) << " (" << setprecision(4) << dPerIter << " per iteration)" << endl;

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


/*========================================================================================================================================

strDispTime returns a string formatted as ddd:hh:mm:ss given a parameter in seconds, with rounding and fractions of a second if desired

========================================================================================================================================*/
string CSimulation::strDispTime(double const dTimeIn, bool const bRound, bool const bFrac)
{
   // Make sure no negative times
   double dTmpTime = tMax(dTimeIn, 0.0);

   if (bRound)
      dTmpTime = dRound(dTmpTime);

   string strTime;

   // Display hours
   if (dTmpTime >= 3600)
   {
      unsigned long ulHours = dTmpTime / 3600;
      dTmpTime -= (ulHours * 3600);

      stringstream ststrTmp;
      ststrTmp << FillToWidth('0', 2) << ulHours;
      strTime.append(ststrTmp.str());
      strTime.append("h ");
   }
   else
      strTime.append("00h ");

   // Display minutes
   if (dTmpTime >= 60)
   {
      unsigned long ulMinutes = dTmpTime / 60;
      dTmpTime -= (ulMinutes * 60);

      stringstream ststrTmp;
      ststrTmp << FillToWidth('0', 2) << ulMinutes;
      strTime.append(ststrTmp.str());
      strTime.append("m ");
   }
   else
      strTime.append("00m ");

   // Display seconds
   stringstream ststrTmp;
   ststrTmp << FillToWidth('0', 2) << static_cast<int>(dTmpTime);
   strTime.append(ststrTmp.str());

   if (bFrac)
   {
      // Fractions of a second
      strTime.append(".");
      ststrTmp.clear();
      ststrTmp.str(string());
      ststrTmp << FillToWidth('0', 2) << static_cast<int>(dTmpTime * 100) % 100;
      strTime.append(ststrTmp.str());
   }
   strTime.append("s");

   return strTime;
}


/*========================================================================================================================================

 Returns the date and time on which the program was compiled

========================================================================================================================================*/
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


/*========================================================================================================================================

 Displays information regarding the progress of the simulation

========================================================================================================================================*/
void CSimulation::AnnounceProgress(void)
{
   if (isatty(1))
   {
      // Stdout is connected to a tty, so not running as a background job
      static double sdElapsed = 0;
      static double sdToGo = 0;

      // Get current time
      time_t tNow = time(nullptr);

      // Calculate time elapsed and remaining
      sdElapsed = difftime(tNow, m_tSysStartTime);
      sdToGo = (sdElapsed * m_dSimulationDuration / m_dSimulatedTimeElapsed) - sdElapsed;

      // Tell the user about progress (note need to make several separate calls to cout here, or MS VC++ compiler appears to get confused)
      cout << SIMULATING << strDispTime(m_dSimulatedTimeElapsed, false, true);
      cout << setiosflags(ios::fixed) << setprecision(3) << setw(9) << 100 * m_dSimulatedTimeElapsed / m_dSimulationDuration;
      cout << "%   (elapsed " << strDispTime(sdElapsed, true, false) << " remaining ";
      cout << strDispTime(sdToGo, true, false) << ")  ";
      cout.flush();
   }
}


/*========================================================================================================================================

 This routine checks for instability during the simulation: if any of the per-iteration totals are infeasibly large, the routine return an error code.

========================================================================================================================================*/
int CSimulation::nCheckForInstability(void)
{
   // If an absurd per-cell average depth of sediment has been detached by flow during this iteration, then abort the run
   if (tAbs(m_dThisIterClayFlowDetach + m_dThisIterSiltFlowDetach + m_dThisIterSandFlowDetach) / m_ulNActiveCells > ERROR_FLOW_DETACH_MAX)
      return (RTN_ERR_FLOWDETACHMAX);

   // If an absurd per-cell average depth of sediment has been deposited by flow during this iteration, then abort the run
   if (tAbs(m_dThisIterClayFlowDeposit + m_dThisIterSiltFlowDeposit + m_dThisIterSandFlowDeposit) / m_ulNActiveCells > ERROR_FLOW_DEPOSIT_MAX)
      return (RTN_ERR_FLOWDEPOSITMAX);

   // If an absurd per-cell average depth of sediment is being transported by flow during this iteration, then abort the run
   if (tAbs(m_dThisIterClaySedLoad + m_dThisIterSiltSedLoad + m_dThisIterSandSedimentLoad) / m_ulNActiveCells > ERROR_SEDIMENT_TRANSPORT_MAX)
      return (RTN_ERR_TRANSPORTMAX);

   if (m_bSplashThisIter)
   {
      // We are calculating splash, and we are doing so this iteration. If an absurd per-cell average splash detachment has occurred this iteration, then abort the run
      if (tAbs(m_dThisIterClaySplashDetach + m_dThisIterSiltSplashDetach + m_dThisIterSandSplashDetach) / m_ulNActiveCells > ERROR_SPLASH_DETACH_MAX)
         return (RTN_ERR_SPLASHDETMAX);

      // If an absurd per-cell average splash deposition has occurred this iteration, then abort the run
      if (tAbs(m_dThisIterClaySplashDeposit + m_dThisIterSiltSplashDeposit + m_dThisIterSandSplashDeposit) / m_ulNActiveCells > ERROR_SPLASH_DEPOSIT_MAX)
         return (RTN_ERR_SPLASHDEPMAX);
   }

   if (m_bSlumpThisIter)
   {
      // We are calculating slump, and we are doing so this iteration. If an absurd per-cell average slump detachment has occurred this iteration, then abort the run
      if (tAbs(m_dThisIterClaySlumpDetach + m_dThisIterSiltSlumpDetach + m_dThisIterSandSlumpDetach) / m_ulNActiveCells > ERROR_SLUMP_DETACH_MAX)
         return (RTN_ERR_SLUMPDETMAX);

      // If an absurd per-cell average topple detachment has occurred this iteration, then abort the run
      if (tAbs(m_dThisIterClayToppleDetach + m_dThisIterSiltToppleDetach + m_dThisIterSandToppleDetach) / m_ulNActiveCells > ERROR_TOPPLE_DETACH_MAX)
         return (RTN_ERR_TOPPLEDETMAX);
   }

   // All OK
   return (RTN_OK);
}


/*========================================================================================================================================

 Deterministically (i.e. using a known Z value) returns a probability from a cumulative unit Gaussian (normal) distribution. A numerical approximation to the normal distribution is used, this is from Abramowitz and Stegun's "Handbook of Mathematical Functions", Dover Publications, 1965 (originally published by the US National Bureau of Standards, 1964)

========================================================================================================================================*/
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


/*========================================================================================================================================

  This member function of CSimulation outputs lambda - friction factor (calculated as a cubic spline) for checking purposes

========================================================================================================================================*/
void CSimulation::CheckFF(void)
{
   // Put together file name for CSV friction factor check file
   string strFilePathName = m_strOutputPath;
   strFilePathName.append(FRICTION_FACTOR_CHECK);
   strFilePathName.append(CSV_EXT);

   // Create an ofstream object then try to open file for output
   ofstream FFStream;
   FFStream.open(strFilePathName.c_str(), ios::out | ios::trunc);

   // Did it open OK?
   if (! FFStream.is_open())
   {
      // Error: cannot open friction factor check file for output
      cerr << ERR << "cannot open " << strFilePathName << " for output" << endl;
      return;
   }

   FFStream << setiosflags(ios::scientific);
   FFStream << setprecision(6);

//   FFStream << "lambda\t,\tFF" << endl;
   double dLambda = 1e-6;                                             // Arbitrarily small
   while (dLambda <= 1000)                                           // Arbitrarily large
   {
      double fFF = dCalcFrictionFactor(0, 0, dLambda, true);          // 1 / sqrt(friction factor)
      FFStream << dLambda << "\t,\t" << 1 / (fFF * fFF) << endl;
      dLambda *= 1.1;
   }

   // Close file
   FFStream.close();
}


/*========================================================================================================================================

 This calculates and displays process statistics

========================================================================================================================================*/
void CSimulation::CalcProcessStats(void)
{
   string const      NA = "Not available";

   m_ofsOut << endl;
   m_ofsOut << "Process statistics" << endl;
   m_ofsOut << "------------------" << endl;

#if defined _MSC_VER
   // First, find out which version of Windows we are running under
   OSVERSIONINFOEX osvi;
   BOOL bOsVersionInfoEx;

   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));              // fill this much memory with zeros
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   if (! (bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *) &osvi)))
   {
      // OSVERSIONINFOEX didn't work so try OSVERSIONINFO instead
      osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

      if (! GetVersionEx((OSVERSIONINFO *) &osvi))
      {
         // That didn't work either, too risky to proceed so give up
         m_ofsOut << NA << endl;
         return;
      }
   }

   // OK, we have Windows version so display it
   m_ofsOut << "Running under                                \t: ";
   switch (osvi.dwPlatformId)
   {
      case VER_PLATFORM_WIN32_NT:
         if (osvi.dwMajorVersion <= 4)
            m_ofsOut << "Windows NT ";
         else if (5 == osvi.dwMajorVersion && 0 == osvi.dwMinorVersion)
            m_ofsOut << "Windows 2000 ";
         else if (5 == osvi.dwMajorVersion && 1 == osvi.dwMinorVersion)
            m_ofsOut << "Windows XP ";
         else if (6 == osvi.dwMajorVersion && 0 == osvi.dwMinorVersion)
            m_ofsOut << "Windows Vista ";
         // TODO add info for other Windows version
         else
            m_ofsOut << "unknown Windows version ";

         // Display version, service pack (if any), and build number
         if (osvi.dwMajorVersion <= 4)
            // TODO does this still work on 64-bit platforms?
            m_ofsOut << "version " << osvi.dwMajorVersion << "." << osvi.dwMinorVersion << " " << osvi.szCSDVersion << " (Build " << (osvi.dwBuildNumber & 0xFFFF) << ")" << endl;
         else
            // TODO does this still work on 64-bit platforms?
            m_ofsOut << osvi.szCSDVersion << " (Build " << (osvi.dwBuildNumber & 0xFFFF) << ")" << endl;
         break;

      case VER_PLATFORM_WIN32_WINDOWS:
         if (4 == osvi.dwMajorVersion && 0 == osvi.dwMinorVersion)
         {
             m_ofsOut << "Windows 95";
             if ('C' == osvi.szCSDVersion[1] || 'B' == osvi.szCSDVersion[1])
                m_ofsOut << " OSR2";
             m_ofsOut << endl;
         }
         else if (4 == osvi.dwMajorVersion && 10 == osvi.dwMinorVersion)
         {
             m_ofsOut << "Windows 98";
             if ('A' == osvi.szCSDVersion[1])
                m_ofsOut << "SE";
             m_ofsOut << endl;
         }
         else if (4 == osvi.dwMajorVersion && 90 == osvi.dwMinorVersion)
             m_ofsOut << "Windows Me" << endl;
         else
             m_ofsOut << "unknown 16-bit Windows version " << endl;
         break;

      case VER_PLATFORM_WIN32s:
         m_ofsOut << "Win32s" << endl;
         break;
   }

   // Now get process timimgs: this only works under 32-bit windows
   if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId )
   {
      FILETIME ftCreate, ftExit, ftKernel, ftUser;
      if (GetProcessTimes(GetCurrentProcess(), &ftCreate, &ftExit, &ftKernel, &ftUser))
      {
         ULARGE_INTEGER ul;
         ul.LowPart = ftUser.dwLowDateTime;
         ul.HighPart = ftUser.dwHighDateTime;
         m_ofsOut << "Time spent executing user code               \t: " << strDispTime(static_cast<double>(ul.QuadPart) * 1e-7, false, true) << endl;
         ul.LowPart = ftKernel.dwLowDateTime;
         ul.HighPart = ftKernel.dwHighDateTime;
         m_ofsOut << "Time spent executing kernel code             \t: " << strDispTime(static_cast<double>(ul.QuadPart) * 1e-7, false, true) << endl;
      }
   }
   else
      m_ofsOut << "Process timings                              \t: " << NA << endl;

   // Finally get more process statistics: this needs psapi.dll, so only proceed if it is present on this system
   HINSTANCE hDLL = LoadLibrary("psapi.dll");
   if (hDLL != NULL)
   {
      // The dll has been found
      typedef BOOL (__stdcall* DLLPROC) (HANDLE, PPROCESS_MEMORY_COUNTERS, DWORD);
      DLLPROC ProcAdd;

      // Try to get the address of the function we will call
      ProcAdd = (DLLPROC) GetProcAddress(hDLL, "GetProcessMemoryInfo");
      if (ProcAdd)
      {
         // Address was found
         PROCESS_MEMORY_COUNTERS pmc;

         // Now call the function
         if ((ProcAdd) (GetCurrentProcess(), &pmc, sizeof(pmc)))
         {
            m_ofsOut << "Peak working set size                        \t: " << pmc.PeakWorkingSetSize / (1024.0 * 1024.0) << " Mb" << endl;
            m_ofsOut << "Current working set size                     \t: " << pmc.WorkingSetSize / (1024.0 * 1024.0) << " Mb" << endl;
            m_ofsOut << "Peak paged pool usage                        \t: " << pmc.QuotaPeakPagedPoolUsage / (1024.0 * 1024.0) << " Mb" << endl;
            m_ofsOut << "Current paged pool usage                     \t: " << pmc.QuotaPagedPoolUsage / (1024.0 * 1024.0) << " Mb" << endl;
            m_ofsOut << "Peak non-paged pool usage                    \t: " << pmc.QuotaPeakNonPagedPoolUsage / (1024.0 * 1024.0) << " Mb" << endl;
            m_ofsOut << "Current non-paged pool usage                 \t: " << pmc.QuotaNonPagedPoolUsage / (1024.0 * 1024.0) << " Mb" << endl;
            m_ofsOut << "Peak pagefile usage                          \t: " << pmc.PeakPagefileUsage / (1024.0 * 1024.0) << " Mb" << endl;
            m_ofsOut << "Current pagefile usage                       \t: " << pmc.PagefileUsage / (1024.0 * 1024.0) << " Mb" << endl;
            m_ofsOut << "No. of page faults                           \t: " << pmc.PageFaultCount << endl;
         }
      }

      // Free the memory used by the dll
      FreeLibrary(hDLL);
   }

#elif defined __GNUG__
   rusage ru;
   if (getrusage(RUSAGE_SELF, &ru) >= 0)
   {
      m_ofsOut << "Time spent executing user code               \t: "  << strDispTime(ru.ru_utime.tv_sec, false, true) << endl;
//      m_ofsOut << "ru_utime.tv_usec                             \t: " << ru.ru_utime.tv_usec << endl;
      m_ofsOut << "Time spent executing kernel code             \t: " << strDispTime(ru.ru_stime.tv_sec, false, true) << endl;
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


/*========================================================================================================================================

 Returns an error message given an error code

========================================================================================================================================*/
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
   case RTN_ERR_SPLASHEFF:
      strErr = "error reading splash efficiency file";
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
   case RTN_ERR_FLOWDEPOSITMAX:
      strErr = "unstable simulation, unrealistically high flow deposition";
      break;
   case RTN_ERR_TRANSPORTMAX:
      strErr = "unstable simulation, unrealistically high sediment transport";
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


/*========================================================================================================================================

 Notifies the user that the simulation has ended, asks for keypress if necessary, and if compiled under GNU can send an email

========================================================================================================================================*/
void CSimulation::DoEndRun(int const nRtn)
{
   switch (nRtn)
   {
   case (RTN_OK):
      // normal ending
      cout << RUN_END_NOTICE << ctime(&m_tSysEndTime);
      break;

   case (RTN_HELPONLY):
   case (RTN_CHECKONLY):
      return;

   default:
      // Aborting because of some error
      time(&m_tSysEndTime);
      cout << ERROR_NOTICE << nRtn << " (" << pszGetErrorText(nRtn) << ") on " << ctime(&m_tSysEndTime);

      if (m_ofsLog && m_ofsLog.is_open())
      {
         m_ofsLog << ERR << "run aborted (error code " << nRtn << "): " << pszGetErrorText(nRtn) << " on " << ctime(&m_tSysEndTime);
         m_ofsLog.flush();
      }

      if (m_ofsOut && m_ofsOut.is_open())
      {
         m_ofsOut << ERR << "run aborted (error code " << nRtn << "): " << pszGetErrorText(nRtn) << " on " << ctime(&m_tSysEndTime);
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

         // Send an email using Linux/Unix mail command
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

         int nRet = system(strCmd.c_str());
         if (WEXITSTATUS(nRet) != 0)
            cout << ERR << EMAIL_ERROR << endl;

      }
   }
#endif
}


/*========================================================================================================================================

 Returns the simulation-wide missing-value value

========================================================================================================================================*/
double CSimulation::dGetMissingValue(void) const
{
   return m_dMissingValue;
}


/*========================================================================================================================================

 Returns the simulation-wide cell side value

========================================================================================================================================*/
double CSimulation::dGetCellSide(void) const
{
   return m_dCellSide;
}


/*========================================================================================================================================

 Returns the simulation-wide cell diagonal value

 ========================================================================================================================================*/
double CSimulation::dGetCellSideDiag(void) const
{
   return m_dCellDiag;
}


/*========================================================================================================================================

 Returns the reverse (opposite) direction

========================================================================================================================================*/
int CSimulation::nCalcOppositeDirection(int const nDir) const
{
   int nReverseDir = (nDir + 4) % 8;

   return nReverseDir;
}


/*==============================================================================================================================

 Trims whitespace from the left side of a string, does not change the original string

==============================================================================================================================*/
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


/*==============================================================================================================================

 Trims whitespace from the right side of a string, does not change the original string

==============================================================================================================================*/
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

/*==============================================================================================================================

 Trims whitespace from both sides of a string, does not change the original string

==============================================================================================================================*/
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
      strTmp = strTmp.substr(0, nFoundPos+1);

   return strTmp;
}

/*==============================================================================================================================

 Returns the lower case version of an string, leaving the original unchanged

=============================================================================================================================*/
string CSimulation::strToLower(string const* strIn)
{
   string strOut = *strIn;
   transform(strIn->begin(), strIn->end(), strOut.begin(), tolower);
   return strOut;
}

/*==============================================================================================================================

 Returns a string with a substring removed

==============================================================================================================================*/
string CSimulation::strRemoveSubstr(string* strIn, string const* strSub)
{
   size_t nPos = strIn->find(*strSub);

   // If not found, return the string unchanged
   if (nPos != string::npos)
      strIn->replace(nPos, strSub->size(), "");

   return *strIn;
}


/*==============================================================================================================================

 These two functions are from http://stackoverflow.com/questions/236129/split-a-string-in-c They implement (approximately) Python's split() function. This first version puts the results into a pre-constructed string vector. It ignores empty items

==============================================================================================================================*/vector<string>* CSimulation::VstrSplit(string const* s, char const delim, vector<string>* elems)
{
   stringstream ss(*s);
   string item;
   while (getline(ss, item, delim))
   {
      if (! item.empty())
         elems->push_back(item);
   }
   return elems;
}


/*==============================================================================================================================

 This second version returns a new string vector (it calls the first version)

==============================================================================================================================*/
vector<string> CSimulation::VstrSplit(string const* s, char const delim)
{
   vector<string> elems;
   VstrSplit(s, delim, &elems);
   return elems;
}
