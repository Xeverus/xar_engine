#include <xar_engine/graphics/vulkan/impl/vulkan_swap_chain.hpp>

#include <algorithm>
#include <cmath>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan::impl
{
    struct VulkanSwapChain::State
    {
    public:
        explicit State(const Parameters& parameters);

        ~State();

    public:
        VulkanDevice vulkan_device;
        VulkanSurface vulkan_surface;

        VkSwapchainKHR vk_swap_chain;
        VkExtent2D vk_extent_2d;
        VkSurfaceFormatKHR vk_surface_format_khr;

        std::uint32_t buffering_level;
        std::uint32_t frame_buffer_index;
        std::uint32_t image_index;

        std::vector<VkImage> vk_images;
        std::vector<VulkanImageView> vulkan_image_view_list;

        std::vector<VkSemaphore> image_available_vk_semaphore;
        std::vector<VkSemaphore> render_finished_vk_semaphore;
        std::vector<VkFence> in_flight_vk_fence;
    };

    VulkanSwapChain::State::State(const Parameters& parameters)
        : vulkan_device{parameters.vulkan_device}
        , vulkan_surface{parameters.vulkan_window_surface->get_vulkan_surface()}
        , vk_swap_chain{nullptr}
        , vk_extent_2d{}
        , vk_surface_format_khr{parameters.vk_surface_format_khr}
        , buffering_level{static_cast<std::uint32_t>(parameters.buffering_level)}
        , frame_buffer_index{0}
        , image_index{0}
        , vk_images{}
        , vulkan_image_view_list{}
        , image_available_vk_semaphore{}
        , render_finished_vk_semaphore{}
        , in_flight_vk_fence{}
    {
        const auto vk_surface_capabilities_khr = parameters.vulkan_device.get_physical_device().get_vk_surface_capabilities_khr(parameters.vulkan_window_surface->get_vulkan_surface().get_native());

        vk_extent_2d = {
            static_cast<std::uint32_t>(parameters.vulkan_window_surface->get_pixel_size().x),
            static_cast<std::uint32_t>(parameters.vulkan_window_surface->get_pixel_size().y),
        };
        vk_extent_2d.width = std::clamp(
            vk_extent_2d.width,
            vk_surface_capabilities_khr.minImageExtent.width,
            vk_surface_capabilities_khr.maxImageExtent.width);
        vk_extent_2d.height = std::clamp(
            vk_extent_2d.height,
            vk_surface_capabilities_khr.minImageExtent.height,
            vk_surface_capabilities_khr.maxImageExtent.height);
        auto image_count = std::uint32_t{vk_surface_capabilities_khr.minImageCount + 1};
        image_count = vk_surface_capabilities_khr.maxImageCount == 0 ? image_count : std::min(
            image_count,
            vk_surface_capabilities_khr.maxImageCount);

        auto vk_swap_chain_create_info_khr = VkSwapchainCreateInfoKHR{};
        vk_swap_chain_create_info_khr.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        vk_swap_chain_create_info_khr.surface = vulkan_surface.get_native();
        vk_swap_chain_create_info_khr.minImageCount = image_count;
        vk_swap_chain_create_info_khr.imageFormat = parameters.vk_surface_format_khr.format;
        vk_swap_chain_create_info_khr.imageColorSpace = parameters.vk_surface_format_khr.colorSpace;
        vk_swap_chain_create_info_khr.imageExtent = vk_extent_2d;
        vk_swap_chain_create_info_khr.imageArrayLayers = 1;
        vk_swap_chain_create_info_khr.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        vk_swap_chain_create_info_khr.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        vk_swap_chain_create_info_khr.preTransform = vk_surface_capabilities_khr.currentTransform;
        vk_swap_chain_create_info_khr.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        vk_swap_chain_create_info_khr.presentMode = parameters.vk_present_mode_khr;
        vk_swap_chain_create_info_khr.clipped = VK_TRUE;
        vk_swap_chain_create_info_khr.oldSwapchain = VK_NULL_HANDLE;

        const auto create_swap_chain_khr_result = vkCreateSwapchainKHR(
            vulkan_device.get_native(),
            &vk_swap_chain_create_info_khr,
            nullptr,
            &vk_swap_chain);
        XAR_THROW_IF(
            create_swap_chain_khr_result != VK_SUCCESS,
            error::XarException,
            "vkCreateSwapchainKHR failed");

        auto swap_chain_images_count = std::uint32_t{0};
        vkGetSwapchainImagesKHR(
            vulkan_device.get_native(),
            vk_swap_chain,
            &swap_chain_images_count,
            nullptr);

        vk_images.resize(swap_chain_images_count);
        vkGetSwapchainImagesKHR(
            vulkan_device.get_native(),
            vk_swap_chain,
            &swap_chain_images_count,
            vk_images.data());

        auto vk_semaphore_create_info = VkSemaphoreCreateInfo{};
        vk_semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        auto vk_fence_create_info = VkFenceCreateInfo{};
        vk_fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        vk_fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        image_available_vk_semaphore.resize(parameters.buffering_level);
        render_finished_vk_semaphore.resize(parameters.buffering_level);
        in_flight_vk_fence.resize(parameters.buffering_level);

        for (auto i = 0; i < parameters.buffering_level; ++i)
        {
            const auto image_available_vk_create_semaphore_result = vkCreateSemaphore(
                vulkan_device.get_native(),
                &vk_semaphore_create_info,
                nullptr,
                &image_available_vk_semaphore[i]);
            const auto render_finished_vk_create_semaphore_result = vkCreateSemaphore(
                vulkan_device.get_native(),
                &vk_semaphore_create_info,
                nullptr,
                &render_finished_vk_semaphore[i]);
            const auto in_flight_vk_create_fence_result = vkCreateFence(
                vulkan_device.get_native(),
                &vk_fence_create_info,
                nullptr,
                &in_flight_vk_fence[i]);

            XAR_THROW_IF(
                image_available_vk_create_semaphore_result != VK_SUCCESS ||
                render_finished_vk_create_semaphore_result != VK_SUCCESS ||
                in_flight_vk_create_fence_result != VK_SUCCESS,
                error::XarException,
                "vkCreateSemaphore or vkCreateFence for synchronization object set nr {} failed",
                i);
        }

        vulkan_image_view_list.reserve(vk_images.size());
        for (auto& vk_image: vk_images)
        {
            vulkan_image_view_list.emplace_back(
                VulkanImageView::Parameters{
                    vulkan_device,
                    vk_image,
                    parameters.vk_surface_format_khr.format,
                    VK_IMAGE_ASPECT_COLOR_BIT,
                    1,
                });
        }
    }

    VulkanSwapChain::State::~State()
    {
        vkDestroySwapchainKHR(
            vulkan_device.get_native(),
            vk_swap_chain,
            nullptr);

        for (auto i = 0; i < image_available_vk_semaphore.size(); ++i)
        {
            vkDestroyFence(
                vulkan_device.get_native(),
                in_flight_vk_fence[i],
                nullptr);
            vkDestroySemaphore(
                vulkan_device.get_native(),
                render_finished_vk_semaphore[i],
                nullptr);
            vkDestroySemaphore(
                vulkan_device.get_native(),
                image_available_vk_semaphore[i],
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
            _state->vulkan_device.get_native(),
            1,
            &_state->in_flight_vk_fence[_state->frame_buffer_index],
            VK_TRUE,
            UINT64_MAX);
        vkResetFences(
            _state->vulkan_device.get_native(),
            1,
            &_state->in_flight_vk_fence[_state->frame_buffer_index]);

        _state->image_index = uint32_t{0};
        const auto acquire_img_result = vkAcquireNextImageKHR(
            _state->vulkan_device.get_native(),
            get_native(),
            UINT64_MAX,
            _state->image_available_vk_semaphore[_state->frame_buffer_index],
            VK_NULL_HANDLE,
            &_state->image_index);

        if (acquire_img_result == VK_SUCCESS)
        {
            vkResetFences(
                _state->vulkan_device.get_native(),
                1,
                &_state->in_flight_vk_fence[_state->frame_buffer_index]);
        }

        return {
            acquire_img_result,
            _state->image_index,
            _state->frame_buffer_index,
        };
    }

    VulkanSwapChain::EndFrameResult VulkanSwapChain::end_frame(VkCommandBuffer vk_command_buffer)
    {
        VkSemaphore wait_vk_semaphore_list[] = {_state->image_available_vk_semaphore[_state->frame_buffer_index]};
        VkPipelineStageFlags wait_vk_pipeline_stage_flags_list[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSemaphore signal_vk_semaphore_list[] = {_state->render_finished_vk_semaphore[_state->frame_buffer_index]};

        auto vk_submit_info = VkSubmitInfo{};
        vk_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        vk_submit_info.waitSemaphoreCount = 1;
        vk_submit_info.pWaitSemaphores = wait_vk_semaphore_list;
        vk_submit_info.pWaitDstStageMask = wait_vk_pipeline_stage_flags_list;
        vk_submit_info.commandBufferCount = 1;
        vk_submit_info.pCommandBuffers = &vk_command_buffer;
        vk_submit_info.signalSemaphoreCount = 1;
        vk_submit_info.pSignalSemaphores = signal_vk_semaphore_list;

        const auto vk_queue_submit_result = vkQueueSubmit(
            _state->vulkan_device.get_graphics_queue(),
            1,
            &vk_submit_info,
            _state->in_flight_vk_fence[_state->frame_buffer_index]);
        XAR_THROW_IF(
            vk_queue_submit_result != VK_SUCCESS,
            error::XarException,
            "vkQueueSubmit failed");

        VkSwapchainKHR vk_swap_chain_khr_list[] = {get_native()};

        auto vk_present_info_khr = VkPresentInfoKHR{};
        vk_present_info_khr.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        vk_present_info_khr.waitSemaphoreCount = 1;
        vk_present_info_khr.pWaitSemaphores = signal_vk_semaphore_list;
        vk_present_info_khr.swapchainCount = 1;
        vk_present_info_khr.pSwapchains = vk_swap_chain_khr_list;
        vk_present_info_khr.pImageIndices = &_state->image_index;
        vk_present_info_khr.pResults = nullptr;
        const auto vk_queue_present_khr_result = vkQueuePresentKHR(
            _state->vulkan_device.get_graphics_queue(),
            &vk_present_info_khr);

        _state->frame_buffer_index = (_state->frame_buffer_index + 1) % _state->buffering_level;

        return {vk_queue_present_khr_result};
    }

    VkExtent2D VulkanSwapChain::get_extent() const
    {
        return _state->vk_extent_2d;
    }

    VkFormat VulkanSwapChain::get_vk_format() const
    {
        return _state->vk_surface_format_khr.format;
    }

    VkSwapchainKHR VulkanSwapChain::get_native() const
    {
        return _state->vk_swap_chain;
    }

    VkImage VulkanSwapChain::get_vk_image(std::uint32_t index) const
    {
        return _state->vk_images[index];
    }

    const VulkanImageView& VulkanSwapChain::get_vulkan_image_view(std::uint32_t index) const
    {
        return _state->vulkan_image_view_list[index];
    }
}