/*=========================================================================================================================================

 This is random_numbers.cpp: the random number routines for RillGrow

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
#include "simulation.h"


/*=========================================================================================================================================

 RillGrow's two random number generators are maximally equidistributed combined Tausworthe generators. The code is modified from taus.c in gsl-1.9, the GNU Scientific Library. The sequence is

 x_n = (s1_n ^ s2_n ^ s3_n)

 s1_{n+1} = (((s1_n & 4294967294) <<12) ^ (((s1_n <<13) ^ s1_n) >> 19))
 s2_{n+1} = (((s2_n & 4294967288) << 4) ^ (((s2_n << 2) ^ s2_n) >> 25))
 s3_{n+1} = (((s3_n & 4294967280) <<17) ^ (((s3_n << 3) ^ s3_n) >> 11))

 computed modulo 2^32. In the three formulae above '^' means exclusive-or (C-notation), not exponentiation. Note that the algorithm relies on the properties of 32-bit unsigned integers (it is formally defined on bit-vectors of length 32). GNU have added a bitmask to make it also work on 64 bit machines.

 Each generator is initialized with

 s1_1 .. s3_1 = s_n MOD m

 where s_n = (69069 * s_{n-1}) mod 2^32, and s_0 = s is the user-supplied seed.

 The theoretical value of x_{10007} is 2733957125. The subscript 10007 means (1) seed the generator with s=1 (2) do six warm-up iterations, (3) then do 10000 actual iterations. The period of this generator is about 2^88 i.e. 309485009821345068724781056

 From: P. L'Ecuyer, "Maximally equidistributed combined Tausworthe generators", Mathematics of Computation 65, 213 (1996), 203-213. This is available on the net from L'Ecuyer's home page:

 http://www.iro.umontreal.ca/~lecuyer/myftp/papers/tausme.ps
 ftp://ftp.iro.umontreal.ca/pub/simulation/lecuyer/papers/tausme.ps

 Update: April 2002

 There is an erratum in the paper "Tables of Maximally Equidistributed Combined LFSR  Generators", Mathematics of Computation, 68, 225 (1999), 261-269. See http://www.iro.umontreal.ca/~lecuyer/myftp/papers/tausme2.ps

 "... the k_j most significant bits of z_j must be non-zero, for each j. (Note: this  restriction also applies to the computer code given in [4], but was mistakenly not mentioned in that paper.)"

 This affects the seeding procedure by imposing the requirement s1 > 1, s2 > 7, s3 > 15.

 Update: November 2002

 There was a bug in the correction to the seeding procedure for s2. It affected the following seeds 254679140 1264751179 1519430319 2274823218 2529502358 3284895257 3539574397 (s2 < 8).

=========================================================================================================================================*/
unsigned long CSimulation::ulGetRand0(void)
{
   // ulGetRand0() generates random numbers for the rainfall routines
   m_ulRState[0].s1 = ulGetTausworthe(m_ulRState[0].s1, 13, 19, 4294967294ul, 12);
   m_ulRState[0].s2 = ulGetTausworthe(m_ulRState[0].s2, 2, 25, 4294967288ul, 4);
   m_ulRState[0].s3 = ulGetTausworthe(m_ulRState[0].s3, 3, 11, 4294967280ul, 17);

   return (m_ulRState[0].s1 ^ m_ulRState[0].s2 ^ m_ulRState[0].s3);
}


unsigned long CSimulation::ulGetRand1(void)
{
   // ulGetRand1() generates random numbers for the flow routing routines
   m_ulRState[1].s1 = ulGetTausworthe(m_ulRState[1].s1, 13, 19, 4294967294ul, 12);
   m_ulRState[1].s2 = ulGetTausworthe(m_ulRState[1].s2, 2, 25, 4294967288ul, 4);
   m_ulRState[1].s3 = ulGetTausworthe(m_ulRState[1].s3, 3, 11, 4294967280ul, 17);

   return (m_ulRState[1].s1 ^ m_ulRState[1].s2 ^ m_ulRState[1].s3);
}


/*=========================================================================================================================================

 Each of these initializes one of the Tausworthe generators

=========================================================================================================================================*/
void CSimulation::InitRand0(unsigned long ulSeed)
{
   if (0 == ulSeed)
      ulSeed = 1;                                        // default seed is 1

   m_ulRState[0].s1 = ulGetLCG(ulSeed);
   if (m_ulRState[0].s1 < 2)
      m_ulRState[0].s1 += 2UL;

   m_ulRState[0].s2 = ulGetLCG(m_ulRState[0].s1);
   if (m_ulRState[0].s2 < 8)
      m_ulRState[0].s2 += 8UL;

   m_ulRState[0].s3 = ulGetLCG(m_ulRState[0].s2);
   if (m_ulRState[0].s3 < 8)
      m_ulRState[0].s3 += 16UL;

   // Warm it up
   ulGetRand0();
   ulGetRand0();
   ulGetRand0();
   ulGetRand0();
   ulGetRand0();
   ulGetRand0();

   return;
}


void CSimulation::InitRand1(unsigned long ulSeed)
{
   if (0 == ulSeed)
      ulSeed = 1;                                        // default seed is 1

   m_ulRState[1].s1 = ulGetLCG(ulSeed);
   if (m_ulRState[1].s1 < 2)
      m_ulRState[1].s1 += 2UL;

   m_ulRState[1].s2 = ulGetLCG(m_ulRState[1].s1);
   if (m_ulRState[1].s2 < 8)
      m_ulRState[1].s2 += 8UL;

   m_ulRState[1].s3 = ulGetLCG(m_ulRState[1].s2);
   if (m_ulRState[1].s3 < 8)
      m_ulRState[1].s3 += 16UL;

   // Warm it up
   ulGetRand1();
   ulGetRand1();
   ulGetRand1();
   ulGetRand1();
   ulGetRand1();
   ulGetRand1();

   return;
}


unsigned long CSimulation::ulGetTausworthe(unsigned long const ulS, unsigned long const ulA, unsigned long const ulB, unsigned long const ulC, unsigned long const ulD)
{
   return (((ulS & ulC) << ulD) & MASK) ^ ((((ulS << ulA) & MASK) ^ ulS) >> ulB);
}


double CSimulation::dGetRand0d1(void)
{
   // Uses ulGetRand0() to return a double precision floating point number uniformly distributed in the range [0, 1) i.e. includes 0.0 but excludes 1.0. Based on a routine in taus.c from gsl-1.2
   return (static_cast<double>(ulGetRand0()) / 4294967296.0);
}


int CSimulation::nGetRand0To(int const nBound)
{
   // Uses ulGetRand0() to return a random unsigned integer uniformly distributed in the range [0, nBound) i.e. includes 0 but excludes nBound
   int nRtn;
   unsigned long ulScale = 4294967295ul / nBound;                 // nBound must be > 1

   do
   {
      nRtn = static_cast<int>(ulGetRand0() / ulScale);
   }
   while (nRtn >= nBound);

   return (nRtn);
}


int CSimulation::nGetRand1To(int const nBound)
{
   // As above, but uses ulGetRand1()
   int nRtn;
   unsigned long ulScale = 4294967295ul / nBound;                 // nBound must be > 1

   do
   {
      nRtn = static_cast<int>(ulGetRand1() / ulScale);
   }
   while (nRtn >= nBound);

   return (nRtn);
}


double CSimulation::dGetRand0GaussPos(double const dMean, double const dStd)
{
   // Uses ulGetRand0()
   return (tMax((dGetRand0Gaussian() * dStd) + dMean, 0.0));
}


/*=========================================================================================================================================

 Called by the RandSet routines

=========================================================================================================================================*/
unsigned long CSimulation::ulGetLCG(const unsigned long ulN)
{
   return ((69069 * ulN) & MASK);
}


/*=========================================================================================================================================

 Using ulGetRand0(), randomly samples from a unit Gaussian (normal) distribution, is routine gasdev.c in:

 Press, W.H., Teukolsky, S.A., Vetterling, W.T. and Flannery, B.P. (1992). Numerical Recipes in C (Second Edition), Cambridge University Press, Cambridge. 994 pp.

=========================================================================================================================================*/
double CSimulation::dGetRand0Gaussian(void)
{
   static int snSet = 0;
   static double sfGset;
   double fRet;

   if (0 == snSet)                              // we don't have an extra deviate handy, so
   {
      double fFac, fRsq, fV1, fV2;

      do
      {
         // Pick two uniform numbers in the square extending from -1 to +1 in each direction, see if they are in the unit circle
         fV1 = 2 * dGetRand0d1()-1;             // uses ulGetRand0()
         fV2 = 2 * dGetRand0d1()-1;             // ditto
         fRsq = fV1 * fV1 + fV2 * fV2;
      }
      while (fRsq >= 1 || 0 == fRsq);           // if they are not, try again

      fFac = sqrt(-2 * log(fRsq)/fRsq);

      // Now make the Box-Muller transformation to get two normal deviates, return one and save the other for next time
      sfGset = fV1 * fFac;
      snSet = 1;                                // set flag
      fRet = fV2 * fFac;
   }
   else
   {
      snSet = 0;                                // we have an extra deviate handy so unset the flag and return it
      fRet = sfGset;
   }

   return (fRet);
}


#if defined RANDCHECK
/*=========================================================================================================================================

 Outputs random numbers for checking

=========================================================================================================================================*/
void CSimulation::CheckRand(void) const
{
/*
   // Have already done 6 warm-up iterations, plus one to write to .out file
   for (unsigned long k = 6; k < 10005; k++)
   {
      unsigned long n = ulGetRand0();
      if (k > 9990)
         m_ofsLog << k << "\t" << n << endl;
   }
*/

/*
   for (unsigned long k = 0; k < 100000; k++)
   {
      // This is for tests using ENT, must also change m_ofsLog to open in binary mode (see run.cpp)
      m_ofsLog << static_cast<unsigned char>(nGetRand0To(256));        // uses ulGetRand0()
   }
   return (RTN_OK);
*/

   /*
   for (unsigned long k = 0; k < 65536; k++)          // max number that Excel will read
   {
      static unsigned long sulr = 0;
      m_ofsLog << sulr << "\t";
      sulr = ulGetRand0();
      m_ofsLog << sulr << endl;

//      m_ofsLog << dGetRand0d1() << endl;           // uses GetRand0()
//      int x = nGetRand0To(65535);          // ditto
//      int y = nGetRand0To(65535);          // ditto
//      int x = nGetRand0To(150);                     // ditto
//      int y = nGetRand0To(100);                     // ditto
//      m_ofsLog << x << '\t' << y << endl;
   }
*/
}
#endif
