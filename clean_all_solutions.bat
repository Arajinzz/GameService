@echo off
setlocal

set "FOUND=0"

for /d /r %%D in (build-visual-studio-26) do (
    if exist "%%D\" (
        set "FOUND=1"
        echo Deleting: "%%D"
        rd /s /q "%%D"
    )
)

if "%FOUND%"=="0" (
    echo No build-visual-studio-26 folders found.
)

pause