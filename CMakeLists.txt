cmake_minimum_required(VERSION 3.8)
project(BinhoMissionControlSDK)

# ---
# Set global variables
# ---

set(BUILDING_MAIN_PROJECT ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/staging")

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

add_library(bmc_sdk SHARED ${SOURCE_FILES})

set_target_properties(bmc_sdk PROPERTIES
    VERSION 0.3.1
    SOVERSION 1
)

target_compile_definitions(bmc_sdk PRIVATE BUILD_BMC_SDK)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  target_compile_definitions(bmc_sdk PRIVATE NDEBUG)
endif()

target_include_directories(bmc_sdk PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(bmc_sdk PRIVATE nlohmann_json::nlohmann_json)

target_compile_features(bmc_sdk PUBLIC cxx_std_17)

# ---
# Installation instructions
# ---

# Installation path for headers
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)

# Install the library (e.g. .dll, .lib, .so files)
install(TARGETS bmc_sdk
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
)

# Install documentation
install(DIRECTORY ${PROJECT_SOURCE_DIR}/docs/
        DESTINATION docs
        FILES_MATCHING PATTERN "*.md"
)

# ---
# Subdirectory Instructions
# ---

add_subdirectory(examples)

# ---
# Define custom targets
# ---

# Custom command to clean the staging directory
if (WIN32)
    add_custom_target(clean-staging
        COMMAND cmd /c "if exist \"${CMAKE_INSTALL_PREFIX}\" rmdir /S /Q \"${CMAKE_INSTALL_PREFIX}\" && mkdir \"${CMAKE_INSTALL_PREFIX}\""
    )
else()
    add_custom_target(clean-staging
        COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}
    )
endif()
