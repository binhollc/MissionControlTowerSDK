cmake_minimum_required(VERSION 3.8)
project(BMC_SDK_SampleLibrary)

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

target_include_directories(${BMC_INCLUDE_PATH})
target_include_directories(sample_library PUBLIC .)

# Install JSON library

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

# Try to find the bmc_sdk library using the path variable
find_library(BMC_SDK_LIB NAMES bmc_sdk PATHS ${BMC_SDK_PATH})

# Source files
set(SOURCE_FILES
    sample_library.cpp
)

add_library(sample_library SHARED ${SOURCE_FILES})

target_link_libraries(
    sample_library PRIVATE
    nlohmann_json::nlohmann_json
)

target_compile_features(bmc_sdk PUBLIC cxx_std_17)
