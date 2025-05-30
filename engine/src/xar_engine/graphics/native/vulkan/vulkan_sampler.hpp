#pragma once

#include <memory>

#include <volk.h>

#include <xar_engine/graphics/native/vulkan/vulkan_device.hpp>


namespace xar_engine::graphics::native::vulkan
{
    class VulkanSampler
    {
    public:
        struct Parameters;

    public:
        VulkanSampler();
        explicit VulkanSampler(const Parameters& parameters);

        ~VulkanSampler();


        [[nodiscard]]
        VkSampler get_native() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct VulkanSampler::Parameters
    {
        VulkanDevice vulkan_device;
        float max_anisotropy;
        float max_lod;
    };
}