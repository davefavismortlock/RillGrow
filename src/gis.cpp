/*=========================================================================================================================================

 This is gis.cpp: these CSimulation member functions use GDAL to read and write GIS files in several formats.

 Copyright (C) 2020 David Favis-Mortlock

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

//    // This does not work for ENGCRS (engineering CRS, a kind of Local CRS) TODO try with files having other CRS
//    const OGRSpatialReference* pSpatialRef = pGDALDataset->GetSpatialRef();
//    const_cast<OGRSpatialReference*>(pSpatialRef)->dumpReadable();
//
//    char** ppszName = NULL;
//    double dMult = pSpatialRef->GetTargetLinearUnits("LOCAL_CS", ppszName);
//    cout << ppszName << endl;

   // Do we have a projection reference?
   if (! m_strGDALDEMProjection.empty())
   {
      string strTmp = strToLower(&m_strGDALDEMProjection);

      // NOTE the code below is very crude, however I could not get the OGRSpatialReference-based methods in GDAL 3.0.2 (see code above) to work

      // Check whether the projection is plane
      if (strTmp.find("plane") == string::npos)
      {
         // Error: must be a plane
         cerr << ERR << "microtopography file projection (" << m_strGDALDEMProjection << ") in " << m_strDEMFile.c_str() << " must be 'plane'" << endl;

         return RTN_ERR_DEMFILE;
      }

//       // Check whether the X-Y units are in metres (note US spelling)
//       if (strTmp.find("meter") == string::npos)
//       {
//          // error: x-y values must be in metres
//          cerr << ERR << "microtopography  file x-y values (" << strTmp << ") in " << m_strDEMFile.c_str() << " must be in metres" << endl;
//
//          return RTN_ERR_DEMFILE;
//       }
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

            cerr << WARN << "Z units in DEM are '" << strDispDEMZUnits << "' but Z units in " << m_strDataPathName << " are '" << strDispUserDEMZUnits << "'" << endl;
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
   // Set the planview top row of the grid as edge cells
   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      Cell[nX][0].SetEdgeCell(DIRECTION_TOP);
      if (Cell[nX][0].bIsMissingValue())
         m_ulMissingValueCells++;
   }

   // Then search the rest of the grid, moving planview downwards, marking missing-value cells as edge cells. Keep going until we find a row with no missing-value cells
   for (int nY = 1; nY < m_nYGridMax; nY++)
   {
      bool bFound = false;
      for (int nX = 0; nX < m_nXGridMax; nX++)
      {
         if (Cell[nX][nY].bIsMissingValue())
         {
            m_ulMissingValueCells++;
            bFound = true;
            Cell[nX][nY].SetEdgeCell(DIRECTION_TOP);
         }
      }
      if (! bFound)
         break;
   }

   // Set the planview bottom row of the grid as edge cells
   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      Cell[nX][m_nYGridMax-1].SetEdgeCell(DIRECTION_BOTTOM);
      if (Cell[nX][m_nYGridMax-1].bIsMissingValue())
         m_ulMissingValueCells++;
   }

   // Then search the rest of the grid, moving planview upwards, marking missing-value cells as edge cells. Keep going until we find a row with no missing-value cells
   for (int nY = m_nYGridMax-2; nY >= 0; nY--)
   {
      bool bFound = false;
      for (int nX = 0; nX < m_nXGridMax; nX++)
      {
         if (Cell[nX][nY].bIsMissingValue())
         {
            m_ulMissingValueCells++;
            bFound = true;
            Cell[nX][nY].SetEdgeCell(DIRECTION_BOTTOM);
         }
      }
      if (! bFound)
         break;
   }

   // Set the planview left column  of the grid as edge cells
   for (int nY = 0; nY < m_nYGridMax; nY++)
   {
      Cell[0][nY].SetEdgeCell(DIRECTION_LEFT);
      if (Cell[0][nY].bIsMissingValue())
         m_ulMissingValueCells++;
   }

   // Then search the rest of the grid, moving planview rightwards, marking missing-value cells as edge cells. Keep going until we find a row with no missing-value cells
   for (int nX = 1; nX < m_nXGridMax; nX++)
   {
      bool bFound = false;
      for (int nY = 0; nY < m_nYGridMax; nY++)
      {
         if (Cell[nX][nY].bIsMissingValue())
         {
            m_ulMissingValueCells++;
            bFound = true;
            Cell[nX][nY].SetEdgeCell(DIRECTION_LEFT);
         }
      }
      if (! bFound)
         break;
   }

   // Set the planview right column  of the grid as edge cells
   for (int nY = 0; nY < m_nYGridMax; nY++)
   {
      Cell[m_nXGridMax-1][nY].SetEdgeCell(DIRECTION_RIGHT);
      if (Cell[m_nXGridMax-1][nY].bIsMissingValue())
         m_ulMissingValueCells++;
   }

   // Then search the rest of the grid, moving planview rightwards, marking missing-value cells as edge cells. Keep going until we find a row with no missing-value cells
   for (int nX = m_nXGridMax-2; nX >= 0; nX--)
   {
      bool bFound = false;
      for (int nY = 0; nY < m_nYGridMax; nY++)
      {
         if (Cell[nX][nY].bIsMissingValue())
         {
            m_ulMissingValueCells++;
            bFound = true;
            Cell[nX][nY].SetEdgeCell(DIRECTION_RIGHT);
         }
      }
      if (! bFound)
         break;
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

   // Do we have a projection reference?
   if (! m_strGDALDEMProjection.empty())
   {
      string strTmp = strToLower(&m_strGDALDEMProjection);

      // NOTE the code below is very crude, however I could not get the OGRSpatialReference-based methods in GDAL 3.0.2 to work

      // Check whether the projection is plane
      if (strTmp.find("plane") == string::npos)
      {
         // Error: must be a plane
         cerr << ERR << "rainfall variation file projection (" << m_strGDALDEMProjection << ") in " << m_strDEMFile.c_str() << " must be 'plane'" << endl;

         return RTN_ERR_RAIN_VARIATION_FILE;
      }

      //       // Check whether the X-Y units are in metres (note US spelling)
      //       if (strTmp.find("meter") == string::npos)
      //       {
      //          // error: x-y values must be in metres
      //          cerr << ERR << "rainfall variation file x-y values (" << strTmp << ") in " << m_strDEMFile.c_str() << " must be in metres" << endl;
      //
      //          return RTN_ERR_RAIN_VARIATION_FILE;
      //       }
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
   pDriver = GetGDALDriverManager()->GetDriverByName(m_strGISOutFormat.c_str());
   if (NULL == pDriver)
   {
      // Can't load GDAL driver. Incorrectly specified?
      cerr << ERR << "Unknown GIS output format '" << m_strGISOutFormat << "'." << endl;
      return (false);
   }

   // Get the metadata for this driver
   char** papszMetadata = pDriver->GetMetadata();

   if (! CSLFetchBoolean(papszMetadata, GDAL_DCAP_CREATE, FALSE))
   {
      // Driver does not support create() method
      cerr << ERR << "Cannot write GIS files using GDAL driver '" << m_strGISOutFormat << "'. Choose another format." << endl;
      return (false);
   }

   if (! strstr(CSLFetchNameValue (papszMetadata, "DMD_CREATIONDATATYPES"), "Float32"))
   {
      // Driver does not support 32-bit doubles
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
bool CSimulation::bWriteGISFileFloat(int const nDataItem, string const* pstrPlotTitle)
{
   // Increment file number when soil loss file is written (this is done first, and is always saved)
   if (GIS_CUMUL_ALL_SIZE_FLOW_DETACH == nDataItem)
      m_nGISSave++;

   // Now begin constructing the file name for this save
   string strFilDat = m_strOutputPath;

   switch (nDataItem)
   {
      case (GIS_CUMUL_RAIN) :
         strFilDat.append(GIS_CUMUL_RAIN_FILENAME);
         break;

      case (GIS_RAIN_SPATIAL_VARIATION) :
         strFilDat.append(GIS_RAIN_SPATIAL_VARIATION_FILENAME);
         break;

      case (GIS_CUMUL_RUNON) :
         strFilDat.append(GIS_CUMUL_RUNON_FILENAME);
         break;

      case (GIS_ELEVATION) :
         strFilDat.append(GIS_ELEVATION_FILENAME);
         break;

      case (GIS_DETREND_ELEVATION) :
         strFilDat.append(GIS_DETREND_ELEVATION_FILENAME);
         break;

      case (GIS_SURFACE_WATER_DEPTH) :
         strFilDat.append(GIS_SURFACE_WATER_DEPTH_FILENAME);
         break;

      case (GIS_ALL_SIZE_FLOW_DETACH) :
         strFilDat.append(GIS_ALL_SIZE_FLOW_DETACH_FILENAME);
         break;

      case (GIS_INFILT) :
         strFilDat.append(GIS_INFILT_FILENAME);
         break;

      case (GIS_CUMUL_INFILT) :
         strFilDat.append(GIS_CUMUL_INFILT_FILENAME);
         break;

      case (GIS_SOIL_WATER) :
         strFilDat.append(GIS_SOIL_WATER_FILENAME);
         break;

      case (GIS_INFILT_DEPOSIT) :
         strFilDat.append(GIS_INFILT_DEPOSIT_FILENAME);
         break;

      case (GIS_CUMUL_INFILT_DEPOSIT) :
         strFilDat.append(GIS_CUMUL_INFILT_DEPOSIT_FILENAME);
         break;

      case (GIS_TOP_SURFACE) :
         strFilDat.append(GIS_TOP_SURFACE_FILENAME);
         break;

      case (GIS_SPLASH) :
         strFilDat.append(GIS_SPLASH_FILENAME);
         break;

      case (GIS_CUMUL_SPLASH) :
         strFilDat.append(GIS_CUMUL_SPLASH_FILENAME);
         break;

      case (GIS_SURFACE_WATER_SPEED) :
         strFilDat.append(GIS_SURFACE_WATER_SPEED_FILENAME);
         break;

      case (GIS_SURFACE_WATER_DW_SPEED) :
         strFilDat.append(GIS_SURFACE_WATER_DW_SPEED_FILENAME);
         break;

      case (GIS_AVG_SURFACE_WATER_FROM_EDGES) :
         strFilDat.append(GIS_AVG_SURFACE_WATER_FROM_EDGES_FILENAME);
         break;

      case (GIS_STREAMPOWER) :
         strFilDat.append(GIS_STREAMPOWER_FILENAME);
         break;

      case (GIS_SHEAR_STRESS) :
         strFilDat.append(GIS_SHEAR_STRESS_FILENAME);
         break;

      case (GIS_FRICTION_FACTOR) :
         strFilDat.append(GIS_FRICTION_FACTOR_FILENAME);
         break;

      case (GIS_AVG_SHEAR_STRESS) :
         strFilDat.append(GIS_AVG_SHEAR_STRESS_FILENAME);
         break;

      case (GIS_REYNOLDS_NUMBER) :
         strFilDat.append(GIS_REYNOLDS_NUMBER_FILENAME);
         break;

      case (GIS_FROUDE_NUMBER) :
         strFilDat.append(GIS_FROUDE_NUMBER_FILENAME);
         break;

      case (GIS_TRANSPORT_CAPACITY) :
         strFilDat.append(GIS_TRANSPORT_CAPACITY_FILENAME);
         break;

      case (GIS_AVG_SURFACE_WATER_DEPTH) :
         strFilDat.append(GIS_AVG_SURFACE_WATER_DEPTH_FILENAME);
         break;

      case (GIS_AVG_SURFACE_WATER_SPEED) :
         strFilDat.append(GIS_AVG_SURFACE_WATER_SPEED_FILENAME);
         break;

      case (GIS_AVG_SURFACE_WATER_DW_SPEED) :
         strFilDat.append(GIS_AVG_SURFACE_WATER_DW_SPEED_FILENAME);
         break;

      case (GIS_CUMUL_ALL_SIZE_FLOW_DETACH) :
         strFilDat.append(GIS_CUMUL_ALL_SIZE_FLOW_DETACH_FILENAME);
         break;

      case (GIS_CUMUL_ALL_SIZE_FLOW_DEPOSIT) :
         strFilDat.append(GIS_CUMUL_ALL_SIZE_FLOW_DEPOSIT_FILENAME);
         break;

      case (GIS_SEDIMENT_CONCENTRATION) :
         strFilDat.append(GIS_SEDIMENT_CONCENTRATION_FILENAME);
         break;

      case (GIS_SEDIMENT_LOAD) :
         strFilDat.append(GIS_SEDIMENT_LOAD_FILENAME);
         break;

      case (GIS_AVG_SEDIMENT_LOAD) :
         strFilDat.append(GIS_AVG_SEDIMENT_LOAD_FILENAME);
         break;

      case (GIS_CUMUL_SLUMP_DETACH) :
         strFilDat.append(GIS_CUMUL_SLUMP_DETACH_FILENAME);
         break;

      case (GIS_CUMUL_SLUMP_DEPOSIT) :
         strFilDat.append(GIS_CUMUL_SLUMP_DEPOSIT_FILENAME);
         break;

      case (GIS_CUMUL_TOPPLE_DETACH) :
         strFilDat.append(GIS_CUMUL_TOPPLE_DETACH_FILENAME);
         break;

      case (GIS_CUMUL_TOPPLE_DEPOSIT) :
         strFilDat.append(GIS_CUMUL_TOPPLE_DEPOSIT_FILENAME);
         break;

      case (GIS_CUMUL_ALL_PROC_SURF_LOWER) :
         strFilDat.append(GIS_CUMUL_ALL_PROC_SURF_LOWER_FILENAME);
   }

   // Append the 'save number' to the filename, and prepend zeros to the save number
   strFilDat.append("_");
   stringstream ststrTmp;
   ststrTmp << FillToWidth('0', MAX_GIS_FILENAME_SAVE_DIGITS) << m_nGISSave;
   strFilDat.append(ststrTmp.str());

   // Maybe append the extension
   if (! m_strGDALOutputDriverExtension.empty())
   {
      strFilDat.append(".");
      strFilDat.append(m_strGDALOutputDriverExtension);
   }

   GDALDriver* pDriver;
   pDriver = GetGDALDriverManager()->GetDriverByName(m_strGISOutFormat.c_str());
   GDALDataset* pOutDataSet;
   char** papszOptions = NULL;                                          // For driver-specific options

   pOutDataSet = pDriver->Create(strFilDat.c_str(), m_nXGridMax, m_nYGridMax, 1, GDT_Float32, papszOptions);
   if (NULL == pOutDataSet)
   {
      // Couldn't create file
      cerr << ERR << "cannot create " << m_strGISOutFormat << " file named " << strFilDat << "\n" << CPLGetLastErrorMsg() << endl;
      return false;
   }

   // Set projection info for output dataset (will be same as was read in from DEM)
   CPLPushErrorHandler(CPLQuietErrorHandler);                           // Needed to get next line to fail silently, if it fails
   pOutDataSet->SetProjection(m_strGDALDEMProjection.c_str());          // Will fail for some formats
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
      return false;
   }

   int n = 0;
   double
      dTmp = 0,
      dTmp1,
      dDiff = 0;

   // Fill the array
   double dMaxFF = 0;
   if (nDataItem == GIS_FRICTION_FACTOR)
      cerr << endl;
   for (int nY = 0; nY < m_nYGridMax; nY++)
   {
      for (int nX = 0; nX < m_nXGridMax; nX++)
      {
         switch (nDataItem)
         {
            case (GIS_CUMUL_RAIN) :
               dTmp = Cell[nX][nY].pGetRainAndRunon()->dGetCumulRain();
               break;

            case (GIS_RAIN_SPATIAL_VARIATION) :
               dTmp = Cell[nX][nY].pGetRainAndRunon()->dGetRainVarM();
               break;

            case (GIS_CUMUL_RUNON) :
               dTmp = Cell[nX][nY].pGetRainAndRunon()->dGetCumulRunOn();
               break;

            case (GIS_ELEVATION) :
               dTmp = Cell[nX][nY].pGetSoil()->dGetSoilSurfaceElevation();
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

            case (GIS_DETREND_ELEVATION) :
               dTmp = Cell[nX][nY].pGetSoil()->dGetSoilSurfaceElevation();
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

            case (GIS_SURFACE_WATER_DEPTH) :
               dTmp = Cell[nX][nY].pGetSurfaceWater()->dGetSurfaceWaterDepth();
               break;

            case (GIS_ALL_SIZE_FLOW_DETACH) :
               dTmp = Cell[nX][nY].pGetSoil()->dGetTotFlowDetach();
               break;

            case (GIS_INFILT) :
               dTmp = Cell[nX][nY].pGetSoilWater()->dGetInfiltration();
               break;

            case (GIS_CUMUL_INFILT) :
               dTmp = Cell[nX][nY].pGetSoilWater()->dGetCumulInfiltration();
               break;

            case (GIS_SOIL_WATER) :
               dTmp = Cell[nX][nY].pGetSoilWater()->dGetTopLayerSoilWater();
               break;

            case (GIS_INFILT_DEPOSIT) :
               dTmp = Cell[nX][nY].pGetSoil()->dGetTotInfiltDeposit();
               break;

            case (GIS_CUMUL_INFILT_DEPOSIT) :
               dTmp = Cell[nX][nY].pGetSoil()->dGetCumulTotInfiltDeposit();
               break;

            case (GIS_TOP_SURFACE):
               dTmp = Cell[nX][nY].dGetTopElevation();
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

            case (GIS_SPLASH) :
               dTmp = Cell[nX][nY].pGetSoil()->dGetAllSizeSplashDetach() - Cell[nX][nY].pGetSoil()->dGetAllSizeSplashDeposit();
               break;

            case (GIS_CUMUL_SPLASH) :
               dTmp = Cell[nX][nY].pGetSoil()->dGetCumulAllSizeSplashDetach() - Cell[nX][nY].pGetSoil()->dGetCumulAllSizeSplashDeposit();
               break;

            case (GIS_SURFACE_WATER_SPEED) :
               dTmp = Cell[nX][nY].pGetSurfaceWater()->dGetFlowSpd();
               break;

            case (GIS_SURFACE_WATER_DW_SPEED) :
               dTmp = Cell[nX][nY].pGetSurfaceWater()->dGetDWFlowSpd();
               break;

            case (GIS_AVG_SURFACE_WATER_FROM_EDGES) :
               dTmp = Cell[nX][nY].pGetSurfaceWater()->dGetCumulSurfaceWaterLost() / m_dSimulatedTimeElapsed;
               break;

            case (GIS_STREAMPOWER) :
               dTmp = Cell[nX][nY].pGetSurfaceWater()->dGetStreamPower();
               break;

            case GIS_SHEAR_STRESS:
               dTmp1 = m_dSimulatedTimeElapsed - m_dLastSlumpCalcTime;
               if (dTmp1 > 0)
                  dTmp = Cell[nX][nY].pGetSoil()->dGetShearStress() / (m_dSimulatedTimeElapsed - m_dLastSlumpCalcTime);
               else
                  dTmp = 0;
               break;

            case (GIS_FRICTION_FACTOR) :
               if (Cell[nX][nY].pGetSurfaceWater()->dGetFrictionFactor() > dMaxFF)
                  dMaxFF = Cell[nX][nY].pGetSurfaceWater()->dGetFrictionFactor();

               dTmp = Cell[nX][nY].pGetSurfaceWater()->dGetFrictionFactor();

               cerr << nX << " " << nY << " " << Cell[nX][nY].pGetSurfaceWater()->dGetFrictionFactor() << endl;
               break;

            case (GIS_AVG_SHEAR_STRESS) :
               dTmp = Cell[nX][nY].pGetSoil()->dGetCumulShearStress() / m_dSimulatedTimeElapsed;
               break;

            case (GIS_REYNOLDS_NUMBER) :
               dTmp = dGetReynolds(nX, nY);
               break;

            case (GIS_FROUDE_NUMBER) :
               dTmp = Cell[nX][nY].pGetSurfaceWater()->dGetFroude(m_dG);
               break;

            case (GIS_TRANSPORT_CAPACITY) :
               dTmp = Cell[nX][nY].pGetSurfaceWater()->dGetTransportCapacity();
               break;

            case (GIS_AVG_SURFACE_WATER_DEPTH) :
               dTmp = Cell[nX][nY].pGetSurfaceWater()->dGetCumulSurfaceWater() / m_dSimulatedTimeElapsed;
               break;

            case (GIS_AVG_SURFACE_WATER_SPEED) :
               dTmp = Cell[nX][nY].pGetSurfaceWater()->dCumulFlowSpeed() / m_dSimulatedTimeElapsed;
               break;

            case (GIS_AVG_SURFACE_WATER_DW_SPEED) :
               dTmp = Cell[nX][nY].pGetSurfaceWater()->dGetCumulDWFlowSpd() / m_dSimulatedTimeElapsed;
               break;

            case (GIS_CUMUL_ALL_SIZE_FLOW_DETACH) :
               dTmp = Cell[nX][nY].pGetSoil()->dGetCumulAllSizeFlowDetach();
               break;

            case (GIS_CUMUL_ALL_SIZE_FLOW_DEPOSIT) :
               dTmp = Cell[nX][nY].pGetSoil()->dGetCumulAllSizeFlowDeposit();
               break;

            case (GIS_SEDIMENT_CONCENTRATION) :
               dTmp = Cell[nX][nY].pGetSedLoad()->dGetAllSizeSedConc();
               break;

            case (GIS_SEDIMENT_LOAD) :
               dTmp = Cell[nX][nY].pGetSedLoad()->dGetAllSizeSedLoad();
               break;

            case (GIS_AVG_SEDIMENT_LOAD) :
               dTmp = Cell[nX][nY].pGetSedLoad()->dGetCumulAllSizeSedLoad() / m_dSimulatedTimeElapsed;
               break;

            case (GIS_CUMUL_SLUMP_DETACH) :
               dTmp = Cell[nX][nY].pGetSoil()->dGetCumulAllSizeSlumpDetach();
               break;

            case (GIS_CUMUL_SLUMP_DEPOSIT) :
               dTmp = Cell[nX][nY].pGetSoil()->dGetCumulAllSizeSlumpDeposit();
               break;

            case (GIS_CUMUL_TOPPLE_DETACH) :
               dTmp = Cell[nX][nY].pGetSoil()->dGetCumulAllSizeToppleDetach();
               break;

            case (GIS_CUMUL_TOPPLE_DEPOSIT) :
               dTmp = Cell[nX][nY].pGetSoil()->dGetCumulAllSizeToppleDeposit();
               break;

            case (GIS_CUMUL_ALL_PROC_SURF_LOWER):
               // Detachment is +ve, deposition is -ve
               dTmp = Cell[nX][nY].pGetSoil()->dGetCumulAllSizeLowering();
         }

         // Write this value to the array
         pfRaster[n++] = dTmp;
      }

      dDiff += m_dYInc;    // GIS_DETREND_ELEVATION and GIS_TOP_SURFACE only
   }

   if (nDataItem == GIS_FRICTION_FACTOR)
      cerr << "dMaxFF = " << dMaxFF << endl;

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
      case (GIS_ELEVATION) :
      case (GIS_DETREND_ELEVATION) :
      case (GIS_CUMUL_RAIN) :
      case (GIS_CUMUL_RUNON) :
      case (GIS_SURFACE_WATER_DEPTH) :
      case (GIS_AVG_SURFACE_WATER_DEPTH) :
      case (GIS_INFILT) :
      case (GIS_CUMUL_INFILT) :
      case (GIS_SOIL_WATER) :
      case (GIS_INFILT_DEPOSIT) :
      case (GIS_CUMUL_INFILT_DEPOSIT) :
      case (GIS_TOP_SURFACE) :
      case (GIS_SEDIMENT_LOAD) :
      case (GIS_AVG_SEDIMENT_LOAD) :
      case (GIS_SPLASH) :
      case (GIS_CUMUL_SPLASH) :
      case (GIS_ALL_SIZE_FLOW_DETACH) :
      case (GIS_TRANSPORT_CAPACITY) :
      case (GIS_CUMUL_SLUMP_DETACH) :
      case (GIS_CUMUL_SLUMP_DEPOSIT) :
      case (GIS_CUMUL_TOPPLE_DETACH) :
      case (GIS_CUMUL_TOPPLE_DEPOSIT) :
      case (GIS_CUMUL_ALL_SIZE_FLOW_DETACH) :
      case (GIS_CUMUL_ALL_SIZE_FLOW_DEPOSIT) :
      case (GIS_CUMUL_ALL_PROC_SURF_LOWER) :
      case (GIS_AVG_SURFACE_WATER_FROM_EDGES) :
         strUnits = "mm";
         break;

      case (GIS_SURFACE_WATER_SPEED) :
      case (GIS_AVG_SURFACE_WATER_SPEED) :
      case (GIS_SURFACE_WATER_DW_SPEED) :
      case (GIS_AVG_SURFACE_WATER_DW_SPEED) :
         strUnits = "mm/sec";
         break;

      case (GIS_STREAMPOWER) :
         strUnits = "kg/s**3";
         break;

      case (GIS_SHEAR_STRESS) :
      case (GIS_AVG_SHEAR_STRESS) :
         strUnits = "kg/m s**2";
         break;

      case (GIS_SEDIMENT_CONCENTRATION) :
         strUnits = "%";
         break;

      case (GIS_RAIN_SPATIAL_VARIATION) :
      case (GIS_REYNOLDS_NUMBER) :
      case (GIS_FROUDE_NUMBER) :
      case (GIS_FRICTION_FACTOR) :
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
bool CSimulation::bWriteGISFileInt(int const nDataItem, string const* pstrPlotTitle)
{
   // Begin constructing the file name for this save
   string strFilDat = m_strOutputPath;

   switch (nDataItem)
   {
      case (GIS_INUNDATION_REGIME) :
         strFilDat.append(GIS_INUNDATION_REGIME_FILENAME);
         break;

      case (GIS_SURFACE_WATER_DIRECTION) :
         strFilDat.append(GIS_SURFACE_WATER_DIRECTION_FILENAME);
         break;

      case (GIS_CUMUL_BINARY_HEADCUT_RETREAT) :
         strFilDat.append(GIS_CUMUL_BINARY_HEADCUT_RETREAT_FILENAME);
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
      return false;
   }

   // Fill the array
   int nTmp  = 0;
   int n = 0;
   for (int nY = 0; nY < m_nYGridMax; nY++)
   {
      for (int nX = 0; nX < m_nXGridMax; nX++)
      {
         switch (nDataItem)
         {
            case (GIS_INUNDATION_REGIME) :
               nTmp = Cell[nX][nY].pGetSurfaceWater()->nGetInundation();
               break;

            case (GIS_SURFACE_WATER_DIRECTION) :
               nTmp = Cell[nX][nY].pGetSurfaceWater()->nGetFlowDirection();
               break;

            case (GIS_CUMUL_BINARY_HEADCUT_RETREAT):
               nTmp = (Cell[nX][nY].bHasHadHeadcutRetreat() ? 1 : 0);
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
      case (GIS_INUNDATION_REGIME):
      case (GIS_SURFACE_WATER_DIRECTION):
      case (GIS_CUMUL_BINARY_HEADCUT_RETREAT) :
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
      case (GIS_INUNDATION_REGIME) :
         papszCategoryNames = CSLAddString(papszCategoryNames, "Dry");
         papszCategoryNames = CSLAddString(papszCategoryNames, "Shallow");
         papszCategoryNames = CSLAddString(papszCategoryNames, "Marginally inundated");
         papszCategoryNames = CSLAddString(papszCategoryNames, "Well inundated");
         break;

      case (GIS_SURFACE_WATER_DIRECTION) :
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

//             m_ofsLog << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(6);
//             m_ofsLog << m_ulIter << ": [" << nMinElevPos << "][" << m_nYGridMax-1 << "] from " << dMinElev << " to " << dNewMinElev;
//             m_ofsLog << " (" << (dMinElev < dNewMinElev ? "GAIN" : "loss") << " = " << tAbs(dMinElev - dNewMinElev);
//             m_ofsLog << " m_dPlotElevMin = " << m_dPlotElevMin << " m_dBaseLevel = " << m_dBaseLevel << " dMinElevAround = " << dMinElevAround << " dAvgElevAround = " << dAvgElevAround << ")" << endl;
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

//             m_ofsLog << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(6);
//             m_ofsLog << m_ulIter << ": [" << nMinElevPos << "][" << 0 << "] from " << dMinElev << " to " << dNewMinElev;
//             m_ofsLog << " (" << (dMinElev < dNewMinElev ? "GAIN" : "loss") << " = " << tAbs(dMinElev - dNewMinElev);
//             m_ofsLog << " m_dPlotElevMin = " << m_dPlotElevMin << " m_dBaseLevel = " << m_dBaseLevel << " dMinElevAround = " << dMinElevAround << " dAvgElevAround = " << dAvgElevAround << ")" << endl;
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

//             m_ofsLog << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(6);
//             m_ofsLog << m_ulIter << ": [" << 0 << "][" << nMinElevPos << "] from " << dMinElev << " to " << dNewMinElev;
//             m_ofsLog << " (" << (dMinElev < dNewMinElev ? "GAIN" : "loss") << " = " << tAbs(dMinElev - dNewMinElev);
//             m_ofsLog << " m_dPlotElevMin = " << m_dPlotElevMin << " m_dBaseLevel = " << m_dBaseLevel << " dMinElevAround = " << dMinElevAround << " dAvgElevAround = " << dAvgElevAround << ")" << endl;
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

//             m_ofsLog << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(6);
//             m_ofsLog << m_ulIter << ": [" << m_nXGridMax-1 << "][" << nMinElevPos << "] from " << dMinElev << " to " << dNewMinElev;
//             m_ofsLog << " (" << (dMinElev < dNewMinElev ? "GAIN" : "loss") << " = " << tAbs(dMinElev - dNewMinElev);
//             m_ofsLog << " m_dPlotElevMin = " << m_dPlotElevMin << " m_dBaseLevel = " << m_dBaseLevel << " dMinElevAround = " << dMinElevAround << " dAvgElevAround = " << dAvgElevAround << ")" << endl;
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

   // These are always written. Note must write GIS_CUMUL_ALL_SIZE_FLOW_DETACH first, to increment save number
   if (! bWriteGISFileFloat(GIS_CUMUL_ALL_SIZE_FLOW_DETACH, &GIS_CUMUL_ALL_SIZE_FLOW_DETACH_TITLE))      // Increments filename count
      return (false);

   if (! bWriteGISFileFloat(GIS_CUMUL_RAIN, &GIS_CUMUL_RAIN_TITLE))
      return (false);

   if (! bWriteGISFileFloat(GIS_CUMUL_SPLASH, &GIS_CUMUL_SPLASH_TITLE))
      return (false);

   if (! bWriteGISFileFloat(GIS_SURFACE_WATER_DEPTH, &GIS_SURFACE_WATER_DEPTH_TITLE))
      return (false);

   if (! bWriteGISFileFloat(GIS_SURFACE_WATER_SPEED, &GIS_SURFACE_WATER_SPEED_TITLE))
      return (false);

   if (! bWriteGISFileFloat(GIS_SURFACE_WATER_DW_SPEED, &GIS_SURFACE_WATER_DW_SPEED_TITLE))
      return (false);

   // These are optional
   if (m_bElevSave)
   {
      if (! bWriteGISFileFloat(GIS_ELEVATION, &GIS_ELEVATION_TITLE))
         return (false);
   }

   if (m_bDetrendElevSave)
   {
      if (! bWriteGISFileFloat(GIS_DETREND_ELEVATION, &GIS_DETREND_ELEVATION_TITLE))
         return (false);
   }

   if (m_bRunOn)
   {
      if (! bWriteGISFileFloat(GIS_CUMUL_RUNON, &GIS_CUMUL_RUNON_TITLE))
         return (false);
   }

   if (m_bSplashSave)
   {
      if (! bWriteGISFileFloat(GIS_SPLASH, &GIS_SPLASH_TITLE))
         return (false);
   }

   if (m_bCumulSplashSave)
   {
      if (! bWriteGISFileFloat(GIS_CUMUL_SPLASH, &GIS_CUMUL_SPLASH_TITLE))
         return (false);
   }

   if (m_bFlowDetachSave)
   {
      if (! bWriteGISFileFloat(GIS_ALL_SIZE_FLOW_DETACH, &GIS_CUMUL_ALL_SIZE_FLOW_DETACH_TITLE))
         return (false);
   }

   if (m_bInundationSave)
   {
      if (! bWriteGISFileInt(GIS_INUNDATION_REGIME, &GIS_INUNDATION_REGIME_TITLE))
         return (false);
   }

   if (m_bFlowDirSave)
   {
      if (! bWriteGISFileInt(GIS_SURFACE_WATER_DIRECTION, &GIS_SURFACE_WATER_DIRECTION_TITLE))
         return (false);
   }

   if (m_bInfiltSave)
   {
      if (! bWriteGISFileFloat(GIS_INFILT, &GIS_INFILT_TITLE))
         return (false);
   }

   if (m_bCumulInfiltSave)
   {
      if (! bWriteGISFileFloat(GIS_CUMUL_INFILT, &GIS_CUMUL_INFILT_TITLE))
         return (false);
   }

   if (m_bSoilWaterSave)
   {
      if (! bWriteGISFileFloat(GIS_SOIL_WATER, &GIS_SOIL_WATER_TITLE))
         return (false);
   }

   if (m_bInfiltDepositSave)
   {
      if (! bWriteGISFileFloat(GIS_INFILT_DEPOSIT, &GIS_INFILT_DEPOSIT_TITLE))
         return (false);
   }

   if (m_bCumulInfiltDepositSave)
   {
      if (! bWriteGISFileFloat(GIS_CUMUL_INFILT_DEPOSIT, &GIS_CUMUL_INFILT_DEPOSIT_TITLE))
         return (false);
   }

   if (m_bTopSurfaceSave)
   {
      if (! bWriteGISFileFloat(GIS_TOP_SURFACE, &GIS_TOP_SURFACE_TITLE))
         return (false);
   }

   if (m_bLostSave)
   {
      if (! bWriteGISFileFloat(GIS_AVG_SURFACE_WATER_FROM_EDGES, &GIS_AVG_SURFACE_WATER_FROM_EDGES_TITLE))
         return (false);
   }

   if (m_bStreamPowerSave)
   {
      if (! bWriteGISFileFloat(GIS_STREAMPOWER, &GIS_STREAMPOWER_TITLE))
         return (false);
   }

   if (m_bShearStressSave)
   {
      if (! bWriteGISFileFloat(GIS_SHEAR_STRESS, &GIS_SHEAR_STRESS_TITLE))
         return (false);
   }

   if (m_bFrictionFactorSave)
   {
      if (! bWriteGISFileFloat(GIS_FRICTION_FACTOR, &GIS_FRICTION_FACTOR_TITLE))
         return (false);
   }

   if (m_bCumulAvgShearStressSave)
   {
      if (! bWriteGISFileFloat(GIS_AVG_SHEAR_STRESS, &GIS_AVG_SHEAR_STRESS_TITLE))
         return (false);
   }

   if (m_bReynoldsSave)
   {
      if (! bWriteGISFileFloat(GIS_REYNOLDS_NUMBER, &GIS_REYNOLDS_NUMBER_TITLE))
         return (false);
   }

   if (m_bFroudeSave)
   {
      if (! bWriteGISFileFloat(GIS_FROUDE_NUMBER, &GIS_FROUDE_NUMBER_TITLE))
         return (false);
   }

   if (m_bTCSave)
   {
      if (! bWriteGISFileFloat(GIS_TRANSPORT_CAPACITY, &GIS_TRANSPORT_CAPACITY_TITLE))
         return (false);
   }

   if (m_bCumulAvgDepthSave)
   {
      if (! bWriteGISFileFloat(GIS_AVG_SURFACE_WATER_DEPTH, &GIS_AVG_SURFACE_WATER_DEPTH_TITLE))
         return (false);
   }

   if (m_bCumulAvgSpdSave)
   {
      if (! bWriteGISFileFloat(GIS_AVG_SURFACE_WATER_SPEED, &GIS_AVG_SURFACE_WATER_SPEED_TITLE))
         return (false);
   }

   if (m_bCumulAvgDWSpdSave)
   {
      if (! bWriteGISFileFloat(GIS_AVG_SURFACE_WATER_DW_SPEED, &GIS_AVG_SURFACE_WATER_DW_SPEED_TITLE))
         return (false);
   }

   if (m_bSedConcSave)
   {
      if (! bWriteGISFileFloat(GIS_SEDIMENT_CONCENTRATION, &GIS_SEDIMENT_CONCENTRATION_TITLE))
         return (false);
   }

   if (m_bSedLoadSave)
   {
      if (! bWriteGISFileFloat(GIS_SEDIMENT_LOAD, &GIS_SEDIMENT_LOAD_TITLE))
         return (false);
   }

   if (m_bAvgSedLoadSave)
   {
      if (! bWriteGISFileFloat(GIS_AVG_SEDIMENT_LOAD, &GIS_AVG_SEDIMENT_LOAD_TITLE))
         return (false);
   }

   if (m_bCumulFlowDepositSave)
   {
      if (! bWriteGISFileFloat(GIS_CUMUL_ALL_SIZE_FLOW_DEPOSIT, &GIS_CUMUL_ALL_SIZE_FLOW_DEPOSIT_TITLE))
         return (false);
   }

   if (m_bSlumpSave)
   {
      if (! bWriteGISFileFloat(GIS_CUMUL_SLUMP_DETACH, &GIS_CUMUL_SLUMP_DETACH_TITLE))
         return (false);

      if (! bWriteGISFileFloat(GIS_CUMUL_SLUMP_DEPOSIT, &GIS_CUMUL_SLUMP_DEPOSIT_TITLE))
         return (false);
   }

   if (m_bToppleSave)
   {
      if (! bWriteGISFileFloat(GIS_CUMUL_TOPPLE_DETACH, &GIS_CUMUL_TOPPLE_DETACH_TITLE))
         return (false);

      if (! bWriteGISFileFloat(GIS_CUMUL_TOPPLE_DEPOSIT, &GIS_CUMUL_TOPPLE_DEPOSIT_TITLE))
         return (false);
   }

   if (m_bCumulLoweringSave)
   {
      if (! bWriteGISFileFloat(GIS_CUMUL_ALL_PROC_SURF_LOWER, &GIS_CUMUL_ALL_PROC_SURF_LOWER_TITLE))
         return (false);
   }

   if (m_bHeadcutRetreat)
   {
      if (! bWriteGISFileInt(GIS_CUMUL_BINARY_HEADCUT_RETREAT, &GIS_CUMUL_BINARY_HEADCUT_RETREAT_TITLE))
         return (false);
   }

   return (true);
}


/*========================================================================================================================================

 This routine imposes a user-specified overall gradient upon the microtopographic grid. If no overall gradient has been specified, it crudely estimates the grid's pre-existing gradient

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
      // We do not have a user-specified gradient, so crudely estimate the DEM's own gradient. First get the average elevation of the planview top nDistIn rows
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


