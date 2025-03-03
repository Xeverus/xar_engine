#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <xar_engine/graphics/renderer.hpp>
#include <xar_engine/graphics/graphics_backend.hpp>

#include <xar_engine/graphics/vulkan/vulkan_graphics_backend.hpp>

#include <xar_engine/graphics/vulkan/impl/vulkan_buffer.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_command_buffer_pool.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_descriptor_pool.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_descriptor_set.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_descriptor_set_layout.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_device.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_graphics_pipeline.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_image.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_image_view.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_instance.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_physical_device.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_sampler.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_shader.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_surface.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_swap_chain.hpp>

#include <xar_engine/os/window.hpp>


namespace xar_engine::graphics::vulkan::impl
{
    class VulkanRenderer
        : public IRenderer
    {
    public:
        explicit VulkanRenderer(
            const std::shared_ptr<VulkanInstance>& vulkan_instance,
            std::shared_ptr<VulkanWindowSurface> vulkan_window_surface);

        ~VulkanRenderer() override;

        void update() override;

    private:
        void destroy_swapchain();
        void init_descriptor_set_layout();
        void init_graphics_pipeline();
        void init_vertex_data();
        void init_index_data();
        void init_model();
        void init_ubo_data();
        void init_descriptors();
        void init_color_msaa();
        void init_depth();
        void init_texture();
        void init_texture_view();
        void init_sampler();

        void run_frame_sandbox();

        void cleanup_sandbox();

    private:
        void updateUniformBuffer(uint32_t currentImage);

        EImageFormat findDepthFormat();

    private:
        std::shared_ptr<VulkanInstance> _instance;

        uint32_t mipLevels;
        uint32_t frameCounter;

        std::shared_ptr<VulkanWindowSurface> _vulkan_window_surface;

        std::unique_ptr<IGraphicsBackend> _vulkan_graphics_backend;
        std::vector<CommandBufferReference> _command_buffer_list;

        SwapChainReference _swap_chain_ref;
        ShaderReference _vertex_shader_ref;
        ShaderReference _fragment_shader_ref;

        BufferReference _vertex_buffer_ref;
        BufferReference _index_buffer_ref;
        std::vector<BufferReference> _uniform_buffer_ref_list;

        ImageReference _texture_image_ref;
        ImageViewReference _texture_image_view_ref;
        SamplerReference _sampler_ref;

        DescriptorPoolReference _descriptor_pool_ref;
        DescriptorSetLayoutReference _descriptor_set_layout_ref;
        DescriptorSetListReference _descriptor_set_list_ref;

        ImageReference _color_image_ref;
        ImageViewReference _color_image_view_ref;
        ImageReference _depth_image_ref;
        ImageViewReference _depth_image_view_ref;

        GraphicsPipelineReference _graphics_pipeline_ref;
    };
}