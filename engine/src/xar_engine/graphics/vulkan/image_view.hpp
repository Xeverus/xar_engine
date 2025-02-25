#pragma once

#include <memory>

#include <volk.h>

#include <xar_engine/math/vector.hpp>

#include <xar_engine/graphics/vulkan/device.hpp>
#include <xar_engine/graphics/vulkan/image.hpp>


namespace xar_engine::graphics::vulkan
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
        Device device;
        VkImage vk_image;

        VkFormat format;
        VkImageAspectFlags aspect_mask;
        std::uint32_t mip_levels;
    };
}