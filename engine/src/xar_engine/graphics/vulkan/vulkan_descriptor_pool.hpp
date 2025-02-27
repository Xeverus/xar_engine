#pragma once

#include <memory>

#include <volk.h>

#include <xar_engine/graphics/vulkan/vulkan_device.hpp>


namespace xar_engine::graphics::vulkan
{
    class VulkanDescriptorPool
    {
    public:
        struct Parameters;

    public:
        VulkanDescriptorPool();
        explicit VulkanDescriptorPool(const Parameters& parameters);

        ~VulkanDescriptorPool();

        [[nodiscard]]
        VkDescriptorPool get_native() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct VulkanDescriptorPool::Parameters
    {
        VulkanDevice vulkan_device;

        std::int32_t uniform_buffer_count;
        std::int32_t combined_image_sampler_count;
        std::int32_t max_descriptor_set_count;
    };
}