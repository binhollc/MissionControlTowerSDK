cmake_minimum_required(VERSION 3.16)

project(i3c_ibis)

set(CMAKE_CXX_STANDARD 17)

# ---
# Source Files Declaration
# ---

set(SOURCES main.cpp)
set(TARGET_NAME i3c_ibis)

# ---
# Target Definition and Properties
# ---

add_executable(${TARGET_NAME} ${SOURCES})
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME sample_app)

# Include needed headers
target_include_directories(${TARGET_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  .
)

# ---
# External Dependencies and Libraries
# ---

# Link against needed libraries from the parent project
target_link_libraries(${TARGET_NAME}
  bmc_sdk
  nlohmann_json::nlohmann_json
)

# ---
# Installation Configuration
# ---

install(TARGETS ${TARGET_NAME}
  RUNTIME DESTINATION staging/examples/${PROJECT_NAME}
)

# For macOS, set RPATH for the installed executable
if(APPLE)
  set_target_properties(${TARGET_NAME} PROPERTIES
    INSTALL_RPATH "@loader_path/../../lib"
  )
endif()
