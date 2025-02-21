#pragma once

#include <volk.h>


namespace xar_engine::graphics::vulkan
{
    class VulkanSampler
    {
    public:
        struct Parameters;

    public:
        explicit VulkanSampler(const Parameters& parameters);
        ~VulkanSampler();

        [[nodiscard]]
        VkSampler get_native() const;

    private:
        VkDevice _vk_device;
        VkSampler _vk_sampler;
    };

    struct VulkanSampler::Parameters
    {
        VkDevice vk_device;
        float max_anisotropy;
        float max_lod;
    };
}