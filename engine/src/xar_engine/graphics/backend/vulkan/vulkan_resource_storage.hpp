#pragma once

#include <xar_engine/meta/resource_map.hpp>

#include <xar_engine/graphics/api/buffer_reference.hpp>
#include <xar_engine/graphics/api/command_buffer_reference.hpp>
#include <xar_engine/graphics/api/descriptor_pool_reference.hpp>
#include <xar_engine/graphics/api/descriptor_set_layout_reference.hpp>
#include <xar_engine/graphics/api/descriptor_set_reference.hpp>
#include <xar_engine/graphics/api/graphics_pipeline_reference.hpp>
#include <xar_engine/graphics/api/image_reference.hpp>
#include <xar_engine/graphics/api/image_view_reference.hpp>
#include <xar_engine/graphics/api/queue_reference.hpp>
#include <xar_engine/graphics/api/sampler_reference.hpp>
#include <xar_engine/graphics/api/shader_reference.hpp>
#include <xar_engine/graphics/api/surface_reference.hpp>
#include <xar_engine/graphics/api/swap_chain_reference.hpp>

#include <xar_engine/graphics/native/vulkan/vulkan_buffer.hpp>
#include <xar_engine/graphics/native/vulkan/vulkan_command_buffer.hpp>
#include <xar_engine/graphics/native/vulkan/vulkan_command_buffer_pool.hpp>
#include <xar_engine/graphics/native/vulkan/vulkan_descriptor_pool.hpp>
#include <xar_engine/graphics/native/vulkan/vulkan_descriptor_set.hpp>
#include <xar_engine/graphics/native/vulkan/vulkan_descriptor_set_layout.hpp>
#include <xar_engine/graphics/native/vulkan/vulkan_device.hpp>
#include <xar_engine/graphics/native/vulkan/vulkan_instance.hpp>
#include <xar_engine/graphics/native/vulkan/vulkan_graphics_pipeline.hpp>
#include <xar_engine/graphics/native/vulkan/vulkan_physical_device.hpp>
#include <xar_engine/graphics/native/vulkan/vulkan_queue.hpp>
#include <xar_engine/graphics/native/vulkan/vulkan_sampler.hpp>
#include <xar_engine/graphics/native/vulkan/vulkan_shader.hpp>
#include <xar_engine/graphics/native/vulkan/vulkan_swap_chain.hpp>


namespace xar_engine::graphics::backend::vulkan
{
    class VulkanResourceStorage
    {
    public:
        const native::vulkan::VulkanBuffer& get(const api::BufferReference& reference) const;
        native::vulkan::VulkanBuffer& get(const api::BufferReference& reference);

        const native::vulkan::VulkanCommandBuffer& get(const api::CommandBufferReference& reference) const;
        native::vulkan::VulkanCommandBuffer& get(const api::CommandBufferReference& reference);

        const native::vulkan::VulkanDescriptorPool& get(const api::DescriptorPoolReference& reference) const;
        native::vulkan::VulkanDescriptorPool& get(const api::DescriptorPoolReference& reference);

        const native::vulkan::VulkanDescriptorSet& get(const api::DescriptorSetReference& reference) const;
        native::vulkan::VulkanDescriptorSet& get(const api::DescriptorSetReference& reference);

        const native::vulkan::VulkanDescriptorSetLayout& get(const api::DescriptorSetLayoutReference& reference) const;
        native::vulkan::VulkanDescriptorSetLayout& get(const api::DescriptorSetLayoutReference& reference);

        const native::vulkan::VulkanGraphicsPipeline& get(const api::GraphicsPipelineReference& reference) const;
        native::vulkan::VulkanGraphicsPipeline& get(const api::GraphicsPipelineReference& reference);

        const native::vulkan::VulkanImage& get(const api::ImageReference& reference) const;
        native::vulkan::VulkanImage& get(const api::ImageReference& reference);

        const native::vulkan::VulkanImageView& get(const api::ImageViewReference& reference) const;
        native::vulkan::VulkanImageView& get(const api::ImageViewReference& reference);

        const native::vulkan::VulkanQueue& get(const api::QueueReference& reference) const;
        native::vulkan::VulkanQueue& get(const api::QueueReference& reference);

        const native::vulkan::VulkanSampler& get(const api::SamplerReference& reference) const;
        native::vulkan::VulkanSampler& get(const api::SamplerReference& reference);

        const native::vulkan::VulkanShader& get(const api::ShaderReference& reference) const;
        native::vulkan::VulkanShader& get(const api::ShaderReference& reference);

        const native::vulkan::VulkanSwapChain& get(const api::SwapChainReference& reference) const;
        native::vulkan::VulkanSwapChain& get(const api::SwapChainReference& reference);


        api::BufferReference add(native::vulkan::VulkanBuffer resource);

        api::CommandBufferReference add(native::vulkan::VulkanCommandBuffer resource);

        api::DescriptorPoolReference add(native::vulkan::VulkanDescriptorPool resource);

        api::DescriptorSetReference add(native::vulkan::VulkanDescriptorSet resource);

        api::DescriptorSetLayoutReference add(native::vulkan::VulkanDescriptorSetLayout resource);

        api::GraphicsPipelineReference add(native::vulkan::VulkanGraphicsPipeline resource);

        api::ImageReference add(native::vulkan::VulkanImage resource);

        api::ImageViewReference add(native::vulkan::VulkanImageView resource);

        api::QueueReference add(native::vulkan::VulkanQueue resource);

        api::SamplerReference add(native::vulkan::VulkanSampler resource);

        api::ShaderReference add(native::vulkan::VulkanShader resource);

        api::SwapChainReference add(native::vulkan::VulkanSwapChain resource);

    private:
        meta::TResourceMap<api::BufferTag, native::vulkan::VulkanBuffer> _vulkan_buffer_map;
        meta::TResourceMap<api::CommandBufferTag, native::vulkan::VulkanCommandBuffer> _vulkan_command_buffer_map;
        meta::TResourceMap<api::DescriptorPoolTag, native::vulkan::VulkanDescriptorPool> _vulkan_descriptor_pool_map;
        meta::TResourceMap<api::DescriptorSetTag, native::vulkan::VulkanDescriptorSet> _vulkan_descriptor_set_map;
        meta::TResourceMap<api::DescriptorSetLayoutTag, native::vulkan::VulkanDescriptorSetLayout> _vulkan_descriptor_layout_map;
        meta::TResourceMap<api::GrahicsPipelineTag, native::vulkan::VulkanGraphicsPipeline> _vulkan_graphics_pipeline_map;
        meta::TResourceMap<api::ImageTag, native::vulkan::VulkanImage> _vulkan_image_map;
        meta::TResourceMap<api::ImageViewTag, native::vulkan::VulkanImageView> _vulkan_image_view_map;
        meta::TResourceMap<api::QueueTag, native::vulkan::VulkanQueue> _vulkan_queue_map;
        meta::TResourceMap<api::SamplerTag, native::vulkan::VulkanSampler> _vulkan_sampler_map;
        meta::TResourceMap<api::ShaderTag, native::vulkan::VulkanShader> _vulkan_shader_map;
        meta::TResourceMap<api::SwapChainTag, native::vulkan::VulkanSwapChain> _vulkan_swap_chain_map;
    };
}