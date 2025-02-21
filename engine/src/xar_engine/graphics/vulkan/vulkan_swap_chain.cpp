#include <xar_engine/graphics/vulkan/vulkan_swap_chain.hpp>

#include <algorithm>
#include <cmath>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    VulkanSwapChain::VulkanSwapChain(const VulkanSwapChain::Parameters& parameters)
        : _vk_swap_chain(nullptr)
        , _vk_device(parameters.vk_device)
        , _vk_surface_khr(parameters.vk_surface_khr)
        , _vk_extent({})
        , _vk_surface_format_khr(parameters.surface_format_khr)
    {
        _vk_extent = {
            static_cast<std::uint32_t>(parameters.dimension.x),
            static_cast<std::uint32_t>(parameters.dimension.y),
        };
        _vk_extent.width = std::clamp(
            _vk_extent.width,
            parameters.vk_surface_capabilities_khr.minImageExtent.width,
            parameters.vk_surface_capabilities_khr.maxImageExtent.width);
        _vk_extent.height = std::clamp(
            _vk_extent.height,
            parameters.vk_surface_capabilities_khr.minImageExtent.height,
            parameters.vk_surface_capabilities_khr.maxImageExtent.height);
        std::uint32_t image_count = parameters.vk_surface_capabilities_khr.minImageCount + 1;
        image_count = parameters.vk_surface_capabilities_khr.maxImageCount == 0 ? image_count : std::min(
            image_count,
            parameters.vk_surface_capabilities_khr.maxImageCount);

        VkSwapchainCreateInfoKHR swapchain_info{};
        swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_info.surface = _vk_surface_khr;
        swapchain_info.minImageCount = image_count;
        swapchain_info.imageFormat = parameters.surface_format_khr.format;
        swapchain_info.imageColorSpace = parameters.surface_format_khr.colorSpace;
        swapchain_info.imageExtent = _vk_extent;
        swapchain_info.imageArrayLayers = 1;
        swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_info.preTransform = parameters.vk_surface_capabilities_khr.currentTransform;
        swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchain_info.presentMode = parameters.present_mode_khr;
        swapchain_info.clipped = VK_TRUE;
        swapchain_info.oldSwapchain = VK_NULL_HANDLE;

        const auto swap_chain_result = vkCreateSwapchainKHR(
            _vk_device,
            &swapchain_info,
            nullptr,
            &_vk_swap_chain);
        XAR_THROW_IF(
            swap_chain_result != VK_SUCCESS,
            error::XarException,
            "Vulkan swapchain creation failed");

        std::uint32_t swap_chain_images_count = 0;
        vkGetSwapchainImagesKHR(
            _vk_device,
            _vk_swap_chain,
            &swap_chain_images_count,
            nullptr);

        _vk_swap_chain_images.resize(swap_chain_images_count);
        vkGetSwapchainImagesKHR(
            _vk_device,
            _vk_swap_chain,
            &swap_chain_images_count,
            _vk_swap_chain_images.data());

        // sync
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        imageAvailableSemaphore.resize(parameters.buffering_level);
        renderFinishedSemaphore.resize(parameters.buffering_level);
        inFlightFence.resize(parameters.buffering_level);

        for (auto i = 0; i < parameters.buffering_level; ++i)
        {
            const auto create_available_result = vkCreateSemaphore(
                _vk_device,
                &semaphoreInfo,
                nullptr,
                &imageAvailableSemaphore[i]);
            const auto create_finished_result = vkCreateSemaphore(
                _vk_device,
                &semaphoreInfo,
                nullptr,
                &renderFinishedSemaphore[i]);
            const auto create_flight_result = vkCreateFence(
                _vk_device,
                &fenceInfo,
                nullptr,
                &inFlightFence[i]);

            XAR_THROW_IF(
                create_available_result != VK_SUCCESS ||
                create_finished_result != VK_SUCCESS ||
                create_flight_result != VK_SUCCESS,
                error::XarException,
                "failed to create sync object set nr {}!",
                i);
        }
    }

    VulkanSwapChain::~VulkanSwapChain()
    {
        vkDestroySwapchainKHR(
            _vk_device,
            _vk_swap_chain,
            nullptr);

        for (auto i = 0; i < imageAvailableSemaphore.size(); ++i)
        {
            vkDestroyFence(
                _vk_device,
                inFlightFence[i],
                nullptr);
            vkDestroySemaphore(
                _vk_device,
                renderFinishedSemaphore[i],
                nullptr);
            vkDestroySemaphore(
                _vk_device,
                imageAvailableSemaphore[i],
                nullptr);
        }
    }

    VulkanSwapChain::BeginFrameResult VulkanSwapChain::begin_frame(std::uint32_t frame_index)
    {
        vkWaitForFences(
            _vk_device,
            1,
            &inFlightFence[frame_index],
            VK_TRUE,
            UINT64_MAX);
        vkResetFences(
            _vk_device,
            1,
            &inFlightFence[frame_index]);

        uint32_t imageIndex;
        const auto acquire_img_result = vkAcquireNextImageKHR(
            _vk_device,
            get_native(),
            UINT64_MAX,
            imageAvailableSemaphore[frame_index],
            VK_NULL_HANDLE,
            &imageIndex);
        if (acquire_img_result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            return {acquire_img_result, imageIndex};
        }
        else if (acquire_img_result != VK_SUCCESS && acquire_img_result != VK_SUBOPTIMAL_KHR)
        {
            return {acquire_img_result, imageIndex};
        }

        // Only reset the fence if we are submitting work
        vkResetFences(
            _vk_device,
            1,
            &inFlightFence[frame_index]);

        return {acquire_img_result, imageIndex};
    }

    VulkanSwapChain::EndFrameResult VulkanSwapChain::end_frame(
        const std::uint32_t frame_index,
        const std::uint32_t image_index,
        VkQueue vk_queue,
        VkCommandBuffer vk_command_buffer)
    {
        VkSemaphore waitSemaphores[] = {imageAvailableSemaphore[frame_index]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSemaphore signalSemaphores[] = {renderFinishedSemaphore[frame_index]};

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &vk_command_buffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(
            vk_queue,
            1,
            &submitInfo,
            inFlightFence[frame_index]) != VK_SUCCESS)
        {
            XAR_THROW(error::XarException,
                      "failed to submit draw command buffer!");
        }

        VkSwapchainKHR swapChains[] = {get_native()};
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &image_index;
        presentInfo.pResults = nullptr; // Optional
        const auto present_result = vkQueuePresentKHR(
            vk_queue,
            &presentInfo);

        return {present_result};
    }

    const std::vector<VkImage>& VulkanSwapChain::get_swap_chain_images() const
    {
        return _vk_swap_chain_images;
    }

    VkExtent2D VulkanSwapChain::get_extent() const
    {
        return _vk_extent;
    }

    VkFormat VulkanSwapChain::get_format() const
    {
        return _vk_surface_format_khr.format;
    }

    VkSwapchainKHR VulkanSwapChain::get_native() const
    {
        return _vk_swap_chain;
    }
}