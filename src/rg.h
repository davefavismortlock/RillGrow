#ifndef __RG_H__
   #define __RG_H__

/*!
\mainpage
\section intro_sec Introduction
<b>RillGrow</b> simulates soil erosion by water as a self-organising complex system: rills are formed emergently by overland flow. Development of RillGrow is ongoing. Watch this space!\n\n

RillGrow was devised and constructed by David Favis-Mortlock (British Geological Survey: dfm1@bgs.ac.uk and d.favismortlock@gmail.com; formerly at the Environmental Change Institute, University of Oxford). I am very grateful to the following for inspiration and for discussions regarding aspects of this model: John Boardman (University of Oxford), Bob Evans (Anglia Polytechnic University), Peter Kinnell (University of Canberra), Mike Kirkby (University of Leeds), Mark Nearing (USDA), Tony Parsons (University of Sheffield), Olivier Planchon (INRA), and (the late) John Thornes (King's College London).\nzn

See <a href="https://github.com/davefavismortlock/RillGrow" target="_blank">https://github.com/davefavismortlock/RillGrow</a> for the up-to-date version of the source code, and <a href="http://rillgrow.co.uk" target="_blank">http://rillgrow.co.uk</a> for more information.\n\n

\section install_sec Installation

\subsection step1 Step 1: Opening the box

\subsection step2 Step 2: Running RillGrow

\subsection step3 Step 3: Building datasets

\file rg.h
\brief This file contains global definitions for RillGrow
\details TODO
\author David Favis-Mortlock
\date 2025
\copyright GNU General Public License
*/
/*=========================================================================================================================================
This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
=========================================================================================================================================*/
#include <stdlib.h>

#include <fstream>
using std::ofstream;
using std::ifstream;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
using std::ios;

#include <iomanip>
using std::setw;
using std::setfill;
using std::setiosflags;
using std::setprecision;

#include <sstream>
using std::ostream;
using std::ostringstream;

#include <string.h>
#include <cmath>
#include <cfloat>
#include <climits>
#include <ctime>
#include <assert.h>

#include <vector>
using std::vector;

#include <string>
using std::string;
using std::to_string;

#include <gdal_priv.h>
#include <cpl_string.h>

#ifdef _DEBUG
   #define DEBUG_SEDLOAD(x) DEBUGShowSedLoad(x);
#else
   #define DEBUG_SEDLOAD(x)
#endif

//========================================================== Platform-Specific Stuff ======================================================
// #if defined _MSC_VER
//    // MS Visual C++, byte order is IEEE little-endian, 32-bit
//    #include <windows.h>
//    #include <direct.h>                                // For chdir()
//    #include <io.h>                                    // For isatty()
//    #include <psapi.h>                                 // Not available if compiling under Win 9.x?
// //   #pragma warning(disable:4786)                    // Disable warning C4786: symbol greater than 255 characters, since using part of STL
// //   #define WIN32_LEAN_AND_MEAN                      // See <windows.h>
//    #if defined _DEBUG
//       #include <crtdbg.h>                             // Useful
//    #endif
//
//    // clock_t is a signed long: see <time.h>
//    const long     CLOCK_T_MIN                               = LONG_MIN;
//    double const   CLOCK_T_RANGE                             = static_cast<double>(LONG_MAX) - static_cast<double>(CLOCK_T_MIN);
//    #if defined _M_ALPHA
//       string const      PLATFORM                            = "Alpha/MS Visual C++";
//    #elif defined _M_IX86
//       string const      PLATFORM                            = "Intel x86/MS Visual C++";
//    #elif defined _M_MPPC
//       string const      PLATFORM                            = "Power PC/MS Visual C++";
//    #elif defined _M_MRX000
//       string const      PLATFORM                            = "MIPS/MS Visual C++";
//    #else
//       string const      PLATFORM                            = "Other/MS Visual C++";
//    #endif
// #endif
//
// #if defined __BCPLUSPLUS__
//    // Borland C++, byte order is IEEE little-endian, 32-bit
//
//    // clock_t is a signed long: see <time.h>
//    const long     CLOCK_T_MIN                               = LONG_MIN;
//    double const   CLOCK_T_RANGE                             = static_cast<double>(LONG_MAX) - static_cast<double>(CLOCK_T_MIN);
//    string const      PLATFORM                               = "Intel x86/Borland C++";
// #endif

#if defined __GNUG__
   // GNU C++
   #include <sys/resource.h>                             // needed for CalcProcessStats()

   #ifndef CPU
      #error GNU CPU not defined!
   #else
      #if defined x86
         // Intel x86, byte order is little-endian, 32-bit
         string const      PLATFORM                         = "Intel x86/GNU C++";
         // clock_t is an unsigned long: see <time.h>
         const unsigned long CLOCK_T_MIN                    = 0;
         double const CLOCK_T_RANGE                         = static_cast<double>(ULONG_MAX);

      #elif defined rs6000
         // IBM RS-6000, byte order is big-endian, 32-bit
         string const      PLATFORM                         = "IBM RS-6000/GNU C++";
         // clock_t is a signed long: see <time.h> NEED TO CHECK
         const long CLOCK_T_MIN                             = LONG_MIN;
         double const CLOCK_T_RANGE                         = static_cast<double>(LONG_MAX) - static_cast<double>(CLOCK_T_MIN);
      #elif defined ultrasparc
         // Sun UltraSparc, byte order is big-endian, 32-bit
         string const        PLATFORM                       = "Sun UltraSPARC/GNU C++";
         // clock_t is a signed long: see <time.h>
         const long CLOCK_T_MIN                             = LONG_MIN;
         double const CLOCK_T_RANGE                         = static_cast<double>(LONG_MAX) - static_cast<double>(CLOCK_T_MIN);
      #else
         // Something else, assume 32-bit
         string const      PLATFORM                         = "Other/GNU C++";
         // clock_t is a signed long: NEED TO CHECK <time.h>
         const long CLOCK_T_MIN                             = LONG_MIN;
         double const CLOCK_T_RANGE                         = static_cast<double>(LONG_MAX) - static_cast<double>(CLOCK_T_MIN);
      #endif
   #endif
#endif

// #if defined __HP_aCC
//    // HP-UX aCC, byte order is big-endian, can be either 32-bit or 64-bit
//    string const      PLATFORM                               = "HP-UX aC++";
//    // clock_t is an unsigned long: see <time.h>
//    const unsigned long CLOCK_T_MIN                          = 0;
// #endif

// #ifdef __ia64
//    // However, clock_t is a 32-bit unsigned long and we are using 64-bit unsigned longs here
//    double const CLOCK_T_RANGE                               = 4294967295UL;   // crude, improve
// #endif

//========================================================== Hard-Wired Constants =========================================================
string const   PROGNAME                                     = "RillGrow serial 1.1.02 (21 Feb 2025)";
string const   SHORTNAME                                    = "RG";
string const   RG_INI                                       = "rg.ini";

string const   COPYRIGHT                                    = "(C) 2025 David Favis-Mortlock (d.favismortlock@gmail.com and dfm1@bgs.ac.uk)";
string const   LINE                                         = "-------------------------------------------------------------------------------";
string const   DISCLAIMER1                                  = "This program is distributed in the hope that it will be useful, but WITHOUT ANY";
string const   DISCLAIMER2                                  = "WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A";
string const   DISCLAIMER3                                  = "PARTICULAR PURPOSE. See the GNU General Public License for more details. You";
string const   DISCLAIMER4                                  = "should have received a copy of the GNU General Public License along with this";
string const   DISCLAIMER5                                  = "program; if not, contact the Free Software Foundation, Inc., 675 Mass Ave,";
string const   DISCLAIMER6                                  = "Cambridge, MA 02139, USA.";

string const   ABOUT                                        = "RillGrow simulates the growth and development of rills on a bare soil area: the rills are a self-organizing complex system.";
string const   THANKS                                       = "For discussions regarding aspects of this model, many thanks to:\n\tJohn Boardman\n\tBob Evans\n\tPeter Kinnell\n\tMike Kirkby\n\tMark Nearing\n\tTony Parsons\n\tOlivier Planchon\n\t(the late) John Thornes.";
string const   GDALDRIVERS                                  = "GDAL drivers";

string const   USAGE0                                       = "Usage: rg [OPTION]...";
string const   USAGE1                                       = "  --gdal             List GDAL drivers";
string const   USAGE2                                       = "  --about            Information about this program";
string const   USAGE3                                       = "  --help             Display this text";
string const   USAGE4                                       = "  --home=DIRECTORY   Specify the location of the .ini file etc.";
string const   USAGE5                                       = "  --datafile=FILE    Specify the location and name of the main datafile";

string const   START_NOTICE                                 = "- Started on ";
string const   INIT_NOTICE                                  = "- Initializing";
string const   READ_DEM_FILE                                = "  - Reading microtopography: ";
string const   READ_RAIN_VARIATION_FILE                           = "  - Reading rain variation: ";
string const   READ_FILE_LOCATIONS                          = "  - Reading file locations: ";
string const   READ_RUN_DATA_FILE                           = "  - Reading run data: ";
string const   ALLOCATE_MEMORY                              = "  - Allocating memory";
string const   RUN_NOTICE                                   = "- Running simulation";
string const   SIMULATING                                   = "\r  - Simulating ";
string const   FINAL_OUTPUT                                 = "  - Writing final output";
string const   SEND_EMAIL                                   = "  - Sending email to ";
string const   RUN_END_NOTICE                               = "- Run ended at ";
string const   PRESS_KEY                                    = "Press any key to continue...";

string const   ERROR_NOTICE                                 = "- Run ended with error code ";
string const   EMAIL_ERROR                                  = "Could not send email";

char const     SPACE                                        = ' ';
char const     QUOTE1                                       = ';';
char const     QUOTE2                                       = '#';
char const     PATH_SEPARATOR                               = '/';

int const      BUFFER_SIZE                                  = 2048;              // Max length (inc. terminating NULL) of any C-type string
int const      MAX_GIS_FILENAME_SAVE_DIGITS                 = 3;                 // Maximum number of digits for GIS save number
int const      CLOCK_CHECK_ITERATIONS                       = 5000;
int const      NUMBER_OF_RNGS                               = 2;                 // Might add more RNGs in later version
int const      CALC_INFILT_INTERVAL                         = 4;                 // Number of iterations between infilt calculations
int const      CALC_SLUMP_INTERVAL                          = 10;                // Number of iterations between slump calculations
int const      CALC_HEADCUT_RETREAT_INTERVAL                = 7;                 // Number of iterations between headcut retreat calculations
int const      OUTPUT_WIDTH                                 = 90;                // Width of rh bit of .out file, wrap after this
int const      MAX_RECURSION_DEPTH                          = 100;               // Is a safety device, to prevent extreme recursion devouring all memory

// TODO does this still work on 64-bit platforms?
const unsigned long  MASK                                   = 0xfffffffful;

double const   NODATA                                       = -9999;
double const   PI                                           = 3.141592653589793238462643;
double const   INIT_MAX_SPEED_GUESS                         = 10;                // mm/sec
double const   COURANT_ALPHA                                = 0.95;              // i.e. 5% margin
double const   TOLERANCE                                    = 1e-10;              // In mm. If too small (e.g. 1e-10), get spurious "rounding" errors
double const   WATER_TOLERANCE                              = 1e-10;              // In mm, ditto
double const   SEDIMENT_TOLERANCE                           = 1e-10;              // In mm, ditto

double const   ERROR_FLOW_DETACH_MAX                        = 10;                // In mm, is max avg depth per timestep before aborting run
double const   ERROR_FLOW_DEPOSIT_MAX                       = 10;                // Ditto
double const   ERROR_SPLASH_DETACH_MAX                      = 10;                // Ditto
double const   ERROR_SPLASH_DEPOSIT_MAX                     = 10;                // Ditto
double const   ERROR_SLUMP_DETACH_MAX                       = 100;               // Ditto
double const   ERROR_TOPPLE_DETACH_MAX                      = 100;               // Ditto

double const   FOREVER                                      = 0;

string const   ERR                                          = "ERROR: ";
string const   WARN                                         = "WARNING: ";

string const      PERITERHEAD1 =
   "                  <--------------- HYDROLOGY --------------><FLOW EROSION/DEPOSITION><---- SPLASH --->           <--SLUMP-><-TOPPLE-><-INFILT-><-HEADCUT>";
string const      PERITERHEAD2 =
   "Iteration  Elapsed    Rain  Runon Infilt OffEdge  SurfaceWtr   Detach Deposit OffEdge   Detach Deposit    Sedload    Redist    Redist   Deposit    Detach";
string const      PERITERHEAD =
   "PER-ITERATION RESULTS ======================================================================================================================================";
string const      ENDRAINHEAD =
   "RAINFALL ===================================================================================================================================================";
string const      ENDINFILTHEAD =
   "INFILTRATION ===============================================================================================================================================";
string const      ENDHYDHEAD =
   "HYDROLOGY ==================================================================================================================================================";
string const      ENDSEDDELHEAD =
   "SEDIMENT LOST ==============================================================================================================================================";
string const      DETDEPHEAD =
   "DETACHMENT AND DEPOSITION BY PROCESS =======================================================================================================================";
string const      RELCONTRIBLOSTHEAD =
   "CONTRIBUTION OF EACH PROCESS TO TOTAL SEDIMENT ERODED ======================================================================================================";
string const      RELCONTRIBDETACHHEAD =
   "CONTRIBUTION OF EACH PROCESS TO TOTAL SEDIMENT DETACHED ====================================================================================================";
string const      PERFORMHEAD  =
   "PERFORMANCE ================================================================================================================================================";

int const      NO_FLOW                                      = 0;
int const      SHALLOW_FLOW                                 = 1;
int const      MARGINAL_FLOW                                = 2;
int const      DEEP_FLOW                                    = 3;

int const      EDGE_TOP                                     = 0;                 // Planview
int const      EDGE_RIGHT                                   = 1;
int const      EDGE_BOTTOM                                  = 2;
int const      EDGE_LEFT                                    = 3;

// NOTE cannot change these: need to be like this to calc opposite direction to flow for splash and for headcut retreat
int const      DIRECTION_NONE                               = -1;                 // Planview
int const      DIRECTION_TOP                                = 0;
int const      DIRECTION_TOP_RIGHT                          = 1;
int const      DIRECTION_RIGHT                              = 2;
int const      DIRECTION_BOTTOM_RIGHT                       = 3;
int const      DIRECTION_BOTTOM                             = 4;
int const      DIRECTION_BOTTOM_LEFT                        = 5;
int const      DIRECTION_LEFT                               = 6;
int const      DIRECTION_TOP_LEFT                           = 7;

int const      Z_UNIT_NONE                                  = -1;
int const      Z_UNIT_MM                                    = 0;
int const      Z_UNIT_CM                                    = 1;
int const      Z_UNIT_M                                     = 2;

string const   FRICTION_FACTOR_CHECK                        = "friction_factor_check";
string const   SPLASH_ATTENUATION_CHECK                      = "splash_efficiency_check";

string const   OUT_EXT                                      = ".out";
string const   LOG_EXT                                      = ".log";
string const   CSV_EXT                                      = ".csv";

// GIS output: filenames and input flags
string const   GIS_ALL_CODE                                 = "all";

string const   GIS_CUMUL_RAIN_FILENAME                      = "cumul_rain";
string const   GIS_RAIN_SPATIAL_VARIATION_FILENAME          = "rain_variation";
string const   GIS_RAIN_VARIATION_CODE                      = "rainvar";

string const   GIS_CUMUL_RUNON_FILENAME                     = "cumul_runon";
string const   GIS_CUMUL_RUNON_CODE                         = "c_runon";

string const   GIS_ELEVATION_FILENAME                       = "elevation";
string const   GIS_ELEVATION_CODE                           = "elev";
string const   GIS_INITIAL_ELEVATION_CODE                   = "elevinit";
string const   GIS_DETREND_ELEVATION_FILENAME               = "elevation_detrend";
string const   GIS_DETREND_ELEVATION_CODE                   = "elevdetrend";

string const   GIS_INFILT_FILENAME                          = "infilt";
string const   GIS_INFILT_CODE                              = "infilt";
string const   GIS_CUMUL_INFILT_FILENAME                    = "cumul_infilt";
string const   GIS_CUMUL_INFILT_CODE                        = "c_infilt";

string const   GIS_SOIL_WATER_FILENAME                      = "soil_water";
string const   GIS_SOIL_WATER_CODE                          = "soilwater";

string const   GIS_INFILT_DEPOSIT_FILENAME                  = "infilt_deposit";
string const   GIS_INFILT_DEPOSIT_CODE                      = "infiltdeposit";
string const   GIS_CUMUL_INFILT_DEPOSIT_FILENAME            = "cumul_infilt_deposit";
string const   GIS_CUMUL_INFILT_DEPOSIT_CODE                = "c_infiltdeposit";

string const   GIS_SURFACE_WATER_DEPTH_FILENAME              = "water_depth";
string const   GIS_AVG_SURFACE_WATER_DEPTH_FILENAME          = "avg_water_depth";
string const   GIS_AVG_SURFACE_WATER_DEPTH_CODE              = "avg_water_depth";

string const   GIS_TOP_SURFACE_DETREND_FILENAME              = "top_surface";
string const   GIS_TOP_SURFACE_DETREND_CODE                  = "topsurf";

string const   GIS_SPLASH_FILENAME                           = "splash";
string const   GIS_SPLASH_CODE                               = "splash";

string const   GIS_CUMUL_SPLASH_FILENAME                     = "cumul_splash";
string const   GIS_CUMUL_SPLASH_CODE                         = "c_splash";

string const   GIS_INUNDATION_REGIME_FILENAME                = "inundation_regime";
string const   GIS_INUNDATION_REGIME_CODE                    = "inund";

string const   GIS_SURFACE_WATER_DIRECTION_FILENAME          = "flow_direction";
string const   GIS_SURFACE_WATER_DIRECTION_CODE              = "flowdir";

string const   GIS_SURFACE_WATER_SPEED_FILENAME              = "flow_speed";
string const   GIS_AVG_SURFACE_WATER_SPEED_FILENAME          = "avg_flow_speed";
string const   GIS_AVG_SURFACE_WATER_SPEED_CODE              = "avg_flowspeed";

string const   GIS_SURFACE_WATER_DW_SPEED_FILENAME           = "flow_speed_dw";
string const   GIS_AVG_SURFACE_WATER_DW_SPEED_FILENAME       = "avg_flow_speed_dw";
string const   GIS_AVG_SURFACE_WATER_DW_SPEED_CODE           = "avg_flowspeed_dw";

string const   GIS_AVG_SURFACE_WATER_FROM_EDGES_FILENAME     = "lost_from_edge";
string const   GIS_AVG_SURFACE_WATER_FROM_EDGES_CODE         = "fromedge";

string const   GIS_STREAMPOWER_FILENAME                     = "stream_power";
string const   GIS_STREAMPOWER_CODE                         = "streampower";

string const   GIS_SHEAR_STRESS_FILENAME                    = "shear_stress";
string const   GIS_SHEAR_STRESS_CODE                        = "shearstress";

string const   GIS_FRICTION_FACTOR_FILENAME                 = "friction_factor";
string const   GIS_FRICTION_FACTOR_CODE                     = "ff";

string const   GIS_AVG_SHEAR_STRESS_FILENAME                = "avg_shear_stress";
string const   GIS_AVG_SHEAR_STRESS_CODE                    = "avg_shear_stress";

string const   GIS_REYNOLDS_NUMBER_FILENAME                 = "reynolds_number";
string const   GIS_REYNOLDS_NUMBER_CODE                     = "reynolds";

string const   GIS_FROUDE_NUMBER_FILENAME                   = "froude_number";
string const   GIS_FROUDE_NUMBER_CODE                       = "froude";

string const   GIS_TRANSPORT_CAPACITY_FILENAME              = "transport_capacity";
string const   GIS_TRANSPORT_CAPACITY_CODE                  = "transcap";

string const   GIS_ALL_SIZE_FLOW_DETACH_FILENAME            = "flow_detach";
string const   GIS_ALL_SIZE_FLOW_DETACH_CODE                = "flowdetach";

string const   GIS_CUMUL_ALL_SIZE_FLOW_DETACH_FILENAME      = "cumul_flow_detach";
string const   GIS_CUMUL_ALL_SIZE_FLOW_DETACH_CODE          = "c_flowdeposit";

string const   GIS_CUMUL_ALL_SIZE_FLOW_DEPOSIT_FILENAME     = "cumul_sedload_deposit";
string const   GIS_CUMUL_ALL_SIZE_FLOW_DEPOSIT_CODE         = "c_flowdetach";

string const   GIS_SEDIMENT_CONCENTRATION_FILENAME          = "sediment_concentration";
string const   GIS_SEDIMENT_CONCENTRATION_CODE              = "sedconc";
string const   GIS_SEDIMENT_LOAD_FILENAME                   = "sedload";
string const   GIS_SEDIMENT_LOAD_CODE                       = "sedload";
string const   GIS_AVG_SEDIMENT_LOAD_FILENAME               = "avg_sedload";
string const   GIS_AVG_SEDIMENT_LOAD_CODE                   = "avg_sedload";

string const   GIS_CUMUL_SLUMP_DETACH_FILENAME              = "cumul_slump_detach";
string const   GIS_CUMUL_SLUMP_DEPOSIT_FILENAME             = "cumul_slump_deposition";
string const   GIS_CUMUL_SLUMP_CODE                         = "c_slump";
string const   GIS_CUMUL_TOPPLE_DETACH_FILENAME             = "cumul_toppling_detach";
string const   GIS_CUMUL_TOPPLE_DEPOSIT_FILENAME            = "cumul_toppling_deposition";
string const   GIS_CUMUL_TOPPLE_CODE                        = "c_topple";

string const   GIS_CUMUL_ALL_PROC_SURF_LOWER_FILENAME       = "cumul_surface_lowering_all_processes";
string const   GIS_CUMUL_ALL_PROC_SURF_LOWER_CODE           = "c_lowerall";

string const   GIS_CUMUL_BINARY_HEADCUT_RETREAT_FILENAME    = "headcut_retreat";

int const     GIS_ELEVATION                                 = 1;
string const  GIS_ELEVATION_TITLE                           = "Elevation";
int const     GIS_DETREND_ELEVATION                         = 2;
string const  GIS_DETREND_ELEVATION_TITLE                   = "Detrended elevation";
int const     GIS_TOP_SURFACE_DETREND                       = 3;
string const  GIS_TOP_SURFACE_DETREND_TITLE                 = "Top surface elevation";
int const     GIS_CUMUL_RAIN                                = 4;
string const  GIS_CUMUL_RAIN_TITLE                          = "Cumulative rain";
int const     GIS_RAIN_SPATIAL_VARIATION                    = 5;
string const  GIS_RAIN_SPATIAL_VARIATION_TITLE              = "Rain variation";
int const     GIS_CUMUL_RUNON                               = 6;
string const  GIS_CUMUL_RUNON_TITLE                         = "Cumulative runon";
int const     GIS_INFILT                                    = 7;
string const  GIS_INFILT_TITLE                              = "Infiltration";
int const     GIS_CUMUL_INFILT                              = 8;
string const  GIS_CUMUL_INFILT_TITLE                        = "Cumulative infiltration depth";
int const     GIS_SOIL_WATER                                = 9;
string const  GIS_SOIL_WATER_TITLE                          = "Soil water content";
int const     GIS_INFILT_DEPOSIT                            = 10;
string const  GIS_INFILT_DEPOSIT_TITLE                      = "Infiltration deposition";
int const     GIS_CUMUL_INFILT_DEPOSIT                      = 11;
string const  GIS_CUMUL_INFILT_DEPOSIT_TITLE                = "Cumulative infiltration deposition";
int const     GIS_SPLASH                                    = 12;
string const  GIS_SPLASH_TITLE                              = "Splash lowering";
int const     GIS_CUMUL_SPLASH                              = 13;
string const  GIS_CUMUL_SPLASH_TITLE                        = "Cumulative splash lowering";
int const     GIS_SURFACE_WATER_DEPTH                       = 14;
string const  GIS_SURFACE_WATER_DEPTH_TITLE                 = "Surface water depth";
int const     GIS_AVG_SURFACE_WATER_DEPTH                   = 15;
string const  GIS_AVG_SURFACE_WATER_DEPTH_TITLE             = "Average surface water depth";
int const     GIS_INUNDATION_REGIME                         = 16;
string const  GIS_INUNDATION_REGIME_TITLE                   = "Surface water Lawrence inundation regime";
int const     GIS_SURFACE_WATER_DIRECTION                   = 17;
string const  GIS_SURFACE_WATER_DIRECTION_TITLE             = "Surface water flow direction";
int const     GIS_SURFACE_WATER_SPEED                       = 18;
string const  GIS_SURFACE_WATER_SPEED_TITLE                 = "Surface water flow speed";
int const     GIS_SURFACE_WATER_DW_SPEED                    = 19;
string const  GIS_SURFACE_WATER_DW_SPEED_TITLE              = "Surface water depth-weighted flow speed";
int const     GIS_AVG_SURFACE_WATER_SPEED                   = 20;
string const  GIS_AVG_SURFACE_WATER_SPEED_TITLE             = "Surface water average flow speed";
int const     GIS_AVG_SURFACE_WATER_DW_SPEED                = 21;
string const  GIS_AVG_SURFACE_WATER_DW_SPEED_TITLE          = "Surface water average depth-weighted flow speed";
int const     GIS_STREAMPOWER                               = 22;
string const  GIS_STREAMPOWER_TITLE                         = "Surface water stream power";
int const     GIS_SHEAR_STRESS                              = 23;
string const  GIS_SHEAR_STRESS_TITLE                        = "Shear stress due to surface water";
int const     GIS_FRICTION_FACTOR                           = 24;
string const  GIS_FRICTION_FACTOR_TITLE                     = "Friction factor for surface water";
int const     GIS_AVG_SHEAR_STRESS                          = 25;
string const  GIS_AVG_SHEAR_STRESS_TITLE                    = "Average shear stress due to surface water";
int const     GIS_REYNOLDS_NUMBER                           = 26;
string const  GIS_REYNOLDS_NUMBER_TITLE                     = "Surface water Reynolds number";
int const     GIS_FROUDE_NUMBER                             = 27;
string const  GIS_FROUDE_NUMBER_TITLE                       = "Surface water Froude number";
int const     GIS_TRANSPORT_CAPACITY                        = 28;
string const  GIS_TRANSPORT_CAPACITY_TITLE                  = "Surface water transport capacity";
int const     GIS_ALL_SIZE_FLOW_DETACH                      = 29;
string const  GIS_ALL_SIZE_FLOW_DETACH_TITLE                = "Detachment due to surface water flow, all size classes";
int const     GIS_CUMUL_ALL_SIZE_FLOW_DETACH                = 30;
string const  GIS_CUMUL_ALL_SIZE_FLOW_DETACH_TITLE          = "Cumulative detachment due to surface water flow, all size classes";
int const     GIS_SEDIMENT_CONCENTRATION                    = 31;
string const  GIS_SEDIMENT_CONCENTRATION_TITLE              = "Sediment concentration in surface water, all size classes";
int const     GIS_SEDIMENT_LOAD                             = 32;
string const  GIS_SEDIMENT_LOAD_TITLE                       = "Sediment load of surface water, all size classes";
int const     GIS_AVG_SEDIMENT_LOAD                         = 33;
string const  GIS_AVG_SEDIMENT_LOAD_TITLE                   = "Average sediment load of surface water, all size classes";
int const     GIS_CUMUL_ALL_SIZE_FLOW_DEPOSIT               = 34;
string const  GIS_CUMUL_ALL_SIZE_FLOW_DEPOSIT_TITLE         = "Cumulative deposition from surface water flow, all size classes";
int const     GIS_CUMUL_SLUMP_DETACH                        = 35;
string const  GIS_CUMUL_SLUMP_DETACH_TITLE                  = "Cumulative detachment due to slumping, all size classes";
int const     GIS_CUMUL_SLUMP_DEPOSIT                       = 36;
string const  GIS_CUMUL_SLUMP_DEPOSIT_TITLE                 = "Cumulative deposition due to slumping, all size classes";
int const     GIS_CUMUL_TOPPLE_DETACH                       = 37;
string const  GIS_CUMUL_TOPPLE_DETACH_TITLE                 = "Cumulative detachment due to toppling, all size classes";
int const     GIS_CUMUL_TOPPLE_DEPOSIT                      = 38;
string const  GIS_CUMUL_TOPPLE_DEPOSIT_TITLE                = "Cumulative deposition due to toppling, all size classes";
int const     GIS_CUMUL_BINARY_HEADCUT_RETREAT              = 39;
string const  GIS_CUMUL_BINARY_HEADCUT_RETREAT_TITLE        = "Cumulative binary headcut retreat";
int const     GIS_CUMUL_ALL_PROC_SURF_LOWER                 = 40;
string const  GIS_CUMUL_ALL_PROC_SURF_LOWER_TITLE           = "Cumulative surface lowering, all processes";
int const     GIS_AVG_SURFACE_WATER_FROM_EDGES              = 201;
string const  GIS_AVG_SURFACE_WATER_FROM_EDGES_TITLE        = "Total lost from grid edges";

// Time series codes
string const  ERROR_TIME_SERIES_NAME                        = "error";

string const  TIMESTEP_TIME_SERIES_NAME                     = "timestep";
string const  TIMESTEP_TIME_SERIES_CODE                     = "timestep";

string const  AREA_WET_TIME_SERIES_NAME                     = "area_wet";
string const  AREA_WET_TIME_SERIES_CODE                     = "area_wet";

string const  RAIN_TIME_SERIES_NAME                         = "rain";
string const  RAIN_TIME_SERIES_CODE                         = "rain";

string const  INFILT_TIME_SERIES_NAME                       = "infilt";
string const  INFILT_TIME_SERIES_CODE                       = "infilt";

string const  EXFILT_TIME_SERIES_NAME                       = "exfilt";
string const  EXFILT_TIME_SERIES_CODE                       = "exfilt";

string const  RUNON_TIME_SERIES_NAME                        = "run_on";
string const  RUNON_TIME_SERIES_CODE                        = "run_on";

string const  SURFACE_WATER_TIME_SERIES_NAME                 = "surface_water";
string const  SURFACE_WATER_TIME_SERIES_CODE                = "surface_water";

string const  WATER_LOST_TIME_SERIES_NAME                   = "discharge";
string const  WATER_LOST_TIME_SERIES_CODE                   = "discharge";

string const  FLOW_DETACH_TIME_SERIES_NAME                  = "flow_detach";
string const  FLOW_DETACH_TIME_SERIES_CODE                  = "flow_detach";

string const  SLUMP_DETACH_TIME_SERIES_NAME                 = "slump_detach";
string const  SLUMP_DETACH_TIME_SERIES_CODE                 = "slump_detach";

string const  TOPPLE_DETACH_TIME_SERIES_NAME                = "topple_detach";
string const  TOPPLE_DETACH_TIME_SERIES_CODE                = "topple_detach";

string const  SEDLOAD_DEPOSIT_TIME_SERIES_NAME              = "sedload_deposit";
string const  SEDLOAD_DEPOSIT_TIME_SERIES_CODE              = "sedload_deposit";

string const  SEDLOAD_LOST_TIME_SERIES_NAME                 = "sedload_lost";
string const  SEDLOAD_LOST_TIME_SERIES_CODE                 = "sedload_lost";

string const  SEDLOAD_TIME_SERIES_NAME                      = "sedload";
string const  SEDLOAD_TIME_SERIES_CODE                      = "sedload";

string const  INFILT_DEPOSIT_TIME_SERIES_NAME               = "infilt_deposit";
string const  INFILT_DEPOSIT_TIME_SERIES_CODE               = "infilt_deposit";

string const  SPLASH_REDIST_TIME_SERIES_NAME                = "splash_redist";
string const  SPLASH_REDIST_TIME_SERIES_CODE                = "splash_redist";

string const  SPLASH_KE_TIME_SERIES_NAME                    = "splash_KE";
string const  SPLASH_KE_TIME_SERIES_CODE                    = "splash_KE";

string const  SOIL_WATER_TIME_SERIES_NAME                   = "soil_water";
string const  SOIL_WATER_TIME_SERIES_CODE                   = "soil_water";

// Return codes
int const   RTN_OK                                          = 0;
int const   RTN_HELPONLY                                    = 1;
int const   RTN_CHECKONLY                                   = 2;
int const   RTN_USERABORT                                   = 3;
int const   RTN_ERR_BADPARAM                                = 4;
int const   RTN_ERR_INI                                     = 5;
int const   RTN_ERR_RGDIR                                   = 6;
int const   RTN_ERR_RUNDATA                                 = 7;
int const   RTN_ERR_LOGFILE                                 = 8;
int const   RTN_ERR_OUTFILE                                 = 9;
int const   RTN_ERR_TSFILE                                  = 10;
int const   RTN_ERR_DEMFILE                                 = 11;
int const   RTN_ERR_RAIN_VARIATION_FILE                     = 12;
int const   RTN_ERR_RAINFALLTSFILE                          = 13;
int const   RTN_ERR_MEMALLOC                                = 14;
int const   RTN_ERR_GISOUTPUTFORMAT                         = 15;
int const   RTN_ERR_TEXTFILEWRITE                           = 16;
int const   RTN_ERR_GISFILEWRITE                            = 17;
int const   RTN_ERR_TSFILEWRITE                             = 18;
int const   RTN_ERR_SPLASH_ATTENUATION                      = 19;
int const   RTN_ERR_FLOWDETACHMAX                           = 20;
int const   RTN_ERR_SEDLOADDEPOSITMAX                       = 21;
int const   RTN_ERR_SPLASHDETMAX                            = 22;
int const   RTN_ERR_SPLASHDEPMAX                            = 23;
int const   RTN_ERR_SLUMPDETMAX                             = 24;
int const   RTN_ERR_TOPPLEDETMAX                            = 25;

//====================================================== debugging stuff ==================================================================
//#define CLOCKCHECK          // uncomment to check CPU clock rollover settings
//#define RANDCHECK           // uncomment to check randomness of random number generator

#ifndef DOXYGEN_SHOULD_SKIP_THIS
//===================================================== globally-available functions ======================================================
template <class T> T tMax(T a, T b)
{
   return ((a > b) ? a : b);
}

template <class T> T tMax(T a, T b, T c)
{
   T max = (a < b ) ? b : a;
   return (( max < c ) ? c : max);
}

template <class T> T tMin(T a, T b)
{
   return ((a < b) ? a : b);
}

template <class T> T tMin(T a, T b, T c)
{
   return (a < b ? (a < c ? a : c) : (b < c ? b : c));
}

template <class T> T tAbs(T a)
{
   // From a posting dated 18 Nov 93 by rmartin@rcmcon.com (Robert Martin), archived in cpp_tips
   return ((a < 0) ? -a : a);
}

template <class T> bool bIsBetween(T a, T b, T c)
{
   // Assumes b > c
   return ((a >= b) && (a <= c));
}

template <typename T> string strNumToStr(const T& t)
{
   // From http://stackoverflow.com/questions/2125880/convert-float-to-stdstring-in-c
   ostringstream os;
   os << t;
   return os.str();
}

//==============================================================================================================================
// For comparison of two floating-point numbers, with a specified accuracy. This is "essentiallyEqual" from https://stackoverflow.com/questions/17333/how-do-you-compare-float-and-double-while-accounting-for-precision-loss, which is derived from Knuth, D. E. The Art of Computer Programming. Volume 2. Seminumerical Algorithms (Third Edition). Reading MA: Addison-Wesley Longman, 1997.
//==============================================================================================================================
template <class T>
bool bFpEQ(const T d1, const T d2, const T dEpsilon)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
   return tAbs(d1 - d2) <= ( (tAbs(d1) > tAbs(d2) ? tAbs(d2) : tAbs(d2)) * dEpsilon);
#pragma GCC diagnostic pop
}

//==============================================================================================================================
// For greater-than comparison of two floating-point numbers, with a specified accuracy. This is "definitelyGreaterThan" from https://stackoverflow.com/questions/17333/how-do-you-compare-float-and-double-while-accounting-for-precision-loss, which is derived from Knuth, D. E. The Art of Computer Programming. Volume 2. Seminumerical Algorithms (Third Edition). Reading MA: Addison-Wesley Longman, 1997.
//==============================================================================================================================
template <class T>
bool bFpGT(const T d1, const T d2, const T dEpsilon)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
   return (d1 - d2) > ( (tAbs(d1) < tAbs(d2) ? tAbs(d2) : tAbs(d1)) * dEpsilon);
#pragma GCC diagnostic pop
}

//==============================================================================================================================
// For less-than comparison of two floating-point numbers, with a specified accuracy. This is "definitelyLessThan" from https://stackoverflow.com/questions/17333/how-do-you-compare-float-and-double-while-accounting-for-precision-loss, which is derived from Knuth, D. E. The Art of Computer Programming. Volume 2. Seminumerical Algorithms (Third Edition). Reading MA: Addison-Wesley Longman, 1997.
//==============================================================================================================================
template <class T>
bool bFpLT(const T d1, const T d2, const T dEpsilon)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
   return (d2 - d1) > ( (tAbs(d1) < tAbs(d2) ? tAbs(d2) : tAbs(d1)) * dEpsilon);
#pragma GCC diagnostic pop
}
#endif   // DOXYGEN_SHOULD_SKIP_THIS

#endif // __RG_H__
