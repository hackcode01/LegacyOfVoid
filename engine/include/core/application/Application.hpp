#ifndef __ENGINE_APPLICATION_HPP__
#define __ENGINE_APPLICATION_HPP

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <memory>
#include <string>

#include <vulkan/vulkan_raii.hpp>

#include <vulkan/vk_platform.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../../Defines.hpp"

namespace Engine {

    constexpr u32 WIDTH = 800;
    constexpr u32 HEIGHT = 600;

    class Application {

    public:
        void run() {
            initWindow();
            initVulkan();
            mainLoop();
            cleanup();
        }

    private:
        GLFWwindow *m_pWindow{nullptr};

        vk::raii::Context m_context{};
        vk::raii::Instance m_instance{nullptr};

        void initWindow();

        void initVulkan() {
            createInstance();
        }

        void mainLoop() {
            while (!glfwWindowShouldClose(m_pWindow)) {
                glfwPollEvents();
            }
        }

        void cleanup() {
            glfwDestroyWindow(m_pWindow);

            glfwTerminate();
        }

        void createInstance();
    };
}

#endif
