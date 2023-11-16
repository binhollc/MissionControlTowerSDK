cmake_minimum_required(VERSION 3.5)
project(sample_app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the path to the bmc_sdk library directory

# Check if BMC_SDK_PATH is passed in from command line, if not, throw an error
if(NOT DEFINED BMC_SDK_PATH)
    message(FATAL_ERROR "BMC_SDK_PATH is not defined.
    On Windows, the path is typically 'c:\\Program Files (x86)\\BinhoMissionControlSDK'
    and in Dev Mode the path is typically '../../staging'.
    Set it using: cmake .. -DBMC_SDK_PATH=<your-path-here>")
endif()

# Check if BMC_INCLUDE_PATH is passed in from command line, if not, throw an error
if(NOT DEFINED BMC_INCLUDE_PATH)
    message(FATAL_ERROR "BMC_INCLUDE_PATH is not defined.
    On Windows, the path is typically 'c:\\Program Files (x86)\\BinhoMissionControlSDK\\include'
    and in Dev Mode the path is typically '../../staging/include'.
    Set it using: cmake .. -DBMC_INCLUDE_PATH=<your-path-here>")
endif()

set(SAMPLE_LIBRARY_INCLUDE_PATH ../sample_library)
set(SAMPLE_LIBRARY_BINARY_PATH ../sample_library/build/Release)

include_directories(${BMC_INCLUDE_PATH})
include_directories(${SAMPLE_LIBRARY_INCLUDE_PATH})
include_directories(.)

# Install JSON library

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

# Try to find the bmc_sdk library using the path variable
find_library(BMC_SDK_LIB NAMES bmc_sdk PATHS ${BMC_SDK_PATH})

# Try to find the sample_library library using the SAMPLE_LIBRARY_BINARY_PATH variable
find_library(SAMPLE_LIBRARY NAMES sample_library PATHS ${SAMPLE_LIBRARY_BINARY_PATH})

# Specify the source files for the sample_app executable
set(SOURCES main.cpp)

# Create the executable
add_executable(sample_app ${SOURCES})

# Link found library and other libraries
target_link_libraries(
    sample_app PRIVATE
    ${BMC_SDK_LIB}
    nlohmann_json::nlohmann_json
    ${SAMPLE_LIBRARY}
)

# Set the location of the dynamic library for runtime using the path variable
set_target_properties(sample_app PROPERTIES INSTALL_RPATH ${BMC_SDK_PATH})
