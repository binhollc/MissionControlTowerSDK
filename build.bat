@echo off
setlocal enabledelayedexpansion

set PROJECT_DIR=%CD%
set BUILD_DIR=build
set STAGING_DIR=staging

if not exist "%STAGING_DIR%\" (
    mkdir %STAGING_DIR%
)

:: ---
echo Staging include files...
:: ---

if exist "%STAGING_DIR%\include" (
    rmdir /s /q "%STAGING_DIR%\include\"
)
mkdir "%STAGING_DIR%\include"
xcopy /E /I /Y "%PROJECT_DIR%\include" "%STAGING_DIR%\include"

:: ---
echo Staging README file...
:: ---

copy %PROJECT_DIR%\docs\README.md %STAGING_DIR%

:: ---
echo Staging examples...
:: ---

call stage_examples.bat %STAGING_DIR%\examples

:: ---
echo Building the library...
:: ---

set ARTIFACT_NAME=bmc_sdk.dll bmc_sdk.lib

:: Set platform
if not "%~2"=="" (
    set PLATFORM=%~1
) else (
    set PLATFORM=win32
)

:: Configure CMake
cd %PROJECT_DIR%

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
cd %BUILD_DIR%
cmake --build . --config Release

:: Copy artifact to staging
cd %PROJECT_DIR%

for %%i in (%ARTIFACT_NAME%) do (
    if exist "%BUILD_DIR%\Release\%%i" (
        copy "%BUILD_DIR%\Release\%%i" "%PROJECT_DIR%\%STAGING_DIR%\"
    )
)

echo Done!

:: ---
echo Staging the sample_app...
:: ---

copy "sample_app\build\Release\sample_app.exe" "%STAGING_DIR%"

endlocal
