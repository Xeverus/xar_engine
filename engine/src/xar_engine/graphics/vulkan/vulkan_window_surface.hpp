#pragma once

#include <xar_engine/graphics/window_surface.hpp>

#include <xar_engine/graphics/vulkan/impl/vulkan_instance.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_surface.hpp>

#include <xar_engine/os/glfw_window.hpp>


namespace xar_engine::graphics::vulkan
{
    class VulkanWindowSurface : public IWindowSurface
    {
    public:
        explicit VulkanWindowSurface(
            std::shared_ptr<impl::VulkanInstance> vulkan_instance,
            VkSurfaceKHR vk_surface_khr,
            const os::IWindow* os_window);


        [[nodiscard]]
        math::Vector2i32 get_pixel_size() const override;


        [[nodiscard]]
        const impl::VulkanSurface& get_vulkan_surface() const;

    private:
        impl::VulkanSurface _vulkan_surface;
        const os::IWindow* _os_window;
    };
}