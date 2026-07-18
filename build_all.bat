@echo off
setlocal

if "%~1"=="" (
    echo Usage: build_all.bat [Debug^|Release^|RelWithDebInfo^]
    exit /b 1
)

set CONFIG=%~1
set BUILD_DIR=build-visual-studio-26

cmake -S . -B %BUILD_DIR%
if errorlevel 1 exit /b %errorlevel%

cmake --build %BUILD_DIR% --config %CONFIG%
if errorlevel 1 exit /b %errorlevel%

echo Successfully built %CONFIG%.