#pragma once

#include <volk.h>


namespace xar_engine::graphics::vulkan
{
    class VulkanSurface
    {
    public:
        struct Parameters;

    public:
        explicit VulkanSurface(const Parameters& parameters);
        ~VulkanSurface();

        [[nodiscard]]
        VkSurfaceKHR get_native() const;

    private:
        VkInstance _vk_instance;
        VkSurfaceKHR _vk_surface;
    };

    struct VulkanSurface::Parameters
    {
        VkInstance vk_instance;
        VkSurfaceKHR vk_surface;
    };
}