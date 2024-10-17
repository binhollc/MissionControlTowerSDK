@echo off
setlocal enabledelayedexpansion

:: Set the staging directory
set "STAGING_DIR=staging"
:: Set the default platform
set "PLATFORM=win32"

:: If a platform argument is provided to build_and_stage.bat, use it.
if not "%~1"=="" set "PLATFORM=%~1"

:: Remove staging directory
if exist "%STAGING_DIR%" rmdir /S /Q "%STAGING_DIR%"
mkdir "%STAGING_DIR%"

:: Check for existence of bridge.exe in the PATH
where bmcbridge.exe >nul 2>nul
if errorlevel 1 (
    echo WARNING: bmcbridge executable could not be found. Please check README.md
)

echo :: ---
echo Building and staging bmc_sdk library, docs and examples...
echo :: ---

if exist build rmdir /s /q build

mkdir build

cd build

cmake -DCMAKE_BUILD_TYPE=Release .. -G "Visual Studio 17 2022" -A %PLATFORM% || (
    echo CMake configuration failed
    exit /b 1
)

cmake --build . --config Release --target install || (
    echo CMake build failed
    exit /b 1
)

endlocal