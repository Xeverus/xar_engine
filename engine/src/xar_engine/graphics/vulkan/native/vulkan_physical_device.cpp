#include <xar_engine/graphics/vulkan/native/vulkan_physical_device.hpp>

#include <xar_engine/error/exception_utils.hpp>

#include <xar_engine/logging/logger.hpp>


namespace xar_engine::graphics::vulkan::native
{
    namespace
    {
        const auto logging_tag = "vulkan::PhysicalDevice";
    }

    struct VulkanPhysicalDevice::State
    {
    public:
        explicit State(const Parameters& parameters);
        ~State();

    public:
        VkPhysicalDevice vk_physical_device;
        VkPhysicalDeviceProperties vk_physical_device_properties;
        VkPhysicalDeviceFeatures vk_physical_device_features;
        std::vector<VkQueueFamilyProperties> vk_queue_family_properties_list;
        std::vector<VkExtensionProperties> vk_device_extension_properties_list;
    };

    VulkanPhysicalDevice::State::State(const Parameters& parameters)
        : vk_physical_device{parameters.vk_physical_device}
        , vk_physical_device_properties{}
        , vk_physical_device_features{}
        , vk_queue_family_properties_list{}
        , vk_device_extension_properties_list{}
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
            "device:'{}' api:{:#10x} driver:{:#10x}",
            vk_physical_device_properties.deviceName,
            vk_physical_device_properties.apiVersion,
            vk_physical_device_properties.driverVersion);

        auto vk_queue_family_counts = std::uint32_t{0};
        vkGetPhysicalDeviceQueueFamilyProperties(
            vk_physical_device,
            &vk_queue_family_counts,
            nullptr);

        vk_queue_family_properties_list.resize(vk_queue_family_counts);
        vkGetPhysicalDeviceQueueFamilyProperties(
            vk_physical_device,
            &vk_queue_family_counts,
            vk_queue_family_properties_list.data());

        /*auto queue_family_index = std::uint32_t{0};
        for (const auto& vk_queue_family_properties : vk_queue_family_properties_list)
        {
            VkBool32 present_support = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(
                vk_physical_device,
                queue_family_index,
                _vk_surface_khr,
                &present_support);
            ++queue_family_index;
            ++queue_family_index;

            XAR_LOG(
                logging::LogLevel::INFO,
                logging_tag,
                "count={} graphics={} present={}",
                vk_queue_family_properties.queueCount,
                (vk_queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT),
                present_support);
        }*/

        auto vk_device_extension_preoprties_count = std::uint32_t{0};
        vkEnumerateDeviceExtensionProperties(
            vk_physical_device,
            nullptr,
            &vk_device_extension_preoprties_count,
            nullptr);

        vk_device_extension_properties_list.resize(vk_device_extension_preoprties_count);
        vkEnumerateDeviceExtensionProperties(
            vk_physical_device,
            nullptr,
            &vk_device_extension_preoprties_count,
            vk_device_extension_properties_list.data());

        XAR_LOG(
            logging::LogLevel::INFO,
            logging_tag,
            "device extension list:");
        for (const auto& vk_device_extension: vk_device_extension_properties_list)
        {
            XAR_LOG(
                logging::LogLevel::INFO,
                logging_tag,
                "  {} {}",
                vk_device_extension.extensionName,
                vk_device_extension.specVersion);
        }
    }

    VulkanPhysicalDevice::State::~State() = default;


    VulkanPhysicalDevice::VulkanPhysicalDevice()
        : _state(nullptr)
    {
    }

    VulkanPhysicalDevice::VulkanPhysicalDevice(const Parameters& parameters)
        : _state(std::make_shared<State>(parameters))
    {
    }

    VkPhysicalDevice VulkanPhysicalDevice::get_vk_physical_device() const
    {
        return _state->vk_physical_device;
    }

    const VkPhysicalDeviceProperties& VulkanPhysicalDevice::get_vk_device_properties() const
    {
        return _state->vk_physical_device_properties;
    }

    const VkPhysicalDeviceFeatures& VulkanPhysicalDevice::get_vk_device_features() const
    {
        return _state->vk_physical_device_features;
    }

    const std::vector<VkQueueFamilyProperties>& VulkanPhysicalDevice::get_vk_queue_family_properties_list() const
    {
        return _state->vk_queue_family_properties_list;
    }

    const std::vector<VkExtensionProperties>& VulkanPhysicalDevice::get_vk_device_extension_properties_list() const
    {
        return _state->vk_device_extension_properties_list;
    }

    VkSampleCountFlagBits VulkanPhysicalDevice::get_vk_sample_count_flag_bits() const
    {
        const auto& physical_device_properties = get_vk_device_properties();

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

    VkFormatProperties VulkanPhysicalDevice::get_vk_format_properties(VkFormat vk_format) const
    {
        auto vk_format_properties = VkFormatProperties{};
        vkGetPhysicalDeviceFormatProperties(
            get_vk_physical_device(),
            vk_format,
            &vk_format_properties);

        return vk_format_properties;
    }

    VkFormat VulkanPhysicalDevice::find_supported_vk_format(
        const std::vector<VkFormat>& vk_format_candidate_list,
        VkImageTiling requested_vk_image_tiling,
        VkFormatFeatureFlags requested_vk_format_feature_flags) const
    {
        for (const auto vk_format: vk_format_candidate_list)
        {
            const auto vk_format_properties = get_vk_format_properties(vk_format);

            if (requested_vk_image_tiling == VK_IMAGE_TILING_LINEAR &&
                (vk_format_properties.linearTilingFeatures & requested_vk_format_feature_flags) == requested_vk_format_feature_flags)
            {
                return vk_format;
            }
            else if (requested_vk_image_tiling == VK_IMAGE_TILING_OPTIMAL &&
                     (vk_format_properties.optimalTilingFeatures & requested_vk_format_feature_flags) == requested_vk_format_feature_flags)
            {
                return vk_format;
            }
        }

        XAR_THROW(
            error::XarException,
            "No format with tiling={} and features={}",
            static_cast<std::uint32_t>(requested_vk_image_tiling),
            static_cast<std::uint32_t>(requested_vk_format_feature_flags));
    }

    std::uint32_t VulkanPhysicalDevice::find_memory_type(
        const uint32_t type_filter,
        const VkMemoryPropertyFlags requested_vk_memory_property_flags) const
    {
        auto vk_physical_device_memory_properties = VkPhysicalDeviceMemoryProperties{};
        vkGetPhysicalDeviceMemoryProperties(
            _state->vk_physical_device,
            &vk_physical_device_memory_properties);

        for (auto i = std::uint32_t{0}; i < vk_physical_device_memory_properties.memoryTypeCount; i++)
        {
            if ((type_filter & (std::uint32_t{1} << i)) &&
                (vk_physical_device_memory_properties.memoryTypes[i].propertyFlags & requested_vk_memory_property_flags) == requested_vk_memory_property_flags)
            {
                return i;
            }
        }

        XAR_THROW(error::XarException,
                  "No memory with filter={} and properties={}",
                  type_filter,
                  requested_vk_memory_property_flags);
    }

    VkSurfaceCapabilitiesKHR VulkanPhysicalDevice::get_vk_surface_capabilities_khr(VkSurfaceKHR vk_surface) const
    {
        auto vk_surface_capabilities_khr = VkSurfaceCapabilitiesKHR{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            _state->vk_physical_device,
            vk_surface,
            &vk_surface_capabilities_khr);

        return vk_surface_capabilities_khr;
    }

    std::vector<VkSurfaceFormatKHR> VulkanPhysicalDevice::get_vk_surface_format_khr_list(VkSurfaceKHR vk_surface) const
    {
        auto format_counts = std::uint32_t{0};
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            _state->vk_physical_device,
            vk_surface,
            &format_counts,
            nullptr);

        auto vk_surface_format_khr_list = std::vector<VkSurfaceFormatKHR>(format_counts);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            _state->vk_physical_device,
            vk_surface,
            &format_counts,
            vk_surface_format_khr_list.data());

        return vk_surface_format_khr_list;
    }

    std::vector<VkPresentModeKHR> VulkanPhysicalDevice::get_vk_present_mode_khr_list(VkSurfaceKHR vk_surface) const
    {
        auto present_mode_counts = std::uint32_t{0};
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            _state->vk_physical_device,
            vk_surface,
            &present_mode_counts,
            nullptr);

        auto vk_present_mode_list = std::vector<VkPresentModeKHR>(present_mode_counts);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            _state->vk_physical_device,
            vk_surface,
            &present_mode_counts,
            vk_present_mode_list.data());

        return vk_present_mode_list;
    }
}
