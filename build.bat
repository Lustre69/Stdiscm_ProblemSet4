@echo off
REM Build script for Distributed OCR System (Windows)

echo Building Distributed OCR System...

REM Create build directory
if not exist build mkdir build
cd build

REM Configure CMake
echo Configuring CMake...
cmake ..

REM Build
echo Building...
cmake --build . --config Release

echo Build complete!
echo.
echo To run the server:
echo   build\Release\ocr_server.exe [address] [num_workers]
echo.
echo To run the client:
echo   build\Release\ocr_client.exe

pause

