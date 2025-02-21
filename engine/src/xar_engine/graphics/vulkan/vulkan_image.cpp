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
    }

    VulkanImage::VulkanImage(const VulkanImage::Parameters& parameters)
        : _vk_device(parameters.vk_device)
        , _vk_image(nullptr)
        , _vk_image_memory(nullptr)
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

    VkImage VulkanImage::get_native() const
    {
        return _vk_image;
    }
}
