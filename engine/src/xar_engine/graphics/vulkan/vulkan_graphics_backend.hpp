#pragma once

#include <memory>
#include <vector>

#include <xar_engine/graphics/graphics_backend.hpp>

#include <xar_engine/graphics/vulkan/vulkan_resource_storage.hpp>
#include <xar_engine/graphics/vulkan/vulkan_window_surface.hpp>


namespace xar_engine::graphics::vulkan
{
    class VulkanGraphicsBackend
        : public IGraphicsBackend
          , public IGraphicsBackendResource
          , public IGraphicsBackendHost
          , public IGraphicsBackendCommand
    {
    public:
        VulkanGraphicsBackend();


    public: // IGraphicsBackend
        const IGraphicsBackendResource& resource() const override;
        IGraphicsBackendResource& resource() override;

        const IGraphicsBackendHost& host() const override;
        IGraphicsBackendHost& host() override;

        const IGraphicsBackendCommand& command() const override;
        IGraphicsBackendCommand& command() override;


    public: // IGraphicsBackendResource
        BufferReference make_staging_buffer(std::uint32_t byte_size) override;
        BufferReference make_vertex_buffer(std::uint32_t byte_size) override;
        BufferReference make_index_buffer(std::uint32_t byte_size) override;
        BufferReference make_uniform_buffer(std::uint32_t byte_size) override;

        std::vector<CommandBufferReference> make_command_buffer_list(std::uint32_t buffer_counts) override;

        DescriptorPoolReference make_descriptor_pool() override;

        std::vector<DescriptorSetReference> make_descriptor_set_list(
            const DescriptorPoolReference& descriptor_pool,
            const DescriptorSetLayoutReference& descriptor_set_layout,
            const std::vector<BufferReference>& uniform_buffer_list,
            const ImageViewReference& texture_image_view,
            const SamplerReference& sampler) override;

        DescriptorSetLayoutReference make_descriptor_set_layout() override;

        GraphicsPipelineReference make_graphics_pipeline(
            const DescriptorSetLayoutReference& descriptor_set_layout,
            const ShaderReference& vertex_shader,
            const ShaderReference& fragment_shader,
            const std::vector<VertexInputAttribute>& vertex_input_attribute_list,
            const std::vector<VertexInputBinding>& vertex_input_binding_list,
            EFormat color_format,
            EFormat depth_format,
            std::uint32_t sample_counts) override;

        ImageReference make_image(
            EImageType image_type,
            math::Vector3i32 dimension,
            EFormat image_format,
            std::uint32_t mip_levels,
            std::uint32_t sample_count) override;

        ImageViewReference make_image_view(
            const ImageReference& image,
            EImageAspect image_aspect,
            std::uint32_t mip_levels) override;

        SamplerReference make_sampler(float mip_levels) override;

        ShaderReference make_shader(const std::vector<char>& shader_byte_code) override;

        SwapChainReference make_swap_chain(
            std::shared_ptr<IWindowSurface> window_surface,
            std::uint32_t buffering_level) override;


    public: // IGraphicsBackendHostCommand
        void update_buffer(
            const BufferReference& buffer,
            void* data,
            std::uint32_t data_byte_size) override;

        std::tuple<ESwapChainResult, std::uint32_t, std::uint32_t> begin_frame(const SwapChainReference& swap_chain) override;

        [[nodiscard]]
        std::uint32_t get_sample_count() const override;

        [[nodiscard]]
        EFormat find_depth_format() const override;


    public: // IGraphicsBackendDeviceCommand
        void copy_buffer(
            const CommandBufferReference& command_buffer,
            const BufferReference& source_buffer,
            const BufferReference& destination_buffer) override;

        void copy_buffer_to_image(
            const CommandBufferReference& command_buffer,
            const BufferReference& source_buffer,
            const ImageReference& target_image) override;

        ESwapChainResult end_frame(
            const CommandBufferReference& command_buffer,
            const SwapChainReference& swap_chain) override;

        void generate_image_mip_maps(
            const CommandBufferReference& command_buffer,
            const ImageReference& image) override;

        void begin_command_buffer(
            const CommandBufferReference& command_buffer,
            ECommandBufferType command_buffer_type) override;

        void end_command_buffer(const CommandBufferReference& command_buffer) override;

        void submit_command_buffer(const CommandBufferReference& command_buffer) override;

        void set_vertex_buffer_list(
            const CommandBufferReference& command_buffer,
            const std::vector<BufferReference>& vertex_buffer_list,
            const std::vector<std::uint32_t>& vertex_buffer_offset_list,
            std::uint32_t first_slot) override;

        void set_index_buffer(
            const CommandBufferReference& command_buffer,
            const BufferReference& index_buffer,
            std::uint32_t first_index) override;

        void push_constants(
            const CommandBufferReference& command_buffer,
            const GraphicsPipelineReference& graphics_pipeline,
            EShaderType shader_type,
            std::uint32_t offset,
            std::uint32_t byte_size,
            void* data_byte) override;

        void draw_indexed(
            const CommandBufferReference& command_buffer,
            std::uint32_t index_counts,
            std::uint32_t instance_counts,
            std::uint32_t first_index,
            std::uint32_t vertex_offset,
            std::uint32_t first_instance) override;

        void transit_image_layout(
            const CommandBufferReference& command_buffer,
            const ImageReference& image,
            EImageLayout new_image_layout) override;

        void wait_idle() override;

        void TMP_FRAME_START(
            const CommandBufferReference& command_buffer,
            const SwapChainReference& swap_chain,
            const GraphicsPipelineReference& graphics_pipeline,
            std::uint32_t image_index,
            const DescriptorSetReference& descriptor_set,
            const ImageViewReference& color_image_view,
            const ImageViewReference& depth_image_view) override;

        void TMP_FRAME_END(
            const CommandBufferReference& command_buffer,
            const SwapChainReference& swap_chain,
            std::uint32_t image_index) override;

    private:
        std::shared_ptr<impl::VulkanInstance> _vulkan_instance;
        std::vector<impl::VulkanPhysicalDevice> _vulkan_physical_device_list;
        impl::VulkanDevice _vulkan_device;
        impl::VulkanQueue _vulkan_graphics_queue;
        impl::VulkanCommandBufferPool _vulkan_command_buffer_pool;
        VulkanResourceStorage _vulkan_resource_storage;
    };
}