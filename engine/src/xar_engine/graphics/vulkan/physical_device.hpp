#pragma once

#include <memory>
#include <vector>

#include <volk.h>


namespace xar_engine::graphics::vulkan
{
    class PhysicalDevice
    {
    public:
        struct Parameters;

    public:
        PhysicalDevice();
        explicit PhysicalDevice(const Parameters& parameters);


        [[nodiscard]]
        VkPhysicalDevice get_native() const;


        [[nodiscard]]
        const VkPhysicalDeviceProperties& get_device_properties() const;

        [[nodiscard]]
        const VkPhysicalDeviceFeatures& get_device_features() const;

        [[nodiscard]]
        const std::vector<VkQueueFamilyProperties>& get_queue_family_properties() const;

        [[nodiscard]]
        const std::vector<VkExtensionProperties>& get_extension_properties() const;


        [[nodiscard]]
        VkSampleCountFlagBits get_max_sample_count() const;

        [[nodiscard]]
        VkFormatProperties get_format_properties(VkFormat vk_format) const;

        [[nodiscard]]
        VkFormat find_supported_format(
            const std::vector<VkFormat>& candidate_list,
            VkImageTiling vk_tiling,
            VkFormatFeatureFlags vk_feature_flags) const;

        [[nodiscard]]
        std::uint32_t find_memory_type(
            uint32_t type_filter,
            VkMemoryPropertyFlags properties) const;


        [[nodiscard]]
        VkSurfaceCapabilitiesKHR get_surface_capabilities(VkSurfaceKHR vk_surface) const;

        [[nodiscard]]
        std::vector<VkSurfaceFormatKHR> get_surface_formats(VkSurfaceKHR vk_surface) const;

        [[nodiscard]]
        std::vector<VkPresentModeKHR> get_present_modes(VkSurfaceKHR vk_surface) const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct PhysicalDevice::Parameters
    {
        VkPhysicalDevice vk_physical_device;
    };
}