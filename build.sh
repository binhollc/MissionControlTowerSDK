#!/bin/bash

# Set the PROJECT_DIR variable. Use the first argument if provided, otherwise default to current directory ('.')
PROJECT_DIR=${1:-"$(cd "$(dirname "$0")"; pwd)"}

echo "$PROJECT_DIR"

# Build library
echo "---"
echo "Building the library"
echo "---"

cd "$PROJECT_DIR"

if [ -d "build" ]; then
    rm -rf build
fi
mkdir build
cd build
cmake ..
cmake --build . --config Release

# Create staging directory

echo "---"
echo "Creating staging directory"
echo "---"

cd "$PROJECT_DIR"
if [ ! -d "staging" ]; then
    mkdir staging
fi

# Stage the bridge

echo "---"
echo "Staging the bridge"
echo "---"

cd "$PROJECT_DIR"
cp -R build_bridge staging

# Stage library and include dirs

echo "---"
echo "Staging the library and include dirs"
echo "---"

cp build/libbmc_sdk.dylib staging
cp -R include staging

# Loop through each subdirectory in examples/ and build the sample app
for SAMPLE_DIR in "$PROJECT_DIR"/examples/*; do
    if [ -d "$SAMPLE_DIR" ]; then
        echo "---"
        echo "Building sample in directory: $SAMPLE_DIR"
        echo "---"
        
        cd "$SAMPLE_DIR"
        
        if [ -d "build" ]; then
            rm -rf build
        fi
        mkdir build
        cd build
        cmake .. -DBMC_SDK_PATH=../../staging
        cmake --build . --config Release
    fi
done

echo "---"
echo "Done"
echo "---"

cd $PROJECT_DIR