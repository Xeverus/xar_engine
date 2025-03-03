#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <xar_engine/graphics/graphics_backend_resource_types.hpp>
#include <xar_engine/graphics/renderer.hpp>

#include <xar_engine/os/window.hpp>


namespace xar_engine::graphics
{
    class IGraphicsBackend;

    class RendererImpl
        : public IRenderer
    {
    public:
        RendererImpl(
            std::shared_ptr<IGraphicsBackend> graphics_backend,
            std::shared_ptr<IWindowSurface> window_surface);

        ~RendererImpl() override;

        void update() override;

    private:
        void init_vertex_data();
        void init_index_data();
        void init_model();
        void init_color_msaa();
        void init_depth();
        void init_texture();

        void run_frame_sandbox();

        void cleanup_sandbox();

    private:
        void updateUniformBuffer(uint32_t currentImage);

    private:
        std::shared_ptr<IWindowSurface> _window_surface;
        std::shared_ptr<IGraphicsBackend> _graphics_backend;

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

        uint32_t mipLevels;
        uint32_t frameCounter;
    };
}