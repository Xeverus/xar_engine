#pragma once

#include <memory>

#include <volk.h>

#include <xar_engine/math/vector.hpp>

#include <xar_engine/graphics/vulkan/vulkan_device.hpp>


namespace xar_engine::graphics::vulkan
{
    class VulkanImage
    {
    public:
        struct Parameters;

    public:
        VulkanImage();
        explicit VulkanImage(const Parameters& parameters);

        ~VulkanImage();

        void transition_layout(
            VkCommandBuffer vk_command_buffer,
            VkImageLayout new_vk_image_layout);

        void generate_mipmaps(VkCommandBuffer vk_command_buffer);

        [[nodiscard]]
        VkImage get_native() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct VulkanImage::Parameters
    {
        VulkanDevice vulkan_device;

        math::Vector3i32 dimension;
        VkFormat vk_format;
        VkImageTiling vk_image_tiling;
        VkImageUsageFlags vk_image_usage_flags;
        VkMemoryPropertyFlags vm_memory_property_flags;
        std::uint32_t mip_levels;
        VkSampleCountFlagBits vk_sample_count_flag_bits;
    };
}