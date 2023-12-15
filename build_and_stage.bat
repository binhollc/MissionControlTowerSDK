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

echo :: ---
echo Building the Bridge...
echo :: ---

call build_bridge.bat
if errorlevel 1 (
    echo Failed to build the Bridge. Check the errors above.
    exit /b 1
)

echo :: ---
echo Staging the Bridge...
echo :: ---

if not exist build_bridge (
    echo "Couldn't find build_bridge directory. Did you build it?"
    exit /b 1
)

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