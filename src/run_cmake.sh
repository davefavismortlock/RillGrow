#!/bin/sh

rm -f CMakeCache.txt
rm -f Makefile

# For OUCE
#cmake -DCMAKE_CXX_COMPILER=g++44 -DCMAKE_C_COMPILER=gcc44 -DCMAKE_BUILD_TYPE=Release .

cmake -DCMAKE_BUILD_TYPE=Release . -G"CodeBlocks - Unix Makefiles"

#cmake -DCMAKE_BUILD_TYPE=Debug . -G"CodeBlocks - Unix Makefiles"

