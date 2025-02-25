#include <xar_engine/graphics/vulkan/instance.hpp>

#include <vector>

#include <GLFW/glfw3.h>

#include <xar_engine/error/exception_utils.hpp>

#include <xar_engine/logging/logger.hpp>


namespace xar_engine::graphics::vulkan
{
    namespace
    {
        constexpr auto logging_tag = "vulkan::Instance";

        void initialize_volk_once()
        {
            static auto volk_already_initialized = false;
            if (volk_already_initialized)
            {
                return;
            }

            const auto volk_initialize_result = volkInitialize();
            XAR_THROW_IF(
                volk_initialize_result != VK_SUCCESS,
                error::XarException,
                "Volk initialization failed");

            volk_already_initialized = true;
        }

        VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
            VkDebugUtilsMessageSeverityFlagBitsEXT vk_message_severity,
            VkDebugUtilsMessageTypeFlagsEXT vk_message_type,
            const VkDebugUtilsMessengerCallbackDataEXT* vk_callback_data,
            void* user_data)
        {
            switch (vk_message_severity)
            {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                {
                    XAR_LOG(
                        logging::LogLevel::DEBUG,
                        logging_tag,
                        "{}",
                        vk_callback_data->pMessage);
                    break;
                }
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                {
                    XAR_LOG(
                        logging::LogLevel::INFO,
                        logging_tag,
                        "{}",
                        vk_callback_data->pMessage);
                    break;
                }
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                {
                    XAR_LOG(
                        logging::LogLevel::WARNING,
                        logging_tag,
                        "{}",
                        vk_callback_data->pMessage);
                    break;
                }
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                {
                    XAR_LOG(
                        logging::LogLevel::ERROR,
                        logging_tag,
                        "{}",
                        vk_callback_data->pMessage);
                    break;
                }
                default:
                {
                    XAR_LOG(
                        logging::LogLevel::CRITICAL,
                        logging_tag,
                        "{}",
                        vk_callback_data->pMessage);
                    break;
                }
            }

            return VK_FALSE;
        }

        VkInstance make_vk_instance()
        {
            XAR_THROW_IF(
                !glfwVulkanSupported(),
                error::XarException,
                "Vulkan is not supported on this platform");

            auto all_extensions_counts = std::uint32_t{0};
            vkEnumerateInstanceExtensionProperties(
                nullptr,
                &all_extensions_counts,
                nullptr);

            auto all_extensions = std::vector<VkExtensionProperties>(all_extensions_counts);
            vkEnumerateInstanceExtensionProperties(
                nullptr,
                &all_extensions_counts,
                all_extensions.data());

            for (const auto& extension: all_extensions)
            {
                XAR_LOG(
                    logging::LogLevel::INFO,
                    logging_tag,
                    "{} {}",
                    extension.extensionName,
                    extension.specVersion);
            }

            auto extensions_counts = std::uint32_t{0};
            const auto glfw_extensions = glfwGetRequiredInstanceExtensions(&extensions_counts);

            auto extensions = std::vector<const char*>{};
            for (auto i = 0; i < extensions_counts; ++i)
            {
                extensions.emplace_back(glfw_extensions[i]);
            }
            extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

            auto validation_layer_counts = std::uint32_t{0};
            vkEnumerateInstanceLayerProperties(
                &validation_layer_counts,
                nullptr);
            auto all_validation_layers = std::vector<VkLayerProperties>(validation_layer_counts);
            vkEnumerateInstanceLayerProperties(
                &validation_layer_counts,
                all_validation_layers.data());
            for (const auto& validation_layer: all_validation_layers)
            {
                XAR_LOG(
                    logging::LogLevel::INFO,
                    logging_tag,
                    "{} {}",
                    validation_layer.layerName,
                    validation_layer.specVersion);
            }

            const auto validation_layers = std::vector<const char*>{"VK_LAYER_KHRONOS_validation"};

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
            debug_utils_messenger_create_info.pfnUserCallback = vk_debug_callback;
            debug_utils_messenger_create_info.pUserData = nullptr;

            auto vk_instance = VkInstance{nullptr};

            auto vk_application_info = VkApplicationInfo{};
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

            auto vk_instance_create_info = VkInstanceCreateInfo{};
            vk_instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            vk_instance_create_info.pNext = &debug_utils_messenger_create_info;
            vk_instance_create_info.pApplicationInfo = &vk_application_info;
            vk_instance_create_info.enabledExtensionCount = static_cast<std::uint32_t>(extensions.size());
            vk_instance_create_info.ppEnabledExtensionNames = extensions.data();
            vk_instance_create_info.enabledLayerCount = static_cast<std::uint32_t>(validation_layers.size());
            vk_instance_create_info.ppEnabledLayerNames = validation_layers.data();

            const auto create_instance_result = vkCreateInstance(
                &vk_instance_create_info,
                nullptr,
                &vk_instance);
            XAR_THROW_IF(
                create_instance_result != VK_SUCCESS,
                error::XarException,
                "Vulkan instance creation failed");

            return vk_instance;
        }
    }


    struct Instance::State
    {
    public:
        State();
        ~State();

    public:
        VkInstance vk_instance;
    };


    Instance::State::State()
        : vk_instance(nullptr)
    {
        initialize_volk_once();

        vk_instance = make_vk_instance();
        volkLoadInstance(vk_instance);
    }

    Instance::State::~State()
    {
        vkDestroyInstance(
            vk_instance,
            nullptr);
    }


    Instance::Instance()
        : _state(std::make_shared<State>())
    {
    }

    Instance::~Instance() = default;

    std::vector<PhysicalDevice> Instance::get_physical_device_list() const
    {
        auto physical_device_counts = std::uint32_t{0};
        vkEnumeratePhysicalDevices(
            _state->vk_instance,
            &physical_device_counts,
            nullptr);

        auto vk_physical_device_list = std::vector<VkPhysicalDevice>(physical_device_counts);
        vkEnumeratePhysicalDevices(
            _state->vk_instance,
            &physical_device_counts,
            vk_physical_device_list.data());

        auto physical_device_list = std::vector<PhysicalDevice>{};
        physical_device_list.reserve(physical_device_counts);
        for (auto vk_physical_device : vk_physical_device_list)
        {
            physical_device_list.emplace_back(PhysicalDevice::Parameters{
                vk_physical_device,
            });
        }

        return physical_device_list;
    }

    VkInstance Instance::get_native() const
    {
        return _state->vk_instance;
    }
}