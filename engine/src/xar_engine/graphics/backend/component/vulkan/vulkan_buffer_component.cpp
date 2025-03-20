#include <xar_engine/graphics/backend/component/vulkan/vulkan_buffer_component.hpp>


namespace xar_engine::graphics::backend::component::vulkan
{
    api::BufferReference IVulkanBufferComponent::make_staging_buffer(const MakeBufferParameters& parameters)
    {
        return get_state()._vulkan_resource_storage.add(
            native::vulkan::VulkanBuffer{
                {
                    get_state()._vulkan_device,
                    VkDeviceSize{parameters.byte_size},
                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                }});
    }

    api::BufferReference IVulkanBufferComponent::make_vertex_buffer(const MakeBufferParameters& parameters)
    {
        return get_state()._vulkan_resource_storage.add(
            native::vulkan::VulkanBuffer{
                {
                    get_state()._vulkan_device,
                    VkDeviceSize{parameters.byte_size},
                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                }});
    }

    api::BufferReference IVulkanBufferComponent::make_index_buffer(const MakeBufferParameters& parameters)
    {
        return get_state()._vulkan_resource_storage.add(
            native::vulkan::VulkanBuffer{
                {
                    get_state()._vulkan_device,
                    VkDeviceSize{parameters.byte_size},
                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                }});
    }

    api::BufferReference IVulkanBufferComponent::make_uniform_buffer(const MakeBufferParameters& parameters)
    {
        return get_state()._vulkan_resource_storage.add(
            native::vulkan::VulkanBuffer{
                {
                    get_state()._vulkan_device,
                    VkDeviceSize{parameters.byte_size},
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                }});
    }

    void IVulkanBufferComponent::update_buffer(const UpdateBufferParameters& parameters)
    {
        auto& vulkan_buffer = get_state()._vulkan_resource_storage.get(parameters.buffer);

        void* mapped_data = vulkan_buffer.map();
        for (const auto& buffer_update: parameters.data)
        {
            memcpy(
                reinterpret_cast<char*>(mapped_data) + buffer_update.byte_offset,
                buffer_update.data,
                static_cast<std::size_t>(buffer_update.byte_size));
        }
        vulkan_buffer.unmap();
    }

    void IVulkanBufferComponent::copy_buffer(const CopyBufferParameters& parameters)
    {
        const auto& vulkan_source_buffer = get_state()._vulkan_resource_storage.get(parameters.source_buffer);
        const auto& vulkan_destination_buffer = get_state()._vulkan_resource_storage.get(parameters.destination_buffer);

        XAR_THROW_IF(
            vulkan_source_buffer.get_buffer_byte_size() != vulkan_destination_buffer.get_buffer_byte_size(),
            error::XarException,
            "Source buffer and destination buffer sizes are different");

        auto vk_buffer_copy = VkBufferCopy{};
        vk_buffer_copy.srcOffset = 0;
        vk_buffer_copy.dstOffset = 0;
        vk_buffer_copy.size = vulkan_source_buffer.get_buffer_byte_size();

        vkCmdCopyBuffer(
            get_state()._vulkan_resource_storage.get(parameters.command_buffer).get_native(),
            vulkan_source_buffer.get_native(),
            vulkan_destination_buffer.get_native(),
            1,
            &vk_buffer_copy);
    }

    void IVulkanBufferComponent::copy_buffer_to_image(const CopyBufferToImageParameters& parameters)
    {
        const auto& vulkan_target_image = get_state()._vulkan_resource_storage.get(parameters.target_image);

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            static_cast<std::uint32_t>(vulkan_target_image.get_dimension().x),
            static_cast<std::uint32_t>(vulkan_target_image.get_dimension().y),
            static_cast<std::uint32_t>(vulkan_target_image.get_dimension().z),
        };

        vkCmdCopyBufferToImage(
            get_state()._vulkan_resource_storage.get(parameters.command_buffer).get_native(),
            get_state()._vulkan_resource_storage.get(parameters.source_buffer).get_native(),
            vulkan_target_image.get_native(),
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );
    }
}