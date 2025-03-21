#include <xar_engine/graphics/backend/unit/vulkan/vulkan_swap_chain_unit.hpp>

#include <xar_engine/graphics/backend/vulkan/vulkan_type_converters.hpp>


namespace xar_engine::graphics::backend::unit::vulkan
{
    api::SwapChainReference IVulkanSwapChainUnit::make_swap_chain(const MakeSwapChainParameters& parameters)
    {
        auto vulkan_window_surface = std::dynamic_pointer_cast<context::vulkan::VulkanWindowSurface>(parameters.window_surface);

        auto vk_format_to_use = VkSurfaceFormatKHR{};
        const auto vk_format_list = get_state()._vulkan_device.get_native_physical_device().get_vk_surface_format_khr_list(vulkan_window_surface->get_vulkan_surface().get_native());
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
        const auto vk_present_mode_list = get_state()._vulkan_device.get_native_physical_device().get_vk_present_mode_khr_list(vulkan_window_surface->get_vulkan_surface().get_native());
        for (const auto& vk_present_mode: vk_present_mode_list)
        {
            if (vk_present_mode == VK_PRESENT_MODE_FIFO_KHR)
            {
                vk_present_mode_to_use = vk_present_mode;
                break;
            }
        }

        return get_state()._vulkan_resource_storage.add(
            native::vulkan::VulkanSwapChain{
                {
                    get_state()._vulkan_device,
                    get_state()._vulkan_graphics_queue,
                    vulkan_window_surface,
                    vk_present_mode_to_use,
                    vk_format_to_use,
                    static_cast<std::int32_t>(parameters.buffering_level)
                }});
    }

    std::tuple<api::ESwapChainResult, std::uint32_t, std::uint32_t> IVulkanSwapChainUnit::begin_frame(const BeginFrameParameters& parameters)
    {
        const auto result = get_state()._vulkan_resource_storage.get(parameters.swap_chain).begin_frame();

        return {
            backend::vulkan::to_swap_chain_result(result.vk_result),
            result.image_index,
            result.frame_buffer_index
        };
    }

    void IVulkanSwapChainUnit::begin_rendering(const BeginRenderingParameters& parameters)
    {
        get_state()._vulkan_resource_storage.get(parameters.command_buffer).begin(false);

        const auto& vulkan_command_buffer = get_state()._vulkan_resource_storage.get(parameters.command_buffer);
        const auto vk_command_buffer = vulkan_command_buffer.get_native();

        const VkImageMemoryBarrier image_memory_barrier_start{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .image = get_state()._vulkan_resource_storage.get(parameters.swap_chain).get_vk_image(parameters.image_index),
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
        color_vk_rendering_attachment_info_khr.imageView = get_state()._vulkan_resource_storage.get(parameters.color_image_view).get_native();
        color_vk_rendering_attachment_info_khr.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_vk_rendering_attachment_info_khr.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_vk_rendering_attachment_info_khr.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_vk_rendering_attachment_info_khr.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
        color_vk_rendering_attachment_info_khr.resolveImageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
        color_vk_rendering_attachment_info_khr.resolveImageView = get_state()._vulkan_resource_storage.get(parameters.swap_chain).get_vulkan_image_view(parameters.image_index).get_native();

        auto depth_vk_clear_value = VkClearValue{};
        depth_vk_clear_value.depthStencil = {1.0f, 0};

        auto depth_vk_rendering_attachment_info_khr = VkRenderingAttachmentInfoKHR{};
        depth_vk_rendering_attachment_info_khr.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        depth_vk_rendering_attachment_info_khr.clearValue = depth_vk_clear_value;
        depth_vk_rendering_attachment_info_khr.imageView = get_state()._vulkan_resource_storage.get(parameters.depth_image_view).get_native();
        depth_vk_rendering_attachment_info_khr.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_vk_rendering_attachment_info_khr.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_vk_rendering_attachment_info_khr.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        auto vk_rendering_info_khr = VkRenderingInfoKHR{};
        vk_rendering_info_khr.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
        vk_rendering_info_khr.colorAttachmentCount = 1;
        vk_rendering_info_khr.pColorAttachments = &color_vk_rendering_attachment_info_khr;
        vk_rendering_info_khr.pDepthAttachment = &depth_vk_rendering_attachment_info_khr;
        vk_rendering_info_khr.renderArea = VkRect2D{
            VkOffset2D{}, get_state()._vulkan_resource_storage.get(parameters.swap_chain).get_vk_extent()
        };
        vk_rendering_info_khr.layerCount = 1;

        vkCmdBeginRenderingKHR(
            vk_command_buffer,
            &vk_rendering_info_khr);
    }

    void IVulkanSwapChainUnit::end_rendering(const EndRenderingParameters& parameters)
    {

        vkCmdEndRenderingKHR(get_state()._vulkan_resource_storage.get(parameters.command_buffer).get_native());

        const VkImageMemoryBarrier image_memory_barrier_end{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .image = get_state()._vulkan_resource_storage.get(parameters.swap_chain).get_vk_image(parameters.image_index),
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            }
        };

        vkCmdPipelineBarrier(
            get_state()._vulkan_resource_storage.get(parameters.command_buffer).get_native(),
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

        get_state()._vulkan_resource_storage.get(parameters.command_buffer).end();
    }

    api::ESwapChainResult IVulkanSwapChainUnit::end_frame(const EndFrameParameters& parameters)
    {
        const auto result = get_state()._vulkan_resource_storage.get(parameters.swap_chain).end_frame(get_state()._vulkan_resource_storage.get(parameters.command_buffer).get_native());

        return backend::vulkan::to_swap_chain_result(result.vk_result);
    }
}