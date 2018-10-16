#!/bin/sh

rm CMakeCache.txt

# For OUCE
#cmake -DCMAKE_CXX_COMPILER=g++44 -DCMAKE_C_COMPILER=gcc44 -DCMAKE_BUILD_TYPE=Release .

cmake -DCMAKE_BUILD_TYPE=Release .

#cmake -DCMAKE_BUILD_TYPE=Debug .

