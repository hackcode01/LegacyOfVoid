#ifndef __ENGINE_APPLICATION_HPP__
#define __ENGINE_APPLICATION_HPP__

#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <cstdlib>
#include <memory>
#include <algorithm>
#include <limits>

#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vk_platform.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Engine {
    constexpr uint32_t WIDTH = 800;
    constexpr uint32_t HEIGHT = 600;

    const std::vector validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

#ifdef NDEBUG
    constexpr bool enableValidationLayers = false;
#else
    constexpr bool enableValidationLayers = true;
#endif

    class Application {
    public:
        Application() {}

        void run() {
            initWindow();
            initVulkan();
            mainLoop();
            cleanup();
        }

        std::string appName{"LegacyOfVoid"};
        std::string engineName{"LegacyOfVoidEngine"};

    private:
        GLFWwindow *m_window = nullptr;

        vk::raii::Context m_context;
        vk::raii::Instance m_instance = nullptr;
        vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;
        vk::raii::SurfaceKHR m_surface = nullptr;
        vk::raii::PhysicalDevice m_physicalDevice = nullptr;
        vk::raii::Device m_device = nullptr;
        vk::raii::Queue m_graphicsQueue = nullptr;

        vk::raii::SwapchainKHR m_swapChain = nullptr;
        std::vector<vk::Image> m_swapChainImages;
        vk::Format m_swapChainImageFormat = vk::Format::eUndefined;
        vk::Extent2D m_swapChainExtent;
        std::vector<vk::raii::ImageView> m_swapChainImageViews;

        std::vector<const char*> m_requiredDeviceExtension = {
            vk::KHRSwapchainExtensionName,
            vk::KHRSpirv14ExtensionName,
            vk::KHRSynchronization2ExtensionName,
            vk::KHRCreateRenderpass2ExtensionName
        };

        void initWindow() {
            glfwInit();

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        }

        void initVulkan() {
            createInstance();
            setupDebugMessenger();
            createSurface();
            pickPhysicalDevice();
            createLogicalDevice();
            createSwapChain();
        }

        void mainLoop() {
            while (!glfwWindowShouldClose(m_window)) {
                glfwPollEvents();
            }
        }

        void cleanup() {
            glfwDestroyWindow(m_window);

            glfwTerminate();
        }

        void createInstance();

        void setupDebugMessenger();

        void createSurface();

        void pickPhysicalDevice();

        void createLogicalDevice();

        void createSwapChain();

        static vk::Format chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);

        static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);

        vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

        std::vector<const char *> getRequiredExtensions();

        static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
            vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
            vk::DebugUtilsMessageTypeFlagsEXT type,
            const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void *);
    };
}

#endif
