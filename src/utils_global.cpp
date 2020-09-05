/*========================================================================================================================================

 This is utils_global.cpp: globally available utility routines for RillGrow

 Copyright (C) 2020 David Favis-Mortlock

 =========================================================================================================================================

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public  License as published
 by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation,
 Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

========================================================================================================================================*/
#include "rg.h"


/*==============================================================================================================================

 Rounds positive or negative doubles correctly

==============================================================================================================================*/
double dRound(double const d)
{
   return ((d < 0.0) ? ceil(d - 0.5) : floor(d + 0.5));
}


/*==============================================================================================================================

 Uses dRound() to round a double to an int

==============================================================================================================================*/
int nRound(double const d)
{
   return static_cast<int>(dRound(d));
}


/*==============================================================================================================================

 For comparison of two floating-point numbers: since the accuracy of floating-point numbers varies with their magnitude, must compare them using an accuracy threshold which is relative to the magnitude of the two numbers being compared. This is a blend of an example from Knuth's 'The Art of Computer Programming. Volume 1. Fundamental Algorithms' and a posting dated 18 Nov 93 by rmartin@rcmcon.com (Robert Martin), archived in cpp_tips

 ==============================================================================================================================*/
bool bFPIsEqual(double const d1, double const d2, double const dEpsilon)
{
   if ((0 == d1) && (tAbs(d2) < dEpsilon))
      return true;
   else if ((0 == d2) && (tAbs(d1) < dEpsilon))
      return true;
   else
      return ((tAbs(d1 - d2) < (dEpsilon * tAbs(d1))) ? true : false);
}


// bool bIsWhole(double d)
// {
//    // From http://answers.yahoo.com/question/index?qid=20170320132617AAMdb7u
//    return (static_cast<int>(d) == d);
// }


/*==============================================================================================================================

 Operator that inserts a given fill character, to a given width, into an output stream. From http://stackoverflow.com/questions/2839592/equivalent-of-02d-with-stdstringstream

==============================================================================================================================*/
ostream& operator<< (ostream& ostr, const FillToWidth& args)
{
   ostr.fill(args.chFill);
   ostr.width(args.nWidth);

   return ostr;
}

