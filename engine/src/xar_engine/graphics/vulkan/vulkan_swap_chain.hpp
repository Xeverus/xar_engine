#pragma once

#include <vector>

#include <volk.h>

#include <xar_engine/graphics/vulkan/image_view.hpp>

#include <xar_engine/math/vector.hpp>


namespace xar_engine::graphics::vulkan
{
    class VulkanSwapChain
    {
    public:
        struct Parameters;

        struct BeginFrameResult;
        struct EndFrameResult;

    public:
        explicit VulkanSwapChain(const Parameters& parameters);
        ~VulkanSwapChain();


        BeginFrameResult begin_frame();

        EndFrameResult end_frame(
            VkQueue vk_queue,
            VkCommandBuffer vk_command_buffer);


        [[nodiscard]]
        VkExtent2D get_extent() const;

        [[nodiscard]]
        VkFormat get_format() const;

        [[nodiscard]]
        VkSwapchainKHR get_native() const;

    private:
        Device device;

        VkSwapchainKHR _vk_swap_chain;
        VkSurfaceKHR _vk_surface_khr;
        VkExtent2D _vk_extent;
        VkSurfaceFormatKHR _vk_surface_format_khr;

        std::uint32_t _frame_index;
        std::uint32_t _image_index;

        std::vector<VkImage> _vk_images;
        std::vector<VulkanImageView> _vulkan_image_views;

        std::vector<VkSemaphore> imageAvailableSemaphore;
        std::vector<VkSemaphore> renderFinishedSemaphore;
        std::vector<VkFence> inFlightFence;
    };

    struct VulkanSwapChain::Parameters
    {
        Device device;
        VkSurfaceKHR vk_surface_khr;
        VkSurfaceCapabilitiesKHR vk_surface_capabilities_khr;
        math::Vector2i32 dimension;
        VkPresentModeKHR present_mode_khr;
        VkSurfaceFormatKHR surface_format_khr;
        std::int32_t buffering_level;
    };

    struct VulkanSwapChain::BeginFrameResult
    {
        VkResult vk_result;
        VkImage image;
        VkImageView image_view;
    };

    struct VulkanSwapChain::EndFrameResult
    {
        VkResult vk_result;
    };
}