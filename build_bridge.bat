@echo off
setlocal enabledelayedexpansion

:: Check if .venv exists and delete if it does
if exist ".venv" (
    echo Existing virtual environment found. Removing...
    rmdir /s /q ".venv"
)

:: Step 1: Create a virtual environment in the 'venv' folder
echo Creating virtual environment...
python -m venv .venv

:: Step 2: Activate the virtual environment
echo Activating virtual environment...
call .venv\Scripts\Activate

:: Step 4: Install the cx_Freeze package
echo Installing cx_Freeze...
pip install cx_Freeze

:: Step 5: Install the requirements
echo Installing requirements...
pip install -r python-backend\requirements.txt

:: Step 6: Build the executable
echo Building the executable...
python setup.py build

:: Step 7: Inform the user where the executable is located
echo Build process completed.

endlocal
