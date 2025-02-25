#pragma once

#include <memory>

#include <volk.h>

#include <xar_engine/math/vector.hpp>

#include <xar_engine/graphics/vulkan/device.hpp>


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
        Device device;

        math::Vector3i32 dimension;
        VkFormat format;
        VkImageTiling tiling;
        VkImageUsageFlags usage;
        VkMemoryPropertyFlags properties;
        std::uint32_t mip_levels;
        VkSampleCountFlagBits msaa_samples;
    };
}