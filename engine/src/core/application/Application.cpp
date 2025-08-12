#include "../../../include/core/application/Application.hpp"

namespace Engine {
    void Application::initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_pWindow = glfwCreateWindow(WIDTH, HEIGHT, "LegacyOfVoid", nullptr, nullptr);
    }

    void Application::createInstance() {
        constexpr vk::ApplicationInfo appInfo{
            "LegacyOfVoid",
            VK_MAKE_VERSION(1, 0, 0),
            "Engine",
            VK_MAKE_VERSION(1, 0, 0),
            vk::ApiVersion14};

        /** Get the required instance extensions from GLFW. */
        Engine::u32 glfwExtensionCount = 0;
        auto glfwExtensions{glfwGetRequiredInstanceExtensions(&glfwExtensionCount)};

        /** Check if the required GLFW extensions are supported by the Vulkan implementation. */
        auto extensionProperties{m_context.enumerateInstanceExtensionProperties()};

        for (Engine::u32 i{0}; i < glfwExtensionCount; ++i) {
            if (std::ranges::none_of(extensionProperties,
                [glfwExtension = glfwExtensions[i]](auto const &extensionProperty)
                    { return strcmp(extensionProperty.extensionName, glfwExtension) == 0; })) {

                throw std::runtime_error("Required GLFW extensions not supported: " +
                                         std::string(glfwExtensions[i]));
            }
        }

        vk::InstanceCreateInfo createInfo{};
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;

        m_instance = vk::raii::Instance(m_context, createInfo);
    }
}
