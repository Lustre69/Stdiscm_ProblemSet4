#!/bin/bash

# Build script for Distributed OCR System

set -e  # Exit on error

echo "Building Distributed OCR System..."

# Create build directory
if [ ! -d "build" ]; then
    mkdir build
fi

cd build

# Configure CMake
echo "Configuring CMake..."
cmake ..

# Build
echo "Building..."
cmake --build . -j$(nproc)

echo "Build complete!"
echo ""
echo "To run the server:"
echo "  ./build/ocr_server [address] [num_workers]"
echo ""
echo "To run the client:"
echo "  ./build/ocr_client"

