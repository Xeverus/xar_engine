#pragma once

#include <cstdint>
#include <memory>

#include <volk.h>

#include <xar_engine/graphics/vulkan/device.hpp>


namespace xar_engine::graphics::vulkan
{
    class VulkanBuffer
    {
    public:
        struct Parameters;

    public:
        VulkanBuffer();
        explicit VulkanBuffer(const Parameters& parameters);

        ~VulkanBuffer();

        void* map();
        void unmap();

        [[nodiscard]]
        VkBuffer get_native() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct VulkanBuffer::Parameters
    {
        VulkanDevice vulkan_device;

        VkDeviceSize vk_byte_size;
        VkBufferUsageFlags vk_buffer_usage_flags;
        VkMemoryPropertyFlags vk_memory_property_flags;
    };
}