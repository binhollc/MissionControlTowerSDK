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

# Clean the build_bridge directory
if [ -d "build_bridge" ]; then
    rm -rf build_bridge
fi

# Download the bridge for the current OS
if [[ "$OSTYPE" == "darwin"* ]]; then
    print_colored_message "${INFO}" "Downloading the bridge for MacOS"
    curl -L -o bridge.zip https://cdn.binho.io/sw/MissionControlBridge/0.12.0/macos-latest-64-artifacts.zip
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    print_colored_message "${INFO}" "Downloading the bridge for Ubuntu"
    curl -L -o bridge.zip https://cdn.binho.io/sw/MissionControlBridge/0.12.0/ubuntu-latest-64-artifacts.zip
else
    print_colored_message "${ERROR}" "Error: Unsupported OS"
    exit 1
fi

# Unzip the bridge
unzip bridge.zip

# Remove the zip file
rm bridge.zip

# Rename the directory from bridge to build_bridge
mv bridge build_bridge

# Check if build_bridge was created with a file named bridge within. If not, exit with an error.
if [ ! -d "build_bridge" ] || [ ! -f "build_bridge/bridge" ]; then
    print_colored_message "${ERROR}" "Error: The bridge was not downloaded successfully"
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