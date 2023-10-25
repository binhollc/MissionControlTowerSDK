@echo off
setlocal enabledelayedexpansion

:: Set the default platform
set "PLATFORM=win32"

:: If a platform argument is provided to build.bat, use it.
if not "%~1"=="" set "PLATFORM=%~1"

set BUILD_DIR=build

:: Create a build folder if it doesn't exist
if not exist "%BUILD_DIR%\" mkdir build

:: Navigate to the build folder
cd build || (
    echo Failed to navigate to build directory
    exit /b 1
)

echo :: ---
echo Building the library...
echo :: ---

:: Call `cmake .. -G "Visual Studio 17 2022" -A %PLATFORM%`
cmake .. -G "Visual Studio 17 2022" -A !PLATFORM! || (
    echo CMake configuration failed
    exit /b 1
)

:: Call `cmake --build . --config Release`
cmake --build . --config Release || (
    echo CMake build failed
    exit /b 1
)

:: Check if the Release subfolder contains the library
if not exist "Release\bmc_sdk.dll" (
    echo The file bmc_sdk.dll was not found in the Release directory
    exit /b 1
)

cd ..

echo :: ---
echo Build and check successful
echo :: ---

endlocal