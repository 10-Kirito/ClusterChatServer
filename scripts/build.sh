#!/bin/bash

# Create a build directory
mkdir build
cd build

# Generate build files using CMake
cmake ..

# Build the project
make