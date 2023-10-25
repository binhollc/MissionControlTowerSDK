@echo off
setlocal enabledelayedexpansion

if "%~1"=="" (
    echo Please provide the path to the library artifacts ^(BMC_SDK_PATH^)
    exit /b 1
)

:: Get the path to the staging directory
set "BMC_SDK_PATH=%~1"

:: Set the default platform
set "PLATFORM=win32"

:: If a platform argument is provided to build.bat, use it.
if not "%~2"=="" set "PLATFORM=%~2"

if not exist "!BMC_SDK_PATH!\bmc_sdk.dll" (
    echo File bmc_sdk.dll not found in !BMC_SDK_PATH!
    echo Build and stage the library first with build_lib.bat and then stage.bat
    exit /b 1
)

:: Iterate through each directory under the examples folder
for /d %%X in ("%~dp0examples\*") do (
    echo Building example: %%X
    call build_example.bat "%%X" "!BMC_SDK_PATH!" "!PLATFORM!"
    if errorlevel 1 (
        echo Failed to build example: %%X
        exit /b 1
    )
)

echo All examples built successfully!

endlocal