#pragma once

#include <cstdint>
#include <memory>

#include <volk.h>

#include <xar_engine/graphics/vulkan/device.hpp>


namespace xar_engine::graphics::vulkan
{
    class Buffer
    {
    public:
        struct Parameters;

    public:
        Buffer();
        explicit Buffer(const Parameters& parameters);

        ~Buffer();

        void* map();
        void unmap();

        [[nodiscard]]
        VkBuffer get_native() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct Buffer::Parameters
    {
        Device device;

        VkDeviceSize vk_byte_size;
        VkBufferUsageFlags vk_buffer_usage;
        VkMemoryPropertyFlags vk_memory_properties;
    };
}