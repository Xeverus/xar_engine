#include <xar_engine/graphics/vulkan/vulkan_resource_storage.hpp>


namespace xar_engine::graphics::vulkan
{

    const native::VulkanBuffer& VulkanResourceStorage::get(const api::BufferReference& reference) const
    {
        return _vulkan_buffer_map.get_object(reference);
    }

    native::VulkanBuffer& VulkanResourceStorage::get(const api::BufferReference& reference)
    {
        return _vulkan_buffer_map.get_object(reference);
    }

    const native::VulkanCommandBuffer& VulkanResourceStorage::get(const api::CommandBufferReference& reference) const
    {
        return _vulkan_command_buffer_map.get_object(reference);
    }

    native::VulkanCommandBuffer& VulkanResourceStorage::get(const api::CommandBufferReference& reference)
    {
        return _vulkan_command_buffer_map.get_object(reference);
    }

    const native::VulkanDescriptorPool& VulkanResourceStorage::get(const api::DescriptorPoolReference& reference) const
    {
        return _vulkan_descriptor_pool_map.get_object(reference);
    }

    native::VulkanDescriptorPool& VulkanResourceStorage::get(const api::DescriptorPoolReference& reference)
    {
        return _vulkan_descriptor_pool_map.get_object(reference);
    }

    const native::VulkanDescriptorSet& VulkanResourceStorage::get(const api::DescriptorSetReference& reference) const
    {
        return _vulkan_descriptor_set_map.get_object(reference);
    }

    native::VulkanDescriptorSet& VulkanResourceStorage::get(const api::DescriptorSetReference& reference)
    {
        return _vulkan_descriptor_set_map.get_object(reference);
    }

    const native::VulkanDescriptorSetLayout& VulkanResourceStorage::get(const api::DescriptorSetLayoutReference& reference) const
    {
        return _vulkan_descriptor_layout_map.get_object(reference);
    }

    native::VulkanDescriptorSetLayout& VulkanResourceStorage::get(const api::DescriptorSetLayoutReference& reference)
    {
        return _vulkan_descriptor_layout_map.get_object(reference);
    }

    const native::VulkanGraphicsPipeline& VulkanResourceStorage::get(const api::GraphicsPipelineReference& reference) const
    {
        return _vulkan_graphics_pipeline_map.get_object(reference);
    }

    native::VulkanGraphicsPipeline& VulkanResourceStorage::get(const api::GraphicsPipelineReference& reference)
    {
        return _vulkan_graphics_pipeline_map.get_object(reference);
    }

    const native::VulkanImage& VulkanResourceStorage::get(const api::ImageReference& reference) const
    {
        return _vulkan_image_map.get_object(reference);
    }

    native::VulkanImage& VulkanResourceStorage::get(const api::ImageReference& reference)
    {
        return _vulkan_image_map.get_object(reference);
    }

    const native::VulkanImageView& VulkanResourceStorage::get(const api::ImageViewReference& reference) const
    {
        return _vulkan_image_view_map.get_object(reference);
    }

    native::VulkanImageView& VulkanResourceStorage::get(const api::ImageViewReference& reference)
    {
        return _vulkan_image_view_map.get_object(reference);
    }

    const native::VulkanQueue& VulkanResourceStorage::get(const api::QueueReference& reference) const
    {
        return _vulkan_queue_map.get_object(reference);
    }

    native::VulkanQueue& VulkanResourceStorage::get(const api::QueueReference& reference)
    {
        return _vulkan_queue_map.get_object(reference);
    }

    const native::VulkanSampler& VulkanResourceStorage::get(const api::SamplerReference& reference) const
    {
        return _vulkan_sampler_map.get_object(reference);
    }

    native::VulkanSampler& VulkanResourceStorage::get(const api::SamplerReference& reference)
    {
        return _vulkan_sampler_map.get_object(reference);
    }

    const native::VulkanShader& VulkanResourceStorage::get(const api::ShaderReference& reference) const
    {
        return _vulkan_shader_map.get_object(reference);
    }

    native::VulkanShader& VulkanResourceStorage::get(const api::ShaderReference& reference)
    {
        return _vulkan_shader_map.get_object(reference);
    }

    const native::VulkanSwapChain& VulkanResourceStorage::get(const api::SwapChainReference& reference) const
    {
        return _vulkan_swap_chain_map.get_object(reference);
    }

    native::VulkanSwapChain& VulkanResourceStorage::get(const api::SwapChainReference& reference)
    {
        return _vulkan_swap_chain_map.get_object(reference);
    }


    api::BufferReference VulkanResourceStorage::add(native::VulkanBuffer resource)
    {
        return _vulkan_buffer_map.add(std::move(resource));
    }

    api::CommandBufferReference VulkanResourceStorage::add(native::VulkanCommandBuffer resource)
    {
        return _vulkan_command_buffer_map.add(std::move(resource));
    }

    api::DescriptorPoolReference VulkanResourceStorage::add(native::VulkanDescriptorPool resource)
    {
        return _vulkan_descriptor_pool_map.add(std::move(resource));
    }

    api::DescriptorSetReference VulkanResourceStorage::add(native::VulkanDescriptorSet resource)
    {
        return _vulkan_descriptor_set_map.add(std::move(resource));
    }

    api::DescriptorSetLayoutReference VulkanResourceStorage::add(native::VulkanDescriptorSetLayout resource)
    {
        return _vulkan_descriptor_layout_map.add(std::move(resource));
    }

    api::GraphicsPipelineReference VulkanResourceStorage::add(native::VulkanGraphicsPipeline resource)
    {
        return _vulkan_graphics_pipeline_map.add(std::move(resource));
    }

    api::ImageReference VulkanResourceStorage::add(native::VulkanImage resource)
    {
        return _vulkan_image_map.add(std::move(resource));
    }

    api::ImageViewReference VulkanResourceStorage::add(native::VulkanImageView resource)
    {
        return _vulkan_image_view_map.add(std::move(resource));
    }

    api::QueueReference VulkanResourceStorage::add(native::VulkanQueue resource)
    {
        return _vulkan_queue_map.add(std::move(resource));
    }

    api::SamplerReference VulkanResourceStorage::add(native::VulkanSampler resource)
    {
        return _vulkan_sampler_map.add(std::move(resource));
    }

    api::ShaderReference VulkanResourceStorage::add(native::VulkanShader resource)
    {
        return _vulkan_shader_map.add(std::move(resource));
    }

    api::SwapChainReference VulkanResourceStorage::add(native::VulkanSwapChain resource)
    {
        return _vulkan_swap_chain_map.add(std::move(resource));
    }
}
