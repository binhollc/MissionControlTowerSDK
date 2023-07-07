cmake_minimum_required(VERSION 3.5)
project(sample_app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the path to the mct_api library directory
set(MCTAPI_DIR ..)

# Add the mct_api library as a subdirectory
add_subdirectory(${MCTAPI_DIR} mct_api)

include_directories(../include)
include_directories(.)

# Specify the source files for the sample_app executable
set(SOURCES main.cpp CommandDispatcher.cpp)

# Create the executable
add_executable(sample_app ${SOURCES})

# Link dynamic and static libraries
target_link_libraries(sample_app PRIVATE mct_api nlohmann_json::nlohmann_json)

# Set the location of the dynamic library
set_target_properties(sample_app PROPERTIES INSTALL_RPATH mct_api)
