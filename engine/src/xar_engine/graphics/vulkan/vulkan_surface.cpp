#include <xar_engine/graphics/vulkan/vulkan_surface.hpp>


namespace xar_engine::graphics::vulkan
{
    VulkanSurface::VulkanSurface(const VulkanSurface::Parameters& parameters)
        : _vk_instance(parameters.vk_instance)
        , _vk_surface(parameters.vk_surface)
    {
    }

    VulkanSurface::~VulkanSurface()
    {
        vkDestroySurfaceKHR(
            _vk_instance,
            _vk_surface,
            nullptr);
    }

    VkSurfaceKHR VulkanSurface::get_native() const
    {
        return _vk_surface;
    }
}