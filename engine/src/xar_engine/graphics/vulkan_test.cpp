#include <xar_engine/graphics/vulkan_test.hpp>

#include <xar_engine/graphics/vulkan/vulkan.hpp>


namespace xar_engine::graphics
{
    namespace
    {
        vulkan::Vulkan* vulkan_inst;
    }

    void init_vulkan_test(os::IWindow& window)
    {
        // intentional memory leak
        vulkan_inst = new vulkan::Vulkan(dynamic_cast<os::GlfwWindow*>(&window));

        vulkan_inst->init_surface();
        vulkan_inst->init_device();
        vulkan_inst->init_swapchain();
        vulkan_inst->init_shaders();
        vulkan_inst->init_descriptor_set_layout();
        vulkan_inst->init_graphics_pipeline();
        vulkan_inst->init_cmd_buffers();
        vulkan_inst->init_depth();
        vulkan_inst->init_texture();
        vulkan_inst->init_texture_view();
        vulkan_inst->init_sampler();
        vulkan_inst->init_vertex_data();
        vulkan_inst->init_index_data();
        vulkan_inst->init_ubo_data();
        vulkan_inst->init_descriptors();
        vulkan_inst->init_sync_objects();
        vulkan_inst->run_frame_sandbox();
    }

    void run_vulkan_test_frame()
    {
        vulkan_inst->run_frame_sandbox();
    }

    void cleanup_vulkan_test()
    {
        vulkan_inst->cleanup_sandbox();
    }

    void recreate_swapchain()
    {
        vulkan_inst->wait();
        vulkan_inst->destroy_swapchain();
        vulkan_inst->init_swapchain();
    }
}