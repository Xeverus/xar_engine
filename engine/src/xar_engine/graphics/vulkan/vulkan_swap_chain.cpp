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
    }

    VulkanSwapChain::~VulkanSwapChain()
    {
        vkDestroySwapchainKHR(
            _vk_device,
            _vk_swap_chain,
            nullptr);
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