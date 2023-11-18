cmake_minimum_required(VERSION 3.5)
project(SampleAppUsingSampleLibrary)

# ---
# Source Files Declaration
# ---

set(SOURCES main.cpp)
set(TARGET_NAME sample_app_using_sample_library)

# ---
# Target Definition, Properties and Commands
# ---

add_executable(${TARGET_NAME} ${SOURCES})
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME sample_app)

# Include needed headers
target_include_directories(${TARGET_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ../sample_library
    .
)

# Link against needed libraries from the parent project
target_link_libraries(${TARGET_NAME} sample_library bmc_sdk nlohmann_json::nlohmann_json)

# ---
# Installation instructions
# ---

# Install the executable
install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION examples/${TARGET_NAME})

# Install the source files
install(FILES ${SOURCES} DESTINATION examples/${TARGET_NAME})
