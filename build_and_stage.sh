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

# Ensure that the bridge executable is accessible
if ! command -v bridge &> /dev/null
then
    print_colored_message "${WARNING}" "Bridge executable could not be found. Please check README.md"
fi

# Clean the staging directory
if [ -d "staging" ]; then
    rm -rf staging
fi

mkdir staging

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