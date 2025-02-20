#pragma once

#include <volk.h>


namespace xar_engine::graphics::vulkan
{
    class VulkanBuffer
    {
    public:
        struct Parameters;

    public:
        explicit VulkanBuffer(const Parameters& parameters);
        ~VulkanBuffer();

    private:
        VkBuffer _vk_buffer;
        VkDeviceMemory _vk_device_memory;
    };

    struct VulkanBuffer::Parameters
    {
        VkDevice vk_device;
        VkPhysicalDevice vk_physical_device;

        VkDeviceSize vk_byte_size;
        VkBufferUsageFlags vk_buffer_usage;
        VkMemoryPropertyFlags vk_memory_properties;
    };
}