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
        api::BufferReference make_staging_buffer(std::uint32_t byte_size) override;
        api::BufferReference make_vertex_buffer(std::uint32_t byte_size) override;
        api::BufferReference make_index_buffer(std::uint32_t byte_size) override;
        api::BufferReference make_uniform_buffer(std::uint32_t byte_size) override;

        std::vector<api::CommandBufferReference> make_command_buffer_list(std::uint32_t buffer_counts) override;

        api::DescriptorPoolReference make_descriptor_pool(const std::set<api::EDescriptorPoolType>& descriptor_pool_type_list) override;

        api::DescriptorSetLayoutReference make_descriptor_set_layout(const std::set<api::EDescriptorPoolType>& descriptor_pool_type_list) override;

        std::vector<api::DescriptorSetReference> make_descriptor_set_list(
            const api::DescriptorPoolReference& descriptor_pool,
            const api::DescriptorSetLayoutReference& descriptor_set_layout,
            std::uint32_t descriptor_counts) override;

        void write_descriptor_set(
            const api::DescriptorSetReference& descriptor_set,
            std::uint32_t uniform_buffer_first_index,
            const std::vector<api::BufferReference>& uniform_buffer_list,
            std::uint32_t texture_image_first_index,
            const std::vector<api::ImageViewReference>& texture_image_view_list,
            const std::vector<api::SamplerReference>& sampler_list) override;

        api::GraphicsPipelineReference make_graphics_pipeline(
            const std::vector<api::DescriptorSetLayoutReference>& descriptor_set_layout_list,
            const api::ShaderReference& vertex_shader,
            const api::ShaderReference& fragment_shader,
            const std::vector<api::VertexInputAttribute>& vertex_input_attribute_list,
            const std::vector<api::VertexInputBinding>& vertex_input_binding_list,
            api::EFormat color_format,
            api::EFormat depth_format,
            std::uint32_t sample_counts) override;

        api::ImageReference make_image(
            api::EImageType image_type,
            math::Vector3i32 dimension,
            api::EFormat image_format,
            std::uint32_t mip_levels,
            std::uint32_t sample_count) override;

        api::ImageViewReference make_image_view(
            const api::ImageReference& image,
            api::EImageAspect image_aspect,
            std::uint32_t mip_levels) override;

        api::SamplerReference make_sampler(float mip_levels) override;

        api::ShaderReference make_shader(const std::vector<char>& shader_byte_code) override;

        api::SwapChainReference make_swap_chain(
            std::shared_ptr<IWindowSurface> window_surface,
            std::uint32_t buffering_level) override;


    public: // IGraphicsBackendHostCommand
        void update_buffer(
            const api::BufferReference& buffer,
            const std::vector<BufferUpdate>& data) override;

        std::tuple<api::ESwapChainResult, std::uint32_t, std::uint32_t> begin_frame(const api::SwapChainReference& swap_chain) override;

        [[nodiscard]]
        std::uint32_t get_sample_count() const override;

        [[nodiscard]]
        api::EFormat find_depth_format() const override;


    public: // IGraphicsBackendDeviceCommand
        void copy_buffer(
            const api::CommandBufferReference& command_buffer,
            const api::BufferReference& source_buffer,
            const api::BufferReference& destination_buffer) override;

        void copy_buffer_to_image(
            const api::CommandBufferReference& command_buffer,
            const api::BufferReference& source_buffer,
            const api::ImageReference& target_image) override;

        api::ESwapChainResult end_frame(
            const api::CommandBufferReference& command_buffer,
            const api::SwapChainReference& swap_chain) override;

        void generate_image_mip_maps(
            const api::CommandBufferReference& command_buffer,
            const api::ImageReference& image) override;

        void begin_command_buffer(
            const api::CommandBufferReference& command_buffer,
            api::ECommandBufferType command_buffer_type) override;

        void end_command_buffer(const api::CommandBufferReference& command_buffer) override;

        void submit_command_buffer(const api::CommandBufferReference& command_buffer) override;

        void set_vertex_buffer_list(
            const api::CommandBufferReference& command_buffer,
            const std::vector<api::BufferReference>& vertex_buffer_list,
            const std::vector<std::uint32_t>& vertex_buffer_offset_list,
            std::uint32_t first_slot) override;

        void set_index_buffer(
            const api::CommandBufferReference& command_buffer,
            const api::BufferReference& index_buffer,
            std::uint32_t first_index) override;

        void push_constants(
            const api::CommandBufferReference& command_buffer,
            const api::GraphicsPipelineReference& graphics_pipeline,
            api::EShaderType shader_type,
            std::uint32_t offset,
            std::uint32_t byte_size,
            void* data_byte) override;

        void draw_indexed(
            const api::CommandBufferReference& command_buffer,
            std::uint32_t index_counts,
            std::uint32_t instance_counts,
            std::uint32_t first_index,
            std::uint32_t vertex_offset,
            std::uint32_t first_instance) override;

        void transit_image_layout(
            const api::CommandBufferReference& command_buffer,
            const api::ImageReference& image,
            api::EImageLayout new_image_layout) override;

        void wait_idle() override;

        void begin_rendering(
            const api::CommandBufferReference& command_buffer,
            const api::SwapChainReference& swap_chain,
            std::uint32_t image_index,
            const api::ImageViewReference& color_image_view,
            const api::ImageViewReference& depth_image_view) override;

        void set_pipeline_state(
            const api::CommandBufferReference& command_buffer,
            const api::SwapChainReference& swap_chain,
            const api::GraphicsPipelineReference& graphics_pipeline,
            const std::vector<api::DescriptorSetReference>& descriptor_set_list) override;

        void end_rendering(
            const api::CommandBufferReference& command_buffer,
            const api::SwapChainReference& swap_chain,
            std::uint32_t image_index) override;

    private:
        std::shared_ptr<native::VulkanInstance> _vulkan_instance;
        std::vector<native::VulkanPhysicalDevice> _vulkan_physical_device_list;
        native::VulkanDevice _vulkan_device;
        native::VulkanQueue _vulkan_graphics_queue;
        native::VulkanCommandBufferPool _vulkan_command_buffer_pool;
        VulkanResourceStorage _vulkan_resource_storage;
    };
}