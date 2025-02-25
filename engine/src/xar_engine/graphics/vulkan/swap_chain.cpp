#include <xar_engine/graphics/vulkan/swap_chain.hpp>

#include <algorithm>
#include <cmath>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    struct VulkanSwapChain::State
    {
    public:
        explicit State(const Parameters& parameters);

        ~State();

    public:
        Device device;
        VulkanSurface surface;

        VkSwapchainKHR vk_swap_chain;
        VkExtent2D vk_extent;
        VkSurfaceFormatKHR vk_surface_format_khr;

        std::uint32_t buffering_level;
        std::uint32_t frame_index;
        std::uint32_t image_index;

        std::vector<VkImage> vk_images;
        std::vector<VulkanImageView> vulkan_image_views;

        std::vector<VkSemaphore> imageAvailableSemaphore;
        std::vector<VkSemaphore> renderFinishedSemaphore;
        std::vector<VkFence> inFlightFence;
    };

    VulkanSwapChain::State::State(const Parameters& parameters)
        : device{parameters.device}
        , surface{parameters.surface}
        , vk_swap_chain{nullptr}
        , vk_extent{}
        , vk_surface_format_khr{parameters.surface_format_khr}
        , buffering_level{static_cast<std::uint32_t>(parameters.buffering_level)}
        , frame_index{0}
        , image_index{0}
        , vk_images{}
        , vulkan_image_views{}
        , imageAvailableSemaphore{}
        , renderFinishedSemaphore{}
        , inFlightFence{}
    {
        const auto vk_surface_capabilities_khr = parameters.device.get_physical_device().get_surface_capabilities(parameters.surface.get_native());

        vk_extent = {
            static_cast<std::uint32_t>(parameters.dimension.x),
            static_cast<std::uint32_t>(parameters.dimension.y),
        };
        vk_extent.width = std::clamp(
            vk_extent.width,
            vk_surface_capabilities_khr.minImageExtent.width,
            vk_surface_capabilities_khr.maxImageExtent.width);
        vk_extent.height = std::clamp(
            vk_extent.height,
            vk_surface_capabilities_khr.minImageExtent.height,
            vk_surface_capabilities_khr.maxImageExtent.height);
        std::uint32_t image_count = vk_surface_capabilities_khr.minImageCount + 1;
        image_count = vk_surface_capabilities_khr.maxImageCount == 0 ? image_count : std::min(
            image_count,
            vk_surface_capabilities_khr.maxImageCount);

        VkSwapchainCreateInfoKHR swapchain_info{};
        swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_info.surface = surface.get_native();
        swapchain_info.minImageCount = image_count;
        swapchain_info.imageFormat = parameters.surface_format_khr.format;
        swapchain_info.imageColorSpace = parameters.surface_format_khr.colorSpace;
        swapchain_info.imageExtent = vk_extent;
        swapchain_info.imageArrayLayers = 1;
        swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_info.preTransform = vk_surface_capabilities_khr.currentTransform;
        swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchain_info.presentMode = parameters.present_mode_khr;
        swapchain_info.clipped = VK_TRUE;
        swapchain_info.oldSwapchain = VK_NULL_HANDLE;

        const auto swap_chain_result = vkCreateSwapchainKHR(
            device.get_native(),
            &swapchain_info,
            nullptr,
            &vk_swap_chain);
        XAR_THROW_IF(
            swap_chain_result != VK_SUCCESS,
            error::XarException,
            "Vulkan swapchain creation failed");

        std::uint32_t swap_chain_images_count = 0;
        vkGetSwapchainImagesKHR(
            device.get_native(),
            vk_swap_chain,
            &swap_chain_images_count,
            nullptr);

        vk_images.resize(swap_chain_images_count);
        vkGetSwapchainImagesKHR(
            device.get_native(),
            vk_swap_chain,
            &swap_chain_images_count,
            vk_images.data());

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
                device.get_native(),
                &semaphoreInfo,
                nullptr,
                &imageAvailableSemaphore[i]);
            const auto create_finished_result = vkCreateSemaphore(
                device.get_native(),
                &semaphoreInfo,
                nullptr,
                &renderFinishedSemaphore[i]);
            const auto create_flight_result = vkCreateFence(
                device.get_native(),
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

        vulkan_image_views.reserve(vk_images.size());
        for (auto& vk_image: vk_images)
        {
            vulkan_image_views.emplace_back(
                VulkanImageView::Parameters{
                    device,
                    vk_image,
                    parameters.surface_format_khr.format,
                    VK_IMAGE_ASPECT_COLOR_BIT,
                    1,
                });
        }
    }

    VulkanSwapChain::State::~State()
    {
        vkDestroySwapchainKHR(
            device.get_native(),
            vk_swap_chain,
            nullptr);

        for (auto i = 0; i < imageAvailableSemaphore.size(); ++i)
        {
            vkDestroyFence(
                device.get_native(),
                inFlightFence[i],
                nullptr);
            vkDestroySemaphore(
                device.get_native(),
                renderFinishedSemaphore[i],
                nullptr);
            vkDestroySemaphore(
                device.get_native(),
                imageAvailableSemaphore[i],
                nullptr);
        }
    }


    VulkanSwapChain::VulkanSwapChain()
        : _state(nullptr)
    {
    }

    VulkanSwapChain::VulkanSwapChain(const VulkanSwapChain::Parameters& parameters)
        : _state(std::make_shared<State>(parameters))
    {
    }

    VulkanSwapChain::~VulkanSwapChain() = default;

    VulkanSwapChain::BeginFrameResult VulkanSwapChain::begin_frame()
    {
        vkWaitForFences(
            _state->device.get_native(),
            1,
            &_state->inFlightFence[_state->frame_index],
            VK_TRUE,
            UINT64_MAX);
        vkResetFences(
            _state->device.get_native(),
            1,
            &_state->inFlightFence[_state->frame_index]);

        _state->image_index = uint32_t{0};
        const auto acquire_img_result = vkAcquireNextImageKHR(
            _state->device.get_native(),
            get_native(),
            UINT64_MAX,
            _state->imageAvailableSemaphore[_state->frame_index],
            VK_NULL_HANDLE,
            &_state->image_index);

        if (acquire_img_result == VK_SUCCESS)
        {
            vkResetFences(
                _state->device.get_native(),
                1,
                &_state->inFlightFence[_state->frame_index]);
        }

        return {
            acquire_img_result,
            _state->vk_images[_state->image_index],
            _state->vulkan_image_views[_state->image_index].get_native(),
            _state->frame_index,
        };
    }

    VulkanSwapChain::EndFrameResult VulkanSwapChain::end_frame(VkCommandBuffer vk_command_buffer)
    {
        VkSemaphore waitSemaphores[] = {_state->imageAvailableSemaphore[_state->frame_index]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSemaphore signalSemaphores[] = {_state->renderFinishedSemaphore[_state->frame_index]};

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &vk_command_buffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        const auto submit_result = vkQueueSubmit(
            _state->device.get_graphics_queue(),
            1,
            &submitInfo,
            _state->inFlightFence[_state->frame_index]);
        XAR_THROW_IF(
            submit_result != VK_SUCCESS,
            error::XarException,
            "failed to submit draw command buffer!");

        VkSwapchainKHR swapChains[] = {get_native()};
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &_state->image_index;
        presentInfo.pResults = nullptr;
        const auto present_result = vkQueuePresentKHR(
            _state->device.get_graphics_queue(),
            &presentInfo);

        _state->frame_index = (_state->frame_index + 1) % _state->buffering_level;

        return {present_result};
    }

    VkExtent2D VulkanSwapChain::get_extent() const
    {
        return _state->vk_extent;
    }

    VkFormat VulkanSwapChain::get_format() const
    {
        return _state->vk_surface_format_khr.format;
    }

    VkSwapchainKHR VulkanSwapChain::get_native() const
    {
        return _state->vk_swap_chain;
    }
}