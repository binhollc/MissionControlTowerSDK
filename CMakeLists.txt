cmake_minimum_required(VERSION 3.8)

project(BinhoMissionControlSDK)

set(CMAKE_BUILD_TYPE Debug)

add_definitions(-DBUILD_BMC_SDK)

# Plarform-independent files
set(SOURCE_FILES
    src/CommandManager.cpp
    src/CommandDispatcher.cpp
)

# Add platform-dependent files
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND SOURCE_FILES src/BridgeReader_windows.cpp)
else()
    list(APPEND SOURCE_FILES src/bridge_reader.cpp)
endif()

add_library(bmc_sdk SHARED ${SOURCE_FILES})

target_include_directories(bmc_sdk PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

include(FetchContent)

# Install JSON library

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

# Add link libraries

target_link_libraries(
    bmc_sdk PRIVATE
    nlohmann_json::nlohmann_json
)

set_target_properties(bmc_sdk PROPERTIES
    VERSION 0.1.0
    SOVERSION 1
)

target_compile_features(bmc_sdk PUBLIC cxx_std_17)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)
