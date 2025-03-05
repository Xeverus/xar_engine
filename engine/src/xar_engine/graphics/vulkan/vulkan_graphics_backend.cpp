#include <xar_engine/graphics/vulkan/vulkan_graphics_backend.hpp>

#include <xar_engine/graphics/vulkan/vulkan_type_converters.hpp>

#include <xar_engine/graphics/vulkan/native/vulkan_queue.hpp>

#include <xar_engine/meta/ref_counting_singleton.hpp>


namespace xar_engine::graphics::vulkan
{
    namespace
    {
        const std::uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    }


    VulkanGraphicsBackend::VulkanGraphicsBackend()
    {
        _vulkan_instance = meta::RefCountedSingleton::get_instance_t<vulkan::native::VulkanInstance>();
        _vulkan_physical_device_list = _vulkan_instance->get_physical_device_list();
        _vulkan_device = vulkan::native::VulkanDevice{{_vulkan_physical_device_list[0]}};
        _vulkan_graphics_queue = native::VulkanQueue{
            {
                _vulkan_device,
                _vulkan_device.get_graphics_family_index(),
            }
        };

        _vulkan_command_buffer_pool = native::VulkanCommandBufferPool{
            {
                _vulkan_device,
            }};
    }


    const api::IGraphicsBackendResource& VulkanGraphicsBackend::resource() const
    {
        return *this;
    }

    api::IGraphicsBackendResource& VulkanGraphicsBackend::resource()
    {
        return *this;
    }

    const api::IGraphicsBackendHost& VulkanGraphicsBackend::host() const
    {
        return *this;
    }

    api::IGraphicsBackendHost& VulkanGraphicsBackend::host()
    {
        return *this;
    }

    const api::IGraphicsBackendCommand& VulkanGraphicsBackend::command() const
    {
        return *this;
    }

    api::IGraphicsBackendCommand& VulkanGraphicsBackend::command()
    {
        return *this;
    }


    api::BufferReference VulkanGraphicsBackend::make_staging_buffer(const std::uint32_t buffer_byte_size)
    {
        return _vulkan_resource_storage.add(
            native::VulkanBuffer{
                {
                    _vulkan_device,
                    VkDeviceSize{buffer_byte_size},
                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                }});
    }

    api::BufferReference VulkanGraphicsBackend::make_vertex_buffer(const std::uint32_t buffer_byte_size)
    {
        return _vulkan_resource_storage.add(
            native::VulkanBuffer{
                {
                    _vulkan_device,
                    VkDeviceSize{buffer_byte_size},
                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                }});
    }

    api::BufferReference VulkanGraphicsBackend::make_index_buffer(const std::uint32_t buffer_byte_size)
    {
        return _vulkan_resource_storage.add(
            native::VulkanBuffer{
                {
                    _vulkan_device,
                    VkDeviceSize{buffer_byte_size},
                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                }});
    }

    api::BufferReference VulkanGraphicsBackend::make_uniform_buffer(const std::uint32_t byte_size)
    {
        return _vulkan_resource_storage.add(
            native::VulkanBuffer{
                {
                    _vulkan_device,
                    VkDeviceSize{byte_size},
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                }});
    }

    std::vector<api::CommandBufferReference> VulkanGraphicsBackend::make_command_buffer_list(const std::uint32_t buffer_counts)
    {
        auto vk_command_buffer_list = _vulkan_command_buffer_pool.make_buffer_list(MAX_FRAMES_IN_FLIGHT);

        auto _vulkan_command_buffer_list = std::vector<api::CommandBufferReference>{};
        _vulkan_command_buffer_list.reserve(buffer_counts);

        for (auto& vk_command_buffer: vk_command_buffer_list)
        {
            _vulkan_command_buffer_list.push_back(_vulkan_resource_storage.add(vk_command_buffer));
        }

        return _vulkan_command_buffer_list;
    }

    api::DescriptorPoolReference VulkanGraphicsBackend::make_descriptor_pool()
    {
        return _vulkan_resource_storage.add(
            native::VulkanDescriptorPool{
                {
                    _vulkan_device,
                    MAX_FRAMES_IN_FLIGHT,
                    MAX_FRAMES_IN_FLIGHT,
                    MAX_FRAMES_IN_FLIGHT,
                }});
    }

    std::vector<api::DescriptorSetReference> VulkanGraphicsBackend::make_descriptor_set_list(
        const api::DescriptorPoolReference& descriptor_pool,
        const api::DescriptorSetLayoutReference& descriptor_set_layout,
        const std::vector<api::BufferReference>& uniform_buffer_list,
        const api::ImageViewReference& texture_image_view,
        const api::SamplerReference& sampler)
    {
        auto layouts = std::vector<VkDescriptorSetLayout>(
            MAX_FRAMES_IN_FLIGHT,
            _vulkan_resource_storage.get(descriptor_set_layout).get_native());

        auto vulkan_descriptor_set_ref_list = std::vector<api::DescriptorSetReference>();
        auto vulkan_descriptor_set_list = _vulkan_resource_storage.get(descriptor_pool).make_descriptor_set_list(layouts);
        for (auto& vulkan_descriptor_set: vulkan_descriptor_set_list)
        {
            vulkan_descriptor_set_ref_list.push_back(_vulkan_resource_storage.add(vulkan_descriptor_set));
        }

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
            descriptorWrites[0].dstSet = vulkan_descriptor_set_list[i].get_native();
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;
            descriptorWrites[0].pImageInfo = nullptr; // Optional
            descriptorWrites[0].pTexelBufferView = nullptr; // Optional

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = vulkan_descriptor_set_list[i].get_native();
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pBufferInfo = nullptr;
            descriptorWrites[1].pImageInfo = &imageInfo; // Optional
            descriptorWrites[1].pTexelBufferView = nullptr; // Optional

            vulkan_descriptor_set_list[i].write(descriptorWrites);
        }

        return vulkan_descriptor_set_ref_list;
    }

    api::DescriptorSetLayoutReference VulkanGraphicsBackend::make_descriptor_set_layout()
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
            native::VulkanDescriptorSetLayout{
                {
                    _vulkan_device,
                    {uboLayoutBinding, samplerLayoutBinding},
                }});
    }

    api::GraphicsPipelineReference VulkanGraphicsBackend::make_graphics_pipeline(
        const api::DescriptorSetLayoutReference& descriptor_set_layout,
        const api::ShaderReference& vertex_shader,
        const api::ShaderReference& fragment_shader,
        const std::vector<api::VertexInputAttribute>& vertex_input_attribute_list,
        const std::vector<api::VertexInputBinding>& vertex_input_binding_list,
        const api::EFormat color_format,
        const api::EFormat depth_format,
        std::uint32_t sample_counts)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(float);

        return _vulkan_resource_storage.add(
            native::VulkanGraphicsPipeline{
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
                    to_vk_vertex_input_binding_description(vertex_input_binding_list),
                    to_vk_vertex_input_attribute_description(vertex_input_attribute_list),
                    {pushConstantRange},
                    static_cast<VkSampleCountFlagBits>(sample_counts),
                    to_vk_format(color_format),
                    to_vk_format(depth_format),
                }});
    }

    api::ImageReference VulkanGraphicsBackend::make_image(
        api::EImageType image_type,
        const math::Vector3i32 dimension,
        const api::EFormat image_format,
        const std::uint32_t mip_levels,
        const std::uint32_t sample_count)
    {
        auto vk_image_usage = 0;
        switch (image_type)
        {
            case api::EImageType::COLOR_ATTACHMENT:
            {
                vk_image_usage =
                    VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                break;
            }
            case api::EImageType::DEPTH_ATTACHMENT:
            {
                vk_image_usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                break;
            }
            case api::EImageType::TEXTURE:
            {
                vk_image_usage =
                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                    VK_IMAGE_USAGE_SAMPLED_BIT;
                break;
            }
        }

        return _vulkan_resource_storage.add(
            native::VulkanImage{
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

    api::ImageViewReference VulkanGraphicsBackend::make_image_view(
        const api::ImageReference& image,
        api::EImageAspect image_aspect,
        std::uint32_t mip_levels)
    {
        const auto& vulkan_image = _vulkan_resource_storage.get(image);

        return _vulkan_resource_storage.add(
            native::VulkanImageView{
                {
                    _vulkan_device,
                    vulkan_image.get_native(),
                    vulkan_image.get_vk_format(),
                    to_vk_image_aspec(image_aspect),
                    mip_levels
                }});
    }

    api::SamplerReference VulkanGraphicsBackend::make_sampler(const float mip_levels)
    {
        return _vulkan_resource_storage.add(
            native::VulkanSampler{
                {
                    _vulkan_device,
                    _vulkan_device.get_physical_device().get_vk_device_properties().limits.maxSamplerAnisotropy,
                    mip_levels,
                }
            });
    }

    api::ShaderReference VulkanGraphicsBackend::make_shader(const std::vector<char>& shader_byte_code)
    {
        return _vulkan_resource_storage.add(
            native::VulkanShader{
                {
                    _vulkan_device,
                    shader_byte_code,
                }});
    }

    api::SwapChainReference VulkanGraphicsBackend::make_swap_chain(
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
            native::VulkanSwapChain{
                {
                    _vulkan_device,
                    _vulkan_graphics_queue,
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
        const api::BufferReference& buffer,
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

    std::tuple<api::ESwapChainResult, std::uint32_t, std::uint32_t> VulkanGraphicsBackend::begin_frame(const api::SwapChainReference& swap_chain)
    {
        const auto result = _vulkan_resource_storage.get(swap_chain).begin_frame();

        return {
            to_swap_chain_result(result.vk_result),
            result.image_index,
            result.frame_buffer_index
        };
    }

    std::uint32_t VulkanGraphicsBackend::get_sample_count() const
    {
        return _vulkan_device.get_physical_device().get_vk_sample_count_flag_bits();
    }

    api::EFormat VulkanGraphicsBackend::find_depth_format() const
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

        return api::EFormat::D32_SFLOAT;
    }

    api::ESwapChainResult VulkanGraphicsBackend::end_frame(
        const api::CommandBufferReference& command_buffer,
        const api::SwapChainReference& swap_chain)
    {
        const auto result = _vulkan_resource_storage.get(swap_chain).end_frame(_vulkan_resource_storage.get(command_buffer).get_native());

        return to_swap_chain_result(result.vk_result);
    }

    void VulkanGraphicsBackend::copy_buffer(
        const api::CommandBufferReference& command_buffer,
        const api::BufferReference& source_buffer,
        const api::BufferReference& destination_buffer)
    {
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
            _vulkan_resource_storage.get(command_buffer).get_native(),
            vulkan_source_buffer.get_native(),
            vulkan_destination_buffer.get_native(),
            1,
            &vk_buffer_copy);
    }

    void VulkanGraphicsBackend::copy_buffer_to_image(
        const api::CommandBufferReference& command_buffer,
        const api::BufferReference& source_buffer,
        const api::ImageReference& target_image)
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
            _vulkan_resource_storage.get(command_buffer).get_native(),
            _vulkan_resource_storage.get(source_buffer).get_native(),
            vulkan_target_image.get_native(),
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );
    }

    void VulkanGraphicsBackend::generate_image_mip_maps(
        const api::CommandBufferReference& command_buffer,
        const api::ImageReference& image)
    {
        _vulkan_resource_storage.get(image).generate_mipmaps(_vulkan_resource_storage.get(command_buffer).get_native());
    }

    void VulkanGraphicsBackend::begin_command_buffer(
        const api::CommandBufferReference& command_buffer,
        const api::ECommandBufferType command_buffer_type)
    {
        _vulkan_resource_storage.get(command_buffer).begin(command_buffer_type == api::ECommandBufferType::ONE_TIME);
    }

    void VulkanGraphicsBackend::end_command_buffer(const api::CommandBufferReference& command_buffer)
    {
        _vulkan_resource_storage.get(command_buffer).end();
    }

    void VulkanGraphicsBackend::submit_command_buffer(const api::CommandBufferReference& command_buffer)
    {
        _vulkan_graphics_queue.submit(_vulkan_resource_storage.get(command_buffer));
    }

    void VulkanGraphicsBackend::set_vertex_buffer_list(
        const api::CommandBufferReference& command_buffer,
        const std::vector<api::BufferReference>& vertex_buffer_list,
        const std::vector<std::uint32_t>& vertex_buffer_offset_list,
        std::uint32_t first_slot)
    {
        XAR_THROW_IF(
            vertex_buffer_list.size() != vertex_buffer_offset_list.size(),
            error::XarException,
            "Number of buffers {} is different than number of offsets {}",
            vertex_buffer_list.size(),
            vertex_buffer_offset_list.size());

        auto vk_buffer_list = std::vector<VkBuffer>();
        auto vk_buffer_size_list = std::vector<VkDeviceSize>();
        for (auto i = 0; i < vertex_buffer_list.size(); ++i)
        {
            vk_buffer_list.push_back(_vulkan_resource_storage.get(vertex_buffer_list[i]).get_native());
            vk_buffer_size_list.push_back(static_cast<VkDeviceSize>(vertex_buffer_offset_list[i]));
        }

        vkCmdBindVertexBuffers(
            _vulkan_resource_storage.get(command_buffer).get_native(),
            first_slot,
            vertex_buffer_list.size(),
            vk_buffer_list.data(),
            vk_buffer_size_list.data());
    }

    void VulkanGraphicsBackend::set_index_buffer(
        const api::CommandBufferReference& command_buffer,
        const api::BufferReference& index_buffer,
        const std::uint32_t first_index)
    {
        vkCmdBindIndexBuffer(
            _vulkan_resource_storage.get(command_buffer).get_native(),
            _vulkan_resource_storage.get(index_buffer).get_native(),
            first_index,
            VK_INDEX_TYPE_UINT32);
    }

    void VulkanGraphicsBackend::push_constants(
        const api::CommandBufferReference& command_buffer,
        const api::GraphicsPipelineReference& graphics_pipeline,
        const api::EShaderType shader_type,
        const std::uint32_t offset,
        const std::uint32_t byte_size,
        void* const data_byte)
    {
        vkCmdPushConstants(
            _vulkan_resource_storage.get(command_buffer).get_native(),
            _vulkan_resource_storage.get(graphics_pipeline).get_native_pipeline_layout(),
            to_vk_shader_stage(shader_type),
            offset,
            byte_size,
            data_byte);
    }

    void VulkanGraphicsBackend::draw_indexed(
        const api::CommandBufferReference& command_buffer,
        const std::uint32_t index_counts,
        const std::uint32_t instance_counts,
        const std::uint32_t first_index,
        const std::uint32_t vertex_offset,
        const std::uint32_t first_instance)
    {
        vkCmdDrawIndexed(
            _vulkan_resource_storage.get(command_buffer).get_native(),
            static_cast<std::uint32_t>(index_counts),
            static_cast<std::uint32_t>(instance_counts),
            static_cast<std::uint32_t>(first_index),
            static_cast<std::int32_t>(vertex_offset),
            static_cast<std::uint32_t>(first_instance));
    }

    void VulkanGraphicsBackend::transit_image_layout(
        const api::CommandBufferReference& command_buffer,
        const api::ImageReference& image,
        api::EImageLayout new_image_layout)
    {
        VkImageLayout new_vk_image_layout = {};
        switch (new_image_layout)
        {
            case api::EImageLayout::TRANSFER_DESTINATION:
            {
                new_vk_image_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                break;
            }
            case api::EImageLayout::DEPTH_STENCIL_ATTACHMENT:
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
            _vulkan_resource_storage.get(command_buffer).get_native(),
            new_vk_image_layout);
    }

    void VulkanGraphicsBackend::wait_idle()
    {
        _vulkan_device.wait_idle();
    }

    void VulkanGraphicsBackend::begin_rendering(
        const api::CommandBufferReference& command_buffer,
        const api::SwapChainReference& swap_chain,
        std::uint32_t image_index,
        const api::ImageViewReference& color_image_view,
        const api::ImageViewReference& depth_image_view)
    {
        const auto& vulkan_command_buffer = _vulkan_resource_storage.get(command_buffer);
        const auto vk_command_buffer = vulkan_command_buffer.get_native();

        _vulkan_resource_storage.get(command_buffer).begin(false);

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
            vk_command_buffer,
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

        auto color_vk_clear_color = VkClearValue{};
        color_vk_clear_color.color = {0.0f, 0.0f, 0.0f, 1.0f};

        auto color_vk_rendering_attachment_info_khr = VkRenderingAttachmentInfoKHR{};
        color_vk_rendering_attachment_info_khr.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        color_vk_rendering_attachment_info_khr.clearValue = color_vk_clear_color;
        color_vk_rendering_attachment_info_khr.imageView = _vulkan_resource_storage.get(color_image_view).get_native();
        color_vk_rendering_attachment_info_khr.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_vk_rendering_attachment_info_khr.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_vk_rendering_attachment_info_khr.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_vk_rendering_attachment_info_khr.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
        color_vk_rendering_attachment_info_khr.resolveImageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
        color_vk_rendering_attachment_info_khr.resolveImageView = _vulkan_resource_storage.get(swap_chain).get_vulkan_image_view(image_index).get_native();

        auto depth_vk_clear_value = VkClearValue{};
        depth_vk_clear_value.depthStencil = {1.0f, 0};

        auto depth_vk_rendering_attachment_info_khr = VkRenderingAttachmentInfoKHR{};
        depth_vk_rendering_attachment_info_khr.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        depth_vk_rendering_attachment_info_khr.clearValue = depth_vk_clear_value;
        depth_vk_rendering_attachment_info_khr.imageView = _vulkan_resource_storage.get(depth_image_view).get_native();
        depth_vk_rendering_attachment_info_khr.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_vk_rendering_attachment_info_khr.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_vk_rendering_attachment_info_khr.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        auto vk_rendering_info_khr = VkRenderingInfoKHR{};
        vk_rendering_info_khr.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
        vk_rendering_info_khr.colorAttachmentCount = 1;
        vk_rendering_info_khr.pColorAttachments = &color_vk_rendering_attachment_info_khr;
        vk_rendering_info_khr.pDepthAttachment = &depth_vk_rendering_attachment_info_khr;
        vk_rendering_info_khr.renderArea = VkRect2D{
            VkOffset2D{}, _vulkan_resource_storage.get(swap_chain).get_extent()
        };
        vk_rendering_info_khr.layerCount = 1;

        vkCmdBeginRenderingKHR(
            vk_command_buffer,
            &vk_rendering_info_khr);
    }

    void VulkanGraphicsBackend::set_pipeline_state(
        const api::CommandBufferReference& command_buffer,
        const api::SwapChainReference& swap_chain,
        const api::GraphicsPipelineReference& graphics_pipeline,
        const api::DescriptorSetReference& descriptor_set)
    {
        const auto vk_command_buffer = _vulkan_resource_storage.get(command_buffer).get_native();

        vkCmdBindPipeline(
            vk_command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            _vulkan_resource_storage.get(graphics_pipeline).get_native_pipeline());

        const auto swap_chain_vk_extent =_vulkan_resource_storage.get(swap_chain).get_extent();

        auto vk_viewport = VkViewport{};
        vk_viewport.x = 0.0f;
        vk_viewport.y = 0.0f;
        vk_viewport.width = static_cast<float>(swap_chain_vk_extent.width);
        vk_viewport.height = static_cast<float>(swap_chain_vk_extent.height);
        vk_viewport.minDepth = 0.0f;
        vk_viewport.maxDepth = 1.0f;
        vkCmdSetViewport(
            vk_command_buffer,
            0,
            1,
            &vk_viewport);

        auto scissor_vk_rect_2d = VkRect2D{};
        scissor_vk_rect_2d.offset = {0, 0};
        scissor_vk_rect_2d.extent = swap_chain_vk_extent;
        vkCmdSetScissor(
            vk_command_buffer,
            0,
            1,
            &scissor_vk_rect_2d);

        const auto vk_descriptor_set = _vulkan_resource_storage.get(descriptor_set).get_native();
        vkCmdBindDescriptorSets(
            vk_command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            _vulkan_resource_storage.get(graphics_pipeline).get_native_pipeline_layout(),
            0,
            1,
            &vk_descriptor_set,
            0,
            nullptr);
    }

    void VulkanGraphicsBackend::end_rendering(
        const api::CommandBufferReference& command_buffer,
        const api::SwapChainReference& swap_chain,
        const std::uint32_t image_index)
    {
        vkCmdEndRenderingKHR(_vulkan_resource_storage.get(command_buffer).get_native());

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
            _vulkan_resource_storage.get(command_buffer).get_native(),
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

        _vulkan_resource_storage.get(command_buffer).end();
    }
}