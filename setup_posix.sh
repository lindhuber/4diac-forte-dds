#!/bin/bash
# Script to build and install FastDDS locally

# Go to the Fast-DDS directory
cd third_party/Fast-DDS || exit

# Create a build directory if it doesn't exist
mkdir -p build && cd build

# Run CMake with an installation path within your project
cmake .. -DCMAKE_INSTALL_PREFIX=../../fastdds_installed

# Build and install FastDDS
make -j$(nproc)
make install

# Return to the project root
cd ../../..
echo "FastDDS built and installed in fastdds_installed directory."
