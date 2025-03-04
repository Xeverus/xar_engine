#pragma once

#include <memory>
#include <vector>

#include <volk.h>

#include <xar_engine/graphics/vulkan/native/vulkan_device.hpp>


namespace xar_engine::graphics::vulkan::native
{
    class VulkanCommandBuffer;

    class VulkanCommandBufferPool
    {
    public:
        struct Parameters;

    public:
        VulkanCommandBufferPool();
        explicit VulkanCommandBufferPool(const Parameters& parameters);

        ~VulkanCommandBufferPool();


        std::vector<VulkanCommandBuffer> make_buffer_list(std::int32_t count);

        [[nodiscard]]
        const VulkanDevice& get_device() const;

        [[nodiscard]]
        VulkanDevice& get_device();

        [[nodiscard]]
        VkCommandPool get_native() const;

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