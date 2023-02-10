/*=========================================================================================================================================

 This is 2d_vec.cpp: implementations of the class used to represent a 2D vector in RillGrow. The class is very much 'cut down' by comparison to a 'real' vector class: only
 those methods which are necessary for RG are implemented

 Copyright (C) 2023 David Favis-Mortlock

 ==========================================================================================================================================

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation,
 Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
#include "rg.h"
#include "2d_vec.h"


// Default constructor, using initialization list
C2DVec::C2DVec(void) :
   x(0),
   y(0)
{
}

// Overloaded constructor, C2DVec as a param
C2DVec::C2DVec(const C2DVec& vIn) : x(vIn.x), y(vIn.y)
{
}

// Overloaded constructor, pointer to C2DVec as a param
C2DVec::C2DVec(const C2DVec* pvIn) : x(pvIn->x), y(pvIn->y)
{
}

// Overloaded constructor, any two simple types as input
template<class T> C2DVec::C2DVec(const T& t1, const T& t2) : x(t1), y(t2)
{
}


// Destructor
C2DVec::~C2DVec(void)
{
}


// ============================================================ Operators ==============================================================
C2DVec& C2DVec::operator = (const C2DVec& OtherVec)
{
   x = OtherVec.x;
   y = OtherVec.y;

   return (*this);
}

// ============================================================= Addition ==============================================================
// Assignment method with C2DVec as param
C2DVec& C2DVec::operator += (const C2DVec& vIn)
{
   x += vIn.x;
   y += vIn.y;
   return (*this);      // so that method can be 'chained': see 'Effective C++'
}

// Overloaded assignment method with pointer to C2DVec as param
C2DVec& C2DVec::operator += (const C2DVec* pvIn)
{
   x += pvIn->x;
   y += pvIn->y;
   return (*this);      // so that method can be 'chained': see 'Effective C++'
}


// =========================================================== Subtraction =============================================================
// Assignment method with C2DVec as param
C2DVec& C2DVec::operator -= (const C2DVec& vIn)
{
   x -= vIn.x;
   y -= vIn.y;
   return (*this);      // so that method can be 'chained': see 'Effective C++'
}

// Overloaded assignment method with pointer to C2DVec as param
C2DVec& C2DVec::operator -= (const C2DVec* pvIn)
{
   x -= pvIn->x;
   y -= pvIn->y;
   return (*this);      // so that method can be 'chained': see 'Effective C++'
}


// ==================================================== dToScalar multiplication of vector ================================================
//  Assignment method
template<class T> C2DVec& C2DVec::operator *= (const T& tIn)
{
   x *= tIn;
   y *= tIn;

   return (*this);      // so that method can be 'chained': see 'Effective C++'
}

// Stand-alone method
C2DVec C2DVec::operator * (double const dIn)
{
   // Note that the stand-alone method is defined in terms of the assignment method: see 'More Effective C++'
   return (*this *= dIn);
}


// ======================================================= dToScalar division of vector ===================================================
// Assignment method
template<class T> C2DVec& C2DVec::operator /= (const T& tIn)
{
   x /= tIn;
   y /= tIn;

   return (*this);      // so that method can be 'chained': see 'Effective C++'
}


// ======================================================== Convert vector to scalar ===================================================
double C2DVec::dToScalar(void) const
{
   return (sqrt((x*x)+(y*y)));
}
