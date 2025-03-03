#include <xar_engine/graphics/vulkan/vulkan_resource_storage.hpp>


namespace xar_engine::graphics::vulkan
{

    const impl::VulkanBuffer& VulkanResourceStorage::get(const BufferReference& reference) const
    {
        return _vulkan_buffer_map.get_object(reference);
    }

    impl::VulkanBuffer& VulkanResourceStorage::get(const BufferReference& reference)
    {
        return _vulkan_buffer_map.get_object(reference);
    }

    const impl::VulkanCommandBuffer& VulkanResourceStorage::get(const CommandBufferReference& reference) const
    {
        return _vulkan_command_buffer_map.get_object(reference);
    }

    impl::VulkanCommandBuffer& VulkanResourceStorage::get(const CommandBufferReference& reference)
    {
        return _vulkan_command_buffer_map.get_object(reference);
    }

    const impl::VulkanDescriptorPool& VulkanResourceStorage::get(const DescriptorPoolReference& reference) const
    {
        return _vulkan_descriptor_pool_map.get_object(reference);
    }

    impl::VulkanDescriptorPool& VulkanResourceStorage::get(const DescriptorPoolReference& reference)
    {
        return _vulkan_descriptor_pool_map.get_object(reference);
    }

    const impl::VulkanDescriptorSet& VulkanResourceStorage::get(const DescriptorSetReference& reference) const
    {
        return _vulkan_descriptor_set_map.get_object(reference);
    }

    impl::VulkanDescriptorSet& VulkanResourceStorage::get(const DescriptorSetReference& reference)
    {
        return _vulkan_descriptor_set_map.get_object(reference);
    }

    const impl::VulkanDescriptorSetLayout& VulkanResourceStorage::get(const DescriptorSetLayoutReference& reference) const
    {
        return _vulkan_descriptor_layout_map.get_object(reference);
    }

    impl::VulkanDescriptorSetLayout& VulkanResourceStorage::get(const DescriptorSetLayoutReference& reference)
    {
        return _vulkan_descriptor_layout_map.get_object(reference);
    }

    const impl::VulkanGraphicsPipeline& VulkanResourceStorage::get(const GraphicsPipelineReference& reference) const
    {
        return _vulkan_graphics_pipeline_map.get_object(reference);
    }

    impl::VulkanGraphicsPipeline& VulkanResourceStorage::get(const GraphicsPipelineReference& reference)
    {
        return _vulkan_graphics_pipeline_map.get_object(reference);
    }

    const impl::VulkanImage& VulkanResourceStorage::get(const ImageReference& reference) const
    {
        return _vulkan_image_map.get_object(reference);
    }

    impl::VulkanImage& VulkanResourceStorage::get(const ImageReference& reference)
    {
        return _vulkan_image_map.get_object(reference);
    }

    const impl::VulkanImageView& VulkanResourceStorage::get(const ImageViewReference& reference) const
    {
        return _vulkan_image_view_map.get_object(reference);
    }

    impl::VulkanImageView& VulkanResourceStorage::get(const ImageViewReference& reference)
    {
        return _vulkan_image_view_map.get_object(reference);
    }

    const impl::VulkanQueue& VulkanResourceStorage::get(const QueueReference& reference) const
    {
        return _vulkan_queue_map.get_object(reference);
    }

    impl::VulkanQueue& VulkanResourceStorage::get(const QueueReference& reference)
    {
        return _vulkan_queue_map.get_object(reference);
    }

    const impl::VulkanSampler& VulkanResourceStorage::get(const SamplerReference& reference) const
    {
        return _vulkan_sampler_map.get_object(reference);
    }

    impl::VulkanSampler& VulkanResourceStorage::get(const SamplerReference& reference)
    {
        return _vulkan_sampler_map.get_object(reference);
    }

    const impl::VulkanShader& VulkanResourceStorage::get(const ShaderReference& reference) const
    {
        return _vulkan_shader_map.get_object(reference);
    }

    impl::VulkanShader& VulkanResourceStorage::get(const ShaderReference& reference)
    {
        return _vulkan_shader_map.get_object(reference);
    }

    const impl::VulkanSwapChain& VulkanResourceStorage::get(const SwapChainReference& reference) const
    {
        return _vulkan_swap_chain_map.get_object(reference);
    }

    impl::VulkanSwapChain& VulkanResourceStorage::get(const SwapChainReference& reference)
    {
        return _vulkan_swap_chain_map.get_object(reference);
    }


    BufferReference VulkanResourceStorage::add(impl::VulkanBuffer resource)
    {
        return _vulkan_buffer_map.add(std::move(resource));
    }

    CommandBufferReference VulkanResourceStorage::add(impl::VulkanCommandBuffer resource)
    {
        return _vulkan_command_buffer_map.add(std::move(resource));
    }

    DescriptorPoolReference VulkanResourceStorage::add(impl::VulkanDescriptorPool resource)
    {
        return _vulkan_descriptor_pool_map.add(std::move(resource));
    }

    DescriptorSetReference VulkanResourceStorage::add(impl::VulkanDescriptorSet resource)
    {
        return _vulkan_descriptor_set_map.add(std::move(resource));
    }

    DescriptorSetLayoutReference VulkanResourceStorage::add(impl::VulkanDescriptorSetLayout resource)
    {
        return _vulkan_descriptor_layout_map.add(std::move(resource));
    }

    GraphicsPipelineReference VulkanResourceStorage::add(impl::VulkanGraphicsPipeline resource)
    {
        return _vulkan_graphics_pipeline_map.add(std::move(resource));
    }

    ImageReference VulkanResourceStorage::add(impl::VulkanImage resource)
    {
        return _vulkan_image_map.add(std::move(resource));
    }

    ImageViewReference VulkanResourceStorage::add(impl::VulkanImageView resource)
    {
        return _vulkan_image_view_map.add(std::move(resource));
    }

    QueueReference VulkanResourceStorage::add(impl::VulkanQueue resource)
    {
        return _vulkan_queue_map.add(std::move(resource));
    }

    SamplerReference VulkanResourceStorage::add(impl::VulkanSampler resource)
    {
        return _vulkan_sampler_map.add(std::move(resource));
    }

    ShaderReference VulkanResourceStorage::add(impl::VulkanShader resource)
    {
        return _vulkan_shader_map.add(std::move(resource));
    }

    SwapChainReference VulkanResourceStorage::add(impl::VulkanSwapChain resource)
    {
        return _vulkan_swap_chain_map.add(std::move(resource));
    }
}
