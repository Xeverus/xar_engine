#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <xar_engine/graphics/renderer.hpp>

#include <xar_engine/graphics/api/buffer_resource.hpp>
#include <xar_engine/graphics/api/command_buffer_resource.hpp>
#include <xar_engine/graphics/api/descriptor_pool_resource.hpp>
#include <xar_engine/graphics/api/descriptor_set_layout_resource.hpp>
#include <xar_engine/graphics/api/descriptor_set_resource.hpp>
#include <xar_engine/graphics/api/graphics_backend_type.hpp>
#include <xar_engine/graphics/api/graphics_pipeline_resource.hpp>
#include <xar_engine/graphics/api/image_resource.hpp>
#include <xar_engine/graphics/api/image_view_resource.hpp>
#include <xar_engine/graphics/api/queue_resource.hpp>
#include <xar_engine/graphics/api/sampler_resource.hpp>
#include <xar_engine/graphics/api/shader_resource.hpp>
#include <xar_engine/graphics/api/surface_resource.hpp>
#include <xar_engine/graphics/api/swap_chain_resource.hpp>

#include <xar_engine/os/window.hpp>


namespace xar_engine::graphics
{
    class RendererImpl
        : public IRenderer
    {
    public:
        RendererImpl(
            std::shared_ptr<api::IGraphicsBackend> graphics_backend,
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

    private:
        void updateUniformBuffer(uint32_t currentImage);

    private:
        std::shared_ptr<IWindowSurface> _window_surface;
        std::shared_ptr<api::IGraphicsBackend> _graphics_backend;

        std::vector<api::CommandBufferReference> _command_buffer_list;

        api::SwapChainReference _swap_chain_ref;
        api::ShaderReference _vertex_shader_ref;
        api::ShaderReference _fragment_shader_ref;

        api::BufferReference _vertex_buffer_ref;
        api::BufferReference _index_buffer_ref;
        std::vector<api::BufferReference> _uniform_buffer_ref_list;

        api::ImageReference _texture_image_ref;
        api::ImageViewReference _texture_image_view_ref;
        api::SamplerReference _sampler_ref;

        api::DescriptorPoolReference _descriptor_pool_ref;
        api::DescriptorSetLayoutReference _descriptor_set_layout_ref;
        std::vector<api::DescriptorSetReference> _descriptor_set_list_ref;

        api::ImageReference _color_image_ref;
        api::ImageViewReference _color_image_view_ref;
        api::ImageReference _depth_image_ref;
        api::ImageViewReference _depth_image_view_ref;

        api::GraphicsPipelineReference _graphics_pipeline_ref;

        uint32_t mipLevels;
        uint32_t frameCounter;
    };
}