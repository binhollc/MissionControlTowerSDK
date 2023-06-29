cmake_minimum_required(VERSION 3.5)
project(sample_app)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the path to the mct_api library directory
set(MCTAPI_DIR "../")

# Add the mct_api library as a subdirectory
add_subdirectory(${MCTAPI_DIR} mct_api)

include_directories(${MCTAPI_DIR}/include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Specify the source files for the sample_app executable
set(SOURCES main.cpp CommandDispatcher.cpp)

# Create the executable
add_executable(sample_app ${SOURCES})

# Link against the mct_api library
target_link_libraries(sample_app PRIVATE mct_api)

target_link_libraries(sample_app PRIVATE nlohmann_json::nlohmann_json)
