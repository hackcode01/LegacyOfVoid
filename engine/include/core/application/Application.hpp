#ifndef __ENGINE_APPLICATION_HPP__
#define __ENGINE_APPLICATION_HPP

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <algorithm>

#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vk_platform.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Engine
{
    constexpr uint32_t WIDTH = 800;
    constexpr uint32_t HEIGHT = 600;

    const std::vector validationLayers = {
        "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
    constexpr bool enableValidationLayers = false;
#else
    constexpr bool enableValidationLayers = true;
#endif

    class Application {
    public:
        Application() = default;

        void run() {
            initWindow();
            initVulkan();
            mainLoop();
            cleanup();
        }

        std::string appName{"LegacyOfVoid"};
        std::string engineName{"LegacyOfVoidEngine"};

    private:
        GLFWwindow *window = nullptr;

        vk::raii::Context context;
        vk::raii::Instance instance = nullptr;
        vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;

        void initWindow() {
            glfwInit();

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        }

        void initVulkan() {
            createInstance();
            setupDebugMessenger();
        }

        void mainLoop() {
            while (!glfwWindowShouldClose(window)) {
                glfwPollEvents();
            }
        }

        void cleanup() {
            glfwDestroyWindow(window);

            glfwTerminate();
        }

        void createInstance();

        void setupDebugMessenger();

        std::vector<const char *> getRequiredExtensions();

        static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
            vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
            vk::DebugUtilsMessageTypeFlagsEXT type,
            const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void *);
    };
}

#endif
