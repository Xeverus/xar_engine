#pragma once

#include <cstdint>
#include <set>
#include <vector>

#include <xar_engine/graphics/api/buffer_reference.hpp>
#include <xar_engine/graphics/api/command_buffer_reference.hpp>
#include <xar_engine/graphics/api/descriptor_pool_reference.hpp>
#include <xar_engine/graphics/api/descriptor_set_layout_reference.hpp>
#include <xar_engine/graphics/api/descriptor_set_reference.hpp>
#include <xar_engine/graphics/api/format.hpp>
#include <xar_engine/graphics/api/graphics_backend_type.hpp>
#include <xar_engine/graphics/api/graphics_pipeline_reference.hpp>
#include <xar_engine/graphics/api/image_reference.hpp>
#include <xar_engine/graphics/api/image_view_reference.hpp>
#include <xar_engine/graphics/api/queue_reference.hpp>
#include <xar_engine/graphics/api/sampler_reference.hpp>
#include <xar_engine/graphics/api/shader_reference.hpp>
#include <xar_engine/graphics/api/surface_reference.hpp>
#include <xar_engine/graphics/api/swap_chain_reference.hpp>

#include <xar_engine/graphics/context/window_surface.hpp>

#include <xar_engine/math/vector.hpp>


namespace xar_engine::graphics::backend
{
    class IGraphicsBackendResource
    {
    public:
        virtual ~IGraphicsBackendResource();


        virtual api::BufferReference make_staging_buffer(std::uint32_t byte_size) = 0;
        virtual api::BufferReference make_vertex_buffer(std::uint32_t byte_size) = 0;
        virtual api::BufferReference make_index_buffer(std::uint32_t byte_size) = 0;
        virtual api::BufferReference make_uniform_buffer(std::uint32_t byte_size) = 0;

        virtual std::vector<api::CommandBufferReference> make_command_buffer_list(std::uint32_t buffer_counts) = 0;

        virtual api::DescriptorPoolReference make_descriptor_pool(const std::set<api::EDescriptorPoolType>& descriptor_pool_type_list) = 0;

        virtual api::DescriptorSetLayoutReference make_descriptor_set_layout(const std::set<api::EDescriptorPoolType>& descriptor_pool_type_list) = 0;

        virtual std::vector<api::DescriptorSetReference> make_descriptor_set_list(
            const api::DescriptorPoolReference& descriptor_pool,
            const api::DescriptorSetLayoutReference& descriptor_set_layout,
            std::uint32_t descriptor_counts) = 0;

        virtual void write_descriptor_set(
            const api::DescriptorSetReference& descriptor_set,
            std::uint32_t uniform_buffer_first_index,
            const std::vector<api::BufferReference>& uniform_buffer_list,
            std::uint32_t texture_image_first_index,
            const std::vector<api::ImageViewReference>& texture_image_view_list,
            const std::vector<api::SamplerReference>& sampler_list) = 0;

        virtual api::GraphicsPipelineReference make_graphics_pipeline(
            const std::vector<api::DescriptorSetLayoutReference>& descriptor_set_layout_list,
            const api::ShaderReference& vertex_shader,
            const api::ShaderReference& fragment_shader,
            const std::vector<api::VertexInputAttribute>& vertex_input_attribute_list,
            const std::vector<api::VertexInputBinding>& vertex_input_binding_list,
            api::EFormat color_format,
            api::EFormat depth_format,
            std::uint32_t sample_counts) = 0;

        virtual api::ImageReference make_image(
            api::EImageType image_type,
            math::Vector3i32 dimension,
            api::EFormat image_format,
            std::uint32_t mip_levels,
            std::uint32_t sample_count) = 0;

        virtual api::ImageViewReference make_image_view(
            const api::ImageReference& image,
            api::EImageAspect image_aspect,
            std::uint32_t mip_levels) = 0;

        virtual api::SamplerReference make_sampler(float mip_levels) = 0;

        virtual api::ShaderReference make_shader(const std::vector<char>& shader_byte_code) = 0;

        virtual api::SwapChainReference make_swap_chain(
            std::shared_ptr<context::IWindowSurface> window_surface,
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
            const api::BufferReference& buffer,
            const std::vector<BufferUpdate>& data) = 0;

        virtual std::tuple<api::ESwapChainResult, std::uint32_t, std::uint32_t> begin_frame(
            const api::SwapChainReference& swap_chain) = 0;

        [[nodiscard]]
        virtual std::uint32_t get_sample_count() const = 0;

        [[nodiscard]]
        virtual api::EFormat find_depth_format() const = 0;
    };


    class IGraphicsBackendCommand
    {
    public:
        virtual ~IGraphicsBackendCommand();

        virtual void copy_buffer(
            const api::CommandBufferReference& command_buffer,
            const api::BufferReference& source_buffer,
            const api::BufferReference& destination_buffer) = 0;

        virtual void copy_buffer_to_image(
            const api::CommandBufferReference& command_buffer,
            const api::BufferReference& source_buffer,
            const api::ImageReference& target_image) = 0;

        virtual api::ESwapChainResult end_frame(
            const api::CommandBufferReference& command_buffer,
            const api::SwapChainReference& swap_chain) = 0;

        virtual void generate_image_mip_maps(
            const api::CommandBufferReference& command_buffer,
            const api::ImageReference& image) = 0;

        virtual void begin_command_buffer(
            const api::CommandBufferReference& command_buffer,
            api::ECommandBufferType command_buffer_type) = 0;

        virtual void end_command_buffer(const api::CommandBufferReference& command_buffer) = 0;

        virtual void submit_command_buffer(const api::CommandBufferReference& command_buffer) = 0;

        virtual void set_vertex_buffer_list(
            const api::CommandBufferReference& command_buffer,
            const std::vector<api::BufferReference>& vertex_buffer_list,
            const std::vector<std::uint32_t>& vertex_buffer_offset_list,
            std::uint32_t first_slot) = 0;

        virtual void set_index_buffer(
            const api::CommandBufferReference& command_buffer,
            const api::BufferReference& index_buffer,
            std::uint32_t first_index) = 0;

        virtual void push_constants(
            const api::CommandBufferReference& command_buffer,
            const api::GraphicsPipelineReference& graphics_pipeline,
            api::EShaderType shader_type,
            std::uint32_t offset,
            std::uint32_t byte_size,
            void* data_byte) = 0;

        virtual void draw_indexed(
            const api::CommandBufferReference& command_buffer,
            std::uint32_t index_counts,
            std::uint32_t instance_counts,
            std::uint32_t first_index,
            std::uint32_t vertex_offset,
            std::uint32_t first_instance) = 0;

        virtual void transit_image_layout(
            const api::CommandBufferReference& command_buffer,
            const api::ImageReference& image,
            api::EImageLayout new_image_layout) = 0;

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
            const std::vector<api::DescriptorSetReference>& descriptor_set_list) = 0;

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