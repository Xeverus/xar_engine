#include <xar_engine/graphics/vulkan/vulkan_instance.hpp>

#include <vector>

#include <GLFW/glfw3.h>

#include <xar_engine/error/exception_utils.hpp>

#include <xar_engine/logging/console_logger.hpp>
#include <xar_engine/logging/logger.hpp>


namespace xar_engine::graphics::vulkan
{
    namespace
    {
        constexpr auto tag = "Vulkan Sandbox";
        std::unique_ptr<logging::ILogger> logger;

        void initialize_volk_once()
        {
            static auto volk_already_initialized = false;
            if (volk_already_initialized)
            {
                return;
            }

            const auto result = volkInitialize();
            XAR_THROW_IF(
                result != VK_SUCCESS,
                error::XarException,
                "Failed to initialize Volk");

            volk_already_initialized = true;
        }

        VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData)
        {
            XAR_LOG(
                logging::LogLevel::ERROR,
                tag,
                "{}",
                pCallbackData->pMessage);

            return VK_FALSE;
        }

        VkInstance make_vk_instance()
        {
            // enumerate extensions
            std::uint32_t all_extensions_count = 0;
            vkEnumerateInstanceExtensionProperties(
                nullptr,
                &all_extensions_count,
                nullptr);

            std::vector<VkExtensionProperties> all_extensions(all_extensions_count);
            vkEnumerateInstanceExtensionProperties(
                nullptr,
                &all_extensions_count,
                all_extensions.data());

            for (const auto& extension: all_extensions)
            {
                XAR_LOG(
                    logging::LogLevel::INFO,
                    tag,
                    "{} {}",
                    extension.extensionName,
                    extension.specVersion);
            }

            // collect extensions

            std::uint32_t extensions_count = 0;
            const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&extensions_count);

            std::vector<const char*> extensions;
            for (auto i = 0; i < extensions_count; ++i)
            {
                extensions.emplace_back(glfw_extensions[i]);
            }
            extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

            // enumerate validation layers
            std::uint32_t validation_layer_count = 0;
            vkEnumerateInstanceLayerProperties(
                &validation_layer_count,
                nullptr);
            std::vector<VkLayerProperties> all_validation_layers(validation_layer_count);
            vkEnumerateInstanceLayerProperties(
                &validation_layer_count,
                all_validation_layers.data());
            for (const auto& validation_layer: all_validation_layers)
            {
                XAR_LOG(
                    logging::LogLevel::INFO,
                    tag,
                    "{} {}",
                    validation_layer.layerName,
                    validation_layer.specVersion);
            }

            // use layers
            std::vector<const char*> validation_layers = {"VK_LAYER_KHRONOS_validation"};

            // make instance
            XAR_THROW_IF(
                !glfwVulkanSupported(),
                error::XarException,
                "Vulkan is not supported on this platform");

            // debug layer
            VkDebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info{};
            debug_utils_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debug_utils_messenger_create_info.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            debug_utils_messenger_create_info.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            debug_utils_messenger_create_info.pfnUserCallback = debugCallback;
            debug_utils_messenger_create_info.pUserData = nullptr;

            // instance
            VkInstance vk_instance = nullptr;

            VkApplicationInfo vk_application_info = {};
            vk_application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            vk_application_info.pEngineName = "eng-name";
            vk_application_info.engineVersion = VK_MAKE_VERSION(0,
                                                                0,
                                                                0);
            vk_application_info.apiVersion = VK_API_VERSION_1_3;
            vk_application_info.pApplicationName = "app-name";
            vk_application_info.applicationVersion = VK_MAKE_VERSION(0,
                                                                     0,
                                                                     0);

            VkInstanceCreateInfo vk_instance_create_info = {};
            vk_instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            vk_instance_create_info.pNext = &debug_utils_messenger_create_info;
            vk_instance_create_info.pApplicationInfo = &vk_application_info;
            vk_instance_create_info.enabledExtensionCount = extensions.size();
            vk_instance_create_info.ppEnabledExtensionNames = extensions.data();
            vk_instance_create_info.enabledLayerCount = validation_layers.size();
            vk_instance_create_info.ppEnabledLayerNames = validation_layers.data();

            const auto result = vkCreateInstance(
                &vk_instance_create_info,
                nullptr,
                &vk_instance);
            XAR_THROW_IF(
                result != VK_SUCCESS,
                error::XarException,
                "Failed to create Vulkan instance");

            return vk_instance;
        }
    }

    VulkanInstance::VulkanInstance()
        : _vk_instance(nullptr)
    {
        logger = std::make_unique<logging::ConsoleLogger>();

        initialize_volk_once();

        _vk_instance = make_vk_instance();
        volkLoadInstance(_vk_instance);
    }

    VulkanInstance::~VulkanInstance()
    {
        if (_vk_instance != nullptr)
        {
            vkDestroyInstance(
                _vk_instance,
                nullptr);
        }
    }

    VkInstance VulkanInstance::get_native() const
    {
        return _vk_instance;
    }
}