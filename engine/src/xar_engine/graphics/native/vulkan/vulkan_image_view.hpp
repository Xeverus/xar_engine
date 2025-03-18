#pragma once

#include <memory>

#include <volk.h>

#include <xar_engine/math/vector.hpp>

#include <xar_engine/graphics/native/vulkan/vulkan_device.hpp>
#include <xar_engine/graphics/native/vulkan/vulkan_image.hpp>


namespace xar_engine::graphics::native::vulkan
{
    class VulkanImageView
    {
    public:
        struct Parameters;

    public:
        VulkanImageView();
        explicit VulkanImageView(const Parameters& parameters);

        ~VulkanImageView();


        [[nodiscard]]
        VkImageView get_native() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct VulkanImageView::Parameters
    {
        VulkanDevice vulkan_device;
        VkImage vk_image;

        VkFormat vk_format;
        VkImageAspectFlags vk_image_aspect_flags;
        std::uint32_t mip_levels;
    };
}