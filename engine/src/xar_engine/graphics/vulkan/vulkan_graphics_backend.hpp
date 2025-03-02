#pragma once

#include <memory>
#include <vector>

#include <xar_engine/graphics/graphics_backend.hpp>
#include <xar_engine/graphics/resource_map.hpp>

#include <xar_engine/graphics/vulkan/vulkan_window_surface.hpp>

#include <xar_engine/graphics/vulkan/impl/vulkan_buffer.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_command_buffer_pool.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_descriptor_pool.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_descriptor_set.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_descriptor_set_layout.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_device.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_instance.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_graphics_pipeline.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_physical_device.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_sampler.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_shader.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_swap_chain.hpp>


namespace xar_engine::graphics::vulkan
{
    class VulkanGraphicsBackend
        : public IGraphicsBackend
          , public IGraphicsBackendResource
          , public IGraphicsBackendHostCommand
          , public IGraphicsBackendDeviceCommand
    {
    public:
        VulkanGraphicsBackend();


    public: // IGraphicsBackend
        const IGraphicsBackendResource& resource() const override;
        IGraphicsBackendResource& resource() override;

        const IGraphicsBackendHostCommand& host_command() const override;
        IGraphicsBackendHostCommand& host_command() override;

        const IGraphicsBackendDeviceCommand& device_command() const override;
        IGraphicsBackendDeviceCommand& device_command() override;


    public: // IGraphicsBackendResource
        BufferReference make_staging_buffer(std::uint32_t byte_size) override;
        BufferReference make_vertex_buffer(std::uint32_t byte_size) override;
        BufferReference make_index_buffer(std::uint32_t byte_size) override;
        BufferReference make_uniform_buffer(std::uint32_t byte_size) override;

        std::vector<CommandBufferReference> make_command_buffers(std::uint32_t buffer_counts) override;
        CommandBufferReference make_one_time_command_buffer() override;

        DescriptorPoolReference make_descriptor_pool() override;

        DescriptorSetListReference make_descriptor_set_list(
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
            EImageFormat color_format,
            EImageFormat depth_format,
            std::uint32_t sample_counts) override;

        ImageReference make_image(
            EImageType image_type,
            math::Vector3i32 dimension,
            EImageFormat image_format,
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


    public: // IGraphicsBackendDeviceCommand
        void copy_buffer(
            const CommandBufferReference& command_buffer,
            const BufferReference& source_buffer,
            const BufferReference& destination_buffer) override;

        void copy_buffer_to_image(
            const CommandBufferReference& command_buffer,
            const BufferReference& source_buffer,
            const ImageReference& target_image) override;

        void generate_image_mip_maps(
            const CommandBufferReference& command_buffer,
            const ImageReference& image) override;

        void submit_one_time_command_buffer(const CommandBufferReference& command_buffer) override;

        void transit_image_layout(
            const CommandBufferReference& command_buffer,
            const ImageReference& image,
            EImageLayout new_image_layout) override;

        void wait_idle() override;

    private:
        impl::VulkanBuffer& get_object(const BufferReference& reference);
        VkCommandBuffer get_object(const CommandBufferReference& reference);
        impl::VulkanDescriptorPool get_object(const DescriptorPoolReference& reference);
        impl::VulkanDescriptorSet get_object(const DescriptorSetListReference& reference);
        impl::VulkanDescriptorSetLayout get_object(const DescriptorSetLayoutReference& reference);
        impl::VulkanImage get_object(const ImageReference& reference);
        impl::VulkanImageView get_object(const ImageViewReference& reference);
        impl::VulkanSampler get_object(const SamplerReference& reference);
        impl::VulkanShader get_object(const ShaderReference& reference);

    private:
        TResourceMap<BufferTag, impl::VulkanBuffer> _vulkan_buffer_map;
        TResourceMap<CommandBufferTag, VkCommandBuffer> _vulkan_command_buffer_map;
        TResourceMap<DescriptorPoolTag, impl::VulkanDescriptorPool> _vulkan_descriptor_pool_map;
        TResourceMap<DescriptorSetListTag, impl::VulkanDescriptorSet> _vulkan_descriptor_set_map;
        TResourceMap<DescriptorSetLayoutTag, impl::VulkanDescriptorSetLayout> _vulkan_descriptor_layout_map;
        TResourceMap<GrahicsPipelineTag, impl::VulkanGraphicsPipeline> _vulkan_graphics_pipeline_map;
        TResourceMap<ImageTag, impl::VulkanImage> _vulkan_image_map;
        TResourceMap<ImageViewTag, impl::VulkanImageView> _vulkan_image_view_map;
        TResourceMap<SamplerTag, impl::VulkanSampler> _vulkan_sampler_map;
        TResourceMap<ShaderTag, impl::VulkanShader> _vulkan_shader_map;
        TResourceMap<SwapChainTag, impl::VulkanSwapChain> _vulkan_swap_chain_map;

        std::shared_ptr<impl::VulkanInstance> _vulkan_instance;
        std::vector<impl::VulkanPhysicalDevice> _vulkan_physical_device_list;
        impl::VulkanDevice _vulkan_device;
        impl::VulkanCommandBufferPool _vulkan_command_buffer_pool;

        VkSampleCountFlagBits _vk_sample_count_flag_bits;
    };
}