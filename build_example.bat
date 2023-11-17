@echo off
setlocal enabledelayedexpansion

:: Check if at least two arguments are provided
if "%~1"=="" (
    echo Please provide the path to the directory containing the example
    exit /b 1
)
if "%~2"=="" (
    echo Please provide the BMC_SDK_PATH
    exit /b 1
)
if "%~3"=="" (
    echo Please provide the BMC_INCLUDE_PATH
    exit /b 1
)

:: Set variables based on input parameters
set "CMAKE_DIR=%~1"
set "BMC_SDK_PATH=%~2"
set "BMC_INCLUDE_PATH=%~3"
set "PLATFORM=win32"

:: If third parameter is provided, use it as the platform
if not "%~4"=="" set "PLATFORM=%~4"

:: 1. Navigate to the first parameter
cd /d "!CMAKE_DIR!" || (
    echo Failed to navigate to !CMAKE_DIR!
    exit /b 1
)

:: 2. Create a build folder if it doesn't exist
if not exist "build\" mkdir build

:: 3. Navigate to the build folder
cd build || (
    echo Failed to navigate to build directory
    exit /b 1
)

:: 4. Configure cmake
cmake .. -G "Visual Studio 17 2022" -A !PLATFORM! -DBMC_SDK_PATH="!BMC_SDK_PATH!" -DBMC_INCLUDE_PATH="!BMC_INCLUDE_PATH!" || (
    echo CMake configuration failed
    exit /b 1
)

:: 5. Build the example
cmake --build . --config Release || (
    echo CMake build failed
    exit /b 1
)

:: 6. Check if the Release subfolder contains any .exe or .dll file
set "fileFound=false"

:: Check for any .exe file
if exist "Release\*.exe" set "fileFound=true"

:: Check for any .dll file
if exist "Release\*.dll" set "fileFound=true"

if "!fileFound!"=="false" (
    echo No .exe or .dll files were found in the Release directory
    exit /b 1
)

echo Build and check successful

endlocal