cmake_minimum_required(VERSION 3.15)

set(ENGINE_PROJECT_NAME Engine)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

find_package(Vulkan REQUIRED COMPONENTS glslc)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

set(INCLUDE_FILES
    include/engine.hpp
    include/defines.hpp
    include/logger.hpp
    include/asserts.hpp
)

set(SRC_FILES
    src/engine.cpp
)

set(ENGINE_ALL_SOURCES
    ${INCLUDE_FILES} ${SRC_FILES}
)

add_library(${ENGINE_PROJECT_NAME} STATIC
    ${ENGINE_ALL_SOURCES}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    ${ENGINE_ALL_SOURCES}
)

target_include_directories(${ENGINE_PROJECT_NAME} PUBLIC includes)
target_include_directories(${ENGINE_PROJECT_NAME} PUBLIC src)
target_compile_features(${ENGINE_PROJECT_NAME} PUBLIC cxx_std_20)

add_subdirectory(../external/glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)
target_link_libraries(${ENGINE_PROJECT_NAME} PRIVATE glfw)

add_subdirectory(../external/spdlog ${CMAKE_CURRENT_BINARY_DIR}/spdlog)
target_link_libraries(${ENGINE_PROJECT_NAME} PRIVATE spdlog)

target_link_libraries(${ENGINE_PROJECT_NAME} PRIVATE
    Vulkan::Vulkan
    glm::glm
    glad::glad
)

target_include_directories(${ENGINE_PROJECT_NAME} PRIVATE
    $<BUILD_INTERFACE:${glm_SOURCE_DIR}>
)

target_compile_definitions(${ENGINE_PROJECT_NAME} PRIVATE
    GLFW_INCLUDE_NONE
)

if(WIN32)
    target_link_libraries(${ENGINE_PROJECT_NAME} PRIVATE
        opengl32
        gdi32
    )
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(CORE_VIDEO_LIBRARY CoreVideo)
    target_link_libraries(${ENGINE_PROJECT_NAME} PRIVATE
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${CORE_VIDEO_LIBRARY}
    )
else()
    find_package(X11 REQUIRED)
    find_package(Threads REQUIRED)
    target_link_libraries(${ENGINE_PROJECT_NAME} PRIVATE
        X11
        pthread
        dl
        ${CMAKE_THREAD_LIBS_INIT}
    )
endif()
