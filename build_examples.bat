@echo off
setlocal enabledelayedexpansion

:: Set the default platform
set "PLATFORM=win32"

:: If a platform argument is provided to build.bat, use it.
if not "%~1"=="" set "PLATFORM=%~1"

:: Get the absolute path to the staging directory
set "STAGING_DIR=%~dp0staging"

:: Iterate through each directory under the examples folder
for /d %%X in ("%~dp0examples\*") do (
    echo Building example: %%X
    call build_example.bat "%%X" "!STAGING_DIR!" "!PLATFORM!"
    if errorlevel 1 (
        echo Failed to build example: %%X
        exit /b 1
    )
)

echo All examples built successfully!
