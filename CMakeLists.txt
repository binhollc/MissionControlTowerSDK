cmake_minimum_required(VERSION 3.8)

project(mission_control_tower)

set(SOURCE_FILES
    src/mct_api.cpp
)

add_library(mct_api SHARED ${SOURCE_FILES})

target_include_directories(mct_api PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

set_target_properties(mct_api PROPERTIES
    VERSION 0.1.0
    SOVERSION 1
)

target_compile_features(mct_api PUBLIC cxx_std_17)

install(TARGETS mct_api DESTINATION lib)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)
