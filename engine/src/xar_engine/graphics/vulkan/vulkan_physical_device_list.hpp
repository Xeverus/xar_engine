#pragma once

#include <cstdint>
#include <vector>

#include <volk.h>


namespace xar_engine::graphics::vulkan
{
    class VulkanPhysicalDeviceList
    {
    public:
        struct Parameters;

    public:
        explicit VulkanPhysicalDeviceList(const Parameters& parameters);


        [[nodiscard]]
        std::size_t get_count() const;

        [[nodiscard]]
        VkPhysicalDevice get_native(std::uint32_t index) const;

        [[nodiscard]]
        const VkPhysicalDeviceProperties& get_device_properties(std::uint32_t index) const;

        [[nodiscard]]
        const VkPhysicalDeviceFeatures& get_device_features(std::uint32_t index) const;

        [[nodiscard]]
        const std::vector<VkQueueFamilyProperties>& get_queue_family_properties(std::uint32_t index) const;

        [[nodiscard]]
        const std::vector<VkExtensionProperties>& get_extension_properties(std::uint32_t index) const;


        [[nodiscard]]
        VkSurfaceCapabilitiesKHR get_surface_capabilities(std::uint32_t index, VkSurfaceKHR vk_surface) const;

        [[nodiscard]]
        std::vector<VkSurfaceFormatKHR> get_surface_formats(std::uint32_t index, VkSurfaceKHR vk_surface) const;

        [[nodiscard]]
        std::vector<VkPresentModeKHR> get_present_modes(std::uint32_t index, VkSurfaceKHR vk_surface) const;

    private:
        std::vector<VkPhysicalDevice> _vk_physical_devices;
        std::vector<VkPhysicalDeviceProperties> _vk_physical_device_properties;
        std::vector<VkPhysicalDeviceFeatures> _vk_physical_device_features;
        std::vector<std::vector<VkQueueFamilyProperties>> _vk_queue_family_properties;
        std::vector<std::vector<VkExtensionProperties>> _vk_extension_properties;
    };

    struct VulkanPhysicalDeviceList::Parameters
    {
        VkInstance vk_instance;
    };
}