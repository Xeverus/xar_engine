#pragma once

#include <cstdint>

#include <volk.h>


namespace xar_engine::graphics::vulkan
{
    class VulkanDevice
    {
    public:
        struct Parameters;

    public:
        explicit VulkanDevice(const Parameters& parameters);
        ~VulkanDevice();


        void wait_idle() const;

        [[nodiscard]]
        VkDevice get_native() const;

        [[nodiscard]]
        std::uint32_t get_graphics_family_index() const;

        [[nodiscard]]
        VkQueue get_graphics_queue() const;

    private:
        VkDevice _vk_device;
        VkQueue _vk_graphics_queue;
        std::uint32_t _graphics_family_index;
    };

    struct VulkanDevice::Parameters
    {
        VkPhysicalDevice vk_physical_device;
    };
}