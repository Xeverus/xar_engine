#include <xar_engine/graphics/vulkan/vulkan_image_view.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    VulkanImageView::VulkanImageView(const VulkanImageView::Parameters& parameters)
        : _vk_device(parameters.vk_device)
        , _vk_image_view(nullptr)
    {
        auto image_view_create = VkImageViewCreateInfo{};
        image_view_create.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create.image = parameters.vk_image;
        image_view_create.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create.format = parameters.format;
        image_view_create.subresourceRange.aspectMask = parameters.aspect_mask;
        image_view_create.subresourceRange.baseMipLevel = 0;
        image_view_create.subresourceRange.levelCount = parameters.mip_levels;
        image_view_create.subresourceRange.baseArrayLayer = 0;
        image_view_create.subresourceRange.layerCount = 1;

        const auto image_view_result = vkCreateImageView(
            _vk_device,
            &image_view_create,
            nullptr,
            &_vk_image_view);
        XAR_THROW_IF(
            image_view_result != VK_SUCCESS,
            error::XarException,
            "Failed to create image view");

    }

    VulkanImageView::~VulkanImageView()
    {
        vkDestroyImageView(
            _vk_device,
            _vk_image_view,
            nullptr);
    }

    VkImageView VulkanImageView::get_native() const
    {
        return _vk_image_view;
    }
}
