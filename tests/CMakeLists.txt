cmake_minimum_required(VERSION 3.5)
project(Tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
set(STAGING_DIR ${PROJECT_SOURCE_DIR}/../staging)

message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "STAGING_DIR: ${STAGING_DIR}")

# ---
# External Dependencies and Libraries
# ---

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(json googletest)

# ---
# Source Files Declaration
# ---

set(SOURCES
  SupernovaGenericFlowTest.cpp
  NotificationFlowTest.cpp
)

set(TARGET_NAME tests)

# ---
# Target Definition, Properties and Commands
# ---

enable_testing()

add_executable(${TARGET_NAME} ${SOURCES})

# Include needed headers
target_include_directories(${TARGET_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ./include
    ${STAGING_DIR}/include
    .
)

find_library(BMC_SDK_LIBRARY NAMES bmc_sdk PATHS ${STAGING_DIR}/lib NO_DEFAULT_PATH)

if(NOT BMC_SDK_LIBRARY)
  message(FATAL_ERROR "bmc_sdk library not found")
endif()

# Link against needed libraries from the parent project
target_link_libraries(
  ${TARGET_NAME}
  ${BMC_SDK_LIBRARY}
  nlohmann_json::nlohmann_json
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${TARGET_NAME})