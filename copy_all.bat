@echo off
setlocal

if "%~1"=="" (
    echo Usage: copy_all.bat [Project^] [Debug^|Release^|RelWithDebInfo^] [Destination^]
    exit /b 1
)
if "%~2"=="" (
    echo Usage: copy_all.bat [Project^] [Debug^|Release^|RelWithDebInfo^] [Destination^]
    exit /b 1
)
if "%~3"=="" (
    echo Usage: copy_all.bat [Project^] [Debug^|Release^|RelWithDebInfo^] [Destination^]
    exit /b 1
)

set PROJECT=%~1
set CONFIG=%~2
set DEST_DIR=%~3

rem Root of the CMake project (where this script lives)
pushd "%~dp0"

set SOURCE_DIR=build-visual-studio-26\%PROJECT%\%CONFIG%

if not exist "%DEST_DIR%" (
    mkdir "%DEST_DIR%"
)

echo Copying DLLs...
xcopy "%SOURCE_DIR%\*.dll" "%DEST_DIR%\" /Y /D >nul

echo Copying PDBs...
xcopy "%SOURCE_DIR%\*.pdb" "%DEST_DIR%\" /Y /D >nul

echo Done.

popd
exit /b 0