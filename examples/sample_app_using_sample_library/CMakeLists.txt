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

# Set output name
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME sample_app)

# Only set WIN32_EXECUTABLE property on Windows
if(WIN32)
    set_target_properties(${TARGET_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

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
install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION ${STAGING_DIR}/examples/${TARGET_NAME})

# Install the source files
install(FILES ${SOURCES} DESTINATION ${STAGING_DIR}/examples/${TARGET_NAME})

# ---
# Post-installation steps to set RPATH only on macOS
# ---

if(APPLE)
    install(CODE "
      execute_process(COMMAND install_name_tool -add_rpath @loader_path/../../lib ${STAGING_DIR}/examples/${TARGET_NAME}/sample_app)
      execute_process(COMMAND install_name_tool -add_rpath @loader_path/../sample_library ${STAGING_DIR}/examples/${TARGET_NAME}/sample_app)
    ")
endif()