#include "../../../include/core/application/Application.hpp"

namespace Engine {
    void Application::createInstance() {
        vk::ApplicationInfo appInfo{};
        appInfo.pApplicationName = appName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = engineName.c_str();
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = vk::ApiVersion14;

        std::vector<char const *> requiredLayers;
        if (enableValidationLayers) {
            requiredLayers.assign(validationLayers.begin(), validationLayers.end());
        }

        auto layerProperties = m_context.enumerateInstanceLayerProperties();
        for (auto const &requiredLayer : requiredLayers) {
            if (std::ranges::none_of(layerProperties,
                                     [requiredLayer](auto const &layerProperty)
                                     { return strcmp(layerProperty.layerName, requiredLayer) == 0; }))
            {
                throw std::runtime_error("Required layer not supported: " + std::string(requiredLayer));
            }
        }

        auto requiredExtensions = getRequiredExtensions();

        auto extensionProperties = m_context.enumerateInstanceExtensionProperties();
        for (auto const &requiredExtension : requiredExtensions)
        {
            if (std::ranges::none_of(extensionProperties,
                                     [requiredExtension](auto const &extensionProperty)
                                     { return strcmp(extensionProperty.extensionName, requiredExtension) == 0; }))
            {
                throw std::runtime_error("Required extension not supported: " + std::string(requiredExtension));
            }
        }

        vk::InstanceCreateInfo createInfo{};
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size());
        createInfo.ppEnabledLayerNames = requiredLayers.data();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();
        m_instance = vk::raii::Instance(m_context, createInfo);
    }

    void Application::setupDebugMessenger() {
        if (!enableValidationLayers) {
            return;
        }

        vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
        vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
        vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{};
        debugUtilsMessengerCreateInfoEXT.messageSeverity = severityFlags;
        debugUtilsMessengerCreateInfoEXT.messageType = messageTypeFlags;
        debugUtilsMessengerCreateInfoEXT.pfnUserCallback = &debugCallback;
        m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
    }

    void Application::createSurface() {
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(*m_instance, m_window, nullptr, &surface) != 0) {
            throw std::runtime_error("Failed to create window surface!");
        }
        m_surface = vk::raii::SurfaceKHR(m_instance, surface);
    }

    void Application::pickPhysicalDevice()
    {
        std::vector<vk::raii::PhysicalDevice> devices = m_instance.enumeratePhysicalDevices();
        const auto devIter = std::ranges::find_if(
            devices,
            [&](auto const &device)
            {
                bool supportsVulkan1_4 = device.getProperties().apiVersion >= VK_API_VERSION_1_3;

                auto queueFamilies = device.getQueueFamilyProperties();
                bool supportsGraphics =
                    std::ranges::any_of(queueFamilies, [](auto const &qfp)
                                        { return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics); });

                auto availableDeviceExtensions = device.enumerateDeviceExtensionProperties();
                bool supportsAllRequiredExtensions =
                    std::ranges::all_of(m_requiredDeviceExtension,
                                        [&availableDeviceExtensions](auto const &requiredDeviceExtension)
                                        {
                                            return std::ranges::any_of(availableDeviceExtensions,
                                                                       [requiredDeviceExtension](auto const &availableDeviceExtension)
                                                                       { return strcmp(availableDeviceExtension.extensionName, requiredDeviceExtension) == 0; });
                                        });

                auto features = device.template getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
                bool supportsRequiredFeatures = features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
                                                features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

                return supportsVulkan1_4 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;
            });
        if (devIter != devices.end()) {
            m_physicalDevice = *devIter;
        } else {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    void Application::createLogicalDevice() {
        std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_physicalDevice.getQueueFamilyProperties();

        uint32_t queueIndex = ~0;

        for (uint32_t familyIndex = 0; familyIndex < queueFamilyProperties.size(); ++familyIndex) {
            if ((queueFamilyProperties[familyIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
                m_physicalDevice.getSurfaceSupportKHR(familyIndex, *m_surface)) {
                queueIndex = familyIndex;
                break;
            }
        }

        if (queueIndex == ~0) {
            throw std::runtime_error("Could not find a queue for graphics and present -> terminating");
        }

        vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain;
        featureChain.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering = true;
        featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState = true;

        float queuePriority = 0.0f;
        vk::DeviceQueueCreateInfo deviceQueueCreateInfo(
            vk::DeviceQueueCreateFlags(),
            queueIndex,
            1,
            &queuePriority
        );

        vk::DeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>();
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_requiredDeviceExtension.size());
        deviceCreateInfo.ppEnabledExtensionNames = m_requiredDeviceExtension.data();

        m_device = vk::raii::Device(m_physicalDevice, deviceCreateInfo);
        m_graphicsQueue = vk::raii::Queue(m_device, queueIndex, 0);
    }

    void Application::createSwapChain() {
        auto surfaceCapabilities = m_physicalDevice.getSurfaceCapabilitiesKHR(m_surface);
        m_swapChainImageFormat = chooseSwapSurfaceFormat(m_physicalDevice.getSurfaceFormatsKHR(m_surface));
        m_swapChainExtent = chooseSwapExtent(surfaceCapabilities);
        auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
        minImageCount = ( surfaceCapabilities.maxImageCount > 0 && minImageCount > surfaceCapabilities.maxImageCount ) ? surfaceCapabilities.maxImageCount : minImageCount;
        vk::SwapchainCreateInfoKHR swapChainCreateInfo{};
        swapChainCreateInfo.surface = m_surface;
        swapChainCreateInfo.minImageCount = minImageCount;
        swapChainCreateInfo.imageFormat = m_swapChainImageFormat;
        swapChainCreateInfo.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
        swapChainCreateInfo.imageExtent = m_swapChainExtent;
        swapChainCreateInfo.imageArrayLayers = 1;
        swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
        swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
        swapChainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
        swapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        swapChainCreateInfo.presentMode = chooseSwapPresentMode(m_physicalDevice.getSurfacePresentModesKHR(m_surface)),
        swapChainCreateInfo.clipped = true;

        m_swapChain = vk::raii::SwapchainKHR(m_device, swapChainCreateInfo);
        m_swapChainImages = m_swapChain.getImages();
    }

    vk::Format Application::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
        const auto formatIt = std::ranges::find_if(availableFormats,
        [](const auto& format) {
            return format.format == vk::Format::eB8G8R8A8Srgb &&
                   format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
        });

        return formatIt != availableFormats.end() ? formatIt->format : availableFormats[0].format;
    }

    vk::PresentModeKHR Application::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
        return std::ranges::any_of(availablePresentModes,
            [](const vk::PresentModeKHR value) {
                return vk::PresentModeKHR::eMailbox == value;
            }) ? vk::PresentModeKHR::eMailbox : vk::PresentModeKHR::eFifo;
    }

    vk::Extent2D Application::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }

        int32_t width;
        int32_t height;
        glfwGetFramebufferSize(m_window, &width, &height);

        return {
            std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        };
    }

    std::vector<const char *> Application::getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        if (enableValidationLayers) {
            extensions.push_back(vk::EXTDebugUtilsExtensionName);
        }

        return extensions;
    }

    VKAPI_ATTR vk::Bool32 VKAPI_CALL Application::debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void *) {
        if (severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError || severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
            std::cerr << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;
        }

        return vk::False;
    }
}
