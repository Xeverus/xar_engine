#pragma once

#include <cstdint>
#include <vector>

#include <xar_engine/graphics/graphics_backend_resource_types.hpp>
#include <xar_engine/graphics/resource_reference.hpp>
#include <xar_engine/graphics/window_surface.hpp>

#include <xar_engine/math/vector.hpp>


namespace xar_engine::graphics
{
    class IGraphicsBackendResource
    {
    public:
        virtual ~IGraphicsBackendResource();


        virtual BufferReference make_staging_buffer(std::uint32_t byte_size) = 0;
        virtual BufferReference make_vertex_buffer(std::uint32_t byte_size) = 0;
        virtual BufferReference make_index_buffer(std::uint32_t byte_size) = 0;
        virtual BufferReference make_uniform_buffer(std::uint32_t byte_size) = 0;

        virtual std::vector<CommandBufferReference> make_command_buffers(std::uint32_t buffer_counts) = 0;
        virtual CommandBufferReference make_one_time_command_buffer() = 0;

        virtual DescriptorPoolReference make_descriptor_pool() = 0;

        virtual DescriptorSetListReference make_descriptor_set_list(
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
            EImageFormat color_format,
            EImageFormat depth_format,
            std::uint32_t sample_counts) = 0;

        virtual ImageReference make_image(
            EImageType image_type,
            math::Vector3i32 dimension,
            EImageFormat image_format,
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


    class IGraphicsBackendHostCommand
    {
    public:
        virtual ~IGraphicsBackendHostCommand();

        virtual void update_buffer(
            const BufferReference& buffer,
            void* data,
            std::uint32_t data_byte_size) = 0;

        virtual std::tuple<ESwapChainResult, std::uint32_t, std::uint32_t> begin_frame(
            const SwapChainReference& swap_chain) = 0;

        [[nodiscard]]
        virtual std::uint32_t get_sample_count() const = 0;
    };


    class IGraphicsBackendDeviceCommand
    {
    public:
        virtual ~IGraphicsBackendDeviceCommand();

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

        virtual void submit_one_time_command_buffer(const CommandBufferReference& command_buffer) = 0;

        virtual void transit_image_layout(
            const CommandBufferReference& command_buffer,
            const ImageReference& image,
            EImageLayout new_image_layout) = 0;

        virtual void wait_idle() = 0;

        virtual void TMP_RECORD_FRAME(
            const CommandBufferReference& command_buffer,
            const SwapChainReference& swap_chain,
            const GraphicsPipelineReference& graphics_pipeline,
            std::uint32_t image_index,
            std::uint32_t frame_buffer_index,
            const DescriptorSetListReference& descriptor_set_list,
            const BufferReference& vertex_buffer,
            const BufferReference& index_buffer,
            const ImageViewReference& color_image_view,
            const ImageViewReference& depth_image_view,
            std::uint32_t index_counts) = 0;
    };


    class IGraphicsBackend
    {
    public:
        virtual ~IGraphicsBackend();

        [[nodiscard]]
        virtual const IGraphicsBackendResource& resource() const = 0;
        virtual IGraphicsBackendResource& resource() = 0;

        [[nodiscard]]
        virtual const IGraphicsBackendHostCommand& host_command() const = 0;
        virtual IGraphicsBackendHostCommand& host_command() = 0;

        [[nodiscard]]
        virtual const IGraphicsBackendDeviceCommand& device_command() const = 0;
        virtual IGraphicsBackendDeviceCommand& device_command() = 0;
    };
}