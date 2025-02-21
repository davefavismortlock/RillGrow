#!/bin/sh

cppcheck -v --enable=all --inconclusive --check-level=exhaustive --suppress=missingIncludeSystem --suppressions-list=suppressions.txt  -U_DEBUG -URANDCHECK -UCLOCKCHECK -U_WIN32 -U_MSC_VER -U_M_ALPHA -U__MINGW32__ -U__HP_aCC -U__ia64 -U__GNUG__ -UCPU -U_OPENMP -Ux86 --output-file=000_cppcheck_advice.txt --project=./compile_commands.json
