#pragma once

#include <cstdint>

#include <volk.h>


namespace xar_engine::graphics::vulkan
{
    class VulkanDescriptorPool
    {
    public:
        struct Parameters;

    public:
        explicit VulkanDescriptorPool(const Parameters& parameters);
        ~VulkanDescriptorPool();

        [[nodiscard]]
        VkDescriptorPool get_native() const;

    private:
        VkDevice _vk_device;
        VkDescriptorPool _vk_descriptor_pool;
    };

    struct VulkanDescriptorPool::Parameters
    {
        VkDevice vk_device;

        std::int32_t uniform_buffer_count;
        std::int32_t combined_image_sampler_count;
        std::int32_t max_descriptor_set_count;
    };
}