#include <xar_engine/graphics/backend/component/vulkan/vulkan_image_component.hpp>

#include <xar_engine/graphics/backend/vulkan/vulkan_type_converters.hpp>


namespace xar_engine::graphics::backend::component::vulkan
{
    api::ImageReference IVulkanImageComponent::make_image(const MakeImageParameters& parameters)
    {
        auto vk_image_usage = 0;
        switch (parameters.image_type)
        {
            case api::EImageType::COLOR_ATTACHMENT:
            {
                vk_image_usage =
                    VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                break;
            }
            case api::EImageType::DEPTH_ATTACHMENT:
            {
                vk_image_usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                break;
            }
            case api::EImageType::TEXTURE:
            {
                vk_image_usage =
                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                    VK_IMAGE_USAGE_SAMPLED_BIT;
                break;
            }
        }

        return get_state()._vulkan_resource_storage.add(
            native::vulkan::VulkanImage{
                {
                    get_state()._vulkan_device,
                    parameters.dimension,
                    ::xar_engine::graphics::backend::vulkan::to_vk_format(parameters.image_format),
                    VK_IMAGE_TILING_OPTIMAL,
                    static_cast<VkImageUsageFlagBits>(vk_image_usage),
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    parameters.mip_levels,
                    static_cast<VkSampleCountFlagBits>(parameters.sample_count),
                }});
    }

    api::ImageViewReference IVulkanImageComponent::make_image_view(const MakeImageViewParameters& parameters)
    {
        const auto& vulkan_image = get_state()._vulkan_resource_storage.get(parameters.image);

        return get_state()._vulkan_resource_storage.add(
            native::vulkan::VulkanImageView{
                {
                    get_state()._vulkan_device,
                    vulkan_image.get_native(),
                    vulkan_image.get_vk_format(),
                    ::xar_engine::graphics::backend::vulkan::to_vk_image_aspec(parameters.image_aspect),
                    parameters.mip_levels
                }});
    }

    api::SamplerReference IVulkanImageComponent::make_sampler(const MakeSamplerParameters& parameters)
    {
        return get_state()._vulkan_resource_storage.add(
            native::vulkan::VulkanSampler{
                {
                    get_state()._vulkan_device,
                    get_state()._vulkan_device.get_native_physical_device().get_vk_device_properties().limits.maxSamplerAnisotropy,
                    parameters.mip_levels,
                }
            });
    }

    void IVulkanImageComponent::generate_image_mip_maps(const GenerateImageMipMapsParameters& parameters)
    {
        get_state()._vulkan_resource_storage.get(parameters.image).generate_mipmaps(get_state()._vulkan_resource_storage.get(parameters.command_buffer).get_native());
    }

    void IVulkanImageComponent::transit_image_layout(const TransitImageLayoutParameters& parameters)
    {
        VkImageLayout new_vk_image_layout = {};
        switch (parameters.new_image_layout)
        {
            case api::EImageLayout::TRANSFER_DESTINATION:
            {
                new_vk_image_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                break;
            }
            case api::EImageLayout::DEPTH_STENCIL_ATTACHMENT:
            {
                new_vk_image_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                break;
            }
            default:
            {
                new_vk_image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
                break;
            }
        }

        get_state()._vulkan_resource_storage.get(parameters.image).transition_layout(
            get_state()._vulkan_resource_storage.get(parameters.command_buffer).get_native(),
            new_vk_image_layout);
    }
}