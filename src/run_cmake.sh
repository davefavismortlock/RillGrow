#!/bin/sh

rm -rf CMakeFiles/
rm -rf Debug/
rm -rf Release/
rm -rf .cmake
rm -f CMakeCache.txt
rm -f Makefile*
rm -f *.json
rm -f *.cmake
rm -f install_manifest.txt

#cmake -DCMAKE_BUILD_TYPE=Release .

cmake -DCMAKE_BUILD_TYPE=Debug .
