@echo off
setlocal enabledelayedexpansion

:: Set the bridge_build directory
set "BRIDGE_BUILD_DIR=build_bridge"
:: Set the staging directory
set "STAGING_DIR=staging"
:: Set the default platform
set "PLATFORM=win32"

:: If a platform argument is provided to build_and_stage.bat, use it.
if not "%~1"=="" set "PLATFORM=%~1"

:: Remove staging directory
if exist "%STAGING_DIR%" rmdir /S /Q "%STAGING_DIR%"
mkdir "%STAGING_DIR%"

echo :: ---
echo Installing the bridge...
echo :: ---

:: Remove the build_bridge directory
if exist "%BRIDGE_BUILD_DIR%" rmdir /S /Q "%BRIDGE_BUILD_DIR%"

:: Download the bridge depending on the platform
echo Downloading the bridge...
if "%PLATFORM%"=="win32" (
    curl -L https://cdn.binho.io/sw/MissionControlBridge/0.12.0/windows-latest-32-artifacts.zip -o "bridge.zip"
) else (
    curl -L https://cdn.binho.io/sw/MissionControlBridge/0.12.0/windows-latest-64-artifacts.zip -o "bridge.zip"
)

:: Extract the bridge
echo Extracting the bridge...
tar -xf bridge.zip

:: Remove the bridge zip
del bridge.zip

:: Rename the directory from bridge to build_bridge
ren bridge "%BRIDGE_BUILD_DIR%"

:: Check if the bridge was downloaded and extracted successfully
if not exist "%BRIDGE_BUILD_DIR%" (
    echo Bridge download and extraction failed
    exit /b 1
)
if not exist "%BRIDGE_BUILD_DIR%\bridge.exe" (
    echo Bridge download and extraction failed
    exit /b 1
)

echo :: ---
echo Staging the Bridge...
echo :: ---

echo Copying build_bridge to staging...
echo "%STAGING_DIR%\bridge"
rmdir /S /Q "%STAGING_DIR%\bridge"
xcopy /E /I /Y build_bridge "%STAGING_DIR%\bridge"

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