#include <xar_engine/graphics/vulkan/vulkan_renderer.hpp>

#include <chrono>
#include <thread>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <xar_engine/asset/image_loader.hpp>
#include <xar_engine/asset/model_loader.hpp>

#include <xar_engine/error/exception_utils.hpp>

#include <xar_engine/file/file.hpp>

#include <xar_engine/logging/console_logger.hpp>


namespace xar_engine::graphics::vulkan
{
    namespace
    {
        constexpr int MAX_FRAMES_IN_FLIGHT = 2;
        constexpr auto tag = "Vulkan Sandbox";

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
                attributeDescriptions[0].offset = offsetof(Vertex,
                                                           position);

                attributeDescriptions[1].binding = 0;
                attributeDescriptions[1].location = 1;
                attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributeDescriptions[1].offset = offsetof(Vertex,
                                                           color);

                attributeDescriptions[2].binding = 0;
                attributeDescriptions[2].location = 2;
                attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
                attributeDescriptions[2].offset = offsetof(Vertex,
                                                           textureCoords);

                return attributeDescriptions;
            }
        };

        struct UniformBufferObject
        {
            alignas(16) glm::mat4 model;
            alignas(16) glm::mat4 view;
            alignas(16) glm::mat4 proj;
        };

        std::vector<Vertex> vertices;
        std::vector<std::uint32_t> indices;
    }

    void VulkanRenderer::init_device()
    {
        _vulkan_physical_device_list = std::make_unique<VulkanPhysicalDeviceList>(
            VulkanPhysicalDeviceList::Parameters{
                _vk_instance
            });
        _vulkan_device = std::make_unique<VulkanDevice>(
            VulkanDevice::Parameters{
                _vulkan_physical_device_list->get_native(0)
            });
        msaaSamples = _vulkan_physical_device_list->get_max_sample_count(0);
    }

    void VulkanRenderer::destroy_swapchain()
    {
        _vulkan_color_image_view.reset();
        _vulkan_color_image.reset();

        _vulkan_swap_chain_image_views.clear();

        _vulkan_depth_image_view.reset();
        _vulkan_depth_image.reset();

        _vulkan_swap_chain.reset();
    }

    void VulkanRenderer::init_swapchain()
    {
        const auto formats = _vulkan_physical_device_list->get_surface_formats(
            0,
            _vulkan_surface->get_native());
        VkSurfaceFormatKHR format_to_use;
        for (const auto& format: formats)
        {
            if (format.format == VK_FORMAT_R8G8B8A8_SRGB &&
                format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                format_to_use = format;
                break;
            }
        }

        const auto present_modes = _vulkan_physical_device_list->get_present_modes(
            0,
            _vulkan_surface->get_native());
        VkPresentModeKHR present_mode_to_use;
        for (const auto& present_mode: present_modes)
        {
            if (present_mode == VK_PRESENT_MODE_FIFO_KHR)
            {
                present_mode_to_use = present_mode;
                break;
            }
        }

        _vulkan_swap_chain = std::make_unique<VulkanSwapChain>(
            VulkanSwapChain::Parameters{
                _vulkan_device->get_native(),
                _vulkan_surface->get_native(),
                _vulkan_physical_device_list->get_surface_capabilities(
                    0,
                    _vulkan_surface->get_native()),
                _os_window->get_surface_pixel_size(),
                present_mode_to_use,
                format_to_use,
                MAX_FRAMES_IN_FLIGHT,
            });

        _vulkan_swap_chain_image_views.reserve(_vulkan_swap_chain->get_swap_chain_images().size());
        for (auto& swap_chain_image: _vulkan_swap_chain->get_swap_chain_images())
        {
            _vulkan_swap_chain_image_views.emplace_back(
                VulkanImageView::Parameters{
                    _vulkan_device->get_native(),
                    swap_chain_image,
                    format_to_use.format,
                    VK_IMAGE_ASPECT_COLOR_BIT,
                    1,
                });
        }
    }

    void VulkanRenderer::init_shaders()
    {
        _vulkan_vertex_shader = std::make_unique<VulkanShader>(
            VulkanShader::Parameters{
                _vulkan_device->get_native(),
                xar_engine::file::read_binary_file("assets/triangle.vert.spv")
            });

        _vulkan_fragment_shader = std::make_unique<VulkanShader>(
            VulkanShader::Parameters{
                _vulkan_device->get_native(),
                xar_engine::file::read_binary_file("assets/triangle.frag.spv")
            });
    }

    void VulkanRenderer::init_descriptor_set_layout()
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

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<std::uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(
            _vulkan_device->get_native(),
            &layoutInfo,
            nullptr,
            &descriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void VulkanRenderer::init_graphics_pipeline()
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(float);

        _vulkan_graphics_pipeline = std::make_unique<VulkanGraphicsPipeline>(
            VulkanGraphicsPipeline::Parameters{
                _vulkan_device->get_native(),
                {
                    std::make_tuple(
                        _vulkan_vertex_shader->get_native(),
                        VK_SHADER_STAGE_VERTEX_BIT,
                        "main"),
                    std::make_tuple(
                        _vulkan_fragment_shader->get_native(),
                        VK_SHADER_STAGE_FRAGMENT_BIT,
                        "main")
                },
                descriptorSetLayout,
                Vertex::getBindingDescription(),
                Vertex::getAttributeDescriptions(),
                {pushConstantRange},
                msaaSamples,
                _vulkan_swap_chain->get_format(),
                findDepthFormat(),
            });
    }

    void VulkanRenderer::init_vertex_data()
    {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        auto staging_buffer = VulkanBuffer{{
                                               _vulkan_device->get_native(),
                                               _vulkan_physical_device_list->get_native(0),
                                               bufferSize,
                                               VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           }};

        void* const data = staging_buffer.map();
        memcpy(
            data,
            vertices.data(),
            (size_t) bufferSize);
        staging_buffer.unmap();

        _vulkan_vertex_buffer = std::make_unique<VulkanBuffer>(
            VulkanBuffer::Parameters{
                _vulkan_device->get_native(),
                _vulkan_physical_device_list->get_native(0),
                bufferSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            });

        copyBuffer(
            staging_buffer.get_native(),
            _vulkan_vertex_buffer->get_native(),
            bufferSize);
    }

    void VulkanRenderer::init_index_data()
    {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        auto staging_buffer = VulkanBuffer{{
                                               _vulkan_device->get_native(),
                                               _vulkan_physical_device_list->get_native(0),
                                               bufferSize,
                                               VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           }};

        void* const data = staging_buffer.map();
        memcpy(
            data,
            indices.data(),
            (size_t) bufferSize);
        staging_buffer.unmap();

        _vulkan_index_buffer = std::make_unique<VulkanBuffer>(
            VulkanBuffer::Parameters{
                _vulkan_device->get_native(),
                _vulkan_physical_device_list->get_native(0),
                bufferSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            });

        copyBuffer(
            staging_buffer.get_native(),
            _vulkan_index_buffer->get_native(),
            bufferSize);
    }

    void VulkanRenderer::init_model()
    {
        const auto model = xar_engine::asset::ModelLoaderFactory().make()->load_model_from_file("assets/viking_room.obj");

        vertices.resize(model.meshes.back().positions.size());
        for (auto i = 0; i < model.meshes.back().positions.size(); ++i)
        {
            vertices[i].position.x = model.meshes.back().positions[i].x;
            vertices[i].position.y = model.meshes.back().positions[i].y;
            vertices[i].position.z = model.meshes.back().positions[i].z;

            const auto channels = model.meshes.back().texture_coords[0].channel_count;
            vertices[i].textureCoords.x = model.meshes.back().texture_coords[0].coords[i * channels + 0];
            vertices[i].textureCoords.y = model.meshes.back().texture_coords[0].coords[i * channels + 1];
        }

        indices.resize(model.meshes.back().indices.size());
        memcpy(
            indices.data(),
            model.meshes.back().indices.data(),
            sizeof(indices[0]) * indices.size());
    }

    void VulkanRenderer::init_ubo_data()
    {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        _vulkan_uniform_buffers.resize(MAX_FRAMES_IN_FLIGHT);
        _uniform_buffers_mapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            _vulkan_uniform_buffers[i] = std::make_unique<VulkanBuffer>(
                VulkanBuffer::Parameters{
                    _vulkan_device->get_native(),
                    _vulkan_physical_device_list->get_native(0),
                    bufferSize,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                });
            _uniform_buffers_mapped[i] = _vulkan_uniform_buffers[i]->map();
        }
    }

    void VulkanRenderer::init_descriptors()
    {
        _vulkan_descriptor_pool = std::make_unique<VulkanDescriptorPool>(
            VulkanDescriptorPool::Parameters{
                _vulkan_device->get_native(),
                MAX_FRAMES_IN_FLIGHT,
                MAX_FRAMES_IN_FLIGHT,
                MAX_FRAMES_IN_FLIGHT,
            });

        // sets
        std::vector<VkDescriptorSetLayout> layouts(
            MAX_FRAMES_IN_FLIGHT,
            descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = _vulkan_descriptor_pool->get_native();
        allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(
            _vulkan_device->get_native(),
            &allocInfo,
            descriptorSets.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = _vulkan_uniform_buffers[i]->get_native();
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = _vulkan_texture_image_view->get_native();
            imageInfo.sampler = _vulkan_sampler->get_native();

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;
            descriptorWrites[0].pImageInfo = nullptr; // Optional
            descriptorWrites[0].pTexelBufferView = nullptr; // Optional

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pBufferInfo = nullptr;
            descriptorWrites[1].pImageInfo = &imageInfo; // Optional
            descriptorWrites[1].pTexelBufferView = nullptr; // Optional

            vkUpdateDescriptorSets(
                _vulkan_device->get_native(),
                static_cast<std::uint32_t>(descriptorWrites.size()),
                descriptorWrites.data(),
                0,
                nullptr);
        }
    }

    void VulkanRenderer::init_cmd_buffers()
    {
        _vulkan_command_pool = std::make_unique<VulkanCommandBufferPool>(
            VulkanCommandBufferPool::Parameters{
                _vulkan_device->get_native(),
                _vulkan_device->get_graphics_queue(),
                _vulkan_device->get_graphics_family_index(),
            });
        _vk_command_buffers = _vulkan_command_pool->make_buffers(MAX_FRAMES_IN_FLIGHT);
    }

    void VulkanRenderer::init_color_msaa()
    {
        _vulkan_color_image = std::make_unique<VulkanImage>(
            VulkanImage::Parameters{
                _vulkan_device->get_native(),
                _vulkan_physical_device_list->get_native(0),
                {
                    static_cast<std::int32_t>(_vulkan_swap_chain->get_extent().width),
                    static_cast<std::int32_t>(_vulkan_swap_chain->get_extent().height),
                    1
                },
                _vulkan_swap_chain->get_format(),
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                1,
                msaaSamples,
            });

        _vulkan_color_image_view = std::make_unique<VulkanImageView>(
            VulkanImageView::Parameters{
                _vulkan_device->get_native(),
                _vulkan_color_image->get_native(),
                _vulkan_swap_chain->get_format(),
                VK_IMAGE_ASPECT_COLOR_BIT,
                1,
            });
    }

    void VulkanRenderer::init_depth()
    {
        VkFormat depthFormat = findDepthFormat();

        _vulkan_depth_image = std::make_unique<VulkanImage>(
            VulkanImage::Parameters{
                _vulkan_device->get_native(),
                _vulkan_physical_device_list->get_native(0),
                {
                    static_cast<std::int32_t>(_vulkan_swap_chain->get_extent().width),
                    static_cast<std::int32_t>(_vulkan_swap_chain->get_extent().height),
                    1
                },
                findDepthFormat(),
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                1,
                msaaSamples,
            });

        _vulkan_depth_image_view = std::make_unique<VulkanImageView>(
            VulkanImageView::Parameters{
                _vulkan_device->get_native(),
                _vulkan_depth_image->get_native(),
                findDepthFormat(),
                VK_IMAGE_ASPECT_DEPTH_BIT,
                1,
            });

        auto buffer = _vulkan_command_pool->make_one_time_buffer();
        _vulkan_depth_image->transition_layout(
            buffer,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        _vulkan_command_pool->submit_one_time_buffer(buffer);
    }

    void VulkanRenderer::init_texture()
    {
        const auto image = asset::ImageLoaderFactory().make()->load_image_from_file("assets/viking_room.png");
        mipLevels = static_cast<uint32_t>(std::floor(
            std::log2(
                std::max(
                    image.pixel_width,
                    image.pixel_height)))) + 1;

        VkDeviceSize imageSize = asset::image::get_byte_size(image);


        auto staging_buffer = VulkanBuffer{{
                                               _vulkan_device->get_native(),
                                               _vulkan_physical_device_list->get_native(0),
                                               imageSize,
                                               VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           }};

        void* const data = staging_buffer.map();
        memcpy(
            data,
            image.bytes.data(),
            static_cast<size_t>(imageSize));
        staging_buffer.unmap();

        _vulkan_texture_image = std::make_unique<VulkanImage>(
            VulkanImage::Parameters{
                _vulkan_device->get_native(),
                _vulkan_physical_device_list->get_native(0),
                {image.pixel_width, image.pixel_height, 1},
                VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                mipLevels,
                VK_SAMPLE_COUNT_1_BIT,
            });

        auto buffer = _vulkan_command_pool->make_one_time_buffer();
        _vulkan_texture_image->transition_layout(
            buffer,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        _vulkan_command_pool->submit_one_time_buffer(buffer);

        copyBufferToImage(
            staging_buffer.get_native(),
            _vulkan_texture_image->get_native(),
            image.pixel_width,
            image.pixel_height);

        VkCommandBuffer tempCommandBuffer = _vulkan_command_pool->make_one_time_buffer();
        _vulkan_texture_image->generate_mipmaps(
            tempCommandBuffer,
            _vulkan_physical_device_list->get_native(0));
        _vulkan_command_pool->submit_one_time_buffer(tempCommandBuffer);
    }

    void VulkanRenderer::init_texture_view()
    {
        _vulkan_texture_image_view = std::make_unique<VulkanImageView>(
            VulkanImageView::Parameters{
                _vulkan_device->get_native(),
                _vulkan_texture_image->get_native(),
                VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_ASPECT_COLOR_BIT,
                mipLevels,
            });
    }

    void VulkanRenderer::init_sampler()
    {
        _vulkan_sampler = std::make_unique<VulkanSampler>(
            VulkanSampler::Parameters{
                _vulkan_device->get_native(),
                _vulkan_physical_device_list->get_device_properties(0).limits.maxSamplerAnisotropy,
                static_cast<float>(mipLevels),
            });
    }

    void VulkanRenderer::run_frame_sandbox()
    {
        const auto begin_frame_result = _vulkan_swap_chain->begin_frame(currentFrame);
        if (begin_frame_result.vk_result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            XAR_LOG(
                logging::LogLevel::ERROR,
                *_logger,
                tag,
                "Acquire failed because Swapchain is out of date");

            _vulkan_device->wait_idle();
            destroy_swapchain();
            init_swapchain();
            init_color_msaa();
            init_depth();

            XAR_LOG(
                logging::LogLevel::DEBUG,
                *_logger,
                tag,
                "Acquire failed because Swapchain is out of date but swapchain was recreated");
            return;
        }
        else if (begin_frame_result.vk_result != VK_SUCCESS && begin_frame_result.vk_result != VK_SUBOPTIMAL_KHR)
        {
            XAR_LOG(
                logging::LogLevel::ERROR,
                *_logger,
                tag,
                "Acquire failed because Swapchain");
            return;
        }

        // Record CMD buffer
        {
            vkResetCommandBuffer(
                _vk_command_buffers[currentFrame],
                0);

            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0; // Optional
            beginInfo.pInheritanceInfo = nullptr; // Optional

            if (vkBeginCommandBuffer(
                _vk_command_buffers[currentFrame],
                &beginInfo) != VK_SUCCESS)
            {
                XAR_THROW(error::XarException,
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
                .image = _vulkan_swap_chain->get_swap_chain_images()[begin_frame_result.image_index],
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                }
            };

            vkCmdPipelineBarrier(
                _vk_command_buffers[currentFrame],
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
            vkRenderingAttachmentInfoColor.imageView = _vulkan_color_image_view->get_native();
            vkRenderingAttachmentInfoColor.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            vkRenderingAttachmentInfoColor.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            vkRenderingAttachmentInfoColor.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            vkRenderingAttachmentInfoColor.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
            vkRenderingAttachmentInfoColor.resolveImageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
            vkRenderingAttachmentInfoColor.resolveImageView = _vulkan_swap_chain_image_views[begin_frame_result.image_index].get_native();

            VkClearValue clearDepthColor{};
            clearDepthColor.depthStencil = {1.0f, 0};

            VkRenderingAttachmentInfoKHR vkRenderingAttachmentInfoDepth{};
            vkRenderingAttachmentInfoDepth.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
            vkRenderingAttachmentInfoDepth.clearValue = clearDepthColor;
            vkRenderingAttachmentInfoDepth.imageView = _vulkan_depth_image_view->get_native();
            vkRenderingAttachmentInfoDepth.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            vkRenderingAttachmentInfoDepth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            vkRenderingAttachmentInfoDepth.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

            VkRenderingInfoKHR renderingInfo{};
            renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
            renderingInfo.colorAttachmentCount = 1;
            renderingInfo.pColorAttachments = &vkRenderingAttachmentInfoColor;
            renderingInfo.pDepthAttachment = &vkRenderingAttachmentInfoDepth;
            renderingInfo.renderArea = VkRect2D{VkOffset2D{}, _vulkan_swap_chain->get_extent()};
            renderingInfo.layerCount = 1;

            vkCmdBeginRenderingKHR(
                _vk_command_buffers[currentFrame],
                &renderingInfo);

            vkCmdBindPipeline(
                _vk_command_buffers[currentFrame],
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                _vulkan_graphics_pipeline->get_native_pipeline());

            const Constants pc = {static_cast<float>(frameCounter)};
            vkCmdPushConstants(
                _vk_command_buffers[currentFrame],
                _vulkan_graphics_pipeline->get_native_pipeline_layout(),
                VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(Constants),
                &pc);

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float) _vulkan_swap_chain->get_extent().width;
            viewport.height = (float) _vulkan_swap_chain->get_extent().height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(
                _vk_command_buffers[currentFrame],
                0,
                1,
                &viewport);

            VkRect2D scissor{};
            scissor.offset = {0, 0};
            scissor.extent = _vulkan_swap_chain->get_extent();
            vkCmdSetScissor(
                _vk_command_buffers[currentFrame],
                0,
                1,
                &scissor);

            VkBuffer vertexBuffers[] = {_vulkan_vertex_buffer->get_native()};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(
                _vk_command_buffers[currentFrame],
                0,
                1,
                vertexBuffers,
                offsets);
            vkCmdBindIndexBuffer(
                _vk_command_buffers[currentFrame],
                _vulkan_index_buffer->get_native(),
                0,
                VK_INDEX_TYPE_UINT32);

            vkCmdBindDescriptorSets(
                _vk_command_buffers[currentFrame],
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                _vulkan_graphics_pipeline->get_native_pipeline_layout(),
                0,
                1,
                &descriptorSets[currentFrame],
                0,
                nullptr);

            vkCmdDrawIndexed(
                _vk_command_buffers[currentFrame],
                static_cast<uint32_t>(indices.size()),
                1,
                0,
                0,
                0);

            vkCmdEndRenderingKHR(_vk_command_buffers[currentFrame]);

            const VkImageMemoryBarrier image_memory_barrier_end{
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .image = _vulkan_swap_chain->get_swap_chain_images()[begin_frame_result.image_index],
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                }
            };

            vkCmdPipelineBarrier(
                _vk_command_buffers[currentFrame],
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

            if (vkEndCommandBuffer(_vk_command_buffers[currentFrame]) != VK_SUCCESS)
            {
                XAR_THROW(error::XarException,
                          "failed to record command buffer!");
            }

            updateUniformBuffer(currentFrame);
        }

        const auto end_frame_result = _vulkan_swap_chain->end_frame(
            currentFrame,
            begin_frame_result.image_index,
            _vulkan_device->get_graphics_queue(),
            _vk_command_buffers[currentFrame]);
        if (end_frame_result.vk_result == VK_ERROR_OUT_OF_DATE_KHR || end_frame_result.vk_result == VK_SUBOPTIMAL_KHR)
        {
            XAR_LOG(
                logging::LogLevel::ERROR,
                *_logger,
                tag,
                "Present failed because Swapchain is out of date");

            _vulkan_device->wait_idle();
            destroy_swapchain();
            init_swapchain();
            init_color_msaa();
            init_depth();

            XAR_LOG(
                logging::LogLevel::DEBUG,
                *_logger,
                tag,
                "Present failed because Swapchain is out of date but swapchain was recreated");
        }
        else if (end_frame_result.vk_result != VK_SUCCESS)
        {
            XAR_LOG(
                logging::LogLevel::ERROR,
                *_logger,
                tag,
                "Acquire failed because Swapchain");
            return;
        }

        XAR_LOG(
            xar_engine::logging::LogLevel::DEBUG,
            *_logger,
            tag,
            "Frame buffer nr {}, frames in total {}",
            currentFrame,
            frameCounter);

        // change frame index
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        ++frameCounter;

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(16ms);
    }

    void VulkanRenderer::cleanup_sandbox()
    {
        _vulkan_device->wait_idle();

        _vulkan_command_pool.reset();

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            _vulkan_uniform_buffers[i].reset();
        }

        _vulkan_descriptor_pool.reset();

        // here because we may want to reuse it
        vkDestroyDescriptorSetLayout(
            _vulkan_device->get_native(),
            descriptorSetLayout,
            nullptr);

        _vulkan_graphics_pipeline.reset();

        _vulkan_fragment_shader.reset();
        _vulkan_vertex_shader.reset();

        _vulkan_index_buffer.reset();
        _vulkan_vertex_buffer.reset();

        _vulkan_sampler.reset();
        _vulkan_texture_image_view.reset();
        _vulkan_texture_image.reset();

        destroy_swapchain();

        _vulkan_device.reset();
        _vulkan_surface.reset();

        vkDestroyInstance(
            _vk_instance,
            nullptr);
#pragma endregion
    }

    void VulkanRenderer::copyBuffer(
        VkBuffer srcBuffer,
        VkBuffer dstBuffer,
        VkDeviceSize size)
    {
        VkCommandBuffer tmpCommandBuffer = _vulkan_command_pool->make_one_time_buffer();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(
            tmpCommandBuffer,
            srcBuffer,
            dstBuffer,
            1,
            &copyRegion);

        _vulkan_command_pool->submit_one_time_buffer(tmpCommandBuffer);
    }

    void VulkanRenderer::updateUniformBuffer(uint32_t currentImageNr)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo{};

        ubo.model = glm::rotate(
            glm::mat4(1.0f),
            time * glm::radians(90.0f),
            glm::vec3(
                0.0f,
                0.0f,
                1.0f));

        ubo.view = glm::lookAt(
            glm::vec3(
                2.0f,
                2.0f,
                2.0f),
            glm::vec3(
                0.0f,
                0.0f,
                0.0f),
            glm::vec3(
                0.0f,
                0.0f,
                1.0f));

        ubo.proj = glm::perspective(
            glm::radians(45.0f),
            _vulkan_swap_chain->get_extent().width / (float) (_vulkan_swap_chain->get_extent().height),
            0.1f,
            10.0f);

        ubo.proj[1][1] *= -1;

        memcpy(
            _uniform_buffers_mapped[currentImageNr],
            &ubo,
            sizeof(ubo));
    }

    void VulkanRenderer::copyBufferToImage(
        VkBuffer buffer,
        VkImage image,
        uint32_t width,
        uint32_t height)
    {
        VkCommandBuffer tempCommandBuffer = _vulkan_command_pool->make_one_time_buffer();

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
            width,
            height,
            1
        };

        vkCmdCopyBufferToImage(
            tempCommandBuffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

        _vulkan_command_pool->submit_one_time_buffer(tempCommandBuffer);
    }

    VkFormat VulkanRenderer::findDepthFormat()
    {
        return _vulkan_physical_device_list->findSupportedFormat(
            0,
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }
}


namespace xar_engine::graphics::vulkan
{
    VulkanRenderer::VulkanRenderer(
        VkInstance vk_instance,
        VkSurfaceKHR vk_surface_khr,
        const os::IWindow* os_window)
        : _vk_instance(vk_instance)
        , _vulkan_surface(
            std::make_unique<VulkanSurface>(
                VulkanSurface::Parameters{
                    vk_instance,
                    vk_surface_khr,
                }))
        , _os_window(os_window)
        , _logger(std::make_unique<logging::ConsoleLogger>())
        , currentFrame(0)
        , frameCounter(0)
    {
        init_device();
        init_swapchain();
        init_shaders();
        init_descriptor_set_layout();
        init_graphics_pipeline();
        init_cmd_buffers();
        init_color_msaa();
        init_depth();
        init_texture();
        init_texture_view();
        init_sampler();
        init_model();
        init_vertex_data();
        init_index_data();
        init_ubo_data();
        init_descriptors();
    }

    VulkanRenderer::~VulkanRenderer()
    {
        cleanup_sandbox();
    }

    void VulkanRenderer::update()
    {
        run_frame_sandbox();
    }
}