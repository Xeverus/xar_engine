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
        : public meta::TResourceMap<api::BufferTag, native::vulkan::VulkanBuffer>
          , public meta::TResourceMap<api::CommandBufferTag, native::vulkan::VulkanCommandBuffer>
          , public meta::TResourceMap<api::DescriptorPoolTag, native::vulkan::VulkanDescriptorPool>
          , public meta::TResourceMap<api::DescriptorSetTag, native::vulkan::VulkanDescriptorSet>
          , public meta::TResourceMap<api::DescriptorSetLayoutTag, native::vulkan::VulkanDescriptorSetLayout>
          , public meta::TResourceMap<api::GrahicsPipelineTag, native::vulkan::VulkanGraphicsPipeline>
          , public meta::TResourceMap<api::ImageTag, native::vulkan::VulkanImage>
          , public meta::TResourceMap<api::ImageViewTag, native::vulkan::VulkanImageView>
          , public meta::TResourceMap<api::QueueTag, native::vulkan::VulkanQueue>
          , public meta::TResourceMap<api::SamplerTag, native::vulkan::VulkanSampler>
          , public meta::TResourceMap<api::ShaderTag, native::vulkan::VulkanShader>
          , public meta::TResourceMap<api::SwapChainTag, native::vulkan::VulkanSwapChain>
    {
    public:
        using meta::TResourceMap<api::BufferTag, native::vulkan::VulkanBuffer>::get;
        using meta::TResourceMap<api::CommandBufferTag, native::vulkan::VulkanCommandBuffer>::get;
        using meta::TResourceMap<api::DescriptorPoolTag, native::vulkan::VulkanDescriptorPool>::get;
        using meta::TResourceMap<api::DescriptorSetTag, native::vulkan::VulkanDescriptorSet>::get;
        using meta::TResourceMap<api::DescriptorSetLayoutTag, native::vulkan::VulkanDescriptorSetLayout>::get;
        using meta::TResourceMap<api::GrahicsPipelineTag, native::vulkan::VulkanGraphicsPipeline>::get;
        using meta::TResourceMap<api::ImageTag, native::vulkan::VulkanImage>::get;
        using meta::TResourceMap<api::ImageViewTag, native::vulkan::VulkanImageView>::get;
        using meta::TResourceMap<api::QueueTag, native::vulkan::VulkanQueue>::get;
        using meta::TResourceMap<api::SamplerTag, native::vulkan::VulkanSampler>::get;
        using meta::TResourceMap<api::ShaderTag, native::vulkan::VulkanShader>::get;
        using meta::TResourceMap<api::SwapChainTag, native::vulkan::VulkanSwapChain>::get;

        using meta::TResourceMap<api::BufferTag, native::vulkan::VulkanBuffer>::add;
        using meta::TResourceMap<api::CommandBufferTag, native::vulkan::VulkanCommandBuffer>::add;
        using meta::TResourceMap<api::DescriptorPoolTag, native::vulkan::VulkanDescriptorPool>::add;
        using meta::TResourceMap<api::DescriptorSetTag, native::vulkan::VulkanDescriptorSet>::add;
        using meta::TResourceMap<api::DescriptorSetLayoutTag, native::vulkan::VulkanDescriptorSetLayout>::add;
        using meta::TResourceMap<api::GrahicsPipelineTag, native::vulkan::VulkanGraphicsPipeline>::add;
        using meta::TResourceMap<api::ImageTag, native::vulkan::VulkanImage>::add;
        using meta::TResourceMap<api::ImageViewTag, native::vulkan::VulkanImageView>::add;
        using meta::TResourceMap<api::QueueTag, native::vulkan::VulkanQueue>::add;
        using meta::TResourceMap<api::SamplerTag, native::vulkan::VulkanSampler>::add;
        using meta::TResourceMap<api::ShaderTag, native::vulkan::VulkanShader>::add;
        using meta::TResourceMap<api::SwapChainTag, native::vulkan::VulkanSwapChain>::add;
    };
}