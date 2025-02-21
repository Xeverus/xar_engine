#include <xar_engine/graphics/vulkan/vulkan_image.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    namespace
    {
        std::uint32_t findMemoryType(
            VkPhysicalDevice vk_physical_device,
            uint32_t typeFilter,
            VkMemoryPropertyFlags properties)
        {
            VkPhysicalDeviceMemoryProperties memProperties;
            vkGetPhysicalDeviceMemoryProperties(
                vk_physical_device,
                &memProperties);

            for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
            {
                if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
                {
                    return i;
                }
            }

            XAR_THROW(error::XarException,
                      "Failed to find suitable memory type");
        };

        bool hasStencilComponent(const VkFormat format)
        {
            return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
        }
    }

    VulkanImage::VulkanImage(const VulkanImage::Parameters& parameters)
        : _vk_device(parameters.vk_device)
        , _vk_image(nullptr)
        , _vk_image_memory(nullptr)
        , _vk_image_format(parameters.format)
        , _vk_image_layout(VK_IMAGE_LAYOUT_UNDEFINED)
        , _mip_levels(parameters.mip_levels)
    {
        auto image_create_info = VkImageCreateInfo{};
        image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_create_info.imageType = VK_IMAGE_TYPE_2D;
        image_create_info.extent.width = parameters.dimension.x;
        image_create_info.extent.height = parameters.dimension.y;
        image_create_info.extent.depth = parameters.dimension.z;
        image_create_info.mipLevels = parameters.mip_levels;
        image_create_info.arrayLayers = 1;
        image_create_info.format = parameters.format;
        image_create_info.tiling = parameters.tiling;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_create_info.usage = parameters.usage;
        image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_create_info.samples = parameters.msaa_samples;
        image_create_info.flags = 0;

        const auto create_image_result = vkCreateImage(
            _vk_device,
            &image_create_info,
            nullptr,
            &_vk_image);
        XAR_THROW_IF(
            create_image_result != VK_SUCCESS,
            error::XarException,
            "Failed to create image");

        auto memory_requirements = VkMemoryRequirements{};
        vkGetImageMemoryRequirements(
            _vk_device,
            _vk_image,
            &memory_requirements);

        auto memory_allocation = VkMemoryAllocateInfo{};
        memory_allocation.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memory_allocation.allocationSize = memory_requirements.size;
        memory_allocation.memoryTypeIndex = findMemoryType(
            parameters.vk_physical_device,
            memory_requirements.memoryTypeBits,
            parameters.properties);

        const auto memory_allocation_result = vkAllocateMemory(
            _vk_device,
            &memory_allocation,
            nullptr,
            &_vk_image_memory);
        XAR_THROW_IF(
            memory_allocation_result != VK_SUCCESS,
            error::XarException,
            "Failed to allocate image");

        vkBindImageMemory(
            _vk_device,
            _vk_image,
            _vk_image_memory,
            0);
    }

    VulkanImage::~VulkanImage()
    {
        vkDestroyImage(
            _vk_device,
            _vk_image,
            nullptr);
        vkFreeMemory(
            _vk_device,
            _vk_image_memory,
            nullptr);
    }

    void VulkanImage::transition_layout(
        VkCommandBuffer vk_command_buffer,
        VkImageLayout new_vk_image_layout)
    {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = _vk_image_layout;
        barrier.newLayout = new_vk_image_layout;

        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        barrier.image = _vk_image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = _mip_levels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        barrier.srcAccessMask = 0; // TODO
        barrier.dstAccessMask = 0; // TODO

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (new_vk_image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (hasStencilComponent(_vk_image_format))
            {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        }
        else
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        if (_vk_image_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
            new_vk_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (_vk_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                 new_vk_image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (_vk_image_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
                 new_vk_image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask =
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        _vk_image_layout = new_vk_image_layout;

        vkCmdPipelineBarrier(
            vk_command_buffer,
            sourceStage,
            destinationStage,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier);
    }

    VkImage VulkanImage::get_native() const
    {
        return _vk_image;
    }
}
