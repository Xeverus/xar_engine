#include <xar_engine/graphics/vulkan/vulkan_physical_device_list.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    VulkanPhysicalDeviceList::VulkanPhysicalDeviceList(const Parameters& parameters)
    {
        auto physical_devices_count = std::uint32_t{0};
        vkEnumeratePhysicalDevices(
            parameters.vk_instance,
            &physical_devices_count,
            nullptr);

        _vk_physical_devices.resize(physical_devices_count);
        vkEnumeratePhysicalDevices(
            parameters.vk_instance,
            &physical_devices_count,
            _vk_physical_devices.data());

        _vk_physical_device_properties.resize(physical_devices_count);
        _vk_physical_device_features.resize(physical_devices_count);
        _vk_queue_family_properties.resize(physical_devices_count);
        _vk_extension_properties.resize(physical_devices_count);
        for (auto i = std::size_t{0}; i < physical_devices_count; ++i)
        {
            vkGetPhysicalDeviceProperties(
                _vk_physical_devices[i],
                &_vk_physical_device_properties[i]);

            vkGetPhysicalDeviceFeatures(
                _vk_physical_devices[i],
                &_vk_physical_device_features[i]);

            /*XAR_LOG(
                logging::LogLevel::DEBUG,
                *_logger,
                tag,
                "{} {} {}",
                properties.deviceName,
                properties.apiVersion,
                properties.driverVersion);*/

            auto queue_families_count = std::uint32_t{0};
            vkGetPhysicalDeviceQueueFamilyProperties(
                _vk_physical_devices[i],
                &queue_families_count,
                nullptr);
            _vk_queue_family_properties[i].resize(queue_families_count);
            vkGetPhysicalDeviceQueueFamilyProperties(
                _vk_physical_devices[i],
                &queue_families_count,
                _vk_queue_family_properties[i].data());

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

            auto device_extensions_count = std::uint32_t{0};
            vkEnumerateDeviceExtensionProperties(
                _vk_physical_devices[i],
                nullptr,
                &device_extensions_count,
                nullptr);
            _vk_extension_properties[i].resize(device_extensions_count);
            vkEnumerateDeviceExtensionProperties(
                _vk_physical_devices[i],
                nullptr,
                &device_extensions_count,
                _vk_extension_properties[i].data());

            /*for (const auto& device_extension: device_extensions)
            {
                XAR_LOG(
                    logging::LogLevel::INFO,
                    *_logger,
                    tag,
                    "{} {}",
                    device_extension.extensionName,
                    device_extension.specVersion);
            }*/
        }
    }

    std::size_t VulkanPhysicalDeviceList::get_count() const
    {
        return _vk_physical_devices.size();
    }

    VkPhysicalDevice VulkanPhysicalDeviceList::get_native(const std::uint32_t index) const
    {
        return _vk_physical_devices[index];
    }

    const VkPhysicalDeviceProperties& VulkanPhysicalDeviceList::get_device_properties(std::uint32_t index) const
    {
        return _vk_physical_device_properties[index];
    }

    const VkPhysicalDeviceFeatures& VulkanPhysicalDeviceList::get_device_features(std::uint32_t index) const
    {
        return _vk_physical_device_features[index];
    }

    const std::vector<VkQueueFamilyProperties>& VulkanPhysicalDeviceList::get_queue_family_properties(std::uint32_t index) const
    {
        return _vk_queue_family_properties[index];
    }

    const std::vector<VkExtensionProperties>& VulkanPhysicalDeviceList::get_extension_properties(std::uint32_t index) const
    {
        return _vk_extension_properties[index];
    }

    VkSampleCountFlagBits VulkanPhysicalDeviceList::get_max_sample_count(const std::uint32_t index) const
    {
        const auto& physicalDeviceProperties = get_device_properties(index);

        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
                                    physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT)
        {
            return VK_SAMPLE_COUNT_64_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_32_BIT)
        {
            return VK_SAMPLE_COUNT_32_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_16_BIT)
        {
            return VK_SAMPLE_COUNT_16_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_8_BIT)
        {
            return VK_SAMPLE_COUNT_8_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_4_BIT)
        {
            return VK_SAMPLE_COUNT_4_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_2_BIT)
        {
            return VK_SAMPLE_COUNT_2_BIT;
        }

        return VK_SAMPLE_COUNT_1_BIT;
    }

    VkFormat VulkanPhysicalDeviceList::findSupportedFormat(
        const std::uint32_t index,
        const std::vector<VkFormat>& candidates,
        const VkImageTiling tiling,
        const VkFormatFeatureFlags features) const
    {
        for (VkFormat format: candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(
                get_native(index),
                format,
                &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }

        XAR_THROW(
            error::XarException,
            "failed to find supported format!");
    }

    VkSurfaceCapabilitiesKHR VulkanPhysicalDeviceList::get_surface_capabilities(
        const std::uint32_t index,
        VkSurfaceKHR vk_surface) const
    {
        auto capabilities = VkSurfaceCapabilitiesKHR{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            _vk_physical_devices[index],
            vk_surface,
            &capabilities);

        return capabilities;
    }

    std::vector<VkSurfaceFormatKHR> VulkanPhysicalDeviceList::get_surface_formats(
        const std::uint32_t index,
        VkSurfaceKHR vk_surface) const
    {
        auto formats_count = std::uint32_t{0};
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            _vk_physical_devices[0],
            vk_surface,
            &formats_count,
            nullptr);

        auto formats = std::vector<VkSurfaceFormatKHR>(formats_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            _vk_physical_devices[0],
            vk_surface,
            &formats_count,
            formats.data());

        return formats;
    }

    std::vector<VkPresentModeKHR> VulkanPhysicalDeviceList::get_present_modes(
        const std::uint32_t index,
        VkSurfaceKHR vk_surface) const
    {
        auto present_modes_count = std::uint32_t{0};
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            _vk_physical_devices[0],
            vk_surface,
            &present_modes_count,
            nullptr);

        auto present_modes = std::vector<VkPresentModeKHR>(present_modes_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            _vk_physical_devices[0],
            vk_surface,
            &present_modes_count,
            present_modes.data());

        return present_modes;
    }
}
