@echo off
setlocal enabledelayedexpansion

:: Set the default platform
set "PLATFORM=win32"

:: If a platform argument is provided to build.bat, use it.
if not "%~1"=="" set "PLATFORM=%~1"

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
xcopy /E /I /Y build_bridge "%STAGING_DIR%\bridge"

echo :: ---
echo Building and staging bmc_sdk library, docs and examples...
echo :: ---

if exist build rmdir /s /q build
if exist staging rmdir /s /q staging

mkdir build
mkdir staging

cd build

cmake -DCMAKE_BUILD_TYPE=Release .. -G "Visual Studio 17 2022" -A win32 || (
    echo CMake configuration failed
    exit /b 1
)

cmake --build . --config Release --target install || (
    echo CMake build failed
    exit /b 1
)

endlocal