@echo off
setlocal enabledelayedexpansion

set PROJECT_DIR=%CD%
set STAGING_DIR=staging

:: Step 1: Create a virtual environment in the 'venv' folder
echo Creating virtual environment...
python -m venv venv

:: Step 2: Activate the virtual environment
echo Activating virtual environment...
call venv\Scripts\Activate

:: Step 4: Install the cx_Freeze package
echo Installing cx_Freeze...
pip install cx_Freeze

:: Step 5: Execute the script that creates the 'requirements.txt' file
echo Creating requirements.txt...
python create_requirements.py

:: Step 6: Verify that 'requirements.txt' has been created
if not exist "requirements.txt" (
    echo Error: 'requirements.txt' not found!
    exit /b 1
)

:: Step 7: Install the requirements
echo Installing requirements...
pip install -r requirements.txt

:: Step 8: Build the executable
echo Building the executable...
python setup.py build

:: Step 9: Inform the user where the executable is located
echo Build process completed. 

:: Copy build_bridge directory to the staging directory
if not exist "%STAGING_DIR%" (
    mkdir %STAGING_DIR%
)

echo Copying build_bridge to staging...
xcopy /E /I /Y build_bridge "%PROJECT_DIR%\%STAGING_DIR%\bridge"

echo Check the '%STAGING_DIR%\bridge' directory for the executable.

endlocal
