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

#include <xar_engine/graphics/vulkan/native/vulkan_buffer.hpp>
#include <xar_engine/graphics/vulkan/native/vulkan_command_buffer.hpp>
#include <xar_engine/graphics/vulkan/native/vulkan_command_buffer_pool.hpp>
#include <xar_engine/graphics/vulkan/native/vulkan_descriptor_pool.hpp>
#include <xar_engine/graphics/vulkan/native/vulkan_descriptor_set.hpp>
#include <xar_engine/graphics/vulkan/native/vulkan_descriptor_set_layout.hpp>
#include <xar_engine/graphics/vulkan/native/vulkan_device.hpp>
#include <xar_engine/graphics/vulkan/native/vulkan_instance.hpp>
#include <xar_engine/graphics/vulkan/native/vulkan_graphics_pipeline.hpp>
#include <xar_engine/graphics/vulkan/native/vulkan_physical_device.hpp>
#include <xar_engine/graphics/vulkan/native/vulkan_queue.hpp>
#include <xar_engine/graphics/vulkan/native/vulkan_sampler.hpp>
#include <xar_engine/graphics/vulkan/native/vulkan_shader.hpp>
#include <xar_engine/graphics/vulkan/native/vulkan_swap_chain.hpp>


namespace xar_engine::graphics::vulkan
{
    class VulkanResourceStorage
    {
    public:
        const native::VulkanBuffer& get(const api::BufferReference& reference) const;
        native::VulkanBuffer& get(const api::BufferReference& reference);

        const native::VulkanCommandBuffer& get(const api::CommandBufferReference& reference) const;
        native::VulkanCommandBuffer& get(const api::CommandBufferReference& reference);

        const native::VulkanDescriptorPool& get(const api::DescriptorPoolReference& reference) const;
        native::VulkanDescriptorPool& get(const api::DescriptorPoolReference& reference);

        const native::VulkanDescriptorSet& get(const api::DescriptorSetReference& reference) const;
        native::VulkanDescriptorSet& get(const api::DescriptorSetReference& reference);

        const native::VulkanDescriptorSetLayout& get(const api::DescriptorSetLayoutReference& reference) const;
        native::VulkanDescriptorSetLayout& get(const api::DescriptorSetLayoutReference& reference);

        const native::VulkanGraphicsPipeline& get(const api::GraphicsPipelineReference& reference) const;
        native::VulkanGraphicsPipeline& get(const api::GraphicsPipelineReference& reference);

        const native::VulkanImage& get(const api::ImageReference& reference) const;
        native::VulkanImage& get(const api::ImageReference& reference);

        const native::VulkanImageView& get(const api::ImageViewReference& reference) const;
        native::VulkanImageView& get(const api::ImageViewReference& reference);

        const native::VulkanQueue& get(const api::QueueReference& reference) const;
        native::VulkanQueue& get(const api::QueueReference& reference);

        const native::VulkanSampler& get(const api::SamplerReference& reference) const;
        native::VulkanSampler& get(const api::SamplerReference& reference);

        const native::VulkanShader& get(const api::ShaderReference& reference) const;
        native::VulkanShader& get(const api::ShaderReference& reference);

        const native::VulkanSwapChain& get(const api::SwapChainReference& reference) const;
        native::VulkanSwapChain& get(const api::SwapChainReference& reference);


        api::BufferReference add(native::VulkanBuffer resource);

        api::CommandBufferReference add(native::VulkanCommandBuffer resource);

        api::DescriptorPoolReference add(native::VulkanDescriptorPool resource);

        api::DescriptorSetReference add(native::VulkanDescriptorSet resource);

        api::DescriptorSetLayoutReference add(native::VulkanDescriptorSetLayout resource);

        api::GraphicsPipelineReference add(native::VulkanGraphicsPipeline resource);

        api::ImageReference add(native::VulkanImage resource);

        api::ImageViewReference add(native::VulkanImageView resource);

        api::QueueReference add(native::VulkanQueue resource);

        api::SamplerReference add(native::VulkanSampler resource);

        api::ShaderReference add(native::VulkanShader resource);

        api::SwapChainReference add(native::VulkanSwapChain resource);

    private:
        meta::TResourceMap<api::BufferTag, native::VulkanBuffer> _vulkan_buffer_map;
        meta::TResourceMap<api::CommandBufferTag, native::VulkanCommandBuffer> _vulkan_command_buffer_map;
        meta::TResourceMap<api::DescriptorPoolTag, native::VulkanDescriptorPool> _vulkan_descriptor_pool_map;
        meta::TResourceMap<api::DescriptorSetTag, native::VulkanDescriptorSet> _vulkan_descriptor_set_map;
        meta::TResourceMap<api::DescriptorSetLayoutTag, native::VulkanDescriptorSetLayout> _vulkan_descriptor_layout_map;
        meta::TResourceMap<api::GrahicsPipelineTag, native::VulkanGraphicsPipeline> _vulkan_graphics_pipeline_map;
        meta::TResourceMap<api::ImageTag, native::VulkanImage> _vulkan_image_map;
        meta::TResourceMap<api::ImageViewTag, native::VulkanImageView> _vulkan_image_view_map;
        meta::TResourceMap<api::QueueTag, native::VulkanQueue> _vulkan_queue_map;
        meta::TResourceMap<api::SamplerTag, native::VulkanSampler> _vulkan_sampler_map;
        meta::TResourceMap<api::ShaderTag, native::VulkanShader> _vulkan_shader_map;
        meta::TResourceMap<api::SwapChainTag, native::VulkanSwapChain> _vulkan_swap_chain_map;
    };
}