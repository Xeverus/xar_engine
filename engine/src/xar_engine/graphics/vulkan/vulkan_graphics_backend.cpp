#include <xar_engine/graphics/vulkan/vulkan_graphics_backend.hpp>

#include <glm/glm.hpp>

#include <xar_engine/meta/ref_counting_singleton.hpp>


namespace xar_engine::graphics::vulkan
{
    namespace
    {
        const std::uint32_t MAX_FRAMES_IN_FLIGHT = 2;

        VkFormat to_vk(const EFormat image_format)
        {
            switch (image_format)
            {
                case EFormat::D32_SFLOAT:
                {
                    return VK_FORMAT_D32_SFLOAT;
                }
                case EFormat::R32G32_SFLOAT:
                {
                    return VK_FORMAT_R32G32_SFLOAT;
                }
                case EFormat::R32G32B32_SFLOAT:
                {
                    return VK_FORMAT_R32G32B32_SFLOAT;
                }
                case EFormat::R8G8B8A8_SRGB:
                {
                    return VK_FORMAT_R8G8B8A8_SRGB;
                }
            }

            return VK_FORMAT_UNDEFINED;
        }

        VkImageAspectFlagBits to_vk(const EImageAspect image_aspect)
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

            return VK_IMAGE_ASPECT_NONE;
        }

        std::vector<VkVertexInputBindingDescription> to_vk(const std::vector<VertexInputBinding>& vertex_input_binding_list)
        {
            auto vk_vertex_input_binding_list = std::vector<VkVertexInputBindingDescription>(vertex_input_binding_list.size());
            for (auto i = 0; i < vertex_input_binding_list.size(); ++i)
            {
                vk_vertex_input_binding_list[i].binding = vertex_input_binding_list[i].binding_index;
                vk_vertex_input_binding_list[i].stride = vertex_input_binding_list[i].stride;
                vk_vertex_input_binding_list[i].inputRate =
                    vertex_input_binding_list[i].input_rate == VertexInputBindingRate::PER_VERTEX
                    ? VK_VERTEX_INPUT_RATE_VERTEX : VK_VERTEX_INPUT_RATE_INSTANCE;
            }

            return vk_vertex_input_binding_list;
        }

        std::vector<VkVertexInputAttributeDescription> to_vk(const std::vector<VertexInputAttribute>& vertex_input_attribute_list)
        {
            auto vk_vertex_input_attribute_list = std::vector<VkVertexInputAttributeDescription>(vertex_input_attribute_list.size());
            for (auto i = 0; i < vertex_input_attribute_list.size(); ++i)
            {
                vk_vertex_input_attribute_list[i].binding = vertex_input_attribute_list[i].binding_index;
                vk_vertex_input_attribute_list[i].location = vertex_input_attribute_list[i].location;
                vk_vertex_input_attribute_list[i].format = to_vk(vertex_input_attribute_list[i].format);
                vk_vertex_input_attribute_list[i].offset = vertex_input_attribute_list[i].offset;
            }

            return vk_vertex_input_attribute_list;
        }

        ESwapChainResult to_xargine(const VkResult vk_result)
        {
            switch (vk_result)
            {
                case VK_SUCCESS:
                {
                    return ESwapChainResult::OK;
                }
                case VK_ERROR_OUT_OF_DATE_KHR:
                case VK_SUBOPTIMAL_KHR:
                {
                    return ESwapChainResult::RECREATION_REQUIRED;
                }
                default:
                {
                    return ESwapChainResult::ERROR;
                }
            }
        }
    }


    VulkanGraphicsBackend::VulkanGraphicsBackend()
    {
        _vulkan_instance = meta::RefCountedSingleton::get_instance<vulkan::impl::VulkanInstance>();
        _vulkan_physical_device_list = _vulkan_instance->get_physical_device_list();
        _vulkan_device = vulkan::impl::VulkanDevice{{_vulkan_physical_device_list[0]}};

        _vulkan_command_buffer_pool = impl::VulkanCommandBufferPool{
            {
                _vulkan_device,
            }};
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
        return _vulkan_resource_storage.add(
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
        return _vulkan_resource_storage.add(
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
        return _vulkan_resource_storage.add(
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
        return _vulkan_resource_storage.add(
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
            _vulkan_command_buffer_list.push_back(_vulkan_resource_storage.add(vk_command_buffer));
        }

        return _vulkan_command_buffer_list;
    }

    CommandBufferReference VulkanGraphicsBackend::make_one_time_command_buffer()
    {
        return _vulkan_resource_storage.add(_vulkan_command_buffer_pool.make_one_time_buffer());
    }

    DescriptorPoolReference VulkanGraphicsBackend::make_descriptor_pool()
    {
        return _vulkan_resource_storage.add(
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
            _vulkan_resource_storage.get(descriptor_set_layout).get_native());

        auto descriptor_set_list = _vulkan_resource_storage.add(
            impl::VulkanDescriptorSet{
                {
                    _vulkan_device,
                    _vulkan_resource_storage.get(descriptor_pool),
                    std::move(layouts),
                }});
        auto vulkan_descriptor_set_list = _vulkan_resource_storage.get(descriptor_set_list);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            const auto& vulkan_uniform_buffer = _vulkan_resource_storage.get(uniform_buffer_list[i]);

            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = vulkan_uniform_buffer.get_native();
            bufferInfo.offset = 0;
            bufferInfo.range = vulkan_uniform_buffer.get_buffer_byte_size();

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = _vulkan_resource_storage.get(texture_image_view).get_native();
            imageInfo.sampler = _vulkan_resource_storage.get(sampler).get_native();

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
        uboLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        return _vulkan_resource_storage.add(
            impl::VulkanDescriptorSetLayout{
                {
                    _vulkan_device,
                    {uboLayoutBinding, samplerLayoutBinding},
                }});
    }

    GraphicsPipelineReference VulkanGraphicsBackend::make_graphics_pipeline(
        const DescriptorSetLayoutReference& descriptor_set_layout,
        const ShaderReference& vertex_shader,
        const ShaderReference& fragment_shader,
        const std::vector<VertexInputAttribute>& vertex_input_attribute_list,
        const std::vector<VertexInputBinding>& vertex_input_binding_list,
        const EFormat color_format,
        const EFormat depth_format,
        std::uint32_t sample_counts)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(float);

        return _vulkan_resource_storage.add(
            impl::VulkanGraphicsPipeline{
                {
                    _vulkan_device,
                    {
                        {
                            _vulkan_resource_storage.get(vertex_shader).get_native(),
                            VK_SHADER_STAGE_VERTEX_BIT,
                            "main"
                        },
                        {
                            _vulkan_resource_storage.get(fragment_shader).get_native(),
                            VK_SHADER_STAGE_FRAGMENT_BIT,
                            "main"
                        }
                    },
                    _vulkan_resource_storage.get(descriptor_set_layout).get_native(),
                    to_vk(vertex_input_binding_list),
                    to_vk(vertex_input_attribute_list),
                    {pushConstantRange},
                    static_cast<VkSampleCountFlagBits>(sample_counts),
                    to_vk(color_format),
                    to_vk(depth_format),
                }});
    }

    ImageReference VulkanGraphicsBackend::make_image(
        EImageType image_type,
        const math::Vector3i32 dimension,
        const EFormat image_format,
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

        return _vulkan_resource_storage.add(
            impl::VulkanImage{
                {
                    _vulkan_device,
                    dimension,
                    to_vk(image_format),
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
        const auto& vulkan_image = _vulkan_resource_storage.get(image);

        return _vulkan_resource_storage.add(
            impl::VulkanImageView{
                {
                    _vulkan_device,
                    vulkan_image.get_native(),
                    vulkan_image.get_vk_format(),
                    to_vk(image_aspect),
                    mip_levels
                }});
    }

    SamplerReference VulkanGraphicsBackend::make_sampler(const float mip_levels)
    {
        return _vulkan_resource_storage.add(
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
        return _vulkan_resource_storage.add(
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

        return _vulkan_resource_storage.add(
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
        auto& vulkan_buffer = _vulkan_resource_storage.get(buffer);

        void* const mapped_data = vulkan_buffer.map();
        memcpy(
            mapped_data,
            data,
            static_cast<std::size_t>(data_byte_size));
        vulkan_buffer.unmap();
    }

    std::tuple<ESwapChainResult, std::uint32_t, std::uint32_t> VulkanGraphicsBackend::begin_frame(const SwapChainReference& swap_chain)
    {
        const auto result = _vulkan_resource_storage.get(swap_chain).begin_frame();

        return {
            to_xargine(result.vk_result),
            result.image_index,
            result.frame_buffer_index
        };
    }

    std::uint32_t VulkanGraphicsBackend::get_sample_count() const
    {
        return _vulkan_device.get_physical_device().get_vk_sample_count_flag_bits();
    }

    EFormat VulkanGraphicsBackend::find_depth_format() const
    {
        const auto vk_format = _vulkan_device.get_physical_device().find_supported_vk_format(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

        switch (vk_format)
        {
            case VK_FORMAT_D32_SFLOAT:
            {
                break;
            }
            case VK_FORMAT_D32_SFLOAT_S8_UINT:
            {
                XAR_THROW(
                    error::XarException,
                    "VK_FORMAT_D32_SFLOAT_S8_UINT not supported");
                break;
            }
            case VK_FORMAT_D24_UNORM_S8_UINT:
            {
                XAR_THROW(
                    error::XarException,
                    "VK_FORMAT_D24_UNORM_S8_UINT not supported");
                break;
            }
            default:
            {
                XAR_THROW(
                    error::XarException,
                    "VK_FORMAT not supported");
                break;
            }
        }

        return EFormat::D32_SFLOAT;
    }

    ESwapChainResult VulkanGraphicsBackend::end_frame(
        const CommandBufferReference& command_buffer,
        const SwapChainReference& swap_chain)
    {
        const auto result = _vulkan_resource_storage.get(swap_chain).end_frame(_vulkan_resource_storage.get(command_buffer));

        return to_xargine(result.vk_result);
    }

    void VulkanGraphicsBackend::copy_buffer(
        const CommandBufferReference& command_buffer,
        const BufferReference& source_buffer,
        const BufferReference& destination_buffer)
    {
        const auto& vulkan_command_buffer = _vulkan_resource_storage.get(command_buffer);
        const auto& vulkan_source_buffer = _vulkan_resource_storage.get(source_buffer);
        const auto& vulkan_destination_buffer = _vulkan_resource_storage.get(destination_buffer);

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
        const auto& vulkan_target_image = _vulkan_resource_storage.get(target_image);

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
            _vulkan_resource_storage.get(command_buffer),
            _vulkan_resource_storage.get(source_buffer).get_native(),
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
        _vulkan_resource_storage.get(image).generate_mipmaps(_vulkan_resource_storage.get(command_buffer));
    }

    void VulkanGraphicsBackend::submit_one_time_command_buffer(const CommandBufferReference& command_buffer)
    {
        _vulkan_command_buffer_pool.submit_one_time_buffer(_vulkan_resource_storage.get(command_buffer));
    }

    void VulkanGraphicsBackend::transit_image_layout(
        const CommandBufferReference& command_buffer,
        const ImageReference& image,
        EImageLayout new_image_layout)
    {
        VkImageLayout new_vk_image_layout = {};
        switch (new_image_layout)
        {
            case EImageLayout::TRANSFER_DESTINATION:
            {
                new_vk_image_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                break;
            }
            case EImageLayout::DEPTH_STENCIL_ATTACHMENT:
            {
                new_vk_image_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                break;
            }
            default:
            {
                new_vk_image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
                break;
            }
        }

        _vulkan_resource_storage.get(image).transition_layout(
            _vulkan_resource_storage.get(command_buffer),
            new_vk_image_layout);
    }

    void VulkanGraphicsBackend::wait_idle()
    {
        _vulkan_device.wait_idle();
    }

    void VulkanGraphicsBackend::TMP_RECORD_FRAME(
        const CommandBufferReference& command_buffer,
        const SwapChainReference& swap_chain,
        const GraphicsPipelineReference& graphics_pipeline,
        std::uint32_t image_index,
        std::uint32_t frame_buffer_index,
        const DescriptorSetListReference& descriptor_set_list,
        const BufferReference& vertex_buffer,
        const BufferReference& index_buffer,
        const ImageViewReference& color_image_view,
        const ImageViewReference& depth_image_view,
        const std::uint32_t index_counts)
    {
        vkResetCommandBuffer(
            _vulkan_resource_storage.get(command_buffer),
            0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(
            _vulkan_resource_storage.get(command_buffer),
            &beginInfo) != VK_SUCCESS)
        {
            XAR_THROW(
                error::XarException,
                "failed to begin recording command buffer!");
        }

        struct Constants
        {
            float time;
        };

        const VkImageMemoryBarrier image_memory_barrier_start{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .image = _vulkan_resource_storage.get(swap_chain).get_vk_image(image_index),
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            }
        };

        vkCmdPipelineBarrier(
            _vulkan_resource_storage.get(command_buffer),
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,  // srcStageMask
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
            0,
            0,
            nullptr,
            0,
            nullptr,
            1, // imageMemoryBarrierCount
            &image_memory_barrier_start // pImageMemoryBarriers
        );

        VkClearValue clearColorColor{};
        clearColorColor.color = {0.0f, 0.0f, 0.0f, 1.0f};

        VkRenderingAttachmentInfoKHR vkRenderingAttachmentInfoColor{};
        vkRenderingAttachmentInfoColor.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        vkRenderingAttachmentInfoColor.clearValue = clearColorColor;
        vkRenderingAttachmentInfoColor.imageView = _vulkan_resource_storage.get(color_image_view).get_native();
        vkRenderingAttachmentInfoColor.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        vkRenderingAttachmentInfoColor.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        vkRenderingAttachmentInfoColor.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        vkRenderingAttachmentInfoColor.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
        vkRenderingAttachmentInfoColor.resolveImageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
        vkRenderingAttachmentInfoColor.resolveImageView = _vulkan_resource_storage.get(swap_chain).get_vulkan_image_view(image_index).get_native();

        VkClearValue clearDepthColor{};
        clearDepthColor.depthStencil = {1.0f, 0};

        VkRenderingAttachmentInfoKHR vkRenderingAttachmentInfoDepth{};
        vkRenderingAttachmentInfoDepth.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        vkRenderingAttachmentInfoDepth.clearValue = clearDepthColor;
        vkRenderingAttachmentInfoDepth.imageView = _vulkan_resource_storage.get(depth_image_view).get_native();
        vkRenderingAttachmentInfoDepth.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        vkRenderingAttachmentInfoDepth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        vkRenderingAttachmentInfoDepth.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        VkRenderingInfoKHR renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &vkRenderingAttachmentInfoColor;
        renderingInfo.pDepthAttachment = &vkRenderingAttachmentInfoDepth;
        renderingInfo.renderArea = VkRect2D{VkOffset2D{}, _vulkan_resource_storage.get(swap_chain).get_extent()};
        renderingInfo.layerCount = 1;

        vkCmdBeginRenderingKHR(
            _vulkan_resource_storage.get(command_buffer),
            &renderingInfo);

        vkCmdBindPipeline(
            _vulkan_resource_storage.get(command_buffer),
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            _vulkan_resource_storage.get(graphics_pipeline).get_native_pipeline());

        const Constants pc = {static_cast<float>(0)};
        vkCmdPushConstants(
            _vulkan_resource_storage.get(command_buffer),
            _vulkan_resource_storage.get(graphics_pipeline).get_native_pipeline_layout(),
            VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(Constants),
            &pc);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) _vulkan_resource_storage.get(swap_chain).get_extent().width;
        viewport.height = (float) _vulkan_resource_storage.get(swap_chain).get_extent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(
            _vulkan_resource_storage.get(command_buffer),
            0,
            1,
            &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = _vulkan_resource_storage.get(swap_chain).get_extent();
        vkCmdSetScissor(
            _vulkan_resource_storage.get(command_buffer),
            0,
            1,
            &scissor);

        VkBuffer vertexBuffers[] = {_vulkan_resource_storage.get(vertex_buffer).get_native()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(
            _vulkan_resource_storage.get(command_buffer),
            0,
            1,
            vertexBuffers,
            offsets);
        vkCmdBindIndexBuffer(
            _vulkan_resource_storage.get(command_buffer),
            _vulkan_resource_storage.get(index_buffer).get_native(),
            0,
            VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(
            _vulkan_resource_storage.get(command_buffer),
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            _vulkan_resource_storage.get(graphics_pipeline).get_native_pipeline_layout(),
            0,
            1,
            &_vulkan_resource_storage.get(descriptor_set_list).get_native()[frame_buffer_index],
            0,
            nullptr);

        vkCmdDrawIndexed(
            _vulkan_resource_storage.get(command_buffer),
            static_cast<uint32_t>(index_counts),
            1,
            0,
            0,
            0);

        vkCmdEndRenderingKHR(_vulkan_resource_storage.get(command_buffer));

        const VkImageMemoryBarrier image_memory_barrier_end{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .image = _vulkan_resource_storage.get(swap_chain).get_vk_image(image_index),
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            }
        };

        vkCmdPipelineBarrier(
            _vulkan_resource_storage.get(command_buffer),
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // srcStageMask
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, // dstStageMask
            0,
            0,
            nullptr,
            0,
            nullptr,
            1, // imageMemoryBarrierCount
            &image_memory_barrier_end // pImageMemoryBarriers
        );

        if (vkEndCommandBuffer(_vulkan_resource_storage.get(command_buffer)) != VK_SUCCESS)
        {
            XAR_THROW(
                error::XarException,
                "failed to record command buffer!");
        }
    }
}