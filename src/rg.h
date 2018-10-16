#ifndef __RG_H__
   #define __RG_H__
/*=========================================================================================================================================

 This is rg.h: contains global definitions for RillGrow 

 Copyright (C) 2018 David Favis-Mortlock

 ==========================================================================================================================================

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


//========================================================== Platform-Specific Stuff ======================================================
#if defined _MSC_VER
   // MS Visual C++, byte order is IEEE little-endian, 32-bit
   #include <windows.h>
   #include <direct.h>                                // For chdir()
   #include <io.h>                                    // For isatty()
   #include <psapi.h>                                 // Not available if compiling under Win 9.x?
//   #pragma warning(disable:4786)                    // Disable warning C4786: symbol greater than 255 characters, since using part of STL
//   #define WIN32_LEAN_AND_MEAN                      // See <windows.h>
   #if defined _DEBUG
      #include <crtdbg.h>                             // Useful
   #endif

   // clock_t is a signed long: see <time.h>
   const long     CLOCK_T_MIN                               = LONG_MIN;
   double const   CLOCK_T_RANGE                             = static_cast<double>(LONG_MAX) - static_cast<double>(CLOCK_T_MIN);
   #if defined _M_ALPHA
      string const      PLATFORM                            = "Alpha/MS Visual C++";
   #elif defined _M_IX86
      string const      PLATFORM                            = "Intel x86/MS Visual C++";
   #elif defined _M_MPPC
      string const      PLATFORM                            = "Power PC/MS Visual C++";
   #elif defined _M_MRX000
      string const      PLATFORM                            = "MIPS/MS Visual C++";
   #else
      string const      PLATFORM                            = "Other/MS Visual C++";
   #endif

#elif defined __BCPLUSPLUS__
   // Borland C++, byte order is IEEE little-endian, 32-bit

   // clock_t is a signed long: see <time.h>
   const long     CLOCK_T_MIN                               = LONG_MIN;
   double const   CLOCK_T_RANGE                             = static_cast<double>(LONG_MAX) - static_cast<double>(CLOCK_T_MIN);
   string const      PLATFORM                               = "Intel x86/Borland C++";

#elif defined __GNUG__
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

#elif defined __HP_aCC
   // HP-UX aCC, byte order is big-endian, can be either 32-bit or 64-bit
   string const      PLATFORM                               = "HP-UX aC++";
   // clock_t is an unsigned long: see <time.h>
   const unsigned long CLOCK_T_MIN                          = 0;
#ifdef __ia64
   // However, clock_t is a 32-bit unsigned long and we are using 64-bit unsigned longs here
   double const CLOCK_T_RANGE                               = 4294967295UL;   // crude, improve
#else
   double const CLOCK_T_RANGE                               = static_cast<double>(ULONG_MAX);
#endif
#endif

//========================================================== Hard-Wired Constants =========================================================
string const   PROGNAME                                     = "RillGrow serial (2 March 2018 version)";
string const   SHORTNAME                                    = "RG";
string const   RG_INI                                       = "rg.ini";

string const   COPYRIGHT                                    = "(C) 2018 David Favis-Mortlock (david.favis-mortlock@ouce.ox.ac.uk)";
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
string const   USAGE5                                       = "  --datafile =FILE    Specify the location and name of the main datafile";

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
int const      CALC_INFILTRATION_INTERVAL                   = 4;                 // Number of iterations between infiltration calculations
int const      CALC_SLUMP_INTERVAL                          = 10;                // Number of iterations between slump calculations
int const      OUTPUT_WIDTH                                 = 90;                // Width of rh bit of .out file, wrap after this
int const      MAX_RECURSION_DEPTH                          = 100;               // Is a safety device, to prevent extreme recursion devouring all memory

// TODO does this still work on 64-bit platforms?
const unsigned long  MASK                                   = 0xfffffffful;

double const   PI                                           = 3.141592653589793238462643;
double const   DEFAULT_TIMESTEP_RAIN                        = 0.0005;            // Seconds
double const   DEFAULT_TIMESTEP_NORAIN                      = 0.05;              // Seconds
double const   INIT_MAX_SPEED_GUESS                         = 1;                 // In mm/sec
double const   COURANT_ALPHA                                = 0.95;              // I.e. 5% margin
double const   TIMESTEP_CHANGE_FACTOR                       = 0.01;              // Maximum of 1% change
double const   TOLERANCE                                    = 1e-6;              // In mm, relative difference for bFPIsEqual, if too small (e.g. 1e-10), get spurious "rounding" errors
double const   RAIN_MIN_CONSIDERED                          = 1e-2;              // In mm, don't bother calculating various things if relative diff in rain depth less than this
double const   FLOW_MIN_CONSIDERED                          = 1e-2;              // In mm, don't bother calculating flow if relative diff in head less than this
double const   SEDIMENT_MIN_CONSIDERED                      = 1e-2;              // In mm, don't bother calculating various things if relative diff sediment difference less than this
double const   ERROR_FLOW_DETACH_MAX                        = 10;                // In mm, is max avg depth per timestep before aborting run
double const   ERROR_FLOW_DEPOSIT_MAX                       = 10;                // Ditto
double const   ERROR_SEDIMENT_TRANSPORT_MAX                 = 10;                // Ditto
double const   ERROR_SPLASH_DETACH_MAX                      = 10;                // Ditto
double const   ERROR_SPLASH_DEPOSIT_MAX                     = 10;                // Ditto
double const   ERROR_SLUMP_DETACH_MAX                       = 100;               // Ditto
double const   ERROR_TOPPLE_DETACH_MAX                      = 100;               // Ditto
double const   FOREVER                                      = 0;

string const   ERR                                          = "ERROR: ";
string const   WARN                                         = "WARNING: ";

string const      PERITERHEAD1 =
   "                  <-------------- HYDROLOGY -------------><--- FLOW EROSION & DEPOSITION ---><-SPLASH-><---- SLUMPING ----><---- TOPPLING ----><-INFILT->";
string const      PERITERHEAD2 =
   "Iteration  Elapsed   Rain RunOn  Infilt     Stored OffEdge   Eroded   Sedload Deposit OffEdge Redistrib    Detach   Deposit    Detach   Deposit   Deposit";
// 9999999 99999.99999 999999 99999 9999999 9999999999 9999999  9999999 999999999 9999999 9999999   9999999 999999999 999999999  99999999  99999999  99999999
string const      PERITERHEAD   =
   "PER-ITERATION RESULTS ======================================================================================================================================";
string const      ENDRAINHEAD   =
   "RAINFALL ===================================================================================================================================================";
string const      ENDINFILTRATIONHEAD   =
   "INFILTRATION ===============================================================================================================================================";
string const      ENDHYDHEAD    =
   "HYDROLOGY ==================================================================================================================================================";
string const      ENDSEDDELHEAD =
   "SEDIMENT LOST ==============================================================================================================================================";
string const      DETDEPHEAD    =
   "DETACHMENT AND DEPOSITION BY PROCESS =======================================================================================================================";
string const      RELCONTRIBLOSTHEAD    =
   "CONTRIBUTION OF EACH PROCESS TO TOTAL SEDIMENT LOST ========================================================================================================";
string const      RELCONTRIBDETACHHEAD    =
   "CONTRIBUTION OF EACH PROCESS TO TOTAL SEDIMENT DETACHED ====================================================================================================";
string const      PERFORMHEAD   =
   "PERFORMANCE ================================================================================================================================================";

int const      NO_FLOW                                      = 0;
int const      SHALLOW_FLOW                                 = 1;
int const      MARGINAL_FLOW                                = 2;
int const      DEEP_FLOW                                    = 3;

int const      EDGE_TOP                                     = 0;                 // Planview
int const      EDGE_RIGHT                                   = 1;
int const      EDGE_BOTTOM                                  = 2;
int const      EDGE_LEFT                                    = 3;

int const      DIRECTION_NONE                               = 0;                 // Planview
int const      DIRECTION_TOP                                = 1;
int const      DIRECTION_TOP_RIGHT                          = 2;
int const      DIRECTION_RIGHT                              = 3;
int const      DIRECTION_BOTTOM_RIGHT                       = 4;
int const      DIRECTION_BOTTOM                             = 5;
int const      DIRECTION_BOTTOM_LEFT                        = 6;
int const      DIRECTION_LEFT                               = 7;
int const      DIRECTION_TOP_LEFT                           = 8;

int const      Z_UNIT_NONE                                  = -1;                     
int const      Z_UNIT_MM                                    = 0;                     
int const      Z_UNIT_CM                                    = 1;                     
int const      Z_UNIT_M                                     = 2;                     

string const   FRICTION_FACTOR_CHECK                        = "friction_factor_check";
string const   SPLASH_EFFICIENCY_CHECK                      = "splash_efficiency_check";

string const   OUT_EXT                                      = ".out";
string const   LOG_EXT                                      = ".log";
string const   CSV_EXT                                      = ".csv";

// GIS output codes
string const   ALL_CODE                                     = "all";

string const   RAIN_NAME                                    = "rain";
string const   RAIN_CODE                                    = "rain";
string const   RAIN_VARIATION_NAME                          = "rain_variation";
string const   RAIN_VARIATION_CODE                          = "rain_variation";

string const   RUNON_NAME                                   = "run_on";
string const   RUNON_CODE                                   = "run_on";

string const   ELEVATION_NAME                               = "elevation";
string const   ELEVATION_CODE                               = "elevation";
string const   INITIAL_ELEVATION_CODE                       = "elevation_init";
string const   DETRENDED_ELEVATION_NAME                     = "elevation_detrend";
string const   DETRENDED_ELEVATION_CODE                     = "elevation_detrend";

string const   INFILTRATION_NAME                            = "infiltration";
string const   INFILTRATION_CODE                            = "infiltration";
string const   CUMULATIVE_INFILTRATION_NAME                 = "infiltration_total";
string const   CUMULATIVE_INFILTRATION_CODE                 = "infiltration_total";

string const   SOIL_WATER_NAME                              = "soil_water";
string const   SOIL_WATER_CODE                              = "soil_water";

string const   INFILTRATION_DEPOSITION_NAME                 = "infiltration_deposition";
string const   INFILTRATION_DEPOSITION_CODE                 = "infiltration_deposition";
string const   CUMULATIVE_INFILTRATION_DEPOSITION_NAME      = "infiltration_deposition_total";
string const   CUMULATIVE_INFILTRATION_DEPOSITION_CODE      = "infiltration_deposition_total";

string const   SURFACE_WATER_DEPTH_NAME                     = "water_depth";
string const   AVG_SURFACE_WATER_DEPTH_NAME                 = "water_depth_avg";
string const   AVG_SURFACE_WATER_DEPTH_CODE                 = "water_depth_avg";

string const   TOPSNAME                      = "top_surface";
string const   TOPSCODE                      = "top_surface";

string const   SPLASHNAME                    = "splash";
string const   SPLASHCODE                    = "splash";

string const   TOTSPLASHNAME                 = "splash_total";
string const   TOTSPLASHCODE                 = "splash_total";

string const   INUNDATIONNAME                = "inundation_regime";
string const   INUNDATIONCODE                = "inundation_regime";

string const   FLOWDIRNAME                   = "flow_direction";
string const   FLOWDIRCODE                   = "flow_direction";

string const   SPEEDNAME                     = "flow_speed";
string const   AVGSPEEDNAME                  = "flow_speed_avg";
string const   AVGSPEEDCODE                  = "flow_speed_avg";

string const   DWSPEEDNAME                   = "flow_speed_depth_weighted";
string const   AVGDWSPEEDNAME                = "flow_speed_depth_weighted_avg";
string const   AVGDWSPEEDCODE                = "flow_speed_depth_weighted_avg";

#if defined _DEBUG
string const   LOSTNAME                      = "lost_from_edge";
string const   LOSTCODE                      = "lost_from_edge";
#endif

string const   STREAMPOWERNAME               = "stream_power";
string const   STREAMPOWERCODE               = "stream_power";

string const   SHEARSTRESSNAME               = "shear_stress";
string const   SHEARSTRESSCODE               = "shear_stress";

string const   FRICTIONFACTORNAME            = "friction_factor";
string const   FRICTIONFACTORCODE            = "friction_factor";

string const   AVGSHEARSTRESSNAME            = "shear_stress_avg";
string const   AVGSHEARSTRESSCODE            = "shear_stress_avg";

string const   REYNOLDSNAME                  = "reynolds_number";
string const   REYNOLDSCODE                  = "reynolds_number";

string const   FROUDENAME                    = "froude_number";
string const   FROUDECODE                    = "froude_number";

string const   TCNAME                        = "transport_capacity";
string const   TCCODE                        = "transport_capacity";

string const   ALL_SIZE_FLOW_DETACHMENT_NAME                    = "flow_detachment";
string const   DETACHCODE                    = "flow_detachment";
string const   CUMULATIVE_ALL_SIZE_FLOW_DETACHMENT_NAME                 = "flow_detachment_total";

string const   SEDCONCNAME                   = "sediment_concentration";
string const   SEDCONCCODE                   = "sediment_concentration";
string const   SEDLOADNAME                   = "sediment_load";
string const   SEDLOADCODE                   = "sediment_load";
string const   AVGSEDLOADNAME                = "sediment_load_avg";
string const   AVGSEDLOADCODE                = "sediment_load_avg";

string const   CUMULATIVE_ALL_SIZE_FLOW_DEPOSITION_NAME                  = "flow_deposition_total";
string const   CUMULATIVE_ALL_SIZE_FLOW_DEPOSITION_CODE                  = "flow_deposition_total";

string const   NET_SLUMPING_NAME                  = "slumping";
string const   NET_SLUMPING_CODE                  = "slumping";
string const   NET_TOPPLING_NAME                 = "toppling";
string const   NET_TOPPLING_CODE                 = "toppling";

string const   TOT_SURFACE_LOWERING_NAME      = "surface_lowering_all_processes";
string const   TOT_SURFACE_LOWERING_CODE      = "surface_lowering_all_processes";

int const     PLOT_ELEVATION                      = 1;
string const  PLOT_ELEVATION_TITLE                = "Elevation";
string const  PLOT_DETRENDED_ELEVATION_TITLE      = "Detrended elevation";
int const     PLOT_TOP                            = 2;
string const  PLOT_TOP_TITLE                      = "Top surface elevation";
int const     PLOT_TOTAL_RAIN                        = 3;
string const  PLOT_TOTAL_RAIN_TITLE                  = "Total rain";
string const  PLOT_RAIN_SPATIAL_VARIATION_TITLE                  = "Rain variation";
int const     PLOT_TOTAL_RUNON                       = 4;
string const  PLOT_TOTAL_RUNON_TITLE                 = "Total runon";
int const     PLOT_INFILTRATION                   = 5;
string const  PLOT_INFILTRATION_TITLE             = "Infiltration";
int const     PLOT_CUMULATIVE_INFILTRATION_                = 6;
string const  PLOT_CUMULATIVE_INFILTRATION__TITLE          = "Total infiltration";
int const     PLOT_SOIL_WATER                     = 7;
string const  PLOT_SOIL_WATER_TITLE               = "Soil water content";
int const     PLOT_INFILTRATION_DEPOSIT           = 8;
string const  PLOT_INFILTRATION_DEPOSIT_TITLE     = "Infiltration deposition";
int const     PLOT_CUMULATIVE_INFILTRATION_DEPOSIT        = 9;
string const  PLOT_CUMULATIVE_INFILTRATION_DEPOSIT_TITLE  = "Total infiltration deposition";
int const     PLOT_SPLASH                         = 10;
string const  PLOT_SPLASH_TITLE                   = "Splash lowering";
int const     PLOT_TOTAL_SPLASH                      = 11;
string const  PLOT_TOTAL_SPLASH_TITLE                = "Total splash lowering";
int const     PLOT_SURFACE_WATER_DEPTH                          = 12;
string const  PLOT_SURFACE_WATER_DEPTH_TITLE                    = "Surface water depth";
int const     PLOT_AVG_SURFACE_WATER_DEPTH                       = 13;
string const  PLOT_AVG_SURFACE_WATER_DEPTH_TITLE                 = "Mean water depth";
int const     PLOT_INUNDATION_REGIME                     = 14;
string const  PLOT_INUNDATION_REGIME_TITLE               = "Inundation regime";
int const     PLOT_SURFACE_WATER_FLOW_DIRECTION                        = 15;
string const  PLOT_SURFACE_WATER_FLOW_DIRECTION_TITLE           = "Surface water flow direction";
int const     PLOT_SURFACE_WATER_FLOW_SPEED                          = 16;
string const  PLOT_SURFACE_WATER_FLOW_SPEED_TITLE                    = "Flow speed";
int const     PLOT_SURFACE_WATER_DW_FLOW_SPEED                        = 17;
string const  PLOT_SURFACE_WATER_DW_FLOW_SPEED_TITLE                  = "Depth-weighted flow speed";
int const     PLOT_SURFACE_WATER_AVG_FLOW_SPEED                       = 18;
string const  PLOT_SURFACE_WATER_AVG_FLOW_SPEED_TITLE                 = "Mean flow speed";
int const     PLOT_SURFACE_WATER_AVG_DW_FLOW_SPEED                     = 19;
string const  PLOT_SURFACE_WATER_AVG_DW_FLOW_SPEED_TITLE               = "Mean depth-weighted flow speed";
int const     PLOT_STREAMPOWER                    = 20;
string const  PLOT_STREAMPOWER_TITLE              = "Stream power";
int const     PLOT_SHEAR_STRESS                    = 21;
string const  PLOT_SHEAR_STRESS_TITLE              = "Shear stress";
int const     PLOT_FRICTION_FACTOR                 = 22;
string const  PLOT_FRICTION_FACTOR_TITLE           = "Friction factor";
int const     PLOT_AVG_SHEAR_STRESS                 = 23;
string const  PLOT_AVG_SHEAR_STRESS_TITLE           = "Mean shear stress";
int const     PLOT_REYNOLDS_NUMBER                       = 24;
string const  PLOT_REYNOLDS_NUMBER_TITLE                 = "Reynolds number";
int const     PLOT_FROUDE_NUMBER                         = 25;
string const  PLOT_FROUDE_NUMBER_TITLE                   = "Froude number";
int const     PLOT_TRANSPORT_CAPACITY                             = 26;
string const  PLOT_TRANSPORT_CAPACITY_TITLE                       = "Transport capacity";
int const     PLOT_ALL_SIZE_FLOW_DETACHMENT                         = 27;
string const  PLOT_ALL_SIZE_FLOW_DETACHMENT_TITLE                   = "Flow detachment";
int const     PLOT_CUMULATIVE_ALL_SIZE_FLOW_DETACHMENT                      = 28;
string const  PLOT_CUMULATIVE_ALL_SIZE_FLOW_DETACHMENT_TITLE                = "Total flow detachment";
int const     PLOT_SEDIMENT_CONCENTRATION                        = 29;
string const  PLOT_SEDIMENT_CONCENTRATION_TITLE                  = "Sediment concentration";
int const     PLOT_SEDIMENT_LOAD                        = 30;
string const  PLOT_SEDIMENT_LOAD_TITLE                  = "Sediment load";
int const     PLOT_AVG_SEDIMENT_LOAD                     = 31;
string const  PLOT_AVG_SEDIMENT_LOAD_TITLE               = "Mean sediment load";
int const     PLOT_CUMULATIVE_ALL_SIZE_FLOW_DEPOSITION                       = 32;
string const  PLOT_CUMULATIVE_ALL_SIZE_FLOW_DEPOSITION_TITLE                 = "Total deposition";
int const     PLOT_NET_SLUMP                       = 33;
string const  PLOT_NET_SLUMP_TITLE                 = "Total net slumping";
int const     PLOT_NET_TOPPLE                      = 34;
string const  PLOT_NET_TOPPLE_TITLE                = "Total net toppling";
int const     PLOT_TOT_SURFACE_LOWERING           = 35;
string const  PLOT_TOT_SURFACE_LOWERING_TITLE     = "Total net lowering";
int const     PLOT_DETRENDED_ELEVATION            = 101;
int const     PLOT_RAIN_SPATIAL_VARIATION                      = 102;
#if defined _DEBUG
int const     PLOT_LOST_FROM_EDGES                           = 201;
string const  PLOT_LOST_FROM_EDGES_TITLE                     = "Total lost from edges";
#endif

// Time series codes
string const  TIMESTEP_TIME_SERIES_NAME                = "timestep";
string const  TIMESTEP_TIME_SERIES_CODE                = "timestep";

string const         AREA_WET_TIME_SERIES_NAME                 = "area_wet";
string const         AREA_WET_TIME_SERIES_CODE                 = "area_wet";

string const         RAIN_TIME_SERIES_NAME                    = "rain";
string const         RAIN_TIME_SERIES_CODE                    = "rain";

string const         INFILTRATION_TIME_SERIES_NAME            = "infiltration";
string const         INFILTRATION_TIME_SERIES_CODE            = "infiltration";

string const         EXFILTRATION_TIME_SERIES_NAME            = "exfiltration";
string const         EXFILTRATION_TIME_SERIES_CODE            = "exfiltration";

string const         RUNON_TIME_SERIES_NAME                   = "run_on";
string const         RUNON_TIME_SERIES_CODE                   = "run_on";

string const         SURFACE_WATER_TIME_SERIES_NAME            = "surface_water";
string const         SURFACE_WATER_TIME_SERIES_CODE            = "surface_water";

string const         WATER_LOST_TIME_SERIES_NAME               = "discharge";
string const         WATER_LOST_TIME_SERIES_CODE               = "discharge";

string const         FLOW_DETACHMENT_TIME_SERIES_NAME              = "flow_detachment";
string const         FLOW_DETACHMENT_TIME_SERIES_CODE              = "flow_detachment";

string const         SLUMP_DETACHMENT_TIME_SERIES_NAME             = "slump_detachment";
string const         SLUMP_DETACHMENT_TIME_SERIES_CODE             = "slump_detachment";

string const         TOPPLE_DETACHMENT_TIME_SERIES_NAME            = "topple_detachment";
string const         TOPPLE_DETACHMENT_TIME_SERIES_CODE            = "topple_detachment";

string const         FLOW_DEPOSITION_TIME_SERIES_NAME             = "flow_deposition";
string const         FLOW_DEPOSITION_TIME_SERIES_CODE             = "flow_deposition";

string const         SEDIMENT_LOST_TIME_SERIES_NAME                 = "sediment_lost";
string const         SEDIMENT_LOST_TIME_SERIES_CODE                 = "sediment_lost";

string const         SEDIMENT_LOAD_TIME_SERIES_NAME                 = "sediment_load";
string const         SEDIMENT_LOAD_TIME_SERIES_CODE                 = "sedload_load";

string const         INFILTRATION_DEPOSIT_TIME_SERIES_NAME    = "infiltration_deposition";
string const         INFILTRATION_DEPOSIT_TIME_SERIES_CODE    = "infiltration_deposition";

string const         SPLASH_REDISTRIBUTION_TIME_SERIES_NAME            = "splash_redistribution";
string const         SPLASH_REDISTRIBUTION_TIME_SERIES_CODE            = "splash_redistribution";

string const         SPLASH_KINETIC_ENERGY_TIME_SERIES_NAME                = "kinetic_energy";
string const         SPLASH_KINETIC_ENERGY_TIME_SERIES_CODE                = "kinetic_energy";

string const         SOIL_WATER_TIME_SERIES_NAME                = "soil_water";
string const         SOIL_WATER_TIME_SERIES_CODE                = "soil_water";

// Return codes
int const   RTN_OK                        = 0;
int const   RTN_HELPONLY                  = 1;
int const   RTN_CHECKONLY                 = 2;
int const   RTN_USERABORT                 = 3;
int const   RTN_ERR_BADPARAM              = 4;
int const   RTN_ERR_INI                   = 5;
int const   RTN_ERR_RGDIR                 = 6;
int const   RTN_ERR_RUNDATA               = 7;
int const   RTN_ERR_LOGFILE               = 8;
int const   RTN_ERR_OUTFILE               = 9;
int const   RTN_ERR_TSFILE                = 10;
int const   RTN_ERR_DEMFILE               = 11;
int const   RTN_ERR_RAIN_VARIATION_FILE           = 12;
int const   RTN_ERR_RAINFALLTSFILE        = 13;
int const   RTN_ERR_MEMALLOC              = 14;
int const   RTN_ERR_GISOUTPUTFORMAT       = 15;
int const   RTN_ERR_TEXTFILEWRITE         = 16;
int const   RTN_ERR_GISFILEWRITE          = 17;
int const   RTN_ERR_TSFILEWRITE           = 18;
int const   RTN_ERR_SPLASHEFF             = 19;
int const   RTN_ERR_FLOWDETACHMAX         = 20;
int const   RTN_ERR_FLOWDEPOSITMAX        = 21;
int const   RTN_ERR_TRANSPORTMAX          = 22;
int const   RTN_ERR_SPLASHDETMAX          = 23;
int const   RTN_ERR_SPLASHDEPMAX          = 24;
int const   RTN_ERR_SLUMPDETMAX           = 25;
int const   RTN_ERR_TOPPLEDETMAX          = 26;


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

double dRound(double const);
bool bFPIsEqual(double const, double const, double const);
// bool bIsWhole(double);
struct FillToWidth
{
   FillToWidth(char f, int w) : chFill(f), nWidth(w)
   {
   }
   
   char chFill;
   int nWidth;
};
ostream& operator<< (ostream&, const FillToWidth&);


//====================================================== debugging stuff ==================================================================
//#define CLOCKCHECK          // uncomment to check CPU clock rollover settings
//#define RANDCHECK           // uncomment to check randomness of random number generator

#endif // __RG_H__
