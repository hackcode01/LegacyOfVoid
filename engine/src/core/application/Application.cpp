#include "../../../include/core/application/Application.hpp"

namespace Engine {
void Application::createInstance() {
    vk::ApplicationInfo appInfo{};
    appInfo.pApplicationName   = appName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
    appInfo.pEngineName        = engineName.c_str();
    appInfo.engineVersion      = VK_MAKE_VERSION( 1, 0, 0 );
    appInfo.apiVersion         = vk::ApiVersion14;

    std::vector<char const*> requiredLayers;
    if (enableValidationLayers) {
        requiredLayers.assign(validationLayers.begin(), validationLayers.end());
    }

    auto layerProperties = context.enumerateInstanceLayerProperties();
    for (auto const& requiredLayer : requiredLayers) {
        if (std::ranges::none_of(layerProperties,
                                 [requiredLayer](auto const& layerProperty)
                                 { return strcmp(layerProperty.layerName, requiredLayer) == 0; })) {
            throw std::runtime_error("Required layer not supported: " + std::string(requiredLayer));
        }
    }

        auto requiredExtensions = getRequiredExtensions();

        auto extensionProperties = context.enumerateInstanceExtensionProperties();
        for (auto const & requiredExtension : requiredExtensions)
        {
            if (std::ranges::none_of(extensionProperties,
                                     [requiredExtension](auto const& extensionProperty)
                                     { return strcmp(extensionProperty.extensionName, requiredExtension) == 0; }))
            {
                throw std::runtime_error("Required extension not supported: " + std::string(requiredExtension));
            }
        }

        vk::InstanceCreateInfo createInfo{};
            createInfo.pApplicationInfo        = &appInfo;
            createInfo.enabledLayerCount       = static_cast<uint32_t>(requiredLayers.size());
            createInfo.ppEnabledLayerNames     = requiredLayers.data();
            createInfo.enabledExtensionCount   = static_cast<uint32_t>(requiredExtensions.size());
            createInfo.ppEnabledExtensionNames = requiredExtensions.data();
        instance = vk::raii::Instance(context, createInfo);
    }

    void Application::setupDebugMessenger() {
        if (!enableValidationLayers) return;

        vk::DebugUtilsMessageSeverityFlagsEXT severityFlags( vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError );
        vk::DebugUtilsMessageTypeFlagsEXT    messageTypeFlags( vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation );
        vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{};
            debugUtilsMessengerCreateInfoEXT.messageSeverity = severityFlags;
            debugUtilsMessengerCreateInfoEXT.messageType = messageTypeFlags;
            debugUtilsMessengerCreateInfoEXT.pfnUserCallback = &debugCallback;
        debugMessenger = instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
    }

    std::vector<const char*> Application::getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        if (enableValidationLayers) {
            extensions.push_back(vk::EXTDebugUtilsExtensionName );
        }

        return extensions;
    }

    VKAPI_ATTR vk::Bool32 VKAPI_CALL Application::debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*) {
        if (severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError || severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
            std::cerr << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;
        }

        return vk::False;
    }

}