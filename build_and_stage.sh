#!/bin/bash

# Defining some color codes
ERROR='\033[0;31m'
SUCCESS='\033[0;32m'
WARNING='\033[1;33m'
INFO='\033[1;94m'
NC='\033[0m' # No Color

print_colored_message() {
  local color_code=$1
  local message=$2
  local NC='\033[0m' # No Color

  echo -e "${color_code}---${NC}"
  echo -e "${color_code}${message}${NC}"
  echo -e "${color_code}---${NC}"
}

# Set the PROJECT_DIR variable.
PROJECT_DIR="$(pwd)"

# Build bridge
print_colored_message "${INFO}" "Building the Bridge"

# Check if GH_TOKEN is present. If not, exit the script with an error.
if [ -z "$GH_TOKEN" ]; then
    print_colored_message "${ERROR}" "Error: GH_TOKEN is not set."
    exit 1
fi

# If argument 1 is provided, then set $PYTHON to argument 1.
# Else $PYTHON=python
PYTHON=${1:-python}

print_colored_message "${WARNING}" "HEADS UP: $($PYTHON --version 2>&1). Is this what you want?\nIf not, provide the right interpreter as the first argument of this script."

# Deactivate virtual environment (if active) and remove its folder
if [ -n "$VIRTUAL_ENV" ]; then
    deactivate
fi

if [ -d ".venv" ]; then
    rm -rf .venv
fi

# Create Python virtual environment using $PYTHON
$PYTHON -m venv .venv

# Activate virtual env
source .venv/bin/activate

# Execute $PYTHON -m pip install cx_Freeze
python -m pip install cx_Freeze

# Execute $PYTHON -m pip install -r python-backend/requirements.txt
python -m pip install -r python-backend/requirements.txt

# Execute $PYTHON setup.py build
python setup.py build

# Check if build_bridge was created with a file named bridge within. If not, exit with an error.
if [ ! -f "build_bridge/bridge" ]; then
    print_colored_message "${ERROR}" "Error: bridge was not created in build_bridge."
    exit 1
fi

# Clean the staging directory

if [ -d "staging" ]; then
    rm -rf staging
fi

mkdir staging

# Stage the bridge

print_colored_message "${INFO}" "Staging the bridge"

cd "$PROJECT_DIR"
cp -R build_bridge staging/bridge

# Build library and examples

print_colored_message "${INFO}" "Building the bmc_sdk library and examples"

cd "$PROJECT_DIR"

if [ -d "build" ]; then
    rm -rf build
fi
mkdir build

cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release --target install

# Stage library, docs and examples

print_colored_message "${SUCCESS}" "All done!"

cd "$PROJECT_DIR"