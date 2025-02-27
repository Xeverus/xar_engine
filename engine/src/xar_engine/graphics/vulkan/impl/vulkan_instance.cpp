#include <xar_engine/graphics/vulkan/impl/vulkan_instance.hpp>

#include <vector>

#include <GLFW/glfw3.h>

#include <xar_engine/error/exception_utils.hpp>

#include <xar_engine/logging/logger.hpp>


namespace xar_engine::graphics::vulkan::impl
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
                "volkInitialize failed");

            volk_already_initialized = true;
        }

        VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
            const VkDebugUtilsMessageSeverityFlagBitsEXT vk_message_severity,
            const VkDebugUtilsMessageTypeFlagsEXT vk_message_type,
            const VkDebugUtilsMessengerCallbackDataEXT* const vk_callback_data,
            void* const user_data)
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

            auto all_extension_counts = std::uint32_t{0};
            vkEnumerateInstanceExtensionProperties(
                nullptr,
                &all_extension_counts,
                nullptr);

            auto all_vk_extension_properties_list = std::vector<VkExtensionProperties>(all_extension_counts);
            vkEnumerateInstanceExtensionProperties(
                nullptr,
                &all_extension_counts,
                all_vk_extension_properties_list.data());

            XAR_LOG(
                logging::LogLevel::INFO,
                logging_tag,
                "Vulkan instance extensions:");
            for (const auto& vk_extension_properties: all_vk_extension_properties_list)
            {
                XAR_LOG(
                    logging::LogLevel::INFO,
                    logging_tag,
                    "  {} {}",
                    vk_extension_properties.extensionName,
                    vk_extension_properties.specVersion);
            }

            auto glfw_required_instance_extension_counts = std::uint32_t{0};
            const auto glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_required_instance_extension_counts);

            auto selected_instance_extension_list = std::vector<const char*>{};
            for (auto i = 0; i < glfw_required_instance_extension_counts; ++i)
            {
                selected_instance_extension_list.emplace_back(glfw_extensions[i]);
            }
            selected_instance_extension_list.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

            auto instance_layer_properties_count = std::uint32_t{0};
            vkEnumerateInstanceLayerProperties(
                &instance_layer_properties_count,
                nullptr);
            auto all_vk_layer_properties_list = std::vector<VkLayerProperties>(instance_layer_properties_count);
            vkEnumerateInstanceLayerProperties(
                &instance_layer_properties_count,
                all_vk_layer_properties_list.data());

            XAR_LOG(
                logging::LogLevel::INFO,
                logging_tag,
                "Vulkan instance layers:");
            for (const auto& vk_layer_properties: all_vk_layer_properties_list)
            {
                XAR_LOG(
                    logging::LogLevel::INFO,
                    logging_tag,
                    "  {} {}",
                    vk_layer_properties.layerName,
                    vk_layer_properties.specVersion);
            }

            const auto selected_validation_layer_list = std::vector<const char*>{"VK_LAYER_KHRONOS_validation"};

            VkDebugUtilsMessengerCreateInfoEXT vk_debug_utils_messenger_create_info_ext{};
            vk_debug_utils_messenger_create_info_ext.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            vk_debug_utils_messenger_create_info_ext.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            vk_debug_utils_messenger_create_info_ext.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            vk_debug_utils_messenger_create_info_ext.pfnUserCallback = vk_debug_callback;
            vk_debug_utils_messenger_create_info_ext.pUserData = nullptr;

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
            vk_instance_create_info.pNext = &vk_debug_utils_messenger_create_info_ext;
            vk_instance_create_info.pApplicationInfo = &vk_application_info;
            vk_instance_create_info.enabledExtensionCount = static_cast<std::uint32_t>(selected_instance_extension_list.size());
            vk_instance_create_info.ppEnabledExtensionNames = selected_instance_extension_list.data();
            vk_instance_create_info.enabledLayerCount = static_cast<std::uint32_t>(selected_validation_layer_list.size());
            vk_instance_create_info.ppEnabledLayerNames = selected_validation_layer_list.data();

            const auto create_instance_result = vkCreateInstance(
                &vk_instance_create_info,
                nullptr,
                &vk_instance);
            XAR_THROW_IF(
                create_instance_result != VK_SUCCESS,
                error::XarException,
                "vkCreateInstance failed");

            return vk_instance;
        }
    }


    struct VulkanInstance::State
    {
    public:
        State();
        ~State();

    public:
        VkInstance vk_instance;
    };


    VulkanInstance::State::State()
        : vk_instance(nullptr)
    {
        initialize_volk_once();

        vk_instance = make_vk_instance();
        volkLoadInstance(vk_instance);
    }

    VulkanInstance::State::~State()
    {
        vkDestroyInstance(
            vk_instance,
            nullptr);
    }


    VulkanInstance::VulkanInstance()
        : _state(std::make_shared<State>())
    {
    }

    VulkanInstance::~VulkanInstance() = default;

    std::vector<VulkanPhysicalDevice> VulkanInstance::get_physical_device_list() const
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

        auto physical_device_list = std::vector<VulkanPhysicalDevice>{};
        physical_device_list.reserve(physical_device_counts);
        for (auto vk_physical_device : vk_physical_device_list)
        {
            physical_device_list.emplace_back(VulkanPhysicalDevice::Parameters{
                vk_physical_device,
            });
        }

        return physical_device_list;
    }

    VkInstance VulkanInstance::get_native() const
    {
        return _state->vk_instance;
    }
}