cmake_minimum_required(VERSION 3.5)
project(ListDevices)

# ---
# Source Files Declaration
# ---

set(SOURCES main.cpp)

# ---
# Target Definition, Properties and Commands
# ---

add_executable(sample_app ${SOURCES})

# Include needed headers
target_include_directories(sample_app PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    .
)

# Link against needed libraries from the parent project
target_link_libraries(sample_app bmc_sdk nlohmann_json::nlohmann_json)

# ---
# Installation instructions
# ---

# Install the executable
install(TARGETS sample_app RUNTIME DESTINATION examples/list_devices)

# Install the source files
install(FILES ${SOURCES} DESTINATION examples/list_devices)
