#pragma once

#include <cstdint>
#include <memory>

#include <volk.h>

#include <xar_engine/graphics/vulkan/native/vulkan_physical_device.hpp>


namespace xar_engine::graphics::vulkan::native
{
    class VulkanDevice
    {
    public:
        struct Parameters;

    public:
        VulkanDevice();
        explicit VulkanDevice(const Parameters& parameters);

        ~VulkanDevice();


        void wait_idle() const;

        [[nodiscard]]
        const VulkanPhysicalDevice& get_physical_device() const;

        [[nodiscard]]
        VkDevice get_native() const;

        [[nodiscard]]
        std::uint32_t get_graphics_family_index() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct VulkanDevice::Parameters
    {
        VulkanPhysicalDevice vulkan_physical_device;
    };
}