#pragma once

#include <volk.h>

#include <xar_engine/math/vector.hpp>


namespace xar_engine::graphics::vulkan
{
    class VulkanImage
    {
    public:
        struct Parameters;

    public:
        explicit VulkanImage(const Parameters& parameters);
        ~VulkanImage();

        [[nodiscard]]
        VkImage get_native() const;

    private:
        VkDevice _vk_device;
        VkImage _vk_image;
        VkDeviceMemory _vk_image_memory;
    };

    struct VulkanImage::Parameters
    {
        VkDevice vk_device;
        VkPhysicalDevice vk_physical_device;

        math::Vector3i32 dimension;
        VkFormat format;
        VkImageTiling tiling;
        VkImageUsageFlags usage;
        VkMemoryPropertyFlags properties;
        std::uint32_t mip_levels;
        VkSampleCountFlagBits msaa_samples;
    };
}