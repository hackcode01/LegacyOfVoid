cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

set(PROJECT_NAME Editor)
project(${PROJECT_NAME})

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} Engine spdlog)

set_target_properties(${PROJECT_NAME}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/bin/
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    src/main.cpp
)
