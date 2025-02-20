#include <xar_engine/graphics/vulkan/vulkan_buffer.hpp>

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

    VulkanBuffer::VulkanBuffer(const VulkanBuffer::Parameters& parameters)
        : _vk_buffer(nullptr)
        , _vk_device_memory(nullptr)
        , _vk_device(parameters.vk_device)
        , _byte_size(0)
    {
        auto buffer_info = VkBufferCreateInfo{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = parameters.vk_byte_size;
        buffer_info.usage = parameters.vk_buffer_usage;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        const auto vk_create_buffer_result = vkCreateBuffer(
            parameters.vk_device,
            &buffer_info,
            nullptr,
            &_vk_buffer);
        XAR_THROW_IF(
            vk_create_buffer_result != VK_SUCCESS,
            error::XarException,
            "Failed to create buffer");

        auto memory_requirements = VkMemoryRequirements{};
        vkGetBufferMemoryRequirements(
            parameters.vk_device,
            _vk_buffer,
            &memory_requirements);

        auto alloc_info = VkMemoryAllocateInfo{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = memory_requirements.size;
        alloc_info.memoryTypeIndex = findMemoryType(
            parameters.vk_physical_device,
            memory_requirements.memoryTypeBits,
            parameters.vk_memory_properties);

        // TODO(): Or original size?
        _byte_size = memory_requirements.size;

        const auto vk_allocate_memory_result = vkAllocateMemory(
            parameters.vk_device,
            &alloc_info,
            nullptr,
            &_vk_device_memory);
        XAR_THROW_IF(
            vk_allocate_memory_result != VK_SUCCESS,
            error::XarException,
            "Failed to allocate memory for buffer");

        vkBindBufferMemory(
            parameters.vk_device,
            _vk_buffer,
            _vk_device_memory,
            0);
    }

    VulkanBuffer::~VulkanBuffer()
    {
        if (_vk_buffer)
        {
            vkDestroyBuffer(
                _vk_device,
                _vk_buffer,
                nullptr);
        }
        if (_vk_device_memory)
        {
            vkFreeMemory(
                _vk_device,
                _vk_device_memory,
                nullptr);
        }
    }

    void* VulkanBuffer::map()
    {
        void* mapped_data = nullptr;

        vkMapMemory(
            _vk_device,
            _vk_device_memory,
            0,
            _byte_size,
            0,
            &mapped_data);

        return mapped_data;
    }

    void VulkanBuffer::unmap()
    {
        vkUnmapMemory(
            _vk_device,
            _vk_device_memory);
    }

    VkBuffer VulkanBuffer::get_native() const
    {
        return _vk_buffer;
    }
}