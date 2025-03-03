#pragma once

#include <memory>

#include <volk.h>

#include <xar_engine/graphics/vulkan/impl/vulkan_command_buffer.hpp>


namespace xar_engine::graphics::vulkan::impl
{
    class VulkanQueue
    {
    public:
        struct Parameters;

    public:
        VulkanQueue();
        explicit VulkanQueue(const Parameters& parameters);

        ~VulkanQueue();


        void submit(const VulkanCommandBuffer& vulkan_command_buffer);


        [[nodiscard]]
        VkQueue get_native() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct VulkanQueue::Parameters
    {
        VulkanDevice vulkan_device;
        std::uint32_t queue_family_index;
    };
}