#pragma once

#include <xar_engine/graphics/api/buffer_reference.hpp>
#include <xar_engine/graphics/api/command_buffer_reference.hpp>
#include <xar_engine/graphics/api/descriptor_set_layout_reference.hpp>
#include <xar_engine/graphics/api/descriptor_set_reference.hpp>
#include <xar_engine/graphics/api/graphics_pipeline_reference.hpp>
#include <xar_engine/graphics/api/shader_reference.hpp>
#include <xar_engine/graphics/api/swap_chain_reference.hpp>


namespace xar_engine::graphics::backend::component
{
    class IGraphicsPipelineComponent
    {
    public:
        struct MakeGraphicsPipelineParameters;

        struct SetPipelineStateParameters;
        struct SetVertexBufferListParameters;
        struct SetIndexBufferParameters;
        struct PushConstantsParameters;
        struct DrawIndexedParameters;

    public:
        virtual ~IGraphicsPipelineComponent();


        virtual api::GraphicsPipelineReference make_graphics_pipeline(const MakeGraphicsPipelineParameters& parameters) = 0;

        virtual void set_pipeline_state(const SetPipelineStateParameters& parameters) = 0;
        virtual void set_vertex_buffer_list(const SetVertexBufferListParameters& parameters) = 0;
        virtual void set_index_buffer(const SetIndexBufferParameters& parameters) = 0;
        virtual void push_constants(const PushConstantsParameters& parameters) = 0;
        virtual void draw_indexed(const DrawIndexedParameters& parameters) = 0;
    };


    struct IGraphicsPipelineComponent::MakeGraphicsPipelineParameters
    {
        std::vector<api::DescriptorSetLayoutReference> descriptor_set_layout_list;
        api::ShaderReference vertex_shader;
        api::ShaderReference fragment_shader;
        std::vector<api::VertexInputAttribute> vertex_input_attribute_list;
        std::vector<api::VertexInputBinding> vertex_input_binding_list;
        api::EFormat color_format;
        api::EFormat depth_format;
        std::uint32_t sample_counts;
    };

    struct IGraphicsPipelineComponent::SetPipelineStateParameters
    {
        api::CommandBufferReference command_buffer;
        api::SwapChainReference swap_chain;
        api::GraphicsPipelineReference graphics_pipeline;
        std::vector<api::DescriptorSetReference> descriptor_set_list;
    };

    struct IGraphicsPipelineComponent::SetVertexBufferListParameters
    {
        api::CommandBufferReference command_buffer;
        std::vector<api::BufferReference> vertex_buffer_list;
        std::vector<std::uint32_t> vertex_buffer_offset_list;
        std::uint32_t first_slot;
    };

    struct IGraphicsPipelineComponent::SetIndexBufferParameters
    {
        api::CommandBufferReference command_buffer;
        api::BufferReference index_buffer;
        std::uint32_t first_index;
    };

    struct IGraphicsPipelineComponent::PushConstantsParameters
    {

        api::CommandBufferReference command_buffer;
        api::GraphicsPipelineReference graphics_pipeline;
        api::EShaderType shader_type;
        std::uint32_t offset;
        std::uint32_t byte_size;
        void* data_byte;
    };

    struct IGraphicsPipelineComponent::DrawIndexedParameters
    {
        api::CommandBufferReference command_buffer;
        std::uint32_t index_counts;
        std::uint32_t instance_counts;
        std::uint32_t first_index;
        std::uint32_t vertex_offset;
        std::uint32_t first_instance;
    };
}