#pragma once

#include <cstdint>
#include <memory>
#include <vector>

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

#include <xar_engine/graphics/backend/graphics_backend.hpp>

#include <xar_engine/graphics/context/window_surface.hpp>

#include <xar_engine/meta/resource_map.hpp>
#include <xar_engine/meta/shared_state.hpp>

#include <xar_engine/renderer/gpu_asset/gpu_material.hpp>
#include <xar_engine/renderer/gpu_asset/gpu_material_data.hpp>
#include <xar_engine/renderer/gpu_asset/gpu_mesh_instance.hpp>
#include <xar_engine/renderer/gpu_asset/gpu_model_data.hpp>
#include <xar_engine/renderer/gpu_asset/gpu_model_data_buffer.hpp>


namespace xar_engine::renderer
{
    struct RendererState
    {
        std::shared_ptr<graphics::context::IWindowSurface> _window_surface;
        std::shared_ptr<graphics::backend::IGraphicsBackend> _graphics_backend;

        std::vector<graphics::api::CommandBufferReference> _command_buffer_list;

        graphics::api::SwapChainReference _swap_chain_ref;
        graphics::api::ShaderReference _vertex_shader_ref;
        graphics::api::ShaderReference _fragment_shader_ref;

        std::vector<graphics::api::BufferReference> _uniform_buffer_ref_list;

        graphics::api::DescriptorPoolReference _ubo_descriptor_pool_ref;
        graphics::api::DescriptorSetLayoutReference _ubo_descriptor_set_layout_ref;
        std::vector<graphics::api::DescriptorSetReference> _ubo_descriptor_set_list_ref;

        graphics::api::DescriptorPoolReference _image_descriptor_pool_ref;
        graphics::api::DescriptorSetLayoutReference _image_descriptor_set_layout_ref;
        graphics::api::DescriptorSetReference _image_descriptor_set_ref;

        graphics::api::ImageReference _color_image_ref;
        graphics::api::ImageViewReference _color_image_view_ref;
        graphics::api::ImageReference _depth_image_ref;
        graphics::api::ImageViewReference _depth_image_view_ref;

        graphics::api::GraphicsPipelineReference _graphics_pipeline_ref;

        uint32_t frameCounter;

        meta::TResourceMap<gpu_asset::GpuModelDataBufferTag, gpu_asset::GpuModelDataBuffer> _gpu_model_data_buffer_map;
        meta::TResourceMap<gpu_asset::GpuModelTag, gpu_asset::GpuModelData> _gpu_model_data_map;
        meta::TResourceMap<gpu_asset::GpuMeshTag, gpu_asset::GpuMeshData> _gpu_mesh_data_map;
        meta::TResourceMap<gpu_asset::GpuMaterialTag, gpu_asset::GpuMaterialData> _gpu_material_data_map;

        struct RenderItem
        {
            gpu_asset::GpuMeshInstance gpu_mesh_instance;
            gpu_asset::GpuMaterialReference gpu_material;
        };

        std::vector<RenderItem> _redner_item_list;
    };


    class SharedRendererState : public meta::TSharedState<RendererState>
    {
    public:
        using meta::TSharedState<RendererState>::TSharedState;
    };
}