@echo off
setlocal enabledelayedexpansion

set PROJECT_DIR=%CD%
set BUILD_DIR=build
set STAGING_DIR=staging

:: Check the command line argument to decide which project to build or to skip building
if "%~1" == "stage_includes" (
    echo Skipping build, staging include files...
    goto StagingIncludesStep
)

:: Check the command line argument to decide which project to build
if "%~1" == "lib" (
    set ARTIFACT_NAME=bmc_sdk.dll bmc_sdk.lib
    set SUBPROJECT_DIR=%PROJECT_DIR%
) else if "%~1" == "sample_app" (
    set ARTIFACT_NAME=sample_app.exe
    set SUBPROJECT_DIR=%PROJECT_DIR%\sample_app
) else (
    echo Invalid argument. Use "lib" or "sample_app".
    exit /b 1
)

:: Set platform
if not "%~2"=="" (
    set PLATFORM=%~2
) else (
    set PLATFORM=win32
)

:: Configure CMake
cd %SUBPROJECT_DIR%

rem Remove the existing build directory if it exists
if exist "%BUILD_DIR%\" (
    rmdir /s /q "%BUILD_DIR%"
)

if not exist "%BUILD_DIR%\" (
    mkdir %BUILD_DIR%
)

cd %BUILD_DIR%
cmake .. -G "Visual Studio 17 2022" -A %PLATFORM%

:: Build
cd %SUBPROJECT_DIR%
cd %BUILD_DIR%
cmake --build . --config Release

:: Copy artifact to staging
cd %PROJECT_DIR%
if not exist "%STAGING_DIR%\" (
    mkdir %STAGING_DIR%
)
cd %SUBPROJECT_DIR%
for %%i in (%ARTIFACT_NAME%) do (
    if exist "%BUILD_DIR%\Release\%%i" (
        copy "%BUILD_DIR%\Release\%%i" "%PROJECT_DIR%\%STAGING_DIR%\"
    )
)

goto End

:StagingIncludesStep

cd %PROJECT_DIR%

set INCLUDE_ORIGIN_DIR=%PROJECT_DIR%\include
set INCLUDE_TARGET_DIR=%STAGING_DIR%\include

if exist "%INCLUDE_TARGET_DIR%\" (
    rmdir /s /q "%INCLUDE_TARGET_DIR%\"
)
if not exist "%INCLUDE_TARGET_DIR%\" (
    mkdir "%INCLUDE_TARGET_DIR%"
)
xcopy /E /I /Y "%INCLUDE_ORIGIN_DIR%" "%INCLUDE_TARGET_DIR%"

:End

cd %PROJECT_DIR%

echo Done!

endlocal
