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
        std::shared_ptr<graphics::context::IWindowSurface> window_surface;
        std::shared_ptr<graphics::backend::IGraphicsBackend> graphics_backend;

        std::vector<graphics::api::CommandBufferReference> command_buffer_list;

        graphics::api::SwapChainReference swap_chain_ref;
        graphics::api::ShaderReference vertex_shader_ref;
        graphics::api::ShaderReference fragment_shader_ref;

        std::vector<graphics::api::BufferReference> uniform_buffer_ref_list;

        graphics::api::DescriptorPoolReference ubo_descriptor_pool_ref;
        graphics::api::DescriptorSetLayoutReference ubo_descriptor_set_layout_ref;
        std::vector<graphics::api::DescriptorSetReference> ubo_descriptor_set_list_ref;

        graphics::api::DescriptorPoolReference image_descriptor_pool_ref;
        graphics::api::DescriptorSetLayoutReference image_descriptor_set_layout_ref;
        graphics::api::DescriptorSetReference image_descriptor_set_ref;

        graphics::api::ImageReference color_image_ref;
        graphics::api::ImageViewReference color_image_view_ref;
        graphics::api::ImageReference depth_image_ref;
        graphics::api::ImageViewReference depth_image_view_ref;

        graphics::api::GraphicsPipelineReference graphics_pipeline_ref;

        uint32_t frameCounter;

        meta::TResourceMap<gpu_asset::GpuModelDataBufferTag, gpu_asset::GpuModelDataBuffer> gpu_model_data_buffer_map;
        meta::TResourceMap<gpu_asset::GpuModelTag, gpu_asset::GpuModelData> gpu_model_data_map;
        meta::TResourceMap<gpu_asset::GpuMeshTag, gpu_asset::GpuMeshData> gpu_mesh_data_map;
        meta::TResourceMap<gpu_asset::GpuMaterialTag, gpu_asset::GpuMaterialData> gpu_material_data_map;

        struct RenderItem
        {
            gpu_asset::GpuMeshInstance gpu_mesh_instance;
            gpu_asset::GpuMaterialReference gpu_material;
        };

        std::vector<RenderItem> redner_item_list;
    };


    class SharedRendererState
        : public meta::TSharedState<RendererState>
    {
    public:
        using meta::TSharedState<RendererState>::TSharedState;
    };
}