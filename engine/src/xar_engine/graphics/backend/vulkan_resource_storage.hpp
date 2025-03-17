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


namespace xar_engine::graphics::backend
{
    class VulkanResourceStorage
    {
    public:
        const vulkan::native::VulkanBuffer& get(const api::BufferReference& reference) const;
        vulkan::native::VulkanBuffer& get(const api::BufferReference& reference);

        const vulkan::native::VulkanCommandBuffer& get(const api::CommandBufferReference& reference) const;
        vulkan::native::VulkanCommandBuffer& get(const api::CommandBufferReference& reference);

        const vulkan::native::VulkanDescriptorPool& get(const api::DescriptorPoolReference& reference) const;
        vulkan::native::VulkanDescriptorPool& get(const api::DescriptorPoolReference& reference);

        const vulkan::native::VulkanDescriptorSet& get(const api::DescriptorSetReference& reference) const;
        vulkan::native::VulkanDescriptorSet& get(const api::DescriptorSetReference& reference);

        const vulkan::native::VulkanDescriptorSetLayout& get(const api::DescriptorSetLayoutReference& reference) const;
        vulkan::native::VulkanDescriptorSetLayout& get(const api::DescriptorSetLayoutReference& reference);

        const vulkan::native::VulkanGraphicsPipeline& get(const api::GraphicsPipelineReference& reference) const;
        vulkan::native::VulkanGraphicsPipeline& get(const api::GraphicsPipelineReference& reference);

        const vulkan::native::VulkanImage& get(const api::ImageReference& reference) const;
        vulkan::native::VulkanImage& get(const api::ImageReference& reference);

        const vulkan::native::VulkanImageView& get(const api::ImageViewReference& reference) const;
        vulkan::native::VulkanImageView& get(const api::ImageViewReference& reference);

        const vulkan::native::VulkanQueue& get(const api::QueueReference& reference) const;
        vulkan::native::VulkanQueue& get(const api::QueueReference& reference);

        const vulkan::native::VulkanSampler& get(const api::SamplerReference& reference) const;
        vulkan::native::VulkanSampler& get(const api::SamplerReference& reference);

        const vulkan::native::VulkanShader& get(const api::ShaderReference& reference) const;
        vulkan::native::VulkanShader& get(const api::ShaderReference& reference);

        const vulkan::native::VulkanSwapChain& get(const api::SwapChainReference& reference) const;
        vulkan::native::VulkanSwapChain& get(const api::SwapChainReference& reference);


        api::BufferReference add(vulkan::native::VulkanBuffer resource);

        api::CommandBufferReference add(vulkan::native::VulkanCommandBuffer resource);

        api::DescriptorPoolReference add(vulkan::native::VulkanDescriptorPool resource);

        api::DescriptorSetReference add(vulkan::native::VulkanDescriptorSet resource);

        api::DescriptorSetLayoutReference add(vulkan::native::VulkanDescriptorSetLayout resource);

        api::GraphicsPipelineReference add(vulkan::native::VulkanGraphicsPipeline resource);

        api::ImageReference add(vulkan::native::VulkanImage resource);

        api::ImageViewReference add(vulkan::native::VulkanImageView resource);

        api::QueueReference add(vulkan::native::VulkanQueue resource);

        api::SamplerReference add(vulkan::native::VulkanSampler resource);

        api::ShaderReference add(vulkan::native::VulkanShader resource);

        api::SwapChainReference add(vulkan::native::VulkanSwapChain resource);

    private:
        meta::TResourceMap<api::BufferTag, vulkan::native::VulkanBuffer> _vulkan_buffer_map;
        meta::TResourceMap<api::CommandBufferTag, vulkan::native::VulkanCommandBuffer> _vulkan_command_buffer_map;
        meta::TResourceMap<api::DescriptorPoolTag, vulkan::native::VulkanDescriptorPool> _vulkan_descriptor_pool_map;
        meta::TResourceMap<api::DescriptorSetTag, vulkan::native::VulkanDescriptorSet> _vulkan_descriptor_set_map;
        meta::TResourceMap<api::DescriptorSetLayoutTag, vulkan::native::VulkanDescriptorSetLayout> _vulkan_descriptor_layout_map;
        meta::TResourceMap<api::GrahicsPipelineTag, vulkan::native::VulkanGraphicsPipeline> _vulkan_graphics_pipeline_map;
        meta::TResourceMap<api::ImageTag, vulkan::native::VulkanImage> _vulkan_image_map;
        meta::TResourceMap<api::ImageViewTag, vulkan::native::VulkanImageView> _vulkan_image_view_map;
        meta::TResourceMap<api::QueueTag, vulkan::native::VulkanQueue> _vulkan_queue_map;
        meta::TResourceMap<api::SamplerTag, vulkan::native::VulkanSampler> _vulkan_sampler_map;
        meta::TResourceMap<api::ShaderTag, vulkan::native::VulkanShader> _vulkan_shader_map;
        meta::TResourceMap<api::SwapChainTag, vulkan::native::VulkanSwapChain> _vulkan_swap_chain_map;
    };
}