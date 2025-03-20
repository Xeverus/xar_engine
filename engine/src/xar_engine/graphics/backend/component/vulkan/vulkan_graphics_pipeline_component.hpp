#pragma once


#include <xar_engine/graphics/backend/component/graphics_pipeline_component.hpp>

#include <xar_engine/graphics/backend/vulkan/vulkan_graphics_backend_state.hpp>


namespace xar_engine::graphics::backend::component::vulkan
{
    class IVulkanGraphicsPipelineComponent
        : public IGraphicsPipelineComponent
          , public backend::vulkan::SharedVulkanGraphicsBackendState
    {
    public:
        using SharedVulkanGraphicsBackendState::SharedVulkanGraphicsBackendState;

        api::GraphicsPipelineReference make_graphics_pipeline(const MakeGraphicsPipelineParameters& parameters) override;

        void set_pipeline_state(const SetPipelineStateParameters& parameters) override;
        void set_vertex_buffer_list(const SetVertexBufferListParameters& parameters) override;
        void set_index_buffer(const SetIndexBufferParameters& parameters) override;
        void push_constants(const PushConstantsParameters& parameters) override;
        void draw_indexed(const DrawIndexedParameters& parameters) override;
    };
}