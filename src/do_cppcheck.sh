#!/bin/sh

cppcheck -v --force --enable=all --inconclusive --suppress=missingIncludeSystem -U_DEBUG -U__BCPLUSPLUS__ -URANDCHECK -UCLOCKCHECK -U_WIN32 -U_MSC_VER -U_M_ALPHA -U__MINGW32__ -U__HP_aCC -U__ia64 -U__GNUG__ -UCPU -U_OPENMP -U_M_IX86 -U_M_MPPC -U_M_MRX000 -Urs6000 -Uultrasparc -Ux86 *.cpp 2> err.txt


