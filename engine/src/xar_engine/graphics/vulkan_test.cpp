#include <xar_engine/graphics/vulkan_test.hpp>

#include <xar_engine/graphics/vulkan/vulkan.hpp>


namespace xar_engine::graphics
{
    void run_vulkan_test(os::IWindow& window)
    {
        // intentional memory leak
        auto* vulkan = new vulkan::Vulkan(dynamic_cast<os::GlfwWindow*>(&window));

        vulkan->init_surface();
        vulkan->init_device();
        vulkan->init_swapchain();
        vulkan->init_shaders();
        vulkan->init_graphics_pipeline();
    }
}