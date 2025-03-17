#include <xar_engine/graphics/backend/vulkan/vulkan_resource_storage.hpp>


namespace xar_engine::graphics::backend::vulkan
{

    const native::vulkan::VulkanBuffer& VulkanResourceStorage::get(const api::BufferReference& reference) const
    {
        return _vulkan_buffer_map.get_object(reference);
    }

    native::vulkan::VulkanBuffer& VulkanResourceStorage::get(const api::BufferReference& reference)
    {
        return _vulkan_buffer_map.get_object(reference);
    }

    const native::vulkan::VulkanCommandBuffer& VulkanResourceStorage::get(const api::CommandBufferReference& reference) const
    {
        return _vulkan_command_buffer_map.get_object(reference);
    }

    native::vulkan::VulkanCommandBuffer& VulkanResourceStorage::get(const api::CommandBufferReference& reference)
    {
        return _vulkan_command_buffer_map.get_object(reference);
    }

    const native::vulkan::VulkanDescriptorPool& VulkanResourceStorage::get(const api::DescriptorPoolReference& reference) const
    {
        return _vulkan_descriptor_pool_map.get_object(reference);
    }

    native::vulkan::VulkanDescriptorPool& VulkanResourceStorage::get(const api::DescriptorPoolReference& reference)
    {
        return _vulkan_descriptor_pool_map.get_object(reference);
    }

    const native::vulkan::VulkanDescriptorSet& VulkanResourceStorage::get(const api::DescriptorSetReference& reference) const
    {
        return _vulkan_descriptor_set_map.get_object(reference);
    }

    native::vulkan::VulkanDescriptorSet& VulkanResourceStorage::get(const api::DescriptorSetReference& reference)
    {
        return _vulkan_descriptor_set_map.get_object(reference);
    }

    const native::vulkan::VulkanDescriptorSetLayout& VulkanResourceStorage::get(const api::DescriptorSetLayoutReference& reference) const
    {
        return _vulkan_descriptor_layout_map.get_object(reference);
    }

    native::vulkan::VulkanDescriptorSetLayout& VulkanResourceStorage::get(const api::DescriptorSetLayoutReference& reference)
    {
        return _vulkan_descriptor_layout_map.get_object(reference);
    }

    const native::vulkan::VulkanGraphicsPipeline& VulkanResourceStorage::get(const api::GraphicsPipelineReference& reference) const
    {
        return _vulkan_graphics_pipeline_map.get_object(reference);
    }

    native::vulkan::VulkanGraphicsPipeline& VulkanResourceStorage::get(const api::GraphicsPipelineReference& reference)
    {
        return _vulkan_graphics_pipeline_map.get_object(reference);
    }

    const native::vulkan::VulkanImage& VulkanResourceStorage::get(const api::ImageReference& reference) const
    {
        return _vulkan_image_map.get_object(reference);
    }

    native::vulkan::VulkanImage& VulkanResourceStorage::get(const api::ImageReference& reference)
    {
        return _vulkan_image_map.get_object(reference);
    }

    const native::vulkan::VulkanImageView& VulkanResourceStorage::get(const api::ImageViewReference& reference) const
    {
        return _vulkan_image_view_map.get_object(reference);
    }

    native::vulkan::VulkanImageView& VulkanResourceStorage::get(const api::ImageViewReference& reference)
    {
        return _vulkan_image_view_map.get_object(reference);
    }

    const native::vulkan::VulkanQueue& VulkanResourceStorage::get(const api::QueueReference& reference) const
    {
        return _vulkan_queue_map.get_object(reference);
    }

    native::vulkan::VulkanQueue& VulkanResourceStorage::get(const api::QueueReference& reference)
    {
        return _vulkan_queue_map.get_object(reference);
    }

    const native::vulkan::VulkanSampler& VulkanResourceStorage::get(const api::SamplerReference& reference) const
    {
        return _vulkan_sampler_map.get_object(reference);
    }

    native::vulkan::VulkanSampler& VulkanResourceStorage::get(const api::SamplerReference& reference)
    {
        return _vulkan_sampler_map.get_object(reference);
    }

    const native::vulkan::VulkanShader& VulkanResourceStorage::get(const api::ShaderReference& reference) const
    {
        return _vulkan_shader_map.get_object(reference);
    }

    native::vulkan::VulkanShader& VulkanResourceStorage::get(const api::ShaderReference& reference)
    {
        return _vulkan_shader_map.get_object(reference);
    }

    const native::vulkan::VulkanSwapChain& VulkanResourceStorage::get(const api::SwapChainReference& reference) const
    {
        return _vulkan_swap_chain_map.get_object(reference);
    }

    native::vulkan::VulkanSwapChain& VulkanResourceStorage::get(const api::SwapChainReference& reference)
    {
        return _vulkan_swap_chain_map.get_object(reference);
    }


    api::BufferReference VulkanResourceStorage::add(native::vulkan::VulkanBuffer resource)
    {
        return _vulkan_buffer_map.add(std::move(resource));
    }

    api::CommandBufferReference VulkanResourceStorage::add(native::vulkan::VulkanCommandBuffer resource)
    {
        return _vulkan_command_buffer_map.add(std::move(resource));
    }

    api::DescriptorPoolReference VulkanResourceStorage::add(native::vulkan::VulkanDescriptorPool resource)
    {
        return _vulkan_descriptor_pool_map.add(std::move(resource));
    }

    api::DescriptorSetReference VulkanResourceStorage::add(native::vulkan::VulkanDescriptorSet resource)
    {
        return _vulkan_descriptor_set_map.add(std::move(resource));
    }

    api::DescriptorSetLayoutReference VulkanResourceStorage::add(native::vulkan::VulkanDescriptorSetLayout resource)
    {
        return _vulkan_descriptor_layout_map.add(std::move(resource));
    }

    api::GraphicsPipelineReference VulkanResourceStorage::add(native::vulkan::VulkanGraphicsPipeline resource)
    {
        return _vulkan_graphics_pipeline_map.add(std::move(resource));
    }

    api::ImageReference VulkanResourceStorage::add(native::vulkan::VulkanImage resource)
    {
        return _vulkan_image_map.add(std::move(resource));
    }

    api::ImageViewReference VulkanResourceStorage::add(native::vulkan::VulkanImageView resource)
    {
        return _vulkan_image_view_map.add(std::move(resource));
    }

    api::QueueReference VulkanResourceStorage::add(native::vulkan::VulkanQueue resource)
    {
        return _vulkan_queue_map.add(std::move(resource));
    }

    api::SamplerReference VulkanResourceStorage::add(native::vulkan::VulkanSampler resource)
    {
        return _vulkan_sampler_map.add(std::move(resource));
    }

    api::ShaderReference VulkanResourceStorage::add(native::vulkan::VulkanShader resource)
    {
        return _vulkan_shader_map.add(std::move(resource));
    }

    api::SwapChainReference VulkanResourceStorage::add(native::vulkan::VulkanSwapChain resource)
    {
        return _vulkan_swap_chain_map.add(std::move(resource));
    }
}
