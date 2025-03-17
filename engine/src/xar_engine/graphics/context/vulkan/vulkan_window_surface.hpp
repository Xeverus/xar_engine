#pragma once

#include <xar_engine/graphics/context/window_surface.hpp>

#include <xar_engine/graphics/native/vulkan/vulkan_instance.hpp>
#include <xar_engine/graphics/native/vulkan/vulkan_surface.hpp>

#include <xar_engine/os/glfw_window.hpp>


namespace xar_engine::graphics::context::vulkan
{
    class VulkanWindowSurface : public IWindowSurface
    {
    public:
        explicit VulkanWindowSurface(
            std::shared_ptr<native::vulkan::VulkanInstance> vulkan_instance,
            VkSurfaceKHR vk_surface_khr,
            const os::IWindow* os_window);


        [[nodiscard]]
        math::Vector2i32 get_pixel_size() const override;


        [[nodiscard]]
        const native::vulkan::VulkanSurface& get_vulkan_surface() const;

    private:
        native::vulkan::VulkanSurface _vulkan_surface;
        const os::IWindow* _os_window;
    };
}