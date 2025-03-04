#include <xar_engine/graphics/vulkan/vulkan_window_surface.hpp>


namespace xar_engine::graphics::vulkan
{
    VulkanWindowSurface::VulkanWindowSurface(
        std::shared_ptr<native::VulkanInstance> vulkan_instance,
        VkSurfaceKHR vk_surface_khr,
        const os::IWindow* os_window)
        : _vulkan_surface(native::VulkanSurface::Parameters{
            std::move(vulkan_instance),
            vk_surface_khr,
        })
        , _os_window(os_window)
    {
    }

    math::Vector2i32 VulkanWindowSurface::get_pixel_size() const
    {
        return _os_window->get_surface_pixel_size();
    }

    const native::VulkanSurface& VulkanWindowSurface::get_vulkan_surface() const
    {
        return _vulkan_surface;
    }
}