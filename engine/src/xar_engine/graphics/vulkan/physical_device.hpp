#pragma once

#include <memory>
#include <vector>

#include <volk.h>


namespace xar_engine::graphics::vulkan
{
    class VulkanPhysicalDevice
    {
    public:
        struct Parameters;

    public:
        VulkanPhysicalDevice();
        explicit VulkanPhysicalDevice(const Parameters& parameters);


        [[nodiscard]]
        VkPhysicalDevice get_vk_physical_device() const;


        [[nodiscard]]
        const VkPhysicalDeviceProperties& get_vk_device_properties() const;

        [[nodiscard]]
        const VkPhysicalDeviceFeatures& get_vk_device_features() const;

        [[nodiscard]]
        const std::vector<VkQueueFamilyProperties>& get_vk_queue_family_properties_list() const;

        [[nodiscard]]
        const std::vector<VkExtensionProperties>& get_vk_device_extension_properties_list() const;


        [[nodiscard]]
        VkSampleCountFlagBits get_vk_sample_count_flag_bits() const;

        [[nodiscard]]
        VkFormatProperties get_vk_format_properties(VkFormat vk_format) const;

        [[nodiscard]]
        VkFormat find_supported_vk_format(
            const std::vector<VkFormat>& vk_format_candidate_list,
            VkImageTiling requested_vk_image_tiling,
            VkFormatFeatureFlags requested_vk_format_feature_flags) const;

        [[nodiscard]]
        std::uint32_t find_memory_type(
            uint32_t type_filter,
            VkMemoryPropertyFlags requested_vk_memory_property_flags) const;


        [[nodiscard]]
        VkSurfaceCapabilitiesKHR get_vk_surface_capabilities_khr(VkSurfaceKHR vk_surface) const;

        [[nodiscard]]
        std::vector<VkSurfaceFormatKHR> get_vk_surface_format_khr_list(VkSurfaceKHR vk_surface) const;

        [[nodiscard]]
        std::vector<VkPresentModeKHR> get_vk_present_mode_khr_list(VkSurfaceKHR vk_surface) const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct VulkanPhysicalDevice::Parameters
    {
        VkPhysicalDevice vk_physical_device;
    };
}