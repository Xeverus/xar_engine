#pragma once

#include <memory>
#include <vector>

#include <volk.h>

#include <xar_engine/graphics/vulkan/vulkan_window_surface.hpp>

#include <xar_engine/graphics/vulkan/impl/vulkan_image_view.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_queue.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_surface.hpp>

#include <xar_engine/math/vector.hpp>


namespace xar_engine::graphics::vulkan::impl
{
    class VulkanSwapChain
    {
    public:
        struct Parameters;

        struct BeginFrameResult;
        struct EndFrameResult;

    public:
        VulkanSwapChain();
        explicit VulkanSwapChain(const Parameters& parameters);

        ~VulkanSwapChain();


        BeginFrameResult begin_frame();

        EndFrameResult end_frame(VkCommandBuffer vk_command_buffer);


        [[nodiscard]]
        VkExtent2D get_extent() const;

        [[nodiscard]]
        VkFormat get_vk_format() const;

        [[nodiscard]]
        VkSwapchainKHR get_native() const;

        [[nodiscard]]
        VkImage get_vk_image(std::uint32_t index) const;

        [[nodiscard]]
        const VulkanImageView& get_vulkan_image_view(std::uint32_t index) const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct VulkanSwapChain::Parameters
    {
        VulkanDevice vulkan_device;
        VulkanQueue vulkan_queue;
        std::shared_ptr<VulkanWindowSurface> vulkan_window_surface;

        VkPresentModeKHR vk_present_mode_khr;
        VkSurfaceFormatKHR vk_surface_format_khr;
        std::int32_t buffering_level;
    };

    struct VulkanSwapChain::BeginFrameResult
    {
        VkResult vk_result;
        std::uint32_t image_index;
        std::uint32_t frame_buffer_index;
    };

    struct VulkanSwapChain::EndFrameResult
    {
        VkResult vk_result;
    };
}