cmake_minimum_required(VERSION 3.5)
project(SampleLibrary)

# ---
# Source Files Declaration
# ---

set(SOURCES sample_library.cpp)
set(TARGET_NAME sample_library)

# ---
# Target Definition, Properties and Commands
# ---

add_library(${TARGET_NAME} SHARED ${SOURCES})
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME sample_library)

# Include needed headers
target_include_directories(${TARGET_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    .
)

# Link against needed libraries from the parent project
target_link_libraries(${TARGET_NAME} bmc_sdk nlohmann_json::nlohmann_json)

# ---
# Installation instructions
# ---

# Install the library (e.g. .dll, .lib, .so files)
install(TARGETS sample_library
        RUNTIME DESTINATION examples/${TARGET_NAME}
        ARCHIVE DESTINATION examples/${TARGET_NAME}
        LIBRARY DESTINATION examples/${TARGET_NAME}
)

# Install the source files
install(FILES ${SOURCES} DESTINATION examples/${TARGET_NAME})

# Install the header files
install(FILES sample_library.h DESTINATION examples/${TARGET_NAME})
