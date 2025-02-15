#pragma once

#include <xar_engine/os/window.hpp>


namespace xar_engine::graphics
{
    void init_vulkan_test(os::IWindow& window);
    void run_vulkan_test_frame();
    void cleanup_vulkan_test();
    void recreate_swapchain();
}