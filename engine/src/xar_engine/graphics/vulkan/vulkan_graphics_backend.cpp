#include <xar_engine/graphics/vulkan/vulkan_graphics_backend.hpp>

#include <glm/glm.hpp>

#include <xar_engine/meta/ref_counting_singleton.hpp>


namespace xar_engine::graphics::vulkan
{
    namespace
    {
        const std::uint32_t MAX_FRAMES_IN_FLIGHT = 2;

        VkFormat to_vk_format(const EImageFormat image_format)
        {
            switch (image_format)
            {
                case EImageFormat::D32_SFLOAT:
                {
                    return VK_FORMAT_D32_SFLOAT;
                }
                case EImageFormat::R8G8B8A8_SRGB:
                {
                    return VK_FORMAT_R8G8B8A8_SRGB;
                }
            }
        }

        VkImageAspectFlagBits to_vk_aspect(const EImageAspect image_aspect)
        {
            switch (image_aspect)
            {
                case EImageAspect::COLOR:
                {
                    return VK_IMAGE_ASPECT_COLOR_BIT;
                }
                case EImageAspect::DEPTH:
                {
                    return VK_IMAGE_ASPECT_DEPTH_BIT;
                }
            }
        }

        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 color;
            glm::vec2 textureCoords;

            static std::vector<VkVertexInputBindingDescription> getBindingDescription()
            {
                VkVertexInputBindingDescription bindingDescription{};
                bindingDescription.binding = 0;
                bindingDescription.stride = sizeof(Vertex);
                bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                return {bindingDescription};
            }

            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
            {
                std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);

                attributeDescriptions[0].binding = 0;
                attributeDescriptions[0].location = 0;
                attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributeDescriptions[0].offset = offsetof(
                    Vertex,
                    position);

                attributeDescriptions[1].binding = 0;
                attributeDescriptions[1].location = 1;
                attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributeDescriptions[1].offset = offsetof(
                    Vertex,
                    color);

                attributeDescriptions[2].binding = 0;
                attributeDescriptions[2].location = 2;
                attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
                attributeDescriptions[2].offset = offsetof(
                    Vertex,
                    textureCoords);

                return attributeDescriptions;
            }
        };
    }


    VulkanGraphicsBackend::VulkanGraphicsBackend()
        : _vk_sample_count_flag_bits{VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT}
    {
        _vulkan_instance = meta::RefCountedSingleton::get_instance<vulkan::impl::VulkanInstance>();
        _vulkan_physical_device_list = _vulkan_instance->get_physical_device_list();
        _vulkan_device = vulkan::impl::VulkanDevice{{_vulkan_physical_device_list[0]}};

        _vulkan_command_buffer_pool = impl::VulkanCommandBufferPool{
            {
                _vulkan_device,
            }};

        _vk_sample_count_flag_bits = _vulkan_device.get_physical_device().get_vk_sample_count_flag_bits();
    }


    const IGraphicsBackendResource& VulkanGraphicsBackend::resource() const
    {
        return *this;
    }

    IGraphicsBackendResource& VulkanGraphicsBackend::resource()
    {
        return *this;
    }

    const IGraphicsBackendHostCommand& VulkanGraphicsBackend::host_command() const
    {
        return *this;
    }

    IGraphicsBackendHostCommand& VulkanGraphicsBackend::host_command()
    {
        return *this;
    }

    const IGraphicsBackendDeviceCommand& VulkanGraphicsBackend::device_command() const
    {
        return *this;
    }

    IGraphicsBackendDeviceCommand& VulkanGraphicsBackend::device_command()
    {
        return *this;
    }


    BufferReference VulkanGraphicsBackend::make_staging_buffer(const std::uint32_t buffer_byte_size)
    {
        return _vulkan_buffer_map.add(
            impl::VulkanBuffer{
                {
                    _vulkan_device,
                    VkDeviceSize{buffer_byte_size},
                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                }});
    }

    BufferReference VulkanGraphicsBackend::make_vertex_buffer(const std::uint32_t buffer_byte_size)
    {
        return _vulkan_buffer_map.add(
            impl::VulkanBuffer{
                {
                    _vulkan_device,
                    VkDeviceSize{buffer_byte_size},
                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                }});
    }

    BufferReference VulkanGraphicsBackend::make_index_buffer(const std::uint32_t buffer_byte_size)
    {
        return _vulkan_buffer_map.add(
            impl::VulkanBuffer{
                {
                    _vulkan_device,
                    VkDeviceSize{buffer_byte_size},
                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                }});
    }

    BufferReference VulkanGraphicsBackend::make_uniform_buffer(const std::uint32_t byte_size)
    {
        return _vulkan_buffer_map.add(
            impl::VulkanBuffer{
                {
                    _vulkan_device,
                    VkDeviceSize{byte_size},
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                }});
    }

    std::vector<CommandBufferReference> VulkanGraphicsBackend::make_command_buffers(const std::uint32_t buffer_counts)
    {
        auto vk_command_buffer_list = _vulkan_command_buffer_pool.make_buffers(MAX_FRAMES_IN_FLIGHT);

        auto _vulkan_command_buffer_list = std::vector<CommandBufferReference>{};
        _vulkan_command_buffer_list.reserve(buffer_counts);

        for (auto& vk_command_buffer: vk_command_buffer_list)
        {
            _vulkan_command_buffer_list.push_back(_vulkan_command_buffer_map.add(vk_command_buffer));
        }

        return _vulkan_command_buffer_list;
    }

    CommandBufferReference VulkanGraphicsBackend::make_one_time_command_buffer()
    {
        return _vulkan_command_buffer_map.add(_vulkan_command_buffer_pool.make_one_time_buffer());
    }

    DescriptorPoolReference VulkanGraphicsBackend::make_descriptor_pool()
    {
        return _vulkan_descriptor_pool_map.add(
            impl::VulkanDescriptorPool{
                {
                    _vulkan_device,
                    MAX_FRAMES_IN_FLIGHT,
                    MAX_FRAMES_IN_FLIGHT,
                    MAX_FRAMES_IN_FLIGHT,
                }});
    }

    DescriptorSetListReference VulkanGraphicsBackend::make_descriptor_set_list(
        const DescriptorPoolReference& descriptor_pool,
        const DescriptorSetLayoutReference& descriptor_set_layout,
        const std::vector<BufferReference>& uniform_buffer_list,
        const ImageViewReference& texture_image_view,
        const SamplerReference& sampler)
    {
        auto layouts = std::vector<VkDescriptorSetLayout>(
            MAX_FRAMES_IN_FLIGHT,
            get_object(descriptor_set_layout).get_native());

        auto descriptor_set_list = _vulkan_descriptor_set_map.add(
            impl::VulkanDescriptorSet{
                {
                    _vulkan_device,
                    get_object(descriptor_pool),
                    std::move(layouts),
                }});
        auto vulkan_descriptor_set_list = get_object(descriptor_set_list);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            const auto& vulkan_uniform_buffer = get_object(uniform_buffer_list[i]);

            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = vulkan_uniform_buffer.get_native();
            bufferInfo.offset = 0;
            bufferInfo.range = vulkan_uniform_buffer.get_buffer_byte_size();

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = get_object(texture_image_view).get_native();
            imageInfo.sampler = get_object(sampler).get_native();

            std::vector<VkWriteDescriptorSet> descriptorWrites(2);
            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = vulkan_descriptor_set_list.get_native()[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;
            descriptorWrites[0].pImageInfo = nullptr; // Optional
            descriptorWrites[0].pTexelBufferView = nullptr; // Optional

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = vulkan_descriptor_set_list.get_native()[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pBufferInfo = nullptr;
            descriptorWrites[1].pImageInfo = &imageInfo; // Optional
            descriptorWrites[1].pTexelBufferView = nullptr; // Optional

            vulkan_descriptor_set_list.write(descriptorWrites);
        }

        return descriptor_set_list;
    }

    DescriptorSetLayoutReference VulkanGraphicsBackend::make_descriptor_set_layout()
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        return _vulkan_descriptor_layout_map.add(
            impl::VulkanDescriptorSetLayout{{
                                                _vulkan_device,
                                                {uboLayoutBinding, samplerLayoutBinding},
                                            }});
    }

    GraphicsPipelineReference VulkanGraphicsBackend::make_graphics_pipeline(
        const DescriptorSetLayoutReference& descriptor_set_layout,
        const ShaderReference& vertex_shader,
        const ShaderReference& fragment_shader,
        const EImageFormat color_format,
        const EImageFormat depth_format,
        std::uint32_t sample_counts
    )
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(float);

        _vulkan_graphics_pipeline_map.add(
            impl::VulkanGraphicsPipeline{
                {
                    _vulkan_device,
                    {
                        std::make_tuple(
                            get_object(vertex_shader).get_native(),
                            VK_SHADER_STAGE_VERTEX_BIT,
                            "main"),
                        std::make_tuple(
                            get_object(fragment_shader).get_native(),
                            VK_SHADER_STAGE_FRAGMENT_BIT,
                            "main")
                    },
                    get_object(descriptor_set_layout).get_native(),
                    Vertex::getBindingDescription(),
                    Vertex::getAttributeDescriptions(),
                    {pushConstantRange},
                    static_cast<VkSampleCountFlagBits>(sample_counts),
                    to_vk_format(color_format),
                    to_vk_format(depth_format),
                }});
    }

    ImageReference VulkanGraphicsBackend::make_image(
        EImageType image_type,
        const math::Vector3i32 dimension,
        const EImageFormat image_format,
        const std::uint32_t mip_levels,
        const std::uint32_t sample_count)
    {
        auto vk_image_usage = 0;
        switch (image_type)
        {
            case EImageType::COLOR_ATTACHMENT:
            {
                vk_image_usage =
                    VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                break;
            }
            case EImageType::DEPTH_ATTACHMENT:
            {
                vk_image_usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                break;
            }
            case EImageType::TEXTURE:
            {
                vk_image_usage =
                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                    VK_IMAGE_USAGE_SAMPLED_BIT;
                break;
            }
        }

        return _vulkan_image_map.add(
            impl::VulkanImage{
                {
                    _vulkan_device,
                    dimension,
                    to_vk_format(image_format),
                    VK_IMAGE_TILING_OPTIMAL,
                    static_cast<VkImageUsageFlagBits>(vk_image_usage),
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    mip_levels,
                    static_cast<VkSampleCountFlagBits>(sample_count),
                }});
    }

    ImageViewReference VulkanGraphicsBackend::make_image_view(
        const ImageReference& image,
        EImageAspect image_aspect,
        const std::uint32_t mip_levels)
    {
        const auto& vulkan_image = get_object(image);

        return _vulkan_image_view_map.add(
            impl::VulkanImageView{
                {
                    _vulkan_device,
                    vulkan_image.get_native(),
                    vulkan_image.get_vk_format(),
                    to_vk_aspect(image_aspect),
                    mip_levels
                }});
    }

    SamplerReference VulkanGraphicsBackend::make_sampler(const float mip_levels)
    {
        return _vulkan_sampler_map.add(
            impl::VulkanSampler{
                {
                    _vulkan_device,
                    _vulkan_device.get_physical_device().get_vk_device_properties().limits.maxSamplerAnisotropy,
                    mip_levels,
                }
            });
    }

    ShaderReference VulkanGraphicsBackend::make_shader(const std::vector<char>& shader_byte_code)
    {
        return _vulkan_shader_map.add(
            impl::VulkanShader{
                {
                    _vulkan_device,
                    shader_byte_code,
                }});
    }

    SwapChainReference VulkanGraphicsBackend::make_swap_chain(
        std::shared_ptr<IWindowSurface> window_surface,
        const std::uint32_t buffering_level)
    {
        auto vulkan_window_surface = std::dynamic_pointer_cast<VulkanWindowSurface>(window_surface);

        auto vk_format_to_use = VkSurfaceFormatKHR{};
        const auto vk_format_list = _vulkan_device.get_physical_device().get_vk_surface_format_khr_list(vulkan_window_surface->get_vulkan_surface().get_native());
        for (const auto& vk_format: vk_format_list)
        {
            if (vk_format.format == VK_FORMAT_R8G8B8A8_SRGB &&
                vk_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                vk_format_to_use = vk_format;
                break;
            }
        }

        auto vk_present_mode_to_use = VkPresentModeKHR{};
        const auto vk_present_mode_list = _vulkan_device.get_physical_device().get_vk_present_mode_khr_list(vulkan_window_surface->get_vulkan_surface().get_native());
        for (const auto& vk_present_mode: vk_present_mode_list)
        {
            if (vk_present_mode == VK_PRESENT_MODE_FIFO_KHR)
            {
                vk_present_mode_to_use = vk_present_mode;
                break;
            }
        }

        return _vulkan_swap_chain_map.add(
            impl::VulkanSwapChain{
                {
                    _vulkan_device,
                    vulkan_window_surface,
                    vk_present_mode_to_use,
                    vk_format_to_use,
                    std::max(
                        static_cast<std::int32_t>(buffering_level),
                        static_cast<std::int32_t>(MAX_FRAMES_IN_FLIGHT)
                    ),
                }});
    }


    void VulkanGraphicsBackend::update_buffer(
        const xar_engine::graphics::BufferReference& buffer,
        void* const data,
        const std::uint32_t data_byte_size)
    {
        auto& vulkan_buffer = get_object(buffer);

        void* const mapped_data = vulkan_buffer.map();
        memcpy(
            mapped_data,
            data,
            static_cast<std::size_t>(data_byte_size));
        vulkan_buffer.unmap();
    }

    void VulkanGraphicsBackend::copy_buffer(
        const CommandBufferReference& command_buffer,
        const BufferReference& source_buffer,
        const BufferReference& destination_buffer)
    {
        const auto& vulkan_command_buffer = get_object(command_buffer);
        const auto& vulkan_source_buffer = get_object(source_buffer);
        const auto& vulkan_destination_buffer = get_object(destination_buffer);

        XAR_THROW_IF(
            vulkan_source_buffer.get_buffer_byte_size() != vulkan_destination_buffer.get_buffer_byte_size(),
            error::XarException,
            "Source buffer and destination buffer sizes are different");

        auto vk_buffer_copy = VkBufferCopy{};
        vk_buffer_copy.srcOffset = 0;
        vk_buffer_copy.dstOffset = 0;
        vk_buffer_copy.size = vulkan_source_buffer.get_buffer_byte_size();

        vkCmdCopyBuffer(
            vulkan_command_buffer,
            vulkan_source_buffer.get_native(),
            vulkan_destination_buffer.get_native(),
            1,
            &vk_buffer_copy);
    }

    void VulkanGraphicsBackend::copy_buffer_to_image(
        const CommandBufferReference& command_buffer,
        const BufferReference& source_buffer,
        const ImageReference& target_image)
    {
        const auto& vulkan_target_image = get_object(target_image);

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
            get_object(command_buffer),
            get_object(source_buffer).get_native(),
            vulkan_target_image.get_native(),
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );
    }

    void VulkanGraphicsBackend::generate_image_mip_maps(
        const CommandBufferReference& command_buffer,
        const ImageReference& image)
    {
        get_object(image).generate_mipmaps(get_object(command_buffer));
    }

    void VulkanGraphicsBackend::submit_one_time_command_buffer(const CommandBufferReference& command_buffer)
    {
        _vulkan_command_buffer_pool.submit_one_time_buffer(get_object(command_buffer));
    }

    void VulkanGraphicsBackend::transit_image_layout(
        const CommandBufferReference& command_buffer,
        const ImageReference& image,
        EImageLayout new_image_layout)
    {
        const auto new_vk_image_layout =
            new_image_layout == EImageLayout::TRANSFER_DESTINATION ? VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
                                                                   : VK_IMAGE_LAYOUT_UNDEFINED;

        get_object(image).transition_layout(
            get_object(command_buffer),
            new_vk_image_layout);
    }

    void VulkanGraphicsBackend::wait_idle()
    {
        _vulkan_device.wait_idle();
    }

    impl::VulkanBuffer& VulkanGraphicsBackend::get_object(const BufferReference& reference)
    {
        return _vulkan_buffer_map.get_object(reference);
    }

    VkCommandBuffer VulkanGraphicsBackend::get_object(const CommandBufferReference& reference)
    {
        return _vulkan_command_buffer_map.get_object(reference);
    }

    impl::VulkanDescriptorPool VulkanGraphicsBackend::get_object(const DescriptorPoolReference& reference)
    {
        return _vulkan_descriptor_pool_map.get_object(reference);
    }

    impl::VulkanDescriptorSet VulkanGraphicsBackend::get_object(const DescriptorSetListReference& reference)
    {
        return _vulkan_descriptor_set_map.get_object(reference);
    }

    impl::VulkanDescriptorSetLayout VulkanGraphicsBackend::get_object(const DescriptorSetLayoutReference& reference)
    {
        return _vulkan_descriptor_layout_map.get_object(reference);
    }

    impl::VulkanImage VulkanGraphicsBackend::get_object(const ImageReference& reference)
    {
        return _vulkan_image_map.get_object(reference);
    }

    impl::VulkanImageView VulkanGraphicsBackend::get_object(const ImageViewReference& reference)
    {
        return _vulkan_image_view_map.get_object(reference);
    }

    impl::VulkanSampler VulkanGraphicsBackend::get_object(const SamplerReference& reference)
    {
        return _vulkan_sampler_map.get_object(reference);
    }

    impl::VulkanShader VulkanGraphicsBackend::get_object(const ShaderReference& reference)
    {
        return _vulkan_shader_map.get_object(reference);
    }
}