#include <xar_engine/graphics/vulkan/vulkan_surface.hpp>


namespace xar_engine::graphics::vulkan
{
    VulkanSurface::VulkanSurface(const VulkanSurface::Parameters& parameters)
        : _vk_surface(parameters.vk_surface)
    {
    }

    VkSurfaceKHR VulkanSurface::get_native() const
    {
        return _vk_surface;
    }
}