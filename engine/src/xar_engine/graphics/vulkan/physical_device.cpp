#include <xar_engine/graphics/vulkan/physical_device.hpp>

#include <xar_engine/error/exception_utils.hpp>

#include <xar_engine/logging/logger.hpp>


namespace xar_engine::graphics::vulkan
{
    namespace
    {
        const auto logging_tag = "vulkan::PhysicalDevice";
    }

    struct PhysicalDevice::State
    {
    public:
        explicit State(const Parameters& parameters);
        ~State();

    public:
        VkPhysicalDevice vk_physical_device;
        VkPhysicalDeviceProperties vk_physical_device_properties;
        VkPhysicalDeviceFeatures vk_physical_device_features;
        std::vector<VkQueueFamilyProperties> vk_queue_family_properties_list;
        std::vector<VkExtensionProperties> vk_extension_properties_list;
    };

    PhysicalDevice::State::State(const Parameters& parameters)
        : vk_physical_device{parameters.vk_physical_device}
        , vk_physical_device_properties{}
        , vk_physical_device_features{}
        , vk_queue_family_properties_list{}
        , vk_extension_properties_list{}
    {
        vkGetPhysicalDeviceProperties(
            vk_physical_device,
            &vk_physical_device_properties);

        vkGetPhysicalDeviceFeatures(
            vk_physical_device,
            &vk_physical_device_features);

        XAR_LOG(
            logging::LogLevel::DEBUG,
            logging_tag,
            "device:'{}' api:'{}' driver:'{}'",
            vk_physical_device_properties.deviceName,
            vk_physical_device_properties.apiVersion,
            vk_physical_device_properties.driverVersion);

        auto queue_familie_counts = std::uint32_t{0};
        vkGetPhysicalDeviceQueueFamilyProperties(
            vk_physical_device,
            &queue_familie_counts,
            nullptr);

        vk_queue_family_properties_list.resize(queue_familie_counts);
        vkGetPhysicalDeviceQueueFamilyProperties(
            vk_physical_device,
            &queue_familie_counts,
            vk_queue_family_properties_list.data());

        /*auto queue_family_index = std::uint32_t{0};
        for (const auto& vk_queue_family_properties : _vk_queue_family_properties[i])
        {
            VkBool32 present_support = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(
                physical_device,
                queue_family_index,
                _vk_surface_khr,
                &present_support);
            ++queue_family_index;

            XAR_LOG(
                logging::LogLevel::INFO,
                *_logger,
                tag,
                "{} {} {}",
                queue_family.queueCount,
                (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT),
                present_support);
        }*/

        auto device_extension_count = std::uint32_t{0};
        vkEnumerateDeviceExtensionProperties(
            vk_physical_device,
            nullptr,
            &device_extension_count,
            nullptr);

        vk_extension_properties_list.resize(device_extension_count);
        vkEnumerateDeviceExtensionProperties(
            vk_physical_device,
            nullptr,
            &device_extension_count,
            vk_extension_properties_list.data());

        XAR_LOG(
            logging::LogLevel::INFO,
            logging_tag,
            "device extension list:");
        for (const auto& vk_device_extension: vk_extension_properties_list)
        {
            XAR_LOG(
                logging::LogLevel::INFO,
                logging_tag,
                "  {} {}",
                vk_device_extension.extensionName,
                vk_device_extension.specVersion);
        }
    }

    PhysicalDevice::State::~State() = default;


    PhysicalDevice::PhysicalDevice()
        : _state(nullptr)
    {
    }

    PhysicalDevice::PhysicalDevice(const Parameters& parameters)
        : _state(std::make_shared<State>(parameters))
    {
    }

    VkPhysicalDevice PhysicalDevice::get_native() const
    {
        return _state->vk_physical_device;
    }

    const VkPhysicalDeviceProperties& PhysicalDevice::get_device_properties() const
    {
        return _state->vk_physical_device_properties;
    }

    const VkPhysicalDeviceFeatures& PhysicalDevice::get_device_features() const
    {
        return _state->vk_physical_device_features;
    }

    const std::vector<VkQueueFamilyProperties>& PhysicalDevice::get_queue_family_properties() const
    {
        return _state->vk_queue_family_properties_list;
    }

    const std::vector<VkExtensionProperties>& PhysicalDevice::get_extension_properties() const
    {
        return _state->vk_extension_properties_list;
    }

    VkSampleCountFlagBits PhysicalDevice::get_max_sample_count() const
    {
        const auto& physical_device_properties = get_device_properties();

        auto vk_sample_count_flags = VkSampleCountFlags{
            physical_device_properties.limits.framebufferColorSampleCounts &
            physical_device_properties.limits.framebufferDepthSampleCounts
        };

        if (vk_sample_count_flags & VK_SAMPLE_COUNT_64_BIT)
        {
            return VK_SAMPLE_COUNT_64_BIT;
        }
        if (vk_sample_count_flags & VK_SAMPLE_COUNT_32_BIT)
        {
            return VK_SAMPLE_COUNT_32_BIT;
        }
        if (vk_sample_count_flags & VK_SAMPLE_COUNT_16_BIT)
        {
            return VK_SAMPLE_COUNT_16_BIT;
        }
        if (vk_sample_count_flags & VK_SAMPLE_COUNT_8_BIT)
        {
            return VK_SAMPLE_COUNT_8_BIT;
        }
        if (vk_sample_count_flags & VK_SAMPLE_COUNT_4_BIT)
        {
            return VK_SAMPLE_COUNT_4_BIT;
        }
        if (vk_sample_count_flags & VK_SAMPLE_COUNT_2_BIT)
        {
            return VK_SAMPLE_COUNT_2_BIT;
        }

        return VK_SAMPLE_COUNT_1_BIT;
    }

    VkFormatProperties PhysicalDevice::get_format_properties(VkFormat vk_format) const
    {
        auto vk_format_properties = VkFormatProperties{};
        vkGetPhysicalDeviceFormatProperties(
            get_native(),
            vk_format,
            &vk_format_properties);

        return vk_format_properties;
    }

    VkFormat PhysicalDevice::find_supported_format(
        const std::vector<VkFormat>& candidate_vk_format_list,
        const VkImageTiling vk_tiling,
        const VkFormatFeatureFlags vk_feature_flags) const
    {
        for (const auto vk_format: candidate_vk_format_list)
        {
            const auto vk_format_properties = get_format_properties(vk_format);

            if (vk_tiling == VK_IMAGE_TILING_LINEAR &&
                (vk_format_properties.linearTilingFeatures & vk_feature_flags) == vk_feature_flags)
            {
                return vk_format;
            }
            else if (vk_tiling == VK_IMAGE_TILING_OPTIMAL &&
                     (vk_format_properties.optimalTilingFeatures & vk_feature_flags) == vk_feature_flags)
            {
                return vk_format;
            }
        }

        XAR_THROW(
            error::XarException,
            "Cannot find supported format");
    }

    std::uint32_t PhysicalDevice::find_memory_type(
        const uint32_t type_filter,
        const VkMemoryPropertyFlags properties) const
    {
        auto memory_properties = VkPhysicalDeviceMemoryProperties{};
        vkGetPhysicalDeviceMemoryProperties(
            _state->vk_physical_device,
            &memory_properties);

        for (auto i = std::uint32_t{0}; i < memory_properties.memoryTypeCount; i++)
        {
            if ((type_filter & (std::uint32_t{1} << i)) &&
                (memory_properties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        XAR_THROW(error::XarException,
                  "Failed to find suitable memory type");
    }

    VkSurfaceCapabilitiesKHR PhysicalDevice::get_surface_capabilities(VkSurfaceKHR vk_surface) const
    {
        auto capabilities_list = VkSurfaceCapabilitiesKHR{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            _state->vk_physical_device,
            vk_surface,
            &capabilities_list);

        return capabilities_list;
    }

    std::vector<VkSurfaceFormatKHR> PhysicalDevice::get_surface_formats(VkSurfaceKHR vk_surface) const
    {
        auto format_counts = std::uint32_t{0};
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            _state->vk_physical_device,
            vk_surface,
            &format_counts,
            nullptr);

        auto format_list = std::vector<VkSurfaceFormatKHR>(format_counts);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            _state->vk_physical_device,
            vk_surface,
            &format_counts,
            format_list.data());

        return format_list;
    }

    std::vector<VkPresentModeKHR> PhysicalDevice::get_present_modes(VkSurfaceKHR vk_surface) const
    {
        auto present_mode_counts = std::uint32_t{0};
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            _state->vk_physical_device,
            vk_surface,
            &present_mode_counts,
            nullptr);

        auto present_mode_list = std::vector<VkPresentModeKHR>(present_mode_counts);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            _state->vk_physical_device,
            vk_surface,
            &present_mode_counts,
            present_mode_list.data());

        return present_mode_list;
    }
}
