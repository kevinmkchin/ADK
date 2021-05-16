@echo off
if not exist build mkdir build
echo Build started: %time%
cmake -A Win32 -S . -B build
cmake --build build
echo Build finished: %time%