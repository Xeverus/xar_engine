#include <xar_engine/graphics/vulkan/impl/vulkan_renderer.hpp>

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


namespace xar_engine::graphics::vulkan::impl
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

    // DONE
    void VulkanRenderer::destroy_swapchain()
    {
        _color_image_view_ref = {};
        _color_image_ref = {};

        _depth_image_view_ref = {};
        _depth_image_ref = {};
    }

    // DONE
    void VulkanRenderer::init_descriptor_set_layout()
    {
        _descriptor_set_layout_ref = _vulkan_graphics_backend->resource().make_descriptor_set_layout();
    }

    // DONE
    void VulkanRenderer::init_graphics_pipeline()
    {
        _graphics_pipeline_ref = _vulkan_graphics_backend->resource().make_graphics_pipeline(
            _descriptor_set_layout_ref,
            _vertex_shader_ref,
            _fragment_shader_ref,
            EImageFormat::R8G8B8A8_SRGB,
            findDepthFormat(),
            msaaSamples);
    }

    // DONE
    void VulkanRenderer::init_vertex_data()
    {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        auto staging_buffer = _vulkan_graphics_backend->resource().make_staging_buffer(bufferSize);
        _vulkan_graphics_backend->host_command().update_buffer(
            staging_buffer,
            vertices.data(),
            bufferSize);

        _vertex_buffer_ref = _vulkan_graphics_backend->resource().make_vertex_buffer(bufferSize);

        auto tmp_command_buffer = _vulkan_graphics_backend->resource().make_one_time_command_buffer();
        _vulkan_graphics_backend->device_command().copy_buffer(
            tmp_command_buffer,
            staging_buffer,
            _vertex_buffer_ref);
        _vulkan_graphics_backend->device_command().submit_one_time_command_buffer(tmp_command_buffer);
    }

    // DONE
    void VulkanRenderer::init_index_data()
    {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        auto staging_buffer = _vulkan_graphics_backend->resource().make_staging_buffer(bufferSize);
        _vulkan_graphics_backend->host_command().update_buffer(
            staging_buffer,
            indices.data(),
            bufferSize);

        _index_buffer_ref = _vulkan_graphics_backend->resource().make_index_buffer(bufferSize);

        auto tmp_command_buffer = _vulkan_graphics_backend->resource().make_one_time_command_buffer();
        _vulkan_graphics_backend->device_command().copy_buffer(
            tmp_command_buffer,
            staging_buffer,
            _index_buffer_ref);
        _vulkan_graphics_backend->device_command().submit_one_time_command_buffer(tmp_command_buffer);
    }

    // DONE
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

    // DONE
    void VulkanRenderer::init_ubo_data()
    {
        _uniform_buffer_ref_list.resize(MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            _uniform_buffer_ref_list.push_back(_vulkan_graphics_backend->resource().make_uniform_buffer(sizeof(UniformBufferObject)));
        }
    }

    // DONE
    void VulkanRenderer::init_descriptors()
    {
        _descriptor_pool_ref = _vulkan_graphics_backend->resource().make_descriptor_pool();
        _descriptor_set_list_ref = _vulkan_graphics_backend->resource().make_descriptor_set_list(
            _descriptor_pool_ref,
            _descriptor_set_layout_ref,
            _uniform_buffer_ref_list,
            _texture_image_view_ref,
            _sampler_ref);
    }

    // DONE
    void VulkanRenderer::init_color_msaa()
    {
        _color_image_ref = _vulkan_graphics_backend->resource().make_image(
            EImageType::COLOR_ATTACHMENT,
            {
                static_cast<std::int32_t>(_vulkan_window_surface->get_pixel_size().x),
                static_cast<std::int32_t>(_vulkan_window_surface->get_pixel_size().y),
                1
            },
            EImageFormat::R8G8B8A8_SRGB,
            1,
            msaaSamples);


        _color_image_view_ref = _vulkan_graphics_backend->resource().make_image_view(
            _color_image_ref,
            EImageAspect::COLOR,
            1);
    }

    // DONE
    void VulkanRenderer::init_depth()
    {
        _depth_image_ref = _vulkan_graphics_backend->resource().make_image(
            EImageType::DEPTH_ATTACHMENT,
            {
                static_cast<std::int32_t>(_vulkan_window_surface->get_pixel_size().x),
                static_cast<std::int32_t>(_vulkan_window_surface->get_pixel_size().y),
                1
            },
            findDepthFormat(),
            1,
            msaaSamples);

        _depth_image_view_ref = _vulkan_graphics_backend->resource().make_image_view(
            _depth_image_ref,
            EImageAspect::DEPTH,
            1);

        auto cmd_bf = _vulkan_graphics_backend->resource().make_one_time_command_buffer();
        _vulkan_graphics_backend->device_command().transit_image_layout(
            cmd_bf,
            _depth_image_ref,
            EImageLayout::DEPTH_STENCIL_ATTACHMENT);
        _vulkan_graphics_backend->device_command().submit_one_time_command_buffer(cmd_bf);
    }

    // DONE
    void VulkanRenderer::init_texture()
    {
        auto image = asset::ImageLoaderFactory().make()->load_image_from_file("assets/viking_room.png");
        mipLevels = static_cast<uint32_t>(std::floor(
            std::log2(
                std::max(
                    image.pixel_width,
                    image.pixel_height)))) + 1;

        VkDeviceSize imageSize = asset::image::get_byte_size(image);

        auto staging_buffer = _vulkan_graphics_backend->resource().make_staging_buffer(imageSize);
        _vulkan_graphics_backend->host_command().update_buffer(
            staging_buffer,
            image.bytes.data(),
            static_cast<size_t>(imageSize));

        _texture_image_ref = _vulkan_graphics_backend->resource().make_image(
            EImageType::TEXTURE,
            {image.pixel_width, image.pixel_height, 1},
            EImageFormat::R8G8B8A8_SRGB,
            mipLevels,
            1);

        auto tmp_command_buffer = _vulkan_graphics_backend->resource().make_one_time_command_buffer();
        _vulkan_graphics_backend->device_command().transit_image_layout(
            tmp_command_buffer,
            _texture_image_ref,
            EImageLayout::TRANSFER_DESTINATION);
        _vulkan_graphics_backend->device_command().submit_one_time_command_buffer(tmp_command_buffer);

        tmp_command_buffer = _vulkan_graphics_backend->resource().make_one_time_command_buffer();
        _vulkan_graphics_backend->device_command().copy_buffer_to_image(
            tmp_command_buffer,
            staging_buffer,
            _texture_image_ref);
        _vulkan_graphics_backend->device_command().submit_one_time_command_buffer(tmp_command_buffer);

        tmp_command_buffer = _vulkan_graphics_backend->resource().make_one_time_command_buffer();
        _vulkan_graphics_backend->device_command().generate_image_mip_maps(tmp_command_buffer, _texture_image_ref);
        _vulkan_graphics_backend->device_command().submit_one_time_command_buffer(tmp_command_buffer);
    }

    // DONE
    void VulkanRenderer::init_texture_view()
    {
        _texture_image_view_ref = _vulkan_graphics_backend->resource().make_image_view(
            _texture_image_ref,
            EImageAspect::COLOR,
            mipLevels);
    }

    // DONE
    void VulkanRenderer::init_sampler()
    {
        _sampler_ref = _vulkan_graphics_backend->resource().make_sampler(static_cast<float>(mipLevels));
    }

    void VulkanRenderer::run_frame_sandbox()
    {
        const auto begin_frame_result = _swap_chain.begin_frame();

        if (begin_frame_result.vk_result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            XAR_LOG(
                logging::LogLevel::ERROR,
                tag,
                "Acquire failed because Swapchain is out of date");

            _vulkan_graphics_backend->device_command().wait_idle();
            _vulkan_graphics_backend->device_command().wait_idle();
            destroy_swapchain();

            _swap_chain_ref = _vulkan_graphics_backend->resource().make_swap_chain(
                _vulkan_window_surface,
                MAX_FRAMES_IN_FLIGHT);

            init_color_msaa();
            init_depth();

            XAR_LOG(
                logging::LogLevel::DEBUG,
                tag,
                "Acquire failed because Swapchain is out of date but swapchain was recreated");
            return;
        }
        else if (begin_frame_result.vk_result != VK_SUCCESS && begin_frame_result.vk_result != VK_SUBOPTIMAL_KHR)
        {
            XAR_LOG(
                logging::LogLevel::ERROR,
                tag,
                "Acquire failed because Swapchain");
            return;
        }

        const auto currentFrame = begin_frame_result.frame_index;

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
                .image = begin_frame_result.vk_image,
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
            vkRenderingAttachmentInfoColor.imageView = _color_image_view.get_native();
            vkRenderingAttachmentInfoColor.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            vkRenderingAttachmentInfoColor.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            vkRenderingAttachmentInfoColor.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            vkRenderingAttachmentInfoColor.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
            vkRenderingAttachmentInfoColor.resolveImageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
            vkRenderingAttachmentInfoColor.resolveImageView = begin_frame_result.vk_image_view;

            VkClearValue clearDepthColor{};
            clearDepthColor.depthStencil = {1.0f, 0};

            VkRenderingAttachmentInfoKHR vkRenderingAttachmentInfoDepth{};
            vkRenderingAttachmentInfoDepth.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
            vkRenderingAttachmentInfoDepth.clearValue = clearDepthColor;
            vkRenderingAttachmentInfoDepth.imageView = _depth_image_view.get_native();
            vkRenderingAttachmentInfoDepth.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            vkRenderingAttachmentInfoDepth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            vkRenderingAttachmentInfoDepth.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

            VkRenderingInfoKHR renderingInfo{};
            renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
            renderingInfo.colorAttachmentCount = 1;
            renderingInfo.pColorAttachments = &vkRenderingAttachmentInfoColor;
            renderingInfo.pDepthAttachment = &vkRenderingAttachmentInfoDepth;
            renderingInfo.renderArea = VkRect2D{VkOffset2D{}, _swap_chain.get_extent()};
            renderingInfo.layerCount = 1;

            vkCmdBeginRenderingKHR(
                _vk_command_buffers[currentFrame],
                &renderingInfo);

            vkCmdBindPipeline(
                _vk_command_buffers[currentFrame],
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                _vulkan_graphics_pipeline.get_native_pipeline());

            const Constants pc = {static_cast<float>(frameCounter)};
            vkCmdPushConstants(
                _vk_command_buffers[currentFrame],
                _vulkan_graphics_pipeline.get_native_pipeline_layout(),
                VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(Constants),
                &pc);

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float) _swap_chain.get_extent().width;
            viewport.height = (float) _swap_chain.get_extent().height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(
                _vk_command_buffers[currentFrame],
                0,
                1,
                &viewport);

            VkRect2D scissor{};
            scissor.offset = {0, 0};
            scissor.extent = _swap_chain.get_extent();
            vkCmdSetScissor(
                _vk_command_buffers[currentFrame],
                0,
                1,
                &scissor);

            VkBuffer vertexBuffers[] = {_vertex_buffer.get_native()};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(
                _vk_command_buffers[currentFrame],
                0,
                1,
                vertexBuffers,
                offsets);
            vkCmdBindIndexBuffer(
                _vk_command_buffers[currentFrame],
                _index_buffer.get_native(),
                0,
                VK_INDEX_TYPE_UINT32);

            vkCmdBindDescriptorSets(
                _vk_command_buffers[currentFrame],
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                _vulkan_graphics_pipeline.get_native_pipeline_layout(),
                0,
                1,
                &_vulkan_descriptor_sets.get_native()[currentFrame],
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
                .image = begin_frame_result.vk_image,
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
                XAR_THROW(
                    error::XarException,
                    "failed to record command buffer!");
            }

            updateUniformBuffer(currentFrame);
        }

        const auto end_frame_result = _swap_chain.end_frame(_vk_command_buffers[currentFrame]);
        if (end_frame_result.vk_result == VK_ERROR_OUT_OF_DATE_KHR || end_frame_result.vk_result == VK_SUBOPTIMAL_KHR)
        {
            XAR_LOG(
                logging::LogLevel::ERROR,
                tag,
                "Present failed because Swapchain is out of date");

            _vulkan_graphics_backend->device_command().wait_idle();
            destroy_swapchain();

            _swap_chain_ref = _vulkan_graphics_backend->resource().make_swap_chain(
                _vulkan_window_surface,
                MAX_FRAMES_IN_FLIGHT);

            init_color_msaa();
            init_depth();

            XAR_LOG(
                logging::LogLevel::DEBUG,
                tag,
                "Present failed because Swapchain is out of date but swapchain was recreated");
        }
        else if (end_frame_result.vk_result != VK_SUCCESS)
        {
            XAR_LOG(
                logging::LogLevel::ERROR,
                tag,
                "Acquire failed because Swapchain");
            return;
        }

        XAR_LOG(
            xar_engine::logging::LogLevel::DEBUG,
            tag,
            "Frame buffer nr {}, frames in total {}",
            currentFrame,
            frameCounter);

        // change frame index
        ++frameCounter;

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(16ms);
    }

    // DONE
    void VulkanRenderer::cleanup_sandbox()
    {
        _vulkan_graphics_backend->device_command().wait_idle();
    }

    // DONE
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
            _vulkan_window_surface->get_pixel_size().x / (float) _vulkan_window_surface->get_pixel_size().y,
            0.1f,
            10.0f);

        ubo.proj[1][1] *= -1;

        _vulkan_graphics_backend->host_command().update_buffer(
            _uniform_buffer_ref_list[currentImageNr],
            &ubo,
            sizeof(ubo));
    }

    // DONE
    EImageFormat VulkanRenderer::findDepthFormat()
    {
        const auto vk_format = _physical_device_list[0].find_supported_vk_format(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );

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

        return EImageFormat::D32_SFLOAT;
    }
}


namespace xar_engine::graphics::vulkan::impl
{
    VulkanRenderer::VulkanRenderer(
        const std::shared_ptr<VulkanInstance>& vulkan_instance,
        std::shared_ptr<VulkanWindowSurface> vulkan_window_surface)
        : _instance(vulkan_instance)
        , frameCounter(0)
        , _vulkan_window_surface(std::move(vulkan_window_surface))
    {
        _vulkan_graphics_backend = std::make_unique<VulkanGraphicsBackend>();
        _command_buffer_list = _vulkan_graphics_backend->resource().make_command_buffers(MAX_FRAMES_IN_FLIGHT);

        _swap_chain_ref = _vulkan_graphics_backend->resource().make_swap_chain(
            _vulkan_window_surface,
            MAX_FRAMES_IN_FLIGHT);

        _vertex_shader_ref = _vulkan_graphics_backend->resource().make_shader(xar_engine::file::read_binary_file("assets/triangle.vert.spv"));
        _fragment_shader_ref = _vulkan_graphics_backend->resource().make_shader(xar_engine::file::read_binary_file("assets/triangle.frag.spv"));

        init_descriptor_set_layout();
        init_graphics_pipeline();
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