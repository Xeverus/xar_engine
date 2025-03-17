#pragma once

#include <memory>
#include <vector>

#include <volk.h>

#include <xar_engine/graphics/native/vulkan/vulkan_command_buffer_pool.hpp>
#include <xar_engine/graphics/native/vulkan/vulkan_device.hpp>


namespace xar_engine::graphics::native::vulkan
{
    class VulkanCommandBuffer
    {
    public:
        struct Parameters;

    public:
        VulkanCommandBuffer();
        explicit VulkanCommandBuffer(const Parameters& parameters);

        ~VulkanCommandBuffer();


        void begin(bool one_time);
        void end();


        [[nodiscard]]
        VkCommandBuffer get_native() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct VulkanCommandBuffer::Parameters
    {
        VulkanCommandBufferPool vulkan_command_buffer_pool;
        VkCommandBuffer vk_command_buffer;
    };
}