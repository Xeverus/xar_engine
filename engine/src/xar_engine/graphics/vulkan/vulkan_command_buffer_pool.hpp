#pragma once

#include <cstdint>
#include <vector>

#include <volk.h>


namespace xar_engine::graphics::vulkan
{
    class VulkanCommandBufferPool
    {
    public:
        struct Parameters;

    public:
        explicit VulkanCommandBufferPool(const Parameters& parameters);
        ~VulkanCommandBufferPool();

        [[nodiscard]]
        VkCommandBuffer make_one_time_buffer();
        void submit_one_time_buffer(VkCommandBuffer vk_command_buffer);

        [[nodiscard]]
        std::vector<VkCommandBuffer> make_buffers(std::int32_t count);

    private:
        VkDevice _vk_device;
        VkQueue _vk_graphics_queue;
        VkCommandPool _vk_command_pool;
    };

    struct VulkanCommandBufferPool::Parameters
    {
        VkDevice vk_device;
        VkQueue vk_graphics_queue;
        std::uint32_t vk_graphics_family_index;
    };
}