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

        [[nodiscard]]
        VkSurfaceKHR get_native() const;

    private:
        VkSurfaceKHR _vk_surface;
    };

    struct VulkanSurface::Parameters
    {
        VkSurfaceKHR vk_surface;
    };
}