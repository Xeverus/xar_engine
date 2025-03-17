#include <xar_engine/graphics/native/vulkan/vulkan_image.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::native::vulkan
{
    namespace
    {
        bool hasStencilComponent(const VkFormat format)
        {
            return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
        }
    }


    struct VulkanImage::State
    {
    public:
        explicit State(const Parameters& parameters);

        ~State();

    public:
        VulkanDevice device;

        VkImage vk_image;
        VkDeviceMemory vk_device_memory;

        VkFormat vk_format;
        VkImageLayout vk_image_layout;
        math::Vector3i32 dimension;
        std::uint32_t mip_levels;
    };

    VulkanImage::State::State(const Parameters& parameters)
        : device(parameters.vulkan_device)
        , vk_image{nullptr}
        , vk_device_memory{nullptr}
        , vk_format{parameters.vk_format}
        , vk_image_layout{VK_IMAGE_LAYOUT_UNDEFINED}
        , dimension{parameters.dimension}
        , mip_levels{parameters.mip_levels}
    {
        auto vk_image_create_info = VkImageCreateInfo{};
        vk_image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        vk_image_create_info.imageType = VK_IMAGE_TYPE_2D;
        vk_image_create_info.extent.width = parameters.dimension.x;
        vk_image_create_info.extent.height = parameters.dimension.y;
        vk_image_create_info.extent.depth = parameters.dimension.z;
        vk_image_create_info.mipLevels = parameters.mip_levels;
        vk_image_create_info.arrayLayers = 1;
        vk_image_create_info.format = parameters.vk_format;
        vk_image_create_info.tiling = parameters.vk_image_tiling;
        vk_image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        vk_image_create_info.usage = parameters.vk_image_usage_flags;
        vk_image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        vk_image_create_info.samples = parameters.vk_sample_count_flag_bits;
        vk_image_create_info.flags = 0;

        const auto vk_create_image_result = vkCreateImage(
            parameters.vulkan_device.get_native(),
            &vk_image_create_info,
            nullptr,
            &vk_image);
        XAR_THROW_IF(
            vk_create_image_result != VK_SUCCESS,
            error::XarException,
            "vkCreateImage failed");

        auto vk_memory_requirements = VkMemoryRequirements{};
        vkGetImageMemoryRequirements(
            parameters.vulkan_device.get_native(),
            vk_image,
            &vk_memory_requirements);

        auto vk_memory_allocate_info = VkMemoryAllocateInfo{};
        vk_memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        vk_memory_allocate_info.allocationSize = vk_memory_requirements.size;
        vk_memory_allocate_info.memoryTypeIndex = parameters.vulkan_device.get_physical_device().find_memory_type(
            vk_memory_requirements.memoryTypeBits,
            parameters.vm_memory_property_flags);

        const auto vk_allocate_memory_result = vkAllocateMemory(
            parameters.vulkan_device.get_native(),
            &vk_memory_allocate_info,
            nullptr,
            &vk_device_memory);
        XAR_THROW_IF(
            vk_allocate_memory_result != VK_SUCCESS,
            error::XarException,
            "vkAllocateMemory failed");

        vkBindImageMemory(
            parameters.vulkan_device.get_native(),
            vk_image,
            vk_device_memory,
            0);
    }

    VulkanImage::State::~State()
    {
        vkDestroyImage(
            device.get_native(),
            vk_image,
            nullptr);

        vkFreeMemory(
            device.get_native(),
            vk_device_memory,
            nullptr);
    }


    VulkanImage::VulkanImage()
        : _state(nullptr)
    {
    }

    VulkanImage::VulkanImage(const VulkanImage::Parameters& parameters)
        : _state(std::make_shared<State>(parameters))
    {
    }

    VulkanImage::~VulkanImage() = default;

    void VulkanImage::transition_layout(
        VkCommandBuffer vk_command_buffer,
        VkImageLayout new_vk_image_layout)
    {
        auto vk_image_memory_barrier = VkImageMemoryBarrier{};
        vk_image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        vk_image_memory_barrier.oldLayout = _state->vk_image_layout;
        vk_image_memory_barrier.newLayout = new_vk_image_layout;
        vk_image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vk_image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vk_image_memory_barrier.image = _state->vk_image;
        vk_image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        vk_image_memory_barrier.subresourceRange.baseMipLevel = 0;
        vk_image_memory_barrier.subresourceRange.levelCount = _state->mip_levels;
        vk_image_memory_barrier.subresourceRange.baseArrayLayer = 0;
        vk_image_memory_barrier.subresourceRange.layerCount = 1;
        vk_image_memory_barrier.srcAccessMask = 0;
        vk_image_memory_barrier.dstAccessMask = 0;

        auto vk_source_pipeline_stage_flags = VkPipelineStageFlags{};
        auto vk_destination_pipeline_stage_flags = VkPipelineStageFlags{};

        if (new_vk_image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            vk_image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (hasStencilComponent(_state->vk_format))
            {
                vk_image_memory_barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        }
        else
        {
            vk_image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        if (_state->vk_image_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
            new_vk_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            vk_image_memory_barrier.srcAccessMask = 0;
            vk_image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            vk_source_pipeline_stage_flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            vk_destination_pipeline_stage_flags = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (_state->vk_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                 new_vk_image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            vk_image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            vk_image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vk_source_pipeline_stage_flags = VK_PIPELINE_STAGE_TRANSFER_BIT;
            vk_destination_pipeline_stage_flags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (_state->vk_image_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
                 new_vk_image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            vk_image_memory_barrier.srcAccessMask = 0;
            vk_image_memory_barrier.dstAccessMask =
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            vk_source_pipeline_stage_flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            vk_destination_pipeline_stage_flags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else
        {
            XAR_THROW(
                error::XarException,
                "transition from {} to {} is not supported",
                static_cast<std::uint32_t>(_state->vk_image_layout),
                static_cast<std::uint32_t>(new_vk_image_layout));
        }

        _state->vk_image_layout = new_vk_image_layout;

        vkCmdPipelineBarrier(
            vk_command_buffer,
            vk_source_pipeline_stage_flags,
            vk_destination_pipeline_stage_flags,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &vk_image_memory_barrier);
    }

    void VulkanImage::generate_mipmaps(VkCommandBuffer vk_command_buffer)
    {
        const auto vk_format_properties = _state->device.get_physical_device().get_vk_format_properties(_state->vk_format);
        XAR_THROW_IF(
            !(vk_format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT),
            error::XarException,
            "Image format {} does not support linear blitting",
            static_cast<std::int32_t>(_state->vk_format));

        auto vk_image_memory_barrier = VkImageMemoryBarrier{};
        vk_image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        vk_image_memory_barrier.image = _state->vk_image;
        vk_image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vk_image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vk_image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        vk_image_memory_barrier.subresourceRange.baseArrayLayer = 0;
        vk_image_memory_barrier.subresourceRange.layerCount = 1;
        vk_image_memory_barrier.subresourceRange.levelCount = 1;

        auto mip_width = _state->dimension.x;
        auto mip_height = _state->dimension.y;

        for (auto mip_level = std::uint32_t{1}; mip_level < _state->mip_levels; mip_level++)
        {
            vk_image_memory_barrier.subresourceRange.baseMipLevel = mip_level - 1;
            vk_image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            vk_image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            vk_image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            vk_image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(
                vk_command_buffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                0,
                0,
                nullptr,
                0,
                nullptr,
                1,
                &vk_image_memory_barrier);

            auto vk_image_blit = VkImageBlit{};
            vk_image_blit.srcOffsets[0] = {0, 0, 0};
            vk_image_blit.srcOffsets[1] = {mip_width, mip_height, 1};
            vk_image_blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            vk_image_blit.srcSubresource.mipLevel = mip_level - 1;
            vk_image_blit.srcSubresource.baseArrayLayer = 0;
            vk_image_blit.srcSubresource.layerCount = 1;
            vk_image_blit.dstOffsets[0] = {0, 0, 0};
            vk_image_blit.dstOffsets[1] = {mip_width > 1 ? mip_width / 2 : 1, mip_height > 1 ? mip_height / 2 : 1, 1};
            vk_image_blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            vk_image_blit.dstSubresource.mipLevel = mip_level;
            vk_image_blit.dstSubresource.baseArrayLayer = 0;
            vk_image_blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(
                vk_command_buffer,
                _state->vk_image,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                _state->vk_image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &vk_image_blit,
                VK_FILTER_LINEAR);

            vk_image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            vk_image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            vk_image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            vk_image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(
                vk_command_buffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                0,
                0,
                nullptr,
                0,
                nullptr,
                1,
                &vk_image_memory_barrier);

            if (mip_width > 1)
            {
                mip_width /= 2;
            }
            if (mip_height > 1)
            {
                mip_height /= 2;
            }
        }

        vk_image_memory_barrier.subresourceRange.baseMipLevel = _state->mip_levels - 1;
        vk_image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        vk_image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        vk_image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        vk_image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            vk_command_buffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &vk_image_memory_barrier);
    }

    const VulkanDevice& VulkanImage::get_device() const
    {
        return _state->device;
    }

    VulkanDevice& VulkanImage::get_device()
    {
        return _state->device;
    }

    VkImage VulkanImage::get_native() const
    {
        return _state->vk_image;
    }

    math::Vector3i32 VulkanImage::get_dimension() const
    {
        return _state->dimension;
    }

    VkFormat VulkanImage::get_vk_format() const
    {
        return _state->vk_format;
    }
}
