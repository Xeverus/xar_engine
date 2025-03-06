#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <xar_engine/graphics/renderer.hpp>

#include <xar_engine/graphics/gpu_asset/gpu_model.hpp>

#include <xar_engine/graphics/api/buffer_reference.hpp>
#include <xar_engine/graphics/api/command_buffer_reference.hpp>
#include <xar_engine/graphics/api/descriptor_pool_reference.hpp>
#include <xar_engine/graphics/api/descriptor_set_layout_reference.hpp>
#include <xar_engine/graphics/api/descriptor_set_reference.hpp>
#include <xar_engine/graphics/api/graphics_backend_type.hpp>
#include <xar_engine/graphics/api/graphics_pipeline_reference.hpp>
#include <xar_engine/graphics/api/image_reference.hpp>
#include <xar_engine/graphics/api/image_view_reference.hpp>
#include <xar_engine/graphics/api/queue_reference.hpp>
#include <xar_engine/graphics/api/sampler_reference.hpp>
#include <xar_engine/graphics/api/shader_reference.hpp>
#include <xar_engine/graphics/api/surface_reference.hpp>
#include <xar_engine/graphics/api/swap_chain_reference.hpp>

#include <xar_engine/meta/resource_map.hpp>

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


        gpu_asset::GpuModelListReference make_gpu_model_list(const std::vector<asset::Model>& model_list) override;

        void update() override;

    private:
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

        meta::TResourceMap<gpu_asset::GpuModelListTag, gpu_asset::GpuModelList> _gpu_model_map;
        gpu_asset::GpuModelListReference _gpu_model;
    };
}