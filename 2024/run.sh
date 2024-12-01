#!/bin/bash
# This script compiles and runs the solution for a given day.

if [[ -z "$1" ]]; then
    echo "Please provide the day number as an argument."
    echo "Example: $0 1"
    exit 1
fi

# Pad the day number with a leading zero if necessary
if [[ ${#1} -eq 1 ]]; then
    day="0$1"
else
    day="$1"
fi

if [[ -z $(ls src/day_"$day".c 2>/dev/null) ]]; then
    echo "Day $day not found."
    exit 1
fi

cd build || (echo "Build directory not found. Run build.sh first." && exit 1)

verbose=false
if [[ "$2" == "-v" ]]; then
    verbose=true
fi

if [[ "$verbose" == true ]]; then
    make "day_$day" || exit
else
    make "day_$day" > /dev/null 2>&1 || exit
fi

cd ..
./build/day_"$day"
