@echo off

set PROJECT_DIR=%CD%
set BUILD_DIR=build
set STAGING_DIR=staging
set DLL_NAME=mct_api.dll

rem Configure CMake
cd %PROJECT_DIR%

if not exist "%BUILD_DIR%\" (
    mkdir %BUILD_DIR%
)
cd %BUILD_DIR%
cmake .. -G "Visual Studio 17 2022" -A x64

rem Build
cd %PROJECT_DIR%
cd %BUILD_DIR%
cmake --build . --config Release

rem Copy DLL to staging
cd %PROJECT_DIR%
if not exist "%STAGING_DIR%\" (
    mkdir %STAGING_DIR%
)
copy "%BUILD_DIR%\Release\%DLL_NAME%" "%STAGING_DIR%\"

echo Done!
