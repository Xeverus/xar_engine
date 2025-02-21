#pragma once

#include <vector>

#include <volk.h>

#include <xar_engine/math/vector.hpp>


namespace xar_engine::graphics::vulkan
{
    class VulkanSwapChain
    {
    public:
        struct Parameters;

    public:
        explicit VulkanSwapChain(const Parameters& parameters);
        ~VulkanSwapChain();

        [[nodiscard]]
        const std::vector<VkImage>& get_swap_chain_images() const;

        [[nodiscard]]
        VkExtent2D get_extent() const;

        [[nodiscard]]
        VkFormat get_format() const;

        [[nodiscard]]
        VkSwapchainKHR get_native() const;

    private:
        VkDevice _vk_device;
        VkSwapchainKHR _vk_swap_chain;
        VkSurfaceKHR _vk_surface_khr;
        VkExtent2D _vk_extent;
        VkSurfaceFormatKHR _vk_surface_format_khr;

        std::vector<VkImage> _vk_swap_chain_images;
    };

    struct VulkanSwapChain::Parameters
    {
        VkDevice vk_device;
        VkSurfaceKHR vk_surface_khr;
        VkSurfaceCapabilitiesKHR vk_surface_capabilities_khr;
        math::Vector2i32 dimension;
        VkPresentModeKHR present_mode_khr;
        VkSurfaceFormatKHR surface_format_khr;
    };
}