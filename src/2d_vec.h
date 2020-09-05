#ifndef __VEC_H__
   #define __VEC_H__
/*=========================================================================================================================================

This is 2d_vec.h: declarations of the class used to represent a 2D vector in RillGrow. The class is very much 'cut down' by comparison to a 'real' vector class: only
those methods which are necessary for RG are implemented

 Copyright (C) 2020 David Favis-Mortlock

 ==========================================================================================================================================

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation,
 Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/

class C2DVec
{
public:
   double x, y;      // these are public for speed: inelegant, but it works

public:
   C2DVec(void);
   C2DVec(const C2DVec&);
   explicit C2DVec(const C2DVec*);
   template<class T> C2DVec(const T&, const T&);

   ~C2DVec(void);

   C2DVec& operator = (const C2DVec&);
   C2DVec& operator += (const C2DVec&);
   C2DVec& operator += (const C2DVec*);
   C2DVec& operator -= (const C2DVec&);
   C2DVec& operator -= (const C2DVec*);

   template<class T> C2DVec& operator *= (const T&);
   C2DVec operator * (double const);

   template<class T> C2DVec& operator /= (const T&);

   double dToScalar(void) const;
};

#endif         // __VEC_H__
