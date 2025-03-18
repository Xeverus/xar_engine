#pragma once

#include <cstdint>
#include <memory>

#include <volk.h>

#include <xar_engine/graphics/native/vulkan/vulkan_device.hpp>


namespace xar_engine::graphics::native::vulkan
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

        [[nodiscard]]
        std::uint32_t get_buffer_byte_size() const;

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