@echo off

set BUILD_DIR=client_build
set GENERATOR="Visual Studio 18 2026"
set ARCH=-A x64

echo Creating build directory: %BUILD_DIR%
if not exist %BUILD_DIR% (
    mkdir %BUILD_DIR%
)

cd %BUILD_DIR%

echo Running CMake...
cmake .. -G %GENERATOR% %ARCH% -DBUILD_SERVER=OFF -DBUILD_CLIENT=ON

pause