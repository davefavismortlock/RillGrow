/*=========================================================================================================================================

 This is gis.cpp: these CSimulation member functions use GDAL to read and write GIS files in several formats.

 Copyright (C) 2018 David Favis-Mortlock

 ==========================================================================================================================================

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation,
 Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
#include <sstream>
using std::stringstream;

#include "rg.h"
#include "simulation.h"
#include "cell.h"
#include "2d_vec.h"


/*========================================================================================================================================

 Reads the microtopgraphy DEM data to the Cell array, also set each cell's basement elevation

=========================================================================================================================================*/
int CSimulation::nReadMicrotopographyDEMData(void)
{
   // Use GDAL to create a dataset object, which then opens the DEM file
   GDALDataset* pGDALDataset = (GDALDataset *) GDALOpen(m_strDEMFile.c_str(), GA_ReadOnly);

   if (NULL == pGDALDataset)
   {
      // Can't open file (note will already have sent GDAL error message to stdout)
      cerr << ERR << "cannot open " << m_strDEMFile << " for input: " << CPLGetLastErrorMsg() << endl;
      return RTN_ERR_DEMFILE;
   }

   // Opened OK, so get GDAL DEM dataset information
   m_strGDALDEMDriverCode = pGDALDataset->GetDriver()->GetDescription();
   m_strGDALDEMDriverDesc = pGDALDataset->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME);
   m_strGDALDEMProjection = pGDALDataset->GetProjectionRef();

   // If we have reference units, then check that they are in meters (note US spelling)
   if (! m_strGDALDEMProjection.empty())
   {
      string strTmp = strToLower(&m_strGDALDEMProjection);
      if ((! strTmp.empty()) && strTmp.find("meter") == string::npos)
      {
         // error: x-y values must be in metres
         cerr << ERR << "GIS file x-y values (" << m_strGDALDEMProjection << ") in " << m_strDEMFile.c_str() << " must be in metres" << endl;
         
         return RTN_ERR_DEMFILE;
      }
   }

   // Now get dataset size, and do some rudimentary checks
   m_nXGridMax = pGDALDataset->GetRasterXSize();
   if (m_nXGridMax <= 0)
   {
      // Error: silly number of columns specified
      cerr << ERR << "invalid number of columns (" << m_nXGridMax << ") in " << m_strDEMFile << endl;
      return RTN_ERR_DEMFILE;
   }

   m_nYGridMax = pGDALDataset->GetRasterYSize();
   if (m_nYGridMax <= 0)
   {
      // Error: silly number of rows specified
      cerr << ERR << "invalid number of rows (" << m_nYGridMax << ") in " << m_strDEMFile << endl;
      return RTN_ERR_DEMFILE;
   }

   // Get geotransformation info
   if (CE_Failure == pGDALDataset->GetGeoTransform(m_dGeoTransform))
   {
      // Can't get geotransformation (note will already have sent GDAL error message to stdout)
      cerr << ERR << CPLGetLastErrorMsg() << " in " << m_strDEMFile << endl;
      return RTN_ERR_DEMFILE;
   }

   // Get Min X and Min Y
   m_dMinX = m_dGeoTransform[0];
   m_dMinY = m_dGeoTransform[3];

   // Get X and Y pixel resolutions
   double 
      dPixelResX = tAbs(m_dGeoTransform[1]),
      dPixelResY = tAbs(m_dGeoTransform[5]);

   // Calculate Max X and Max Y
   m_dMaxX = m_dMinX + (dPixelResX * m_nXGridMax);
   m_dMaxY = m_dMinY + (dPixelResY * m_nYGridMax);

   if (! bFPIsEqual(dPixelResX, dPixelResY, 1e-2))
   {
      // Error: cell isn't square (enough): note that due to rounding errors in some GIS packages, must expect some discrepancies
      cerr << ERR << "cell not square (" << dPixelResX << " x " << dPixelResY << ") in " << m_strDEMFile << endl;
      return RTN_ERR_DEMFILE;
   }

   // To be on the safe side, average pixel resolutions, then use the result to convert m_dCellSide from m to mm
   m_dCellSide = 1000 * (dPixelResX + dPixelResY) / 2;

   // Now get GDAL raster band information
   GDALRasterBand* pGDALBand = pGDALDataset->GetRasterBand(1);
   int nBlockXSize = 0, nBlockYSize = 0;
   pGDALBand->GetBlockSize(&nBlockXSize, &nBlockYSize);
   m_strGDALDEMDataType = GDALGetDataTypeName(pGDALBand->GetRasterDataType());

   // If we have Z units (aka value units) in the DEM, then check them
   string strUnits = pGDALBand->GetUnitType();
   if (! strUnits.empty())
   {
      // We have Z units in the DEM
      int nDEMZUnits = Z_UNIT_NONE;
      
      strUnits = strToLower(&strUnits);
      if (strUnits.find("mm") != string::npos)
         nDEMZUnits = Z_UNIT_MM;
      else if (strUnits.find("cm") != string::npos)
         nDEMZUnits = Z_UNIT_CM;     
      else if (strUnits.find("m") != string::npos)
         nDEMZUnits = Z_UNIT_M;     
      
      // Do some checking if the user specified Z units
      if (m_nZUnits != Z_UNIT_NONE)
      {
         if (nDEMZUnits != m_nZUnits)
         {
            string strDispDEMZUnits, strDispUserDEMZUnits;
               
            if (nDEMZUnits == Z_UNIT_MM)
               strDispDEMZUnits = "mm";
            else if (nDEMZUnits == Z_UNIT_CM)
               strDispDEMZUnits = "cm";   
            else if (nDEMZUnits == Z_UNIT_M)
               strDispDEMZUnits = "m";   
            
            if (m_nZUnits == Z_UNIT_MM)
               strDispUserDEMZUnits = "mm";
            else if (m_nZUnits == Z_UNIT_CM)
               strDispUserDEMZUnits = "cm";   
            else if (m_nZUnits == Z_UNIT_M)
               strDispUserDEMZUnits = "m";   
            
            cerr << WARN << "microtography Z units in DEM are '" << strDispDEMZUnits << "' but Z units in DEM are '" << strDispDEMZUnits << "'" << endl;
         }
      }
      
      // Use the Z units specified in the DEM
      m_nZUnits = nDEMZUnits;      
   }
   else
   {
      // No Z units in the DEM
      if (m_nZUnits == Z_UNIT_NONE)
      {
         // No Z units specified by the user either, so give up   
         cerr << ERR << "no microtopography Z units specified in " << m_strDataPathName << " or in " << m_strDEMFile << endl;
         return RTN_ERR_DEMFILE;
      }
   }
   
   // If present, get the missing value (NODATA) setting
   CPLPushErrorHandler(CPLQuietErrorHandler);                  // Needed to get next line to fail silently, if it fails
   m_dMissingValue = pGDALBand->GetNoDataValue();              // Will fail for some formats
   CPLPopErrorHandler();

   // Next allocate memory for two 2D arrays of soil cell objects: tell the user what is happening
   AnnounceAllocateMemory();

   // The first array of cells
   Cell = new CCell*[m_nXGridMax];
   if (NULL == Cell)
   {
      // Error, can't allocate memory
      cerr << ERR << "cannot allocate memory for " << m_nXGridMax << " soil cell objects" << endl;
      return (RTN_ERR_MEMALLOC);
   }

   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      Cell[nX] = new CCell[m_nYGridMax];
      if (NULL == Cell[nX])
      {
         // Error, can't allocate memory
         cerr << ERR << "cannot allocate memory for " << m_nYGridMax << " soil cell objects" << endl;
         return (RTN_ERR_MEMALLOC);
      }
   }
   
   // Allocate memory for a 1D floating-point array, to hold the scan line for GDAL
   float* pfScanline;
   pfScanline = new float[m_nXGridMax];

   if (NULL == pfScanline)
   {
      // Error, can't allocate memory
      cerr << ERR << "cannot allocate memory for " << m_nXGridMax << " x 1D array" << endl;
      return (RTN_ERR_MEMALLOC);
   }

   // Now read in the data
   m_dAvgElev = 0;
   m_dMinElev = DBL_MAX;
   int nRead = 0;
   for (int j = 0; j < m_nYGridMax; j++)
   {
      // Read scanline
      if (CE_Failure == pGDALBand->RasterIO(GF_Read, 0, j, m_nXGridMax, 1, pfScanline, m_nXGridMax, 1, GDT_Float32, 0, 0))
      {
         // Error while reading scanline
         cerr << ERR << CPLGetLastErrorMsg() << " in " << m_strDEMFile << endl;
         return RTN_ERR_DEMFILE;
      }

      // All OK, so read scanline into cell elevations
      for (int i = 0; i < m_nXGridMax; i++)
      {
         double dElev = pfScanline[i];
         
         if (dElev != m_dMissingValue)
         {
            // The Z elevation must be in mm, so do some conversion if necessary
            if (m_nZUnits == Z_UNIT_M)
               dElev *= 1e3;         
            else if (m_nZUnits == Z_UNIT_CM)
               dElev *= 1e2;         
            
            // Calculate average and minimum elevations
            m_dAvgElev += dElev;
            
            if (dElev < m_dMinElev)
               m_dMinElev = dElev;
            
            nRead++;
         }

         double dThickness = dElev - m_dBasementElevation;
         if (dThickness <= 0)
         {
            cerr << ERR << "basement elevation must be lower than minimum DEM elevation" << endl;
            return RTN_ERR_DEMFILE;
         }
         
         // Set the initial soil surface elevation (will impose an overall gradient on this later, if the user has specified one)
         Cell[i][j].SetInitialSoilSurfaceElevation(dElev);           
         
         // Also set the basement elevation (again will impose an overall gradient on this later, if the user has specified one)
         Cell[i][j].SetBasementElevation(m_dBasementElevation);
      }
   }

   // Get rid of memory allocated to this array
   delete [] pfScanline;

   // Calculate average elevation 
   m_dAvgElev /= nRead;
   
   return (RTN_OK);
}


/*========================================================================================================================================
  
 Marks edge cells
 
========================================================================================================================================*/
void CSimulation::MarkEdgeCells(void)
{
   // Top edge
   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      for (int nY = 0; nY < m_nYGridMax; nY++)
      {
         if (Cell[nX][nY].bIsMissingValue())
         {
            m_ulMissingValueCells++;
            continue;
         }
         
         Cell[nX][nY].SetEdgeCell(DIRECTION_TOP);         
         break;
      }
   }
      
   // Bottom edge
   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      for (int nY = m_nYGridMax-1; nY >= 0; nY--)
      {
         if (Cell[nX][nY].bIsMissingValue())
         {
            m_ulMissingValueCells++;
            continue;
         }
         
         Cell[nX][nY].SetEdgeCell(DIRECTION_BOTTOM);         
         break;
      }      
   }
   
   // Left edge
   for (int nY = 0; nY < m_nYGridMax; nY++)
   {
      for (int nX = 0; nX < m_nXGridMax; nX++)
      {
         if (Cell[nX][nY].bIsMissingValue())
         {
            m_ulMissingValueCells++;
            continue;
         }
         
         Cell[nX][nY].SetEdgeCell(DIRECTION_LEFT);         
         break;
      }
   }
      
   // Right edge
   for (int nY = 0; nY < m_nYGridMax; nY++)
   {
      for (int nX = m_nXGridMax-1; nX >= 0; nX--)
      {
         if (Cell[nX][nY].bIsMissingValue())
         {
            m_ulMissingValueCells++;
            continue;
         }
         
         Cell[nX][nY].SetEdgeCell(DIRECTION_RIGHT);         
         break;
      }      
   }
}



/*========================================================================================================================================

 Reads the spatial rainfall variation data into the Cell array

=========================================================================================================================================*/
int CSimulation::nReadRainVarData(void)
{
   // Tell the user what is happening
   AnnounceReadRainVar();

   // Use GDAL to create a dataset object, which then opens the rainfall variation file
   GDALDataset* pGDALDataset = (GDALDataset *) GDALOpen(m_strRainVarMFile.c_str(), GA_ReadOnly);
   if (NULL == pGDALDataset)
   {
      // Can't open file (note will already have sent GDAL error message to stdout)
      cerr << ERR << "cannot open " << m_strRainVarMFile << " for input: " << CPLGetLastErrorMsg() << endl;
      return (RTN_ERR_RAIN_VARIATION_FILE);
   }

   // Opened OK, so get GDAL rainfall variation dataset information
   m_strGDALRainVarDriverCode = pGDALDataset->GetDriver()->GetDescription();
   m_strGDALRainVarDriverDesc = pGDALDataset->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME);
   m_strGDALRainVarProjection = pGDALDataset->GetProjectionRef();

   // If we have reference units, then check if they are meters (note US spelling here)
   string strProj = strToLower(&m_strGDALRainVarProjection);
   if ((! strProj.empty()) && (strProj.find("meter") == string::npos))
   {
      // error: microtopography x-y values must be in metres
      cerr << ERR << "microtopography x-y values (" << m_strGDALRainVarProjection << ") in " << m_strRainVarMFile << " must be 'meter'" << endl;
      return (RTN_ERR_RAIN_VARIATION_FILE);
   }

   // Get geotransformation info
   double dGeoTransform[6];

   if (CE_Failure == pGDALDataset->GetGeoTransform(dGeoTransform))
   {
      // Can't get geotransformation (note will already have sent GDAL error message to stdout)
      cerr << ERR << CPLGetLastErrorMsg() << " in " << m_strRainVarMFile << endl;
      return (RTN_ERR_RAIN_VARIATION_FILE);
   }

   // Now get dataset size, and do some checks
   int nTmp = pGDALDataset->GetRasterXSize();

   if (nTmp != m_nXGridMax)
   {
      // Error: incorrect number of columns specified
      cerr << ERR << "different number of columns in " << m_strRainVarMFile << " (" << nTmp << ") and " << m_strDEMFile <<  "(" << m_nXGridMax << ")" << endl;
      return (RTN_ERR_RAIN_VARIATION_FILE);
   }

   nTmp = pGDALDataset->GetRasterYSize();
   if (nTmp != m_nYGridMax)
   {
      // Error: incorrect number of rows specified
      cerr << ERR << "different number of rows in " << m_strRainVarMFile << " (" <<  nTmp << ") and " << m_strDEMFile << " (" << m_nYGridMax << ")" << endl;
      return (RTN_ERR_RAIN_VARIATION_FILE);
   }

   double dTmp = dGeoTransform[0];
   if (! bFPIsEqual(dTmp, m_dMinX, TOLERANCE))
   {
      // Error: different min x from microtopography file
      cerr << ERR << "different min x values in " << m_strRainVarMFile << " (" << dTmp << ") and " << m_strDEMFile << " (" << m_dMinX << ")" << endl;
      return (RTN_ERR_RAIN_VARIATION_FILE);
   }

   dTmp = dGeoTransform[3];
   if (! bFPIsEqual(dTmp, m_dMinY, TOLERANCE))
   {
      // Error: different min x from microtopography file
      cerr << ERR << "different min y values in " << m_strRainVarMFile << " (" << dTmp << ") and " << m_strDEMFile << " (" << m_dMinY << "(" << endl;
      return (RTN_ERR_RAIN_VARIATION_FILE);
   }

   double dTmpResX = tAbs(dGeoTransform[1]);
   double dTmpResY = tAbs(dGeoTransform[5]);
   if (! bFPIsEqual(dTmpResX, dTmpResY, 1e-2))
   {
      // Error: cell isn't square (enough): note that due to rounding errors in some GIS packages, must expect some discrepancies
      cerr << ERR << "cell not square (" << dTmpResX << " x " << dTmpResY << ") in " << m_strRainVarMFile << endl;
      return (RTN_ERR_RAIN_VARIATION_FILE);
   }

   // Average them
   dTmp = (dTmpResX + dTmpResY) / 2;

   // Convert from m to mm
   dTmp *= 1000;

   // Check cell side length
   if (! bFPIsEqual(dTmp, m_dCellSide, TOLERANCE))
   {
      // Error: different cell side length from microtopography file
      cerr << ERR << "different cell side lengths in " << m_strRainVarMFile << " (" << dTmp << ") and " << m_strDEMFile << " (" << m_dCellSide << ")" << endl;
      return (RTN_ERR_RAIN_VARIATION_FILE);
   }

   // Now get GDAL raster band information
   GDALRasterBand* pGDALBand = pGDALDataset->GetRasterBand(1);
   int nBlockXSize = 0, nBlockYSize = 0;
   pGDALBand->GetBlockSize(&nBlockXSize, &nBlockYSize);
   m_strGDALRainVarDataType = GDALGetDataTypeName(pGDALBand->GetRasterDataType());
   
   // If present, get the missing value setting
   CPLPushErrorHandler(CPLQuietErrorHandler);                  // Needed to get next line to fail silently, if it fails                                              
   double dMissingValue = pGDALBand->GetNoDataValue();         // Note will fail for some formats
   CPLPopErrorHandler();
   
   if (dMissingValue != m_dMissingValue)
   {
      // Error: different missing value setting in this file and in DEM
      cerr << WARN << "different NODATA values in " << m_strRainVarMFile << " and " << m_strDEMFile  << endl;
   }
   
   // Needed to calculate mean value of rainfall variation multiplier
   unsigned long ulNCell = 0;
   m_dRainVarMFileMean = 0;

   // Allocate memory for a 1D array, to hold the scan line for GDAL
   float* pfScanline;
   pfScanline = new float[m_nXGridMax];
   if (NULL == pfScanline)
   {
      // Error, can't allocate memory
      cerr << ERR << "cannot allocate memory for " << m_nXGridMax << " x 1D array" << endl;
      return (RTN_ERR_MEMALLOC);
   }

   // Now read in the data
   for (int j = 0; j < m_nYGridMax; j++)
   {
      // Read scanline
      if (CE_Failure == pGDALBand->RasterIO(GF_Read, 0, j, m_nXGridMax, 1, pfScanline, m_nXGridMax, 1, GDT_Float32, 0, 0))
      {
         // Error while reading scanline
         cerr << ERR << CPLGetLastErrorMsg() << " in " << m_strRainVarMFile << endl;
         return (RTN_ERR_RAIN_VARIATION_FILE);
      }

      // All OK, so read scanline into cell elevations
      for (int i = 0; i < m_nXGridMax; i++)
      {
         Cell[i][j].pGetRainAndRunon()->SetRainVarM (pfScanline[i]);

         m_dRainVarMFileMean += pfScanline[i];
         ulNCell++;
      }
   }

   // Calculate mean value of rainfall variation multipliers
   m_dRainVarMFileMean /= ulNCell;

   // Get rid of memory allocated to this array
   delete[] pfScanline;

   return (RTN_OK);
}


/*========================================================================================================================================

 Checks whether the selected GDAL driver supports file creation, 32-bit doubles, etc.

=========================================================================================================================================*/
bool CSimulation::bCheckGISOutputFormat(void)
{
   // If user hasn't specified a GIS output format, assume that we will use the same GIS format as the input DEM
   if (m_strGISOutFormat.empty())
      m_strGISOutFormat = m_strGDALDEMDriverCode;

   GDALDriver* pDriver;
   char** papszMetadata;
   pDriver = GetGDALDriverManager()->GetDriverByName(m_strGISOutFormat.c_str());
   if (NULL == pDriver)
   {
      // Can't load GDAL driver. Incorrectly specified?
      cerr << ERR << "Unknown GIS output format '" << m_strGISOutFormat << "'." << endl;
      return (false);
   }

   // Get the metadata for this driver
   papszMetadata = pDriver->GetMetadata();

   if (! CSLFetchBoolean(papszMetadata, GDAL_DCAP_CREATE, FALSE))
   {
      // Driver does not supports create() method
      cerr << ERR << "Cannot write GIS files using GDAL driver '" << m_strGISOutFormat << "'. Choose another format." << endl;
      return (false);
   }

   if (! strstr(CSLFetchNameValue (papszMetadata, "DMD_CREATIONDATATYPES"), "Float32"))
   {
      // Driver does not supports 32-bit doubles
      cerr << ERR << "Cannot write floating-point values using GDAL driver '" << m_strGISOutFormat << "'. Choose another format." << endl;
      return (false);
   }

   // This driver is OK, so store its longname and the default file extension
   m_strGDALOutputDriverLongname = CSLFetchNameValue(papszMetadata, "DMD_LONGNAME");
   m_strGDALOutputDriverExtension = CSLFetchNameValue(papszMetadata, "DMD_EXTENSION");

   return (true);
}


/*========================================================================================================================================

 Writes floating point GIS files using GDAL, using data from the Cell array

=========================================================================================================================================*/
bool CSimulation::bWriteFileFloat(int const nDataItem, string const* pstrPlotTitle)
{
   // Increment file number when soil loss file is written (this is done first, and is always saved)
   if (PLOT_CUMULATIVE_ALL_SIZE_FLOW_DETACHMENT == nDataItem)
      m_nGISSave++;

   // Now begin constructing the file name for this save
   string strFilDat = m_strOutputPath;

   switch (nDataItem)
   {
      case (PLOT_TOTAL_RAIN) :
         strFilDat.append(RAIN_NAME);
         break;

      case (PLOT_RAIN_SPATIAL_VARIATION) :
         strFilDat.append(RAIN_VARIATION_NAME);
         break;

      case (PLOT_TOTAL_RUNON) :
         strFilDat.append(RUNON_NAME);
         break;

      case (PLOT_ELEVATION) :
         strFilDat.append(ELEVATION_NAME);
         break;

      case (PLOT_DETRENDED_ELEVATION) :
         strFilDat.append(DETRENDED_ELEVATION_NAME);
         break;

      case (PLOT_SURFACE_WATER_DEPTH) :
         strFilDat.append(SURFACE_WATER_DEPTH_NAME);
         break;

      case (PLOT_ALL_SIZE_FLOW_DETACHMENT) :
         strFilDat.append(ALL_SIZE_FLOW_DETACHMENT_NAME);
         break;

      case (PLOT_INFILTRATION) :
         strFilDat.append(INFILTRATION_NAME);
         break;

      case (PLOT_CUMULATIVE_INFILTRATION_) :
         strFilDat.append(CUMULATIVE_INFILTRATION_NAME);
         break;

      case (PLOT_SOIL_WATER) :
         strFilDat.append(SOIL_WATER_NAME);
         break;

      case (PLOT_INFILTRATION_DEPOSIT) :
         strFilDat.append(INFILTRATION_DEPOSITION_NAME);
         break;

      case (PLOT_CUMULATIVE_INFILTRATION_DEPOSIT) :
         strFilDat.append(CUMULATIVE_INFILTRATION_DEPOSITION_NAME);
         break;

      case (PLOT_TOP) :
         strFilDat.append(TOPSNAME);
         break;

      case (PLOT_SPLASH) :
         strFilDat.append(SPLASHNAME);
         break;

      case (PLOT_TOTAL_SPLASH) :
         strFilDat.append(TOTSPLASHNAME);
         break;

      case (PLOT_SURFACE_WATER_FLOW_SPEED) :
         strFilDat.append(SPEEDNAME);
         break;

      case (PLOT_SURFACE_WATER_DW_FLOW_SPEED) :
         strFilDat.append(DWSPEEDNAME);
         break;

#if defined _DEBUG

      case (PLOT_LOST_FROM_EDGES) :
         strFilDat.append(LOSTNAME);
         break;
#endif

      case (PLOT_STREAMPOWER) :
         strFilDat.append(STREAMPOWERNAME);
         break;

      case (PLOT_SHEAR_STRESS) :
         strFilDat.append(SHEARSTRESSNAME);
         break;

      case (PLOT_FRICTION_FACTOR) :
         strFilDat.append(FRICTIONFACTORNAME);
         break;

      case (PLOT_AVG_SHEAR_STRESS) :
         strFilDat.append(AVGSHEARSTRESSNAME);
         break;

      case (PLOT_REYNOLDS_NUMBER) :
         strFilDat.append(REYNOLDSNAME);
         break;

      case (PLOT_FROUDE_NUMBER) :
         strFilDat.append(FROUDENAME);
         break;

      case (PLOT_TRANSPORT_CAPACITY) :
         strFilDat.append(TCNAME);
         break;

      case (PLOT_AVG_SURFACE_WATER_DEPTH) :
         strFilDat.append(AVG_SURFACE_WATER_DEPTH_NAME);
         break;

      case (PLOT_SURFACE_WATER_AVG_FLOW_SPEED) :
         strFilDat.append(AVGSPEEDNAME);
         break;

      case (PLOT_SURFACE_WATER_AVG_DW_FLOW_SPEED) :
         strFilDat.append(AVGDWSPEEDNAME);
         break;

      case (PLOT_CUMULATIVE_ALL_SIZE_FLOW_DETACHMENT) :
         strFilDat.append(CUMULATIVE_ALL_SIZE_FLOW_DETACHMENT_NAME);
         break;

      case (PLOT_SEDIMENT_CONCENTRATION) :
         strFilDat.append(SEDCONCNAME);
         break;

      case (PLOT_SEDIMENT_LOAD) :
         strFilDat.append(SEDLOADNAME);
         break;

      case (PLOT_AVG_SEDIMENT_LOAD) :
         strFilDat.append(AVGSEDLOADNAME);
         break;

      case (PLOT_NET_SLUMP) :
         strFilDat.append(NET_SLUMPING_NAME);
         break;

      case (PLOT_NET_TOPPLE) :
         strFilDat.append(NET_TOPPLING_NAME);
         break;

      case (PLOT_CUMULATIVE_ALL_SIZE_FLOW_DEPOSITION) :
         strFilDat.append(CUMULATIVE_ALL_SIZE_FLOW_DEPOSITION_NAME);
         break;

      case (PLOT_TOT_SURFACE_LOWERING) :
         strFilDat.append(TOT_SURFACE_LOWERING_NAME);
   }

   // Append the 'save number' to the filename, and prepend zeros to the save number
   strFilDat.append("_");
   stringstream ststrTmp;
   ststrTmp << FillToWidth('0', MAX_GIS_FILENAME_SAVE_DIGITS) << m_nGISSave;
   strFilDat.append(ststrTmp.str());

   // Finally, maybe append the extension
   if (! m_strGDALOutputDriverExtension.empty())
   {
      strFilDat.append(".");
      strFilDat.append(m_strGDALOutputDriverExtension);
   }

   GDALDriver* pDriver;
   pDriver = GetGDALDriverManager()->GetDriverByName(m_strGISOutFormat.c_str());
   GDALDataset* pOutDataSet;
   char** papszOptions = NULL;      // driver-specific options
   
   pOutDataSet = pDriver->Create(strFilDat.c_str(), m_nXGridMax, m_nYGridMax, 1, GDT_Float32, papszOptions);
   if (NULL == pOutDataSet)
   {
      // Couldn't create file
      cerr << ERR << "cannot create " << m_strGISOutFormat << " file named " << strFilDat << "\n" << CPLGetLastErrorMsg() << endl;
      return false;
   }

   // Set projection info for output dataset (will be same as was read in from DEM)
   CPLPushErrorHandler(CPLQuietErrorHandler);                  // needed to get next line to fail silently, if it fails
   pOutDataSet->SetProjection(m_strGDALDEMProjection.c_str()); // will fail for some formats
   CPLPopErrorHandler();

   // Set geotransformation info for output dataset (will be same as was read in from DEM)
   if (CE_Failure == pOutDataSet->SetGeoTransform(m_dGeoTransform))
      cerr << WARN << "cannot write geotransformation information to " << m_strGISOutFormat << " file named " << strFilDat << "\n" << CPLGetLastErrorMsg() << endl;
   
   // Allocate memory for a 1D array, to hold the floating point raster band data for GDAL
   float* pfRaster;
   pfRaster = new float[m_nXGridMax * m_nYGridMax];
   if (NULL == pfRaster)
   {
      // Error, can't allocate memory
      cerr << ERR << "cannot allocate memory for " << m_nXGridMax * m_nYGridMax << " x 1D floating-point array for " << m_strGISOutFormat << " file named " << strFilDat << endl;
      return RTN_ERR_MEMALLOC;
   }

   // Fill the array
   int n = 0;
   double 
      dTmp = 0, 
      dTmp1,
      dDiff = 0;
   for (int y = 0; y < m_nYGridMax; y++)
   {
      for (int x = 0; x < m_nXGridMax; x++)
      {
         switch (nDataItem)
         {
            case (PLOT_TOTAL_RAIN) :
               dTmp = Cell[x][y].pGetRainAndRunon()->dGetTotRain();
               break;

            case (PLOT_RAIN_SPATIAL_VARIATION) :
               dTmp = Cell[x][y].pGetRainAndRunon()->dGetRainVarM();
               break;

            case (PLOT_TOTAL_RUNON) :
               dTmp = Cell[x][y].pGetRainAndRunon()->dGetTotRunOn();
               break;

            case (PLOT_ELEVATION) :
               dTmp = Cell[x][y].pGetSoil()->dGetSoilSurfaceElevation();
               if (dTmp != m_dMissingValue)
               {
                  if (m_bOutDEMsUsingInputZUnits)
                  {
                     // The Z elevation is in mm or cm, but the user wants output DEMs with original Z units, so do some conversion if necessary
                     if (m_nZUnits == Z_UNIT_M)
                        dTmp /= 1e3;            
                     else if (m_nZUnits == Z_UNIT_CM)
                        dTmp /= 1e2;            
                  }
               }
               
               break;

            case (PLOT_DETRENDED_ELEVATION) :
               dTmp = Cell[x][y].pGetSoil()->dGetSoilSurfaceElevation();               
               if (dTmp != m_dMissingValue)
               {
                  if (m_bOutDEMsUsingInputZUnits)
                  {
                     // The Z elevation is in mm or cm, but the user wants output DEMs with original Z units, so do some conversion if necessary
                     if (m_nZUnits == Z_UNIT_M)
                        dTmp /= 1e3;            
                     else if (m_nZUnits == Z_UNIT_CM)
                        dTmp /= 1e2;            
                  }
                  
                  dTmp += dDiff; 
               }
               
               break;

            case (PLOT_SURFACE_WATER_DEPTH) :
               dTmp = Cell[x][y].pGetSurfaceWater()->dGetSurfaceWater();
               break;

            case (PLOT_ALL_SIZE_FLOW_DETACHMENT) :
               dTmp = Cell[x][y].pGetSoil()->dGetTotFlowDetach();
               break;

            case (PLOT_INFILTRATION) :
               dTmp = Cell[x][y].pGetSoilWater()->dGetInfiltration();
               break;

            case (PLOT_CUMULATIVE_INFILTRATION_) :
               dTmp = Cell[x][y].pGetSoilWater()->dGetCumulativeInfiltration();
               break;

            case (PLOT_SOIL_WATER) :
               dTmp = Cell[x][y].pGetSoilWater()->dGetTopLayerSoilWater();
               break;

            case (PLOT_INFILTRATION_DEPOSIT) :
               dTmp = Cell[x][y].pGetSoil()->dGetTotInfiltDeposit();
               break;

            case (PLOT_CUMULATIVE_INFILTRATION_DEPOSIT) :
               dTmp = Cell[x][y].pGetSoil()->dGetCumulativeTotInfiltDeposit();
               break;

            case (PLOT_TOP):
               dTmp = Cell[x][y].dGetTopElevation();
               if (dTmp != m_dMissingValue)
               {
                  if (m_bOutDEMsUsingInputZUnits)
                  {
                     // The Z elevation is in mm or cm, but the user wants output DEMs with original Z units, so do some conversion if necessary
                     if (m_nZUnits == Z_UNIT_M)
                        dTmp /= 1e3;            
                     else if (m_nZUnits == Z_UNIT_CM)
                        dTmp /= 1e2;            
                  }
               }
               
               break;

            case (PLOT_SPLASH) :
               dTmp = Cell[x][y].pGetSoil()->dGetTotSplashDetach() - Cell[x][y].pGetSoil()->dGetTotSplashDeposit();
               break;

            case (PLOT_TOTAL_SPLASH) :
               dTmp = Cell[x][y].pGetSoil()->dGetTotSplashDetach() - Cell[x][y].pGetSoil()->dGetTotSplashDeposit();
               break;

            case (PLOT_SURFACE_WATER_FLOW_SPEED) :
               dTmp = Cell[x][y].pGetSurfaceWater()->dGetFlowSpd();
               break;

            case (PLOT_SURFACE_WATER_DW_FLOW_SPEED) :
               dTmp = Cell[x][y].pGetSurfaceWater()->dGetDWFlowSpd();
               break;

#if defined _DEBUG
            case (PLOT_LOST_FROM_EDGES) :
               dTmp = Cell[x][y].pGetSurfaceWater()->dGetCumulativeSurfaceWaterLost();
               break;
#endif

            case (PLOT_STREAMPOWER) :
               dTmp = Cell[x][y].pGetSurfaceWater()->dGetStreamPower();
               break;

            case PLOT_SHEAR_STRESS:
               dTmp1 = m_dSimulatedTimeElapsed - m_dLastSlumpCalcTime;
               if (dTmp1 > 0)
                  dTmp = Cell[x][y].pGetSoil()->dGetShearStress() / (m_dSimulatedTimeElapsed - m_dLastSlumpCalcTime);
               else
                  dTmp = 0;
               break;

            case (PLOT_FRICTION_FACTOR) :
               dTmp = Cell[x][y].pGetSurfaceWater()->dGetFrictionFactor();
               break;

            case (PLOT_AVG_SHEAR_STRESS) :
               dTmp = Cell[x][y].pGetSoil()->dGetTotShearStress() / m_dSimulatedTimeElapsed;
               break;

            case (PLOT_REYNOLDS_NUMBER) :
               dTmp = Cell[x][y].pGetSurfaceWater()->dGetReynolds(m_dNu);
               break;

            case (PLOT_FROUDE_NUMBER) :
               dTmp = Cell[x][y].pGetSurfaceWater()->dGetFroude(m_dG);
               break;

            case (PLOT_TRANSPORT_CAPACITY) :
               dTmp = Cell[x][y].pGetSurfaceWater()->dGetTransportCapacity();
               break;

            case (PLOT_AVG_SURFACE_WATER_DEPTH) :
               dTmp = Cell[x][y].pGetSurfaceWater()->dGetCumulativeSurfaceWater() / m_dSimulatedTimeElapsed;
               break;

            case (PLOT_SURFACE_WATER_AVG_FLOW_SPEED) :
               dTmp = Cell[x][y].pGetSurfaceWater()->dCumulativeFlowSpeed() / m_dSimulatedTimeElapsed;
               break;

            case (PLOT_SURFACE_WATER_AVG_DW_FLOW_SPEED) :
               dTmp = Cell[x][y].pGetSurfaceWater()->dGetTotDWFlowSpd() / m_dSimulatedTimeElapsed;
               break;

            case (PLOT_CUMULATIVE_ALL_SIZE_FLOW_DETACHMENT) :
               dTmp = Cell[x][y].pGetSoil()->dGetCumulativeTotFlowDetach();
               break;

            case (PLOT_SEDIMENT_CONCENTRATION) :
               dTmp = Cell[x][y].pGetSediment()->dGetAllSizeSedimentConcentration();
               break;

            case (PLOT_SEDIMENT_LOAD) :
               dTmp = Cell[x][y].pGetSediment()->dGetAllSizeSedimentLoad();
               break;

            case (PLOT_AVG_SEDIMENT_LOAD) :
               dTmp = Cell[x][y].pGetSediment()->dGetAllSizeSedimentLoad() / m_dSimulatedTimeElapsed;
               break;

            case (PLOT_NET_SLUMP) :
               dTmp = Cell[x][y].pGetSoil()->dGetTotSlumpDetach();
               break;

            case (PLOT_NET_TOPPLE) :
               dTmp = Cell[x][y].pGetSoil()->dGetTotToppleDetach() - Cell[x][y].pGetSoil()->dGetTotToppleDeposit();
               break;

            case (PLOT_CUMULATIVE_ALL_SIZE_FLOW_DEPOSITION) :
               dTmp = Cell[x][y].pGetSoil()->dGetCumulativeTotFlowDeposition();
               break;

            case (PLOT_TOT_SURFACE_LOWERING):
               // Note that detachment is +ve, deposition is -ve
               dTmp = Cell[x][y].pGetSoil()->dGetTotNetSoilLoss();
               break;
         }

         // Write this value to the array
         pfRaster[n++] = dTmp;
      }

      dDiff += m_dYInc;    // PLOT_DETRENDED_ELEVATION and PLOT_TOP only
   }

   // Now write the data. Create a single raster band
   GDALRasterBand* pBand;
   pBand = pOutDataSet->GetRasterBand(1);
   if (pBand->RasterIO(GF_Write, 0, 0, m_nXGridMax, m_nYGridMax, pfRaster, m_nXGridMax, m_nYGridMax, GDT_Float32, 0, 0) == CE_Failure)
   {
      // Write error, better error message
      cerr << ERR << "cannot write data for " << m_strGISOutFormat << " file named " << strFilDat << "\n" << CPLGetLastErrorMsg() << endl;
      delete[] pfRaster;
      return false;
   }

   // Calculate statistics for this band
   double dMin, dMax, dMean, dStdDev;
   if (pBand->ComputeStatistics(false, &dMin, &dMax, &dMean, &dStdDev, NULL, NULL) == CE_Failure)
      cerr << ERR << CPLGetLastErrorMsg() << endl;

   // And then write the statistics, fail silently if not supported by this format
   pBand->SetStatistics(dMin, dMax, dMean, dStdDev);

   // Set value units for this band
   string strUnits;

   switch (nDataItem)
   {
      case (PLOT_ELEVATION) :
      case (PLOT_DETRENDED_ELEVATION) :
      case (PLOT_TOTAL_RAIN) :
      case (PLOT_TOTAL_RUNON) :
      case (PLOT_SURFACE_WATER_DEPTH) :
      case (PLOT_AVG_SURFACE_WATER_DEPTH) :
      case (PLOT_INFILTRATION) :
      case (PLOT_CUMULATIVE_INFILTRATION_) :
      case (PLOT_SOIL_WATER) :
      case (PLOT_INFILTRATION_DEPOSIT) :
      case (PLOT_CUMULATIVE_INFILTRATION_DEPOSIT) :
      case (PLOT_TOP) :
      case (PLOT_SEDIMENT_LOAD) :
      case (PLOT_AVG_SEDIMENT_LOAD) :
      case (PLOT_SPLASH) :
      case (PLOT_TOTAL_SPLASH) :
      case (PLOT_ALL_SIZE_FLOW_DETACHMENT) :
      case (PLOT_TRANSPORT_CAPACITY) :
      case (PLOT_NET_SLUMP) :
      case (PLOT_NET_TOPPLE) :
      case (PLOT_CUMULATIVE_ALL_SIZE_FLOW_DETACHMENT) :
      case (PLOT_CUMULATIVE_ALL_SIZE_FLOW_DEPOSITION) :
      case (PLOT_TOT_SURFACE_LOWERING) :
#if defined _DEBUG
      case (PLOT_LOST_FROM_EDGES) :
#endif
         strUnits = "mm";
         break;

      case (PLOT_SURFACE_WATER_FLOW_SPEED) :
      case (PLOT_SURFACE_WATER_AVG_FLOW_SPEED) :
      case (PLOT_SURFACE_WATER_DW_FLOW_SPEED) :
      case (PLOT_SURFACE_WATER_AVG_DW_FLOW_SPEED) :
         strUnits = "mm/sec";
         break;

      case (PLOT_STREAMPOWER) :
         strUnits = "kg/s**3";
         break;

      case (PLOT_SHEAR_STRESS) :
      case (PLOT_AVG_SHEAR_STRESS) :
         strUnits = "kg/m s**2";
         break;

      case (PLOT_SEDIMENT_CONCENTRATION) :
         strUnits = "%";
         break;

      case (PLOT_RAIN_SPATIAL_VARIATION) :
      case (PLOT_REYNOLDS_NUMBER) :
      case (PLOT_FROUDE_NUMBER) :
      case (PLOT_FRICTION_FACTOR) :
         strUnits = "none";
   }

   CPLPushErrorHandler(CPLQuietErrorHandler);                  // Needed to get next line to fail silently, if it fails
   pBand->SetUnitType(strUnits.c_str());                       // Not supported for some GIS formats
   CPLPopErrorHandler();

   // Tell the output dataset about missing values
   CPLPushErrorHandler(CPLQuietErrorHandler);                  // Needed to get next line to fail silently, if it fails
   pBand->SetNoDataValue(m_dMissingValue);                     // Will fail for some formats
   CPLPopErrorHandler();
   
   // Construct the description
   string strDesc = *pstrPlotTitle;
   strDesc.append(" at ");
   strDesc.append(strDispTime(m_dSimulatedTimeElapsed, true, false));

   // Set the GDAL description
   pBand->SetDescription(strDesc.c_str());

   // Finished, so get rid of dataset object
   delete pOutDataSet;

   // Also get rid of memory allocated to this array
   delete[] pfRaster;

   return true;
}


/*========================================================================================================================================

 Writes integer GIS files using GDAL, using data from the Cell array

=========================================================================================================================================*/
bool CSimulation::bWriteFileInt(int const nDataItem, string const* pstrPlotTitle)
{
   // Begin constructing the file name for this save
   string strFilDat = m_strOutputPath;

   switch (nDataItem)
   {
      case (PLOT_INUNDATION_REGIME) :
         strFilDat.append(INUNDATIONNAME);
         break;

      case (PLOT_SURFACE_WATER_FLOW_DIRECTION) :
         strFilDat.append(FLOWDIRNAME);
         break;
   }

   // Append the 'save number' to the filename, and prepend zeros to the save number
   strFilDat.append("_");
   stringstream ststrTmp;
   ststrTmp << FillToWidth('0', MAX_GIS_FILENAME_SAVE_DIGITS) << m_nGISSave;
   strFilDat.append(ststrTmp.str());

   // Finally, maybe append the extension
   if (! m_strGDALOutputDriverExtension.empty())
   {
      strFilDat.append(".");
      strFilDat.append(m_strGDALOutputDriverExtension);
   }

   GDALDriver* pDriver;
   pDriver = GetGDALDriverManager()->GetDriverByName(m_strGISOutFormat.c_str());
   GDALDataset* pOutDataSet;
   char** papszOptions = NULL;      // driver-specific options
   pOutDataSet = pDriver->Create(strFilDat.c_str(), m_nXGridMax, m_nYGridMax, 1, GDT_Float32, papszOptions);
   if (NULL == pOutDataSet)
   {
      // Couldn't create file
      cerr << ERR << "cannot create " << m_strGISOutFormat << " file named " << strFilDat << "\n" << CPLGetLastErrorMsg() << endl;
      return false;
   }

   // Set projection info for output dataset (will be same as was read in from DEM)
   CPLPushErrorHandler(CPLQuietErrorHandler);                  // needed to get next line to fail silently, if it fails
   pOutDataSet->SetProjection(m_strGDALDEMProjection.c_str()); // will fail for some formats
   CPLPopErrorHandler();

   // Set geotransformation info for output dataset (will be same as was read in from DEM)
   if (CE_Failure == pOutDataSet->SetGeoTransform(m_dGeoTransform))
      cerr << WARN << "cannot write geotransformation information to " << m_strGISOutFormat << " file named " << strFilDat << "\n" << CPLGetLastErrorMsg() << endl;

   // Allocate memory for a 1D array, to hold the integer raster band data for GDAL
   int* pnRaster;
   pnRaster = new int[m_nXGridMax * m_nYGridMax];
   if (NULL == pnRaster)
   {
      // Error, can't allocate memory
      cerr << ERR << "cannot allocate memory for " << m_nXGridMax * m_nYGridMax << " x 1D floating-point array for " << m_strGISOutFormat << " file named " << strFilDat << endl;
      return RTN_ERR_MEMALLOC;
   }

   // Fill the array
   int nTmp  = 0;
   int n = 0;
   for (int y = 0; y < m_nYGridMax; y++)
   {
      for (int x = 0; x < m_nXGridMax; x++)
      {
         switch (nDataItem)
         {
            case (PLOT_INUNDATION_REGIME) :
               nTmp = Cell[x][y].pGetSurfaceWater()->nGetInundation();
               break;

            case (PLOT_SURFACE_WATER_FLOW_DIRECTION) :
               nTmp = Cell[x][y].pGetSurfaceWater()->nGetFlowDirection();
               break;
         }

         // Write this value to the array
         pnRaster[n++] = nTmp;
      }
   }

   // Now write the data. Create a single raster band
   GDALRasterBand* pBand;
   pBand = pOutDataSet->GetRasterBand(1);
   if (CE_Failure == pBand->RasterIO(GF_Write, 0, 0, m_nXGridMax, m_nYGridMax, pnRaster, m_nXGridMax, m_nYGridMax, GDT_Int32, 0, 0))
   {
      // Write error, better error message
      cerr << ERR << "cannot write data for " << m_strGISOutFormat << " file named " << strFilDat << "\n" << CPLGetLastErrorMsg() << endl;
      delete[] pnRaster;
      return false;
   }

   // Calculate statistics for this band
   double dMin, dMax, dMean, dStdDev;
   if (CE_Failure == pBand->ComputeStatistics(false, &dMin, &dMax, &dMean, &dStdDev, NULL, NULL))
      cerr << ERR << CPLGetLastErrorMsg() << endl;

   // And then write the statistics, fail silently if not supported by this format
   pBand->SetStatistics(dMin, dMax, dMean, dStdDev);

   // Set value units for this band
   string strUnits;

   switch (nDataItem)
   {
      case (PLOT_INUNDATION_REGIME):

      case (PLOT_SURFACE_WATER_FLOW_DIRECTION):
         strUnits = "none";
   }

   CPLPushErrorHandler(CPLQuietErrorHandler);                  // Needed to get next line to fail silently, if it fails
   pBand->SetUnitType(strUnits.c_str());                       // Not supported for some GIS formats
   CPLPopErrorHandler();
   
   // Tell the output dataset about missing values
   CPLPushErrorHandler(CPLQuietErrorHandler);                  // Needed to get next line to fail silently, if it fails
   pBand->SetNoDataValue(m_dMissingValue);                     // Will fail for some formats
   CPLPopErrorHandler();

   // Construct the description
   string strDesc = *pstrPlotTitle;
   strDesc.append(" at ");
   strDesc.append(strDispTime(m_dSimulatedTimeElapsed, true, false));

   // Set the GDAL description
   pBand->SetDescription(strDesc.c_str());

   // Set raster category names
   char** papszCategoryNames = NULL;

   switch (nDataItem)
   {
      case (PLOT_INUNDATION_REGIME) :
         papszCategoryNames = CSLAddString(papszCategoryNames, "Dry");
         papszCategoryNames = CSLAddString(papszCategoryNames, "Shallow");
         papszCategoryNames = CSLAddString(papszCategoryNames, "Marginally inundated");
         papszCategoryNames = CSLAddString(papszCategoryNames, "Well inundated");
         break;

      case (PLOT_SURFACE_WATER_FLOW_DIRECTION) :
         papszCategoryNames = CSLAddString(papszCategoryNames, "None");
         papszCategoryNames = CSLAddString(papszCategoryNames, "Top");
         papszCategoryNames = CSLAddString(papszCategoryNames, "Top right");
         papszCategoryNames = CSLAddString(papszCategoryNames, "Right");
         papszCategoryNames = CSLAddString(papszCategoryNames, "Bottom right");
         papszCategoryNames = CSLAddString(papszCategoryNames, "Bottom");
         papszCategoryNames = CSLAddString(papszCategoryNames, "Bottom left");
         papszCategoryNames = CSLAddString(papszCategoryNames, "Left");
         papszCategoryNames = CSLAddString(papszCategoryNames, "Top left");
         break;
   }

   CPLPushErrorHandler(CPLQuietErrorHandler);        // Needed to get next line to fail silently, if it fails
   pBand->SetCategoryNames(papszCategoryNames);      // Not supported for some GIS formats
   CPLPopErrorHandler();

   // Finished, so get rid of dataset object
   delete pOutDataSet;

   // Also get rid of memory allocated to this array
   delete[] pnRaster;

   return true;
}


/*========================================================================================================================================

 This subroutine smooths the unbounded edges of the Cell array of the DEM to prevent excessive deepening of the edge. For each of the N_LOW_POINTS cells with the lowest elevation on each unbounded grid edge, a new elevation is calculated based on the elevations of nearby cells. This new low-point elevation may be lower or higher than the original value

========================================================================================================================================*/
void CSimulation::AdjustUnboundedEdges(void)
{
   int const DIST_ACROSS = 4;                 // Arbitrary, but seems to work OK
   int const DIST_IN     = 8;
   int const N_LOW_POINTS = 5;
   double const WEIGHT = 0.0;
//   double const WEIGHT = 0.5;    // 0.8 works OK for x11

   // Is the planview bottom edge open?
   if (! m_bClosedThisEdge[EDGE_BOTTOM])
   {
      // We have flow off the planview bottom edge, we will look along this edge for N_LOW_POINTS low points
      int nLowEdgePoint[N_LOW_POINTS];

      for (int n = 0; n < N_LOW_POINTS; n++)
      {
         // Find the elevation of the nth lowest point on this edge
         int nMinElevPos = 0;
         double dMinElev = DBL_MAX;

         for (int nX = 0; nX < m_nXGridMax; nX++)
         {
            double dElev = Cell[nX][m_nYGridMax-1].pGetSoil()->dGetSoilSurfaceElevation();
            if (dElev < dMinElev)
            {
               // We have a low point, but have we found it previously?
               bool bFoundPrev = false;
               for (int m = 0; m < n; m++)
               {
                  if (nX == nLowEdgePoint[m])
                  {
                     bFoundPrev = true;
                     break;
                  }
               }

               if (! bFoundPrev)
               {
                  // No, we have not found this one before
                  dMinElev = dElev;
                  nMinElevPos = nX;
               }
            }
         }

         // Are we at or below base level, or have we hit the bottom of the lowest soil layer? If so, do nothing more with this low point
         double dBasementElevation = Cell[nMinElevPos][m_nYGridMax-1].dGetBasementElevation();
         if ((m_bHaveBaseLevel && dMinElev > m_dBaseLevel) || (dMinElev >= dBasementElevation))
            continue;
         
         // We have found a new local minimum, so store it
         nLowEdgePoint[n] = nMinElevPos;

         // Now find the average and minimum elevations from the DIST_ACROSS x DIST_IN cells surrounding this cell
         int nRead = 0;
         double
            dAvgElevAround = 0,
            dMinElevAround = DBL_MAX;

         for (int j = m_nYGridMax-1; j >= (m_nYGridMax - DIST_IN); j--)
         {
            for (int i = (nMinElevPos - DIST_ACROSS); i <= (nMinElevPos + DIST_ACROSS); i++)
            {
               if ((i >= 0) && (i < m_nXGridMax))
               {
                  double dThisElev = Cell[i][j].pGetSoil()->dGetSoilSurfaceElevation();
                  dAvgElevAround += dThisElev;
                  nRead++;

                  if (dThisElev < dMinElevAround)
                     dMinElevAround = dThisElev;
               }
            }
         }

         // Calculate the local average
         if (nRead > 0)
            dAvgElevAround /= nRead;
         else
            dAvgElevAround = dMinElev;

         // Calculate a new value, is a weighted mix of the local minimum and local average values
         double dNewMinElev = (WEIGHT * dMinElevAround) + ((1 - WEIGHT) * dAvgElevAround);

         // Make sure that we don't go below 'base level'
         if (m_bHaveBaseLevel)
            dNewMinElev = tMax(dNewMinElev, m_dBaseLevel);
         
         // Make sure that we don't go below the bottom of the lowest soil layer
         dNewMinElev = tMax(dNewMinElev, dBasementElevation);

         // Is the new elevation value lower than the elevation of the low point?
         if (dNewMinElev > dMinElev)
         {
            // It is, so replace the elevation of the lowest-point cell with the new value
            Cell[nMinElevPos][m_nYGridMax-1].SetSoilSurfaceElevation(dNewMinElev);
            
//             LogStream << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(6);
//             LogStream << m_ulIter << ": [" << nMinElevPos << "][" << m_nYGridMax-1 << "] from " << dMinElev << " to " << dNewMinElev;
//             LogStream << " (" << (dMinElev < dNewMinElev ? "GAIN" : "loss") << " = " << tAbs(dMinElev - dNewMinElev);
//             LogStream << " m_dPlotElevMin = " << m_dPlotElevMin << " m_dBaseLevel = " << m_dBaseLevel << " dMinElevAround = " << dMinElevAround << " dAvgElevAround = " << dAvgElevAround << ")" << endl;
         }
      }
   }

   // Is the planview top edge open?
   if (! m_bClosedThisEdge[EDGE_TOP])
   {
      // We have flow off the planview top edge, we will look along this edge for N_LOW_POINTS low points
      int nLowEdgePoint[N_LOW_POINTS];

      for (int n = 0; n < N_LOW_POINTS; n++)
      {
         // Find the elevation of the nth lowest point on this edge
         int nMinElevPos = 0;
         double dMinElev = DBL_MAX;

         for (int nX = 0; nX < m_nXGridMax; nX++)
         {
            double dElev = Cell[nX][0].pGetSoil()->dGetSoilSurfaceElevation();
            if (dElev < dMinElev)
            {
               // We have a low point, but have we found it previously?
               bool bFoundPrev = false;
               for (int m = 0; m < n; m++)
               {
                  if (nX == nLowEdgePoint[m])
                  {
                     bFoundPrev = true;
                     break;
                  }
               }

               if (! bFoundPrev)
               {
                  // No, we have not found this one before
                  dMinElev = dElev;
                  nMinElevPos = nX;
               }
            }
         }

         // Are we at or below base level, or have we hit the bottom of the lowest soil layer? If so, do nothing more with this low point
         double dBasementElevation = Cell[nMinElevPos][0].dGetBasementElevation();
         if ((m_bHaveBaseLevel && dMinElev > m_dBaseLevel) || (dMinElev >= dBasementElevation))
            continue;
         
         // We have found a new local minimum, so store it
         nLowEdgePoint[n] = nMinElevPos;
         
         // Now find the average and minimum elevations from the DIST_ACROSS x DIST_IN cells surrounding this cell
         int nRead = 0;
         double
            dAvgElevAround = 0,
            dMinElevAround = DBL_MAX;

         for (int j = 0; j < DIST_IN; j++)
         {
            for (int i = (nMinElevPos - DIST_ACROSS); i <= (nMinElevPos + DIST_ACROSS); i++)
            {
               if ((i >= 0) && (i < m_nXGridMax))
               {
                  double dThisElev = Cell[i][j].pGetSoil()->dGetSoilSurfaceElevation();
                  dAvgElevAround += dThisElev;
                  nRead++;

                  if (dThisElev < dMinElevAround)
                     dMinElevAround = dThisElev;
               }
            }
         }

         // Calculate the local average
         if (nRead > 0)
            dAvgElevAround /= nRead;
         else
            dAvgElevAround = dMinElev;

         // Calculate a new value, is a weighted mix of the local minimum and local average values
         double dNewMinElev = (WEIGHT * dMinElevAround) + ((1 - WEIGHT) * dAvgElevAround);

         // Make sure that we don't go below 'base level'
         if (m_bHaveBaseLevel)
            dNewMinElev = tMax(dNewMinElev, m_dBaseLevel);
         
         // Make sure that we don't go below the bottom of the lowest soil layer
         dNewMinElev = tMax(dNewMinElev, dBasementElevation);
         
         // Is the new elevation value lower than the elevation of the low point?
         if (dNewMinElev > dMinElev)
         {
            // It is, so replace the elevation of the lowest-point cell with the new value
            Cell[nMinElevPos][0].SetSoilSurfaceElevation(dNewMinElev);
            
//             LogStream << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(6);
//             LogStream << m_ulIter << ": [" << nMinElevPos << "][" << 0 << "] from " << dMinElev << " to " << dNewMinElev;
//             LogStream << " (" << (dMinElev < dNewMinElev ? "GAIN" : "loss") << " = " << tAbs(dMinElev - dNewMinElev);
//             LogStream << " m_dPlotElevMin = " << m_dPlotElevMin << " m_dBaseLevel = " << m_dBaseLevel << " dMinElevAround = " << dMinElevAround << " dAvgElevAround = " << dAvgElevAround << ")" << endl;
         }
      }
   }

   // Is the planview left edge open?
   if (! m_bClosedThisEdge[EDGE_LEFT])
   {
      // We have flow off the planview left edge, we will look along this edge for N_LOW_POINTS low points
      int nLowEdgePoint[N_LOW_POINTS];

      for (int n = 0; n < N_LOW_POINTS; n++)
      {
         // Find the elevation of the nth lowest point on this edge
         int nMinElevPos = 0;
         double dMinElev = DBL_MAX;

         for (int nY = 0; nY < m_nYGridMax; nY++)
         {
            double dElev = Cell[0][nMinElevPos].pGetSoil()->dGetSoilSurfaceElevation();
            if (dElev < dMinElev)
            {
               // We have a low point, but have we found it previously?
               bool bFoundPrev = false;
               for (int m = 0; m < n; m++)
               {
                  if (nY == nLowEdgePoint[m])
                  {
                     bFoundPrev = true;
                     break;
                  }
               }

               if (! bFoundPrev)
               {
                  // No, we have not found this one before
                  dMinElev = dElev;
                  nMinElevPos = nY;
               }
            }
         }

         // Are we at or below base level, or have we hit the bottom of the lowest soil layer? If so, do nothing more with this low point
         double dBasementElevation = Cell[0][nMinElevPos].dGetBasementElevation();
         if ((m_bHaveBaseLevel && dMinElev > m_dBaseLevel) || (dMinElev >= dBasementElevation))
            continue;
         
         // We have found a new local minimum, so store it
         nLowEdgePoint[n] = nMinElevPos;
                  
         // Now find the average and minimum elevations from the DIST_ACROSS x DIST_IN cells surrounding this cell
         int nRead = 0;
         double
            dAvgElevAround = 0,
            dMinElevAround = DBL_MAX;

         for (int j = 0; j < DIST_IN; j++)
         {
            for (int i = (nMinElevPos - DIST_ACROSS); i <= (nMinElevPos + DIST_ACROSS); i++)
            {
               if ((i >= 0) && (i < m_nYGridMax))
               {
                  double dThisElev = Cell[j][i].pGetSoil()->dGetSoilSurfaceElevation();
                  dAvgElevAround += dThisElev;
                  nRead++;

                  if (dThisElev < dMinElevAround)
                     dMinElevAround = dThisElev;
               }
            }
         }

         // Calculate the local average
         if (nRead > 0)
            dAvgElevAround /= nRead;
         else
            dAvgElevAround = dMinElev;

         // Calculate a new value, is a weighted mix of the local minimum and local average values
         double dNewMinElev = (WEIGHT * dMinElevAround) + ((1 - WEIGHT) * dAvgElevAround);

         // Make sure that we don't go below 'base level'
         if (m_bHaveBaseLevel)
            dNewMinElev = tMax(dNewMinElev, m_dBaseLevel);
         
         // Make sure that we don't go below the bottom of the lowest soil layer
         dNewMinElev = tMax(dNewMinElev, dBasementElevation);
         
         // Is the new elevation value lower than the elevation of the low point?
         if (dNewMinElev > dMinElev)
         {
            // It is, so replace the elevation of the lowest-point cell with the new value
            Cell[0][nMinElevPos].SetSoilSurfaceElevation(dNewMinElev);
            
//             LogStream << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(6);
//             LogStream << m_ulIter << ": [" << 0 << "][" << nMinElevPos << "] from " << dMinElev << " to " << dNewMinElev;
//             LogStream << " (" << (dMinElev < dNewMinElev ? "GAIN" : "loss") << " = " << tAbs(dMinElev - dNewMinElev);
//             LogStream << " m_dPlotElevMin = " << m_dPlotElevMin << " m_dBaseLevel = " << m_dBaseLevel << " dMinElevAround = " << dMinElevAround << " dAvgElevAround = " << dAvgElevAround << ")" << endl;
         }
      }
   }
   
   // Is the planview right edge open?
   if (! m_bClosedThisEdge[EDGE_RIGHT])
   {
      // We have flow off the planview right edge, we will look along this edge for N_LOW_POINTS low points
      int nLowEdgePoint[N_LOW_POINTS];

      for (int n = 0; n < N_LOW_POINTS; n++)
      {
         // Find the elevation of the nth lowest point on this edge
         int nMinElevPos = 0;
         double dMinElev = DBL_MAX;

         for (int nY = 0; nY < m_nYGridMax; nY++)
         {
            double dElev = Cell[m_nXGridMax-1][nY].pGetSoil()->dGetSoilSurfaceElevation();
            if (dElev < dMinElev)
            {
               // We have a low point, but have we found it previously?
               bool bFoundPrev = false;
               for (int m = 0; m < n; m++)
               {
                  if (nY == nLowEdgePoint[m])
                  {
                     bFoundPrev = true;
                     break;
                  }
               }

               if (! bFoundPrev)
               {
                  // No, we have not found this one before
                  dMinElev = dElev;
                  nMinElevPos = nY;
               }
            }
         }

         // Are we at or below base level, or have we hit the bottom of the lowest soil layer? If so, do nothing more with this low point
         double dBasementElevation = Cell[m_nXGridMax-1][nMinElevPos].dGetBasementElevation();
         if ((m_bHaveBaseLevel && dMinElev > m_dBaseLevel) || (dMinElev >= dBasementElevation))
            continue;
         
         // We have found a new local minimum, so store it
         nLowEdgePoint[n] = nMinElevPos;
         
         // Now find the average and minimum elevations from the DIST_ACROSS x DIST_IN cells surrounding this cell
         int nRead = 0;
         double
            dAvgElevAround = 0,
            dMinElevAround = DBL_MAX;

         for (int j = m_nXGridMax-1; j >= (m_nXGridMax - DIST_IN); j--)
         {
            for (int i = (nMinElevPos - DIST_ACROSS); i <= (nMinElevPos + DIST_ACROSS); i++)
            {
               if ((i >= 0) && (i < m_nYGridMax))
               {
                  double dThisElev = Cell[j][i].pGetSoil()->dGetSoilSurfaceElevation();
                  dAvgElevAround += dThisElev;
                  nRead++;

                  if (dThisElev < dMinElevAround)
                     dMinElevAround = dThisElev;
               }
            }
         }

         // Calculate the local average
         if (nRead > 0)
            dAvgElevAround /= nRead;
         else
            dAvgElevAround = dMinElev;

         // Calculate a new value, is a weighted mix of the local minimum and local average values
         double dNewMinElev = (WEIGHT * dMinElevAround) + ((1 - WEIGHT) * dAvgElevAround);

         // Make sure that we don't go below 'base level'
         if (m_bHaveBaseLevel)
            dNewMinElev = tMax(dNewMinElev, m_dBaseLevel);
         
         // Make sure that we don't go below the bottom of the lowest soil layer
         dNewMinElev = tMax(dNewMinElev, dBasementElevation);
         
         // Is the new elevation value lower than the elevation of the low point?
         if (dNewMinElev > dMinElev)
         {
            // It is, so replace the elevation of the lowest-point cell with the new value
            Cell[m_nXGridMax-1][nMinElevPos].SetSoilSurfaceElevation(dNewMinElev);
            
//             LogStream << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(6);
//             LogStream << m_ulIter << ": [" << m_nXGridMax-1 << "][" << nMinElevPos << "] from " << dMinElev << " to " << dNewMinElev;
//             LogStream << " (" << (dMinElev < dNewMinElev ? "GAIN" : "loss") << " = " << tAbs(dMinElev - dNewMinElev);
//             LogStream << " m_dPlotElevMin = " << m_dPlotElevMin << " m_dBaseLevel = " << m_dBaseLevel << " dMinElevAround = " << dMinElevAround << " dAvgElevAround = " << dAvgElevAround << ")" << endl;
         }
      }
   }
}


/*========================================================================================================================================

 The bSaveGISFiles member function saves the GIS files using values from the Cell array

========================================================================================================================================*/
bool CSimulation::bSaveGISFiles(void)
{
   // Set for next save
   if (m_bSaveRegular)
      m_dRSaveTime += m_dRSaveInterval;
   else
      m_nThisSave = tMin(++m_nThisSave, m_nUSave);

   // These are always written
   if (! bWriteFileFloat(PLOT_CUMULATIVE_ALL_SIZE_FLOW_DETACHMENT, &PLOT_ALL_SIZE_FLOW_DETACHMENT_TITLE))   // increments filename count
      return (false);

   if (! bWriteFileFloat(PLOT_TOTAL_RAIN, &PLOT_TOTAL_RAIN_TITLE))
      return (false);

   if (! bWriteFileFloat(PLOT_TOTAL_SPLASH, &PLOT_TOTAL_SPLASH_TITLE))
      return (false);

   if (! bWriteFileFloat(PLOT_SURFACE_WATER_DEPTH, &PLOT_SURFACE_WATER_DEPTH_TITLE))
      return (false);

   if (! bWriteFileFloat(PLOT_SURFACE_WATER_FLOW_SPEED, &PLOT_SURFACE_WATER_FLOW_SPEED_TITLE))
      return (false);

   if (! bWriteFileFloat(PLOT_SURFACE_WATER_DW_FLOW_SPEED, &PLOT_SURFACE_WATER_DW_FLOW_SPEED_TITLE))
      return (false);

   // These are optional
   if (m_bElevSave)
   {
      if (! bWriteFileFloat(PLOT_ELEVATION, &PLOT_ELEVATION_TITLE))
         return (false);
   }

   if (m_bDetrendElevSave)
   {
      if (! bWriteFileFloat(PLOT_DETRENDED_ELEVATION, &PLOT_DETRENDED_ELEVATION_TITLE))
         return (false);
   }

   if (m_bRunOn)
   {
      if (! bWriteFileFloat(PLOT_TOTAL_RUNON, &PLOT_TOTAL_RUNON_TITLE))
         return (false);
   }

   if (m_bSplashSave)
   {
      if (! bWriteFileFloat(PLOT_SPLASH, &PLOT_SPLASH_TITLE))
         return (false);
   }

   if (m_bTotSplashSave)
   {
      if (! bWriteFileFloat(PLOT_TOTAL_SPLASH, &PLOT_TOTAL_SPLASH_TITLE))
         return (false);
   }

   if (m_bSlosSave)
   {
      if (! bWriteFileFloat(PLOT_ALL_SIZE_FLOW_DETACHMENT, &PLOT_ALL_SIZE_FLOW_DETACHMENT_TITLE))
         return (false);
   }

   if (m_bInundationSave)
   {
      if (! bWriteFileInt(PLOT_INUNDATION_REGIME, &PLOT_INUNDATION_REGIME_TITLE))
         return (false);
   }

   if (m_bFlowDirSave)
   {
      if (! bWriteFileInt(PLOT_SURFACE_WATER_FLOW_DIRECTION, &PLOT_SURFACE_WATER_FLOW_DIRECTION_TITLE))
         return (false);
   }

   if (m_bInfiltSave)
   {
      if (! bWriteFileFloat(PLOT_INFILTRATION, &PLOT_INFILTRATION_TITLE))
         return (false);
   }

   if (m_bTotInfiltSave)
   {
      if (! bWriteFileFloat(PLOT_CUMULATIVE_INFILTRATION_, &PLOT_CUMULATIVE_INFILTRATION__TITLE))
         return (false);
   }

   if (m_bSoilWaterSave)
   {
      if (! bWriteFileFloat(PLOT_SOIL_WATER, &PLOT_SOIL_WATER_TITLE))
         return (false);
   }

   if (m_bInfiltDepositSave)
   {
      if (! bWriteFileFloat(PLOT_INFILTRATION_DEPOSIT, &PLOT_INFILTRATION_DEPOSIT_TITLE))
         return (false);
   }

   if (m_bTotInfiltDepositSave)
   {
      if (! bWriteFileFloat(PLOT_CUMULATIVE_INFILTRATION_DEPOSIT, &PLOT_CUMULATIVE_INFILTRATION_DEPOSIT_TITLE))
         return (false);
   }

   if (m_bTopSurfaceSave)
   {
      if (! bWriteFileFloat(PLOT_TOP, &PLOT_TOP_TITLE))
         return (false);
   }

#if defined _DEBUG
   if (m_bLostSave)
   {
      if (! bWriteFileFloat(PLOT_LOST_FROM_EDGES, &PLOT_LOST_FROM_EDGES_TITLE))
         return (false);
   }
#endif

   if (m_bStreamPowerSave)
   {
      if (! bWriteFileFloat(PLOT_STREAMPOWER, &PLOT_STREAMPOWER_TITLE))
         return (false);
   }

   if (m_bShearStressSave)
   {
      if (! bWriteFileFloat(PLOT_SHEAR_STRESS, &PLOT_SHEAR_STRESS_TITLE))
         return (false);
   }

   if (m_bFrictionFactorSave)
   {
      if (! bWriteFileFloat(PLOT_FRICTION_FACTOR, &PLOT_FRICTION_FACTOR_TITLE))
         return (false);
   }

   if (m_bAvgShearStressSave)
   {
      if (! bWriteFileFloat(PLOT_AVG_SHEAR_STRESS, &PLOT_AVG_SHEAR_STRESS_TITLE))
         return (false);
   }

   if (m_bReynoldsSave)
   {
      if (! bWriteFileFloat(PLOT_REYNOLDS_NUMBER, &PLOT_REYNOLDS_NUMBER_TITLE))
         return (false);
   }

   if (m_bFroudeSave)
   {
      if (! bWriteFileFloat(PLOT_FROUDE_NUMBER, &PLOT_FROUDE_NUMBER_TITLE))
         return (false);
   }

   if (m_bTCSave)
   {
      if (! bWriteFileFloat(PLOT_TRANSPORT_CAPACITY, &PLOT_TRANSPORT_CAPACITY_TITLE))
         return (false);
   }

   if (m_bAvgDepthSave)
   {
      if (! bWriteFileFloat(PLOT_AVG_SURFACE_WATER_DEPTH, &PLOT_AVG_SURFACE_WATER_DEPTH_TITLE))
         return (false);
   }

   if (m_bAvgSpdSave)
   {
      if (! bWriteFileFloat(PLOT_SURFACE_WATER_AVG_FLOW_SPEED, &PLOT_SURFACE_WATER_AVG_FLOW_SPEED_TITLE))
         return (false);
   }

   if (m_bAvgDWSpdSave)
   {
      if (! bWriteFileFloat(PLOT_SURFACE_WATER_AVG_DW_FLOW_SPEED, &PLOT_SURFACE_WATER_AVG_DW_FLOW_SPEED_TITLE))
         return (false);
   }

   if (m_bSedConcSave)
   {
      if (! bWriteFileFloat(PLOT_SEDIMENT_CONCENTRATION, &PLOT_SEDIMENT_CONCENTRATION_TITLE))
         return (false);
   }

   if (m_bSedLoadSave)
   {
      if (! bWriteFileFloat(PLOT_SEDIMENT_LOAD, &PLOT_SEDIMENT_LOAD_TITLE))
         return (false);
   }

   if (m_bAvgSedLoadSave)
   {
      if (! bWriteFileFloat(PLOT_AVG_SEDIMENT_LOAD, &PLOT_AVG_SEDIMENT_LOAD_TITLE))
         return (false);
   }

   if (m_bTotDepositSave)
   {
      if (! bWriteFileFloat(PLOT_CUMULATIVE_ALL_SIZE_FLOW_DEPOSITION, &PLOT_CUMULATIVE_ALL_SIZE_FLOW_DEPOSITION_TITLE))
         return (false);
   }

   if (m_bNetSlumpSave)
   {
      if (! bWriteFileFloat(PLOT_NET_SLUMP, &PLOT_NET_SLUMP_TITLE))
         return (false);
   }

   if (m_bNetToppleSave)
   {
      if (! bWriteFileFloat(PLOT_NET_TOPPLE, &PLOT_NET_TOPPLE_TITLE))
         return (false);
   }

   if (m_bTotNetSlosSave)
   {
      if (! bWriteFileFloat(PLOT_TOT_SURFACE_LOWERING, &PLOT_TOT_SURFACE_LOWERING_TITLE))
         return (false);
   }

   return (true);
}


/*========================================================================================================================================

 This routine imposes a user-specified overall gradient upon the microtopographic grid. If no overall gradient has been specified, it estimates the grid's pre-existing gradient

========================================================================================================================================*/
void CSimulation::CalcGradient(void)
{
   m_dPlotElevMin = DBL_MAX;
   m_dPlotElevMax = DBL_MIN;

   if (m_dGradient > 0)
   {
      // A slope was specified by the user, so impose this slope on the DEM. Do it by decrementing each row by dDiff (and increase dDiff each row, by m_dYInc). Note that we must decrement (not increment) because the grid used here has the origin at the top left
      m_dYInc = m_dGradient * m_dCellSide / 100;         // mm
      double dDiff = 0;
      for (int nY = 0; nY < m_nYGridMax; nY++)
      {
         for (int nX = 0; nX < m_nXGridMax; nX++)
         {
            double dElev = Cell[nX][nY].pGetSoil()->dGetSoilSurfaceElevation();
            if (dElev == m_dMissingValue)
               continue;
            
            // Impose the gradient on the soil surface elevation
            double dNewElev = dElev - dDiff;
            Cell[nX][nY].SetInitialSoilSurfaceElevation(dNewElev);

            // Find the lowest and highest elevations
            if (dNewElev < m_dPlotElevMin)
               m_dPlotElevMin = dNewElev;

            if (dNewElev > m_dPlotElevMax)
               m_dPlotElevMax = dNewElev;
            
            // Now impose the same gradient on the basement elevation
            double dNewBasementElev = m_dBasementElevation - dDiff;
            Cell[nX][nY].SetBasementElevation(dNewBasementElev);
         }
         
         dDiff += m_dYInc;
      }
   }
   else
   {
      // We do not have a user-specified gradient, so estimate the DEM's own gradient. First get the average elevation of the planview top nDistIn rows
      int 
         nDistIn = 6,
         nTmp = (m_nYGridMax-1) / 8,
         nRead = 0;
         
      nDistIn = tMin(nDistIn, nTmp);         // Make sure doesn't cause problems on tiny DEMs
      nDistIn = tMax(nDistIn, 1);
      
      double dTot = 0;
      for (int nY = 0; nY <= nDistIn; nY++)
      {
         for (int nX = 0; nX < m_nXGridMax; nX++)
         {
            double dElev = Cell[nX][nY].pGetSoil()->dGetSoilSurfaceElevation();
            if (dElev == m_dMissingValue)
               continue;
            
            dTot += dElev;
            nRead++;

            // Find the lowest and highest elevations
            if (dElev < m_dPlotElevMin)
               m_dPlotElevMin = dElev;

            if (dElev > m_dPlotElevMax)
               m_dPlotElevMax = dElev;
         }
      }

      double dTopAverage = dTot / nRead;

      // Now get the average elevation of the planview bottom nDistIn rows
      nRead = 0;
      dTot = 0;
      for (int nY = m_nYGridMax-nDistIn-1; nY < m_nYGridMax; nY++)
      {
         for (int nX = 0; nX < m_nXGridMax; nX++)
         {
            double dElev = Cell[nX][nY].pGetSoil()->dGetSoilSurfaceElevation();
            dTot += dElev;
            nRead++;

            // Find the lowest elevation
            if (dElev < m_dPlotElevMin)
               m_dPlotElevMin = dElev;
         }
      }
      double dBottomAverage = dTot / nRead;

      // Finally estimate the overall gradient in per cent, as 100 * rise over run
      m_dGradient = 100 * tAbs(dTopAverage - dBottomAverage) / (m_nYGridMax * m_dCellSide);
      m_dYInc = m_dGradient * m_dCellSide / 100;         // mm
   }

   // If the user has specified a difference in plot-end elevation, use this to calculate the lowest point that we can cut down to on any edge (this is a kind of plot-end baselevel)
   if (m_bHaveBaseLevel)
      m_dBaseLevel = m_dPlotElevMin - m_dPlotEndDiff;
}


