#pragma once

#include <xar_engine/graphics/graphics_backend_resource_types.hpp>
#include <xar_engine/graphics/resource_map.hpp>

#include <xar_engine/graphics/vulkan/impl/vulkan_buffer.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_command_buffer.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_command_buffer_pool.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_descriptor_pool.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_descriptor_set.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_descriptor_set_layout.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_device.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_instance.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_graphics_pipeline.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_physical_device.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_queue.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_sampler.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_shader.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_swap_chain.hpp>


namespace xar_engine::graphics::vulkan
{
    class VulkanResourceStorage
    {
    public:
        const impl::VulkanBuffer& get(const BufferReference& reference) const;
        impl::VulkanBuffer& get(const BufferReference& reference);

        const impl::VulkanCommandBuffer& get(const CommandBufferReference& reference) const;
        impl::VulkanCommandBuffer& get(const CommandBufferReference& reference);

        const impl::VulkanDescriptorPool& get(const DescriptorPoolReference& reference) const;
        impl::VulkanDescriptorPool& get(const DescriptorPoolReference& reference);

        const impl::VulkanDescriptorSet& get(const DescriptorSetReference& reference) const;
        impl::VulkanDescriptorSet& get(const DescriptorSetReference& reference);

        const impl::VulkanDescriptorSetLayout& get(const DescriptorSetLayoutReference& reference) const;
        impl::VulkanDescriptorSetLayout& get(const DescriptorSetLayoutReference& reference);

        const impl::VulkanGraphicsPipeline& get(const GraphicsPipelineReference& reference) const;
        impl::VulkanGraphicsPipeline& get(const GraphicsPipelineReference& reference);

        const impl::VulkanImage& get(const ImageReference& reference) const;
        impl::VulkanImage& get(const ImageReference& reference);

        const impl::VulkanImageView& get(const ImageViewReference& reference) const;
        impl::VulkanImageView& get(const ImageViewReference& reference);

        const impl::VulkanQueue& get(const QueueReference& reference) const;
        impl::VulkanQueue& get(const QueueReference& reference);

        const impl::VulkanSampler& get(const SamplerReference& reference) const;
        impl::VulkanSampler& get(const SamplerReference& reference);

        const impl::VulkanShader& get(const ShaderReference& reference) const;
        impl::VulkanShader& get(const ShaderReference& reference);

        const impl::VulkanSwapChain& get(const SwapChainReference& reference) const;
        impl::VulkanSwapChain& get(const SwapChainReference& reference);


        BufferReference add(impl::VulkanBuffer resource);

        CommandBufferReference add(impl::VulkanCommandBuffer resource);

        DescriptorPoolReference add(impl::VulkanDescriptorPool resource);

        DescriptorSetReference add(impl::VulkanDescriptorSet resource);

        DescriptorSetLayoutReference add(impl::VulkanDescriptorSetLayout resource);

        GraphicsPipelineReference add(impl::VulkanGraphicsPipeline resource);

        ImageReference add(impl::VulkanImage resource);

        ImageViewReference add(impl::VulkanImageView resource);

        QueueReference add(impl::VulkanQueue resource);

        SamplerReference add(impl::VulkanSampler resource);

        ShaderReference add(impl::VulkanShader resource);

        SwapChainReference add(impl::VulkanSwapChain resource);

    private:
        TResourceMap<BufferTag, impl::VulkanBuffer> _vulkan_buffer_map;
        TResourceMap<CommandBufferTag, impl::VulkanCommandBuffer> _vulkan_command_buffer_map;
        TResourceMap<DescriptorPoolTag, impl::VulkanDescriptorPool> _vulkan_descriptor_pool_map;
        TResourceMap<DescriptorSetTag, impl::VulkanDescriptorSet> _vulkan_descriptor_set_map;
        TResourceMap<DescriptorSetLayoutTag, impl::VulkanDescriptorSetLayout> _vulkan_descriptor_layout_map;
        TResourceMap<GrahicsPipelineTag, impl::VulkanGraphicsPipeline> _vulkan_graphics_pipeline_map;
        TResourceMap<ImageTag, impl::VulkanImage> _vulkan_image_map;
        TResourceMap<ImageViewTag, impl::VulkanImageView> _vulkan_image_view_map;
        TResourceMap<QueueTag, impl::VulkanQueue> _vulkan_queue_map;
        TResourceMap<SamplerTag, impl::VulkanSampler> _vulkan_sampler_map;
        TResourceMap<ShaderTag, impl::VulkanShader> _vulkan_shader_map;
        TResourceMap<SwapChainTag, impl::VulkanSwapChain> _vulkan_swap_chain_map;
    };
}