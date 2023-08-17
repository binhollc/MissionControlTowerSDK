@echo off
setlocal enabledelayedexpansion

set PROJECT_DIR=%CD%
set BUILD_DIR=build
set STAGING_DIR=staging

:: Check the command line argument to decide which project to build
if "%~1" == "lib" (
    set ARTIFACT_NAME=mct_api.dll
    set SUBPROJECT_DIR=%PROJECT_DIR%
) else if "%~1" == "sample_app" (
    set ARTIFACT_NAME=sample_app.exe
    set SUBPROJECT_DIR=%PROJECT_DIR%\sample_app
) else (
    echo Invalid argument. Use "lib" or "sample_app".
    exit /b 1
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
cmake .. -G "Visual Studio 17 2022" -A win32

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
copy "%BUILD_DIR%\Release\%ARTIFACT_NAME%" "%PROJECT_DIR%\%STAGING_DIR%\"

cd %PROJECT_DIR%

echo Done!
endlocal
