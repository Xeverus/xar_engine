#pragma once

#include <memory>
#include <vector>

#include <volk.h>

#include <xar_engine/graphics/vulkan/vulkan_device.hpp>


namespace xar_engine::graphics::vulkan
{
    class VulkanCommandBufferPool
    {
    public:
        struct Parameters;

    public:
        VulkanCommandBufferPool();
        explicit VulkanCommandBufferPool(const Parameters& parameters);

        ~VulkanCommandBufferPool();


        [[nodiscard]]
        VkCommandBuffer make_one_time_buffer();
        void submit_one_time_buffer(VkCommandBuffer vk_command_buffer);

        [[nodiscard]]
        std::vector<VkCommandBuffer> make_buffers(std::int32_t count);

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct VulkanCommandBufferPool::Parameters
    {
        VulkanDevice vulkan_device;
    };
}