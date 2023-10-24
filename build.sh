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
cmake -DCMAKE_BUILD_TYPE=Release ..
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

cp build/libbmc_sdk.*.dylib staging
cp -R include staging

# Build examples

echo "---"
echo "Building examples"
echo "---"

if [ ! -d "staging/examples" ]; then
    mkdir staging/examples
fi

# Loop through each subdirectory in examples/ and build the sample app
for SAMPLE_DIR in "$PROJECT_DIR"/examples/*; do
    if [ -d "$SAMPLE_DIR" ]; then
        echo "---"
        echo "Building example in directory: $SAMPLE_DIR"
        echo "---"
        echo

        cd "$SAMPLE_DIR"

        echo " * Removing $SAMPLE_DIR/build..."
        if [ -d "build" ]; then
            rm -rf build
        fi

        echo " * Staging $SAMPLE_DIR..."
        cp -R "$SAMPLE_DIR" "$PROJECT_DIR"/staging/examples

        echo " * Creating $SAMPLE_DIR/build..."
        mkdir build
        cd build

        echo " * Building $SAMPLE_DIR..."
        cmake .. -DBMC_SDK_PATH=../../staging
        cmake --build . --config Release

        echo
    fi
done

echo "---"
echo "Done"
echo "---"

cd $PROJECT_DIR