cmake_minimum_required(VERSION 3.8)
project(BinhoMissionControlSDK)

# ---
# Set global variables
# ---

set(BUILDING_MAIN_PROJECT ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}")
set(STAGING_DIR ${CMAKE_INSTALL_PREFIX}/staging)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ---
# Source Files Declaration
# ---

# Platform Checks and Conditional Settings
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(SOURCE_FILES_PLATFORM_SPECIFIC src/BridgeReader_windows.cpp)
else()
    set(SOURCE_FILES_PLATFORM_SPECIFIC src/bridge_reader.cpp)
endif()

set(SOURCE_FILES
    src/CommandManager.cpp
    src/CommandDispatcher.cpp
    ${SOURCE_FILES_PLATFORM_SPECIFIC}
)

# ---
# External Dependencies and Libraries
# ---

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

# ---
# Target Definition, Properties and Commands
# ---

# Define the shared library
add_library(bmc_sdk SHARED ${SOURCE_FILES})

file(READ "VERSION" ver)

set_target_properties(bmc_sdk PROPERTIES
    VERSION ${ver}
    SOVERSION 1
)

get_target_property(target_version bmc_sdk VERSION)
message(STATUS "BMC SDK Version: ${target_version}")

target_compile_definitions(bmc_sdk PRIVATE BUILD_BMC_SDK)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  target_compile_definitions(bmc_sdk PRIVATE NDEBUG)
endif()

target_include_directories(bmc_sdk PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(bmc_sdk PRIVATE nlohmann_json::nlohmann_json)
if(UNIX)
    target_link_libraries(bmc_sdk PRIVATE pthread)
endif()

target_compile_features(bmc_sdk PUBLIC cxx_std_17)

# Define the static library
add_library(bmc_sdk_static STATIC ${SOURCE_FILES})

# Ensure the static library doesn't define `BUILD_BMC_SDK`
target_compile_definitions(bmc_sdk_static PRIVATE USE_STATIC_LIBS)

set_target_properties(bmc_sdk_static PROPERTIES
    OUTPUT_NAME bmc_sdk_static
    VERSION ${ver}
)

target_include_directories(bmc_sdk_static PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(bmc_sdk_static PRIVATE nlohmann_json::nlohmann_json)
if(UNIX)
    target_link_libraries(bmc_sdk_static PRIVATE pthread)
endif()

target_compile_features(bmc_sdk_static PUBLIC cxx_std_17)

# ---
# Installation instructions
# ---

# Installation path for headers
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION ${STAGING_DIR}/include)

# Install the shared library (e.g. .dll, .so files)
install(TARGETS bmc_sdk
        RUNTIME DESTINATION ${STAGING_DIR}/bin
        ARCHIVE DESTINATION ${STAGING_DIR}/lib
        LIBRARY DESTINATION ${STAGING_DIR}/lib
)

# Install the static library (.lib or .a files)
install(TARGETS bmc_sdk_static
        ARCHIVE DESTINATION ${STAGING_DIR}/lib
)

# Install documentation
install(DIRECTORY ${PROJECT_SOURCE_DIR}/docs/
        DESTINATION ${STAGING_DIR}/docs
        FILES_MATCHING PATTERN "*.md"
)

# ---
# Subdirectory Instructions
# ---

add_subdirectory(examples)
# add_subdirectory(tests)

# ---
# Define custom targets
# ---

# Custom command to clean the staging directory
if (WIN32)
    add_custom_target(clean-staging
        COMMAND cmd /c "if exist \"${STAGING_DIR}\" rmdir /S /Q \"${STAGING_DIR}\" && mkdir \"${CMAKE_INSTALL_PREFIX}\""
    )
else()
    add_custom_target(clean-staging
        COMMAND rm -rf ${STAGING_DIR}
    )
endif()
