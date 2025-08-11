cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

set(PROJECT_NAME Engine)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

find_package(Vulkan REQUIRED COMPONENTS glslc)

set(FILES
    include/engine.hpp
    src/engine.cpp
)

add_library(${PROJECT_NAME} STATIC ${FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    Vulkan::Vulkan
)

target_include_directories(${PROJECT_NAME} PRIVATE
    $<BUILD_INTERFACE:${glm_SOURCE_DIR}>
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    GLFW_INCLUDE_NONE
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        opengl32
        gdi32
    )
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(CORE_VIDEO_LIBRARY CoreVideo)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${CORE_VIDEO_LIBRARY}
    )
else()
    find_package(X11 REQUIRED)
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        X11
        pthread
        dl
        ${CMAKE_THREAD_LIBS_INIT}
    )
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
