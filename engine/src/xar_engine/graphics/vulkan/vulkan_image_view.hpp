#pragma once

#include <volk.h>

#include <xar_engine/math/vector.hpp>


namespace xar_engine::graphics::vulkan
{
    class VulkanImageView
    {
    public:
        struct Parameters;

    public:
        explicit VulkanImageView(const Parameters& parameters);
        ~VulkanImageView();

        [[nodiscard]]
        VkImageView get_native() const;

    private:
        VkDevice _vk_device;
        VkImageView _vk_image_view;
    };

    struct VulkanImageView::Parameters
    {
        VkDevice vk_device;
        VkImage vk_image;

        VkFormat format;
        VkImageAspectFlags aspect_mask;
        std::uint32_t mip_levels;
    };
}