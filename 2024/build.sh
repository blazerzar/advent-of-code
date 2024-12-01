#!/bin/bash
# This script builds the project using CMake and vcpkg.

mkdir build
cd build || exit
cmake ..
