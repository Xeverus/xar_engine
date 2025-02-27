#include <xar_engine/graphics/vulkan/vulkan_image_view.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    struct VulkanImageView::State
    {
    public:
        explicit State(const Parameters& parameters);

        ~State();

    public:
        VulkanDevice vulkan_device;
        VkImage vk_image;

        VkImageView vk_image_view;
    };

    VulkanImageView::State::State(const Parameters& parameters)
        : vulkan_device(parameters.vulkan_device)
        , vk_image(parameters.vk_image)
        , vk_image_view(nullptr)
    {
        auto vk_image_view_create_info = VkImageViewCreateInfo{};
        vk_image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        vk_image_view_create_info.image = parameters.vk_image;
        vk_image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        vk_image_view_create_info.format = parameters.vk_format;
        vk_image_view_create_info.subresourceRange.aspectMask = parameters.vk_image_aspect_flags;
        vk_image_view_create_info.subresourceRange.baseMipLevel = 0;
        vk_image_view_create_info.subresourceRange.levelCount = parameters.mip_levels;
        vk_image_view_create_info.subresourceRange.baseArrayLayer = 0;
        vk_image_view_create_info.subresourceRange.layerCount = 1;

        const auto vk_create_image_view_result = vkCreateImageView(
            parameters.vulkan_device.get_native(),
            &vk_image_view_create_info,
            nullptr,
            &vk_image_view);
        XAR_THROW_IF(
            vk_create_image_view_result != VK_SUCCESS,
            error::XarException,
            "vkCreateImageView failed");

    }

    VulkanImageView::State::~State()
    {
        vkDestroyImageView(
            vulkan_device.get_native(),
            vk_image_view,
            nullptr);
    }


    VulkanImageView::VulkanImageView()
        : _state(nullptr)
    {
    }

    VulkanImageView::VulkanImageView(const VulkanImageView::Parameters& parameters)
        : _state(std::make_shared<State>(parameters))
    {
    }

    VulkanImageView::~VulkanImageView() = default;

    VkImageView VulkanImageView::get_native() const
    {
        return _state->vk_image_view;
    }
}
