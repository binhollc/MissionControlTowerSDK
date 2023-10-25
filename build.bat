@echo off
setlocal enabledelayedexpansion

:: Set the default platform
set "PLATFORM=win32"

:: If a platform argument is provided to build.bat, use it.
if not "%~1"=="" set "PLATFORM=%~1"

call build_lib.bat %PLATFORM%
if errorlevel 1 (
    echo Failed to build the library. Check the errors above.
    exit /b 1
)

echo "%~dp0build\Release"
echo %CD%
echo ------

call build_examples.bat "%~dp0build\Release" "%~dp0include" %PLATFORM%
if errorlevel 1 (
    echo Failed to build the examples. Check the errors above.
    exit /b 1
)

endlocal