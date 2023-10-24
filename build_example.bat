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

:: Set variables based on input parameters
set "CMAKE_DIR=%~1"
set "BMC_SDK_PATH=%~2"
set "PLATFORM=win32"

:: If third parameter is provided, use it as the platform
if not "%~3"=="" set "PLATFORM=%~3"

:: Verify that the second argument is an absolute path
:: An absolute path starts with a letter followed by ":\", or it starts with "\\"
:: for a network path.
if not "!BMC_SDK_PATH:~0,1!"=="" (
    if not "!BMC_SDK_PATH:~1,2!"==":\" (
        if not "!BMC_SDK_PATH:~0,2!"=="\\" (
            echo Error: BMC_SDK_PATH must be an absolute path.
            exit /b 1
        )
    )
) else (
    echo Error: BMC_SDK_PATH is empty.
    exit /b 1
)

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

:: 4. Call `cmake .. -G "Visual Studio 17 2022" -A [third parameter] -DBMC_SDK_PATH=[second parameter]`
cmake .. -G "Visual Studio 17 2022" -A !PLATFORM! -DBMC_SDK_PATH=!BMC_SDK_PATH! || (
    echo CMake configuration failed
    exit /b 1
)

:: 5. Call `cmake --build . --config Release`
cmake --build . --config Release || (
    echo CMake build failed
    exit /b 1
)

:: 6. Check if the Release subfolder contains a file called sample_app.exe
if not exist "Release\sample_app.exe" (
    echo The file sample_app.exe was not found in the Release directory
    exit /b 1
)

echo Build and check successful
