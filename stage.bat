@echo off
setlocal enabledelayedexpansion

set PROJECT_DIR=%CD%
set STAGING_DIR=staging

if not exist "%STAGING_DIR%\" (
    mkdir %STAGING_DIR%
)

echo :: ---
echo Staging the Bridge...
echo :: ---

if not exist build_bridge (
    echo "Couldn't find build_bridge directory. Did you build it?"
    exit /b 1
)

echo Copying build_bridge to staging...
xcopy /E /I /Y build_bridge "%STAGING_DIR%\bridge"

echo :: ---
echo Staging include files...
echo :: ---

if exist "%STAGING_DIR%\include" (
    rmdir /s /q "%STAGING_DIR%\include\"
)
mkdir "%STAGING_DIR%\include"
xcopy /E /I /Y "%PROJECT_DIR%\include" "%STAGING_DIR%\include"

echo :: ---
echo Staging README file...
echo :: ---

copy %PROJECT_DIR%\docs\README.md %STAGING_DIR%

echo :: ---
echo Staging examples...
echo :: ---

call stage_examples.bat %STAGING_DIR%\examples
if errorlevel 1 (
    echo Failed to stage the examples. Did you build them with build_examples.bat?
    exit /b 1
)

echo :: ---
echo Staging library...
echo :: ---

set BUILD_DIR=build

if not exist "%BUILD_DIR%\" (
    echo Couldn't find build directory. Did you build the library with build_lib.bat?
    exit /b 1
)

set ARTIFACT_NAME=bmc_sdk.dll bmc_sdk.lib

:: Copy artifacts to staging

for %%i in (%ARTIFACT_NAME%) do (
    if not exist "%BUILD_DIR%\Release\%%i" (
        echo Couldn't find %BUILD_DIR%\Release\%%i. Did you build the library with build_lib.bat?
        exit /b 1
    )
    copy "%BUILD_DIR%\Release\%%i" "%PROJECT_DIR%\%STAGING_DIR%\"
)

echo Done!

endlocal