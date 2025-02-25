#include <xar_engine/graphics/vulkan/image_view.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    struct VulkanImageView::State
    {
    public:
        explicit State(const Parameters& parameters);

        ~State();

    public:
        Device device;
        VkImage vk_image;

        VkImageView vk_image_view;
    };

    VulkanImageView::State::State(const Parameters& parameters)
        : device(parameters.device)
        , vk_image(parameters.vk_image)
        , vk_image_view(nullptr)
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
            parameters.device.get_native(),
            &image_view_create,
            nullptr,
            &vk_image_view);
        XAR_THROW_IF(
            image_view_result != VK_SUCCESS,
            error::XarException,
            "Failed to create image view");

    }

    VulkanImageView::State::~State()
    {
        vkDestroyImageView(
            device.get_native(),
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
