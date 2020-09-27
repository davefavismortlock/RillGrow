/*=========================================================================================================================================

 This is simulate_splash.cpp: it handles splash redistribution

 Copyright (C) 2020 David Favis-Mortlock

 ==========================================================================================================================================

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation,
 Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
#include "rg.h"
#include "simulation.h"
#include "cell.h"


/*=========================================================================================================================================

 This member function of CSimulation does splash redistribution of the whole grid at each timestep. The original was written by Olivier Planchon, 2 May 2001. Modified from the blend scheme used for calculating the Laplacian of Z in:

 Planchon O., Esteves M., Silvera N. and Lapetite J.M. (2000). Raindrop erosion of tillage induced microrelief. Possible use of the diffusion equation. Soil and Tillage Research 56(3-4), 131-144.

=========================================================================================================================================*/
void CSimulation::DoAllSplash(void)
{
   // First calculate the Laplacian for all cells in the grid. Process the interior first
   static bool sbSw = true;
   if (sbSw)
   {
      for (int nX = 0; nX < m_nXGridMax; nX++)
      {
         for (int nY = 0; nY < m_nYGridMax; nY++)
         {
            if (Cell[nX][nY].bIsMissingValue())
               continue;

            Cell[nX][nY].pGetSoil()->SetLaplacian(dCalcLaplacian(nX, nY));
         }
      }
   }
   else
   {
      for (int nX = m_nXGridMax-1; nX >= 0; nX--)
      {
         for (int nY = m_nYGridMax-1; nY >= 0; nY--)
         {
            if (Cell[nX][nY].bIsMissingValue())
               continue;

            Cell[nX][nY].pGetSoil()->SetLaplacian(dCalcLaplacian(nX, nY));
         }
      }
   }

   sbSw = ! sbSw;

   // Now calculate the change in elevation due to splash redistribution for each cell
   m_dThisIterKE = 0;
   double dTempSplashDepositionThisIter = 0;
   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      for (int nY = 0; nY < m_nYGridMax; nY++)
      {
         if (Cell[nX][nY].bIsMissingValue())
            continue;

         // Get the rainfall depth for this iteration. Note that this assumes that splash calcs are run EVERY iteration when there is rain
         double dRain = Cell[nX][nY].pGetRainAndRunon()->dGetRain();
         if (dRain > 0)
         {
            // Some rain has fallen on this cell, so calculate the kinetic energy of the rain = 0.5 m v**2
            double dKE = m_dPartKE * dRain;
            m_dThisIterKE += dKE;

            // Now calculate the amount of splash detachment or deposition resulting from this KE
            double dToChange = dKE * m_VdSplashEffN * Cell[nX][nY].pGetSoil()->dGetLaplacian();
            if (dToChange > 0)
            {
               // Splash deposition: save the dToChange value for the moment, until we know the proportion in each sediment size class for splashed detached sediment during this iteration
               Cell[nX][nY].pGetSoil()->SetSplashDepositionTemporary(dToChange);
               dTempSplashDepositionThisIter += dToChange;
            }
            else
            {
               // Splash detachment, first attenuate the dToChange depending on the depth of overland flow
               dToChange *= dCalcSplashCubicSpline(Cell[nX][nY].pGetSurfaceWater()->dGetSurfaceWaterDepth());

               // Now do the detachment
               Cell[nX][nY].pGetSoil()->DoSplashDetach(-dToChange);
            }
         }
      }
   }

   // OK, we have done all splash detachment, so we now know the proportion in each sediment size class for splashed detached sediment during this iteration
   double dTotSplashDetachThisIter = m_dThisIterClaySplashDetach + m_dThisIterSiltSplashDetach + m_dThisIterSandSplashDetach;
   if (dTotSplashDetachThisIter > 0)
   {
      // In order to preserve mass balance, splash detachment and deposition in any interation must be equal. We know the total of splash detachment for this iteration, so calculate a correction ratio to ensure that splash deposition equals splash detachment
      double dCorrectionRatio = 1;
      if (dTempSplashDepositionThisIter > 0)
         dCorrectionRatio = dTotSplashDetachThisIter / dTempSplashDepositionThisIter;

      // Calculate the weightings for each size fraction
      double
         dClayWeight = dCorrectionRatio * m_dThisIterClaySplashDetach / dTotSplashDetachThisIter,
         dSiltWeight = dCorrectionRatio * m_dThisIterSiltSplashDetach / dTotSplashDetachThisIter,
         dSandWeight = dCorrectionRatio * m_dThisIterSandSplashDetach / dTotSplashDetachThisIter;

      for (int nX = 0; nX < m_nXGridMax; nX++)
      {
         for (int nY = 0; nY < m_nYGridMax; nY++)
         {
            if (Cell[nX][nY].bIsMissingValue())
               continue;

            // Do the splash deposition
            double dToChange = Cell[nX][nY].pGetSoil()->dGetSplashDepositionTemporary();
            if (dToChange > 0)
            {
               Cell[nX][nY].pGetSoil()->DoSplashDeposit(dToChange * dClayWeight, dToChange * dSiltWeight, dToChange * dSandWeight);
            }
         }
      }
   }
}


/*=========================================================================================================================================

 This member function of CSimulation initializes the water depth-splash efficiency relationship using an adaptation of the cubic spline routine
 (spline.c) in:

 Press, W.H., Teukolsky, S.A., Vetterling, W.T. and Flannery, B.P. (1992). Numerical Recipes in C (Second Edition), Cambridge University
 Press, Cambridge. 994 pp.

=========================================================================================================================================*/
void CSimulation::InitSplashEff(void)
{
   int nLen = m_VdSplashDepth.size();
   vector<double> VdU(nLen, 0);

   // First stab at calculating m_VdSplashEffCoeff[0]
   if (m_VdSplashEff[0] > 0.99e30)
      m_VdSplashEffCoeff[0] = VdU[0] = 0;
   else
   {
      m_VdSplashEffCoeff[0] = 100;                  // original value was -0.5;
      VdU[0] = (3 / (m_VdSplashDepth[1] - m_VdSplashDepth[0])) * ((m_VdSplashEff[1] - m_VdSplashEff[0]) / (m_VdSplashDepth[1] - m_VdSplashDepth[0]) - m_VdSplashEff[0]);
   }

   // Now similarly calculate m_VdSplashEffCoeff[1] to m_VdSplashEffCoeff[nLen-2]
   for (int i = 1; i < nLen-1; i++)
   {
      double dSig = (m_VdSplashDepth[i] - m_VdSplashDepth[i-1]) / (m_VdSplashDepth[i+1] - m_VdSplashDepth[i-1]);

      double dP = dSig * m_VdSplashEffCoeff[i-1] + 2;
      m_VdSplashEffCoeff[i] = (dSig - 1) / dP;

      VdU[i] = (m_VdSplashEff[i+1] - m_VdSplashEff[i]) / (m_VdSplashDepth[i+1] - m_VdSplashDepth[i]) - (m_VdSplashEff[i] - m_VdSplashEff[i-1]) / (m_VdSplashDepth[i] - m_VdSplashDepth[i-1]);

      VdU[i] = (6 * VdU[i] / (m_VdSplashDepth[i+1] - m_VdSplashDepth[i-1]) - dSig * VdU[i-1]) / dP;
   }

   double dQn, dUn;
   if (m_VdSplashEff[nLen-1] > 0.99e30)
      dQn = dUn = 0;
   else
   {
      dQn = 0.5;
      dUn = (3 / (m_VdSplashDepth[nLen-1] - m_VdSplashDepth[nLen-2])) * (m_VdSplashEff[nLen-1] - (m_VdSplashEff[nLen-1] - m_VdSplashEff[nLen-2]) / (m_VdSplashDepth[nLen-1] - m_VdSplashDepth[nLen-2]));
   }

   // Similarly calculate m_VdSplashEffCoeff[nLen-1]
   m_VdSplashEffCoeff[nLen-1] = (dUn - dQn * VdU[nLen-2])/ (dQn * m_VdSplashEffCoeff[nLen-2] + 1);

   // Finally recalculate values of m_VdSplashEffCoeff[nLen-2] to m_VdSplashEffCoeff[0]
   for (int j = nLen-2; j >= 0; j--)
      m_VdSplashEffCoeff[j] = m_VdSplashEffCoeff[j] * m_VdSplashEffCoeff[j+1] + VdU[j];
}


/*=========================================================================================================================================

 This member function of CSimulation interpolates a value of splash efficiency using a constrained cubic spline. It is a modification of splint.c in:

 Press, W.H., Teukolsky, S.A., Vetterling, W.T. and Flannery, B.P. (1992). Numerical Recipes in C (Second Edition), Cambridge University  Press, Cambridge. 994 pp.

=========================================================================================================================================*/
double CSimulation::dCalcSplashCubicSpline(double dDepth) const
{
   // Don't bother to do the calculation if there is no overland flow
   if (0 == dDepth)
      return (1);

   int nLen = m_VdSplashDepth.size();

   // If water depth is greater than maximum stored for the splash efficiency-water depth relationship, then use the maximum value (prevents possible spurious values being returned when the spline relationship is used to extrapolate values much higher than those used to define it)
   if (dDepth > m_VdSplashDepth[nLen-1])
      return (0);

   // OK start to calculate the cubic spline value
   int
      nLo = 0,
      nHi = nLen-1;

   while ((nHi - nLo)  > 1)
   {
      int n = (nHi + nLo) >> 1;
      if (m_VdSplashDepth[n] > dDepth)
         nHi = n;
      else
         nLo = n;
   }
   double fH = m_VdSplashDepth[nHi] - m_VdSplashDepth[nLo];

/*
   double fA = (m_VdSplashDepth[khi] - dDepth)/fH;
   double fB = (dDepth - m_VdSplashDepth[nLo])/fH;
   double dEff = (fA * m_VdSplashEff[nLo] + fB * m_VdSplashEff[nHi] + ((fA*fA*fA - fA) * m_VdSplashEffCoeff[nLo] + (fB*fB*fB - fB) * m_VdSplashEffCoeff[nHi]) * (fH*fH)/6);
*/
   // Calculate the normalized splash efficiency. This version is algebraically equivalent to the above, but more efficient:
   // from Prince, T. 'Curve fitting by Chebyshef and other methods', C/C++ Users' Journal
   double dB = (dDepth - m_VdSplashDepth[nLo]);
   double dA = (m_VdSplashDepth[nHi] - dDepth);
   double dEff = ((m_VdSplashEff[nLo] - m_VdSplashEffCoeff[nLo] / 6 * dB * (dA + fH)) * dA + (m_VdSplashEff[nHi] - m_VdSplashEffCoeff[nHi] / 6 * dA * (dB + fH)) * dB) / fH;

   return tMax(0.0, dEff);
}


/*=========================================================================================================================================

 This member function of CSimulation calculates the Laplacian for 4 cells with a centred scheme and ordinary axes (x-x, y-y). It is Eq (20) in the paper.

 The original was written by Olivier Planchon, 2 May 2001

=========================================================================================================================================*/
double CSimulation::dCalcLaplacian(int const nX, int const nY)
{
/*
   // This is the original code
   double dThisElev = Cell[nX][nY].dGetSoilSurfaceElevation();
   return (m_dCellSizeKC * (Deriv2(Cell[nX-1][nY].dGetSoilSurfaceElevation(), dThisElev, Cell[nX+1][nY].dGetSoilSurfaceElevation(), m_dCellSquare) + Deriv2(Cell[nX][nY-1].dGetSoilSurfaceElevation(), dThisElev, Cell[nX][nY+1].dGetSoilSurfaceElevation(), m_dCellSquare)));

   // This is mathematically equivalent to the code above
   return (m_dCellSizeKC * (Cell[nX-1][nY].dGetSoilSurfaceElevation() + Cell[nX+1][nY].dGetSoilSurfaceElevation() + Cell[nX][nY-1].dGetSoilSurfaceElevation() + Cell[nX][nY+1].dGetSoilSurfaceElevation() - (4 * Cell[nX][nY].dGetSoilSurfaceElevation())));
*/
   // Modification of the code above to also work with edge cells (which have less than 4 adjacent cells)
   int nAdj = 0;
   int nXTmp, nYTmp;
   double dLaplacian = 0;

   nXTmp = nX-1;
   nYTmp = nY;
   if ((nXTmp >= 0) && (! Cell[nXTmp][nYTmp].bIsMissingValue()))
   {
      nAdj++;
      dLaplacian += Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation();
   }

   nXTmp = nX+1;
   nYTmp = nY;
   if ((nXTmp < m_nXGridMax) && (! Cell[nXTmp][nYTmp].bIsMissingValue()))
   {
      nAdj++;
      dLaplacian += Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation();
   }

   nXTmp = nX;
   nYTmp = nY-1;
   if ((nYTmp >= 0) && (! Cell[nXTmp][nYTmp].bIsMissingValue()))
   {
      nAdj++;
      dLaplacian += Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation();
   }

   nXTmp = nX;
   nYTmp = nY+1;
   if ((nYTmp < m_nYGridMax) && (! Cell[nXTmp][nYTmp].bIsMissingValue()))
   {
      nAdj++;
      dLaplacian += Cell[nXTmp][nYTmp].pGetSoil()->dGetSoilSurfaceElevation();
   }

   dLaplacian -= (nAdj * Cell[nX][nY].pGetSoil()->dGetSoilSurfaceElevation());
   dLaplacian *= m_dCellSizeKC;

   return (dLaplacian);
}


/*========================================================================================================================================

  This member function of CSimulation outputs splash efficiency (calculated as a constrained cubic spline) for checking purposes

========================================================================================================================================*/
void CSimulation::CheckSplashEff(void)
{
   // Put together file name for CSV splash efficiency check file
   string strFilePathName = m_strOutputPath;
   strFilePathName.append(SPLASH_EFFICIENCY_CHECK);
   strFilePathName.append(CSV_EXT);

   // Create an ofstream object then try to open file for output
   ofstream SplshStream;
   SplshStream.open(strFilePathName, ios::out | ios::trunc);

   // Did it open OK?
   if (! SplshStream.is_open())
   {
      // Error: cannot open splash check file for output
      cerr << ERR << "cannot open " << strFilePathName << " for output" << endl;
      return;
   }

   SplshStream << setiosflags(ios::fixed);
   SplshStream << setprecision(6);

//   SplshStream << "depth\t,\tefficiency" << endl;
   double const dDelta = 0.1;
   for (double d = 0; d <= (m_VdSplashDepth.size() + 10); d += dDelta)
      SplshStream << d << "\t,\t" << dCalcSplashCubicSpline(d) << endl;

   // Close file
   SplshStream.close();
}


/*========================================================================================================================================

 This member function of CSimulation reads in the parameters for the splash efficency-water depth relationship.

========================================================================================================================================*/
bool CSimulation::bReadSplashEffData(void)
{
   // Put together file name
   string strFilePathName = m_strSplshFile;

   // Create ifstream object
   ifstream InStream;

   // Try to open splash efficiency file for input
   InStream.open(strFilePathName, ios::in);
   if (! InStream.is_open())
   {
      // Error: cannot open splash efficiency file for input
      cerr << ERR << "cannot open " << strFilePathName << " for input" << endl;
      return (false);
   }

   string strRec, strErr;
   int nLine = 0;
   while (getline(InStream, strRec))
   {
      // Trim off leading and trailing whitespace
      strRec = strTrim(&strRec);

      nLine++;

      // If it is a blank line or a comment then ignore it
      if ((! strRec.empty()) && (strRec[0] != QUOTE1) && (strRec[0] != QUOTE2))
      {
         // Now remove any trailing comments, starting with a QUOTE1 character
         // TODO what about QUOTE2?
         vector<string> VstrLeftRight = VstrSplit(&strRec, QUOTE1);
         if (VstrLeftRight.size() > 0)
         {
            // Assume this is a depth/efficiency pair, split by space
            vector<string> VstrDepthEfficiency = VstrSplit(&VstrLeftRight[0], SPACE);
            if (VstrDepthEfficiency.size() < 1)
            {
               strErr = "reading splash efficiency: line ";
               strErr.append(to_string(nLine));
               strErr.append(" is invalid: '");
               strErr.append(strRec);
               strErr.append("'");

               break;
            }

            // OK we have a pair, so first read depth, multiplied by drop diameter
            double dDpth = stod(VstrDepthEfficiency[0]) * m_dDropDiameter;      // in mm

            if (dDpth < 0)
            {
               strErr = "reading splash efficiency: line ";
               strErr.append(to_string(nLine));
               strErr.append(" has invalid water depth: '");
               strErr.append(strRec);
               strErr.append("'");

               break;
            }

            // Now read in splash efficiency
            double dSplashEff = stod(VstrDepthEfficiency[1]);

            if (dSplashEff < 0)
            {
               strErr = "reading splash efficiency: line ";
               strErr.append(to_string(nLine));
               strErr.append(" has invalid splash efficiency: '");
               strErr.append(strRec);
               strErr.append("'");

               break;
            }

            // All OK so store values
            m_VdSplashDepth.push_back(dDpth);      // mm
            m_VdSplashEff.push_back(dSplashEff);
            m_VdSplashEffCoeff.push_back(0);
         }
      }

      // Did an error occur?
      if (! strErr.empty())
      {
         // Error in input to splash efficiency file
         cerr << ERR << strErr << endl;
         InStream.close();
         return (false);
      }
   }

   // Now go through all m_VdSplashDepth values, should increase in sequence
   for (unsigned int i = 1; i < m_VdSplashDepth.size(); i++)
   {
      if (m_VdSplashDepth[i] <= m_VdSplashDepth[i-1])
      {
         // Error
         strErr = "reading splash efficiency: line ";
         strErr.append(to_string(nLine));
         strErr.append(" has water depth <= previous water depth: '");
         strErr.append(strRec);
         strErr.append("'");

         // Error in input to splash efficiency file
         cerr << ERR << strErr << endl;
         InStream.close();
         return (false);
      }
   }

   InStream.close();

   return (true);
}


/*=========================================================================================================================================

Adds to the this-operation splash detachment value

=========================================================================================================================================*/
void CSimulation::AddThisIterSplashDetach(double const dClayDetach, double const dSiltDetach, double const dSandDetach)
{
   m_dThisIterClaySplashDetach += dClayDetach;
   m_dThisIterSiltSplashDetach += dSiltDetach;
   m_dThisIterSandSplashDetach += dSandDetach;
}


/*=========================================================================================================================================

Adds to the this-operation splash deposition value for clay-sized sediment

=========================================================================================================================================*/
void CSimulation::AddThisIterClaySplashDeposit(double const dDeposit)
{
   m_dThisIterClaySplashDeposit += dDeposit;
}

/*=========================================================================================================================================

Adds to the this-operation splash deposition value for silt-sized sediment

=========================================================================================================================================*/
void CSimulation::AddThisIterSiltSplashDeposit(double const dDeposit)
{
   m_dThisIterSiltSplashDeposit += dDeposit;
}

/*=========================================================================================================================================

Adds to the this-operation splash deposition value for sand-sized sediment

=========================================================================================================================================*/
void CSimulation::AddThisIterSandSplashDeposit(double const dDeposit)
{
   m_dThisIterSandSplashDeposit += dDeposit;
}
