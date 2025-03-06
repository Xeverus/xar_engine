#pragma once

#include <cstdint>
#include <vector>

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

#include <xar_engine/graphics/window_surface.hpp>

#include <xar_engine/math/vector.hpp>


namespace xar_engine::graphics::api
{
    class IGraphicsBackendResource
    {
    public:
        virtual ~IGraphicsBackendResource();


        virtual BufferReference make_staging_buffer(std::uint32_t byte_size) = 0;
        virtual BufferReference make_vertex_buffer(std::uint32_t byte_size) = 0;
        virtual BufferReference make_index_buffer(std::uint32_t byte_size) = 0;
        virtual BufferReference make_uniform_buffer(std::uint32_t byte_size) = 0;

        virtual std::vector<CommandBufferReference> make_command_buffer_list(std::uint32_t buffer_counts) = 0;

        virtual DescriptorPoolReference make_descriptor_pool() = 0;

        virtual std::vector<DescriptorSetReference> make_descriptor_set_list(
            const DescriptorPoolReference& descriptor_pool,
            const DescriptorSetLayoutReference& descriptor_set_layout,
            const std::vector<BufferReference>& uniform_buffer_list,
            const ImageViewReference& texture_image_view,
            const SamplerReference& sampler) = 0;

        virtual DescriptorSetLayoutReference make_descriptor_set_layout() = 0;

        virtual GraphicsPipelineReference make_graphics_pipeline(
            const DescriptorSetLayoutReference& descriptor_set_layout,
            const ShaderReference& vertex_shader,
            const ShaderReference& fragment_shader,
            const std::vector<VertexInputAttribute>& vertex_input_attribute_list,
            const std::vector<VertexInputBinding>& vertex_input_binding_list,
            EFormat color_format,
            EFormat depth_format,
            std::uint32_t sample_counts) = 0;

        virtual ImageReference make_image(
            EImageType image_type,
            math::Vector3i32 dimension,
            EFormat image_format,
            std::uint32_t mip_levels,
            std::uint32_t sample_count) = 0;

        virtual ImageViewReference make_image_view(
            const ImageReference& image,
            EImageAspect image_aspect,
            std::uint32_t mip_levels) = 0;

        virtual SamplerReference make_sampler(float mip_levels) = 0;

        virtual ShaderReference make_shader(const std::vector<char>& shader_byte_code) = 0;

        virtual SwapChainReference make_swap_chain(
            std::shared_ptr<IWindowSurface> window_surface,
            std::uint32_t buffering_level) = 0;
    };


    struct BufferUpdate
    {
        const void* data;
        std::uint32_t byte_offset;
        std::uint32_t byte_size;
    };


    class IGraphicsBackendHost
    {
    public:
        virtual ~IGraphicsBackendHost();

        virtual void update_buffer(
            const BufferReference& buffer,
            const std::vector<BufferUpdate>& data) = 0;

        virtual std::tuple<ESwapChainResult, std::uint32_t, std::uint32_t> begin_frame(
            const SwapChainReference& swap_chain) = 0;

        [[nodiscard]]
        virtual std::uint32_t get_sample_count() const = 0;

        [[nodiscard]]
        virtual EFormat find_depth_format() const = 0;
    };


    class IGraphicsBackendCommand
    {
    public:
        virtual ~IGraphicsBackendCommand();

        virtual void copy_buffer(
            const CommandBufferReference& command_buffer,
            const BufferReference& source_buffer,
            const BufferReference& destination_buffer) = 0;

        virtual void copy_buffer_to_image(
            const CommandBufferReference& command_buffer,
            const BufferReference& source_buffer,
            const ImageReference& target_image) = 0;

        virtual ESwapChainResult end_frame(
            const CommandBufferReference& command_buffer,
            const SwapChainReference& swap_chain) = 0;

        virtual void generate_image_mip_maps(
            const CommandBufferReference& command_buffer,
            const ImageReference& image) = 0;

        virtual void begin_command_buffer(
            const CommandBufferReference& command_buffer,
            ECommandBufferType command_buffer_type) = 0;

        virtual void end_command_buffer(const CommandBufferReference& command_buffer) = 0;

        virtual void submit_command_buffer(const CommandBufferReference& command_buffer) = 0;

        virtual void set_vertex_buffer_list(
            const CommandBufferReference& command_buffer,
            const std::vector<BufferReference>& vertex_buffer_list,
            const std::vector<std::uint32_t>& vertex_buffer_offset_list,
            std::uint32_t first_slot) = 0;

        virtual void set_index_buffer(
            const CommandBufferReference& command_buffer,
            const BufferReference& index_buffer,
            std::uint32_t first_index) = 0;

        virtual void push_constants(
            const CommandBufferReference& command_buffer,
            const GraphicsPipelineReference& graphics_pipeline,
            EShaderType shader_type,
            std::uint32_t offset,
            std::uint32_t byte_size,
            void* data_byte) = 0;

        virtual void draw_indexed(
            const CommandBufferReference& command_buffer,
            std::uint32_t index_counts,
            std::uint32_t instance_counts,
            std::uint32_t first_index,
            std::uint32_t vertex_offset,
            std::uint32_t first_instance) = 0;

        virtual void transit_image_layout(
            const CommandBufferReference& command_buffer,
            const ImageReference& image,
            EImageLayout new_image_layout) = 0;

        virtual void wait_idle() = 0;

        virtual void begin_rendering(
            const api::CommandBufferReference& command_buffer,
            const api::SwapChainReference& swap_chain,
            std::uint32_t image_index,
            const api::ImageViewReference& color_image_view,
            const api::ImageViewReference& depth_image_view) = 0;

        virtual void set_pipeline_state(
            const api::CommandBufferReference& command_buffer,
            const api::SwapChainReference& swap_chain,
            const api::GraphicsPipelineReference& graphics_pipeline,
            const api::DescriptorSetReference& descriptor_set) = 0;

        virtual void end_rendering(
            const api::CommandBufferReference& command_buffer,
            const api::SwapChainReference& swap_chain,
            std::uint32_t image_index) = 0;
    };


    class IGraphicsBackend
    {
    public:
        virtual ~IGraphicsBackend();

        [[nodiscard]]
        virtual const IGraphicsBackendResource& resource() const = 0;
        virtual IGraphicsBackendResource& resource() = 0;

        [[nodiscard]]
        virtual const IGraphicsBackendHost& host() const = 0;
        virtual IGraphicsBackendHost& host() = 0;

        [[nodiscard]]
        virtual const IGraphicsBackendCommand& command() const = 0;
        virtual IGraphicsBackendCommand& command() = 0;
    };
}