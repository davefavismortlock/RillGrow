/*=========================================================================================================================================

 This is rg.cpp: the start-up routine for RillGrow. This simulates the growth and development of rills on a bare soil area as a
 self-organising complex system.

 Copyright (C) 2020 David Favis-Mortlock

 ==========================================================================================================================================

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation,
 Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================================================================================*/
// TODO check all uses of strcpy for buffer overflow, see http://cwe.mitre.org/data/definitions/676.html#Demonstrative%20Examples
// TODO Read in a saved CSimulation object (use Boost for this: http://www.boost.org/doc/libs/1_39_0/libs/serialization/doc/index.html), and save one at the end; so can restart
// TODO use unsigned integers where poss; check for integer overflow, http://cwe.mitre.org/data/definitions/190.html#Demonstrative%20Examples
// TODO check call code for 64-bit porting issues, see "20 issues of porting c++ code on the 64-bit platform'
// TODO Try ICC, see http://www.gentoo-wiki.info/HOWTO_ICC_and_Portage. See also http://software.intel.com/en-us/articles/non-commercial-software-download/
// TODO Get working with external rainfall simulator, see http://crestic.univ-reims.fr/centreimage/soda-project/VRS/

#include "rg.h"
#include "simulation.h"


/*=========================================================================================================================================

  RillGrow's main function

=========================================================================================================================================*/
int main (int argc, char* argv[])
{
   CSimulation* pRun = new CSimulation;

   // Run the simulation and then check how it ends
   int nRtn = pRun->nDoRun(argc, argv);
   pRun->DoEndRun(nRtn);

   delete pRun;

   // Back to OS
   return nRtn;
}
