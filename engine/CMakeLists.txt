cmake_minimum_required(VERSION 3.15)

set(ENGINE_PROJECT_NAME Engine)
project(${ENGINE_PROJECT_NAME})

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

find_package(Vulkan REQUIRED COMPONENTS glslc)

list(APPEND CMAKE_PREFIX_PATH $ENV{IMGUI_DIR})
find_package(imgui CONFIG REQUIRED)

set(INCLUDE_FILES
    Include/Defines.hpp
    Include/Logger.hpp
    Include/Asserts.hpp

    Include/core/Application/Application.hpp
    Include/Core/Errors/ErrorMacros.hpp
    Include/Core/Errors/Errors.hpp

    Include/Core/Typedefs.hpp
    Include/Core/Templates/SafeRefcount.hpp
    Include/Core/SystemOS/Memory.hpp

    Include/Core/Variant/VariantDeepDuplicate.hpp
    Include/Core/Variant/Array.hpp
)

set(SOURCE_FILES
    Src/Core/Errors/Errors.cpp
    Src/Core/Errors/ErrorMacros.cpp
    Src/Core/Application/Application.cpp
)

add_library(${ENGINE_PROJECT_NAME} STATIC
    ${INCLUDE_FILES} ${SOURCE_FILES}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    ${INCLUDE_FILES} ${SOURCE_FILES}
)

target_include_directories(${ENGINE_PROJECT_NAME} PUBLIC includes)
target_include_directories(${ENGINE_PROJECT_NAME} PUBLIC src)
target_compile_features(${ENGINE_PROJECT_NAME} PUBLIC cxx_std_20)

add_subdirectory(../External/glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)
target_link_libraries(${ENGINE_PROJECT_NAME} PRIVATE glfw)

add_subdirectory(../External/spdlog ${CMAKE_CURRENT_BINARY_DIR}/spdlog)
target_link_libraries(${ENGINE_PROJECT_NAME} PRIVATE spdlog)

target_include_directories(${ENGINE_PROJECT_NAME} PUBLIC ../External/Vulkan-Headers/include)

add_subdirectory(../External/glm ${CMAKE_CURRENT_BINARY_DIR}/glm)
target_link_libraries(${ENGINE_PROJECT_NAME} PRIVATE glm::glm)

target_link_libraries(${ENGINE_PROJECT_NAME} PRIVATE
    Vulkan::Vulkan
    imgui::imgui
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
