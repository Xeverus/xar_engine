#pragma once

#include <memory>
#include <vector>

#include <volk.h>

#include <xar_engine/graphics/vulkan/image_view.hpp>
#include <xar_engine/graphics/vulkan/surface.hpp>

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
        VulkanSwapChain();
        explicit VulkanSwapChain(const Parameters& parameters);

        ~VulkanSwapChain();


        BeginFrameResult begin_frame();

        EndFrameResult end_frame(VkCommandBuffer vk_command_buffer);


        [[nodiscard]]
        VkExtent2D get_extent() const;

        [[nodiscard]]
        VkFormat get_format() const;

        [[nodiscard]]
        VkSwapchainKHR get_native() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct VulkanSwapChain::Parameters
    {
        VulkanDevice vulkan_device;
        VulkanSurface vulkan_surface;

        math::Vector2i32 dimension;
        VkPresentModeKHR vk_present_mode_khr;
        VkSurfaceFormatKHR vk_surface_format_khr;
        std::int32_t buffering_level;
    };

    struct VulkanSwapChain::BeginFrameResult
    {
        VkResult vk_result;
        VkImage vk_image;
        VkImageView vk_image_view;
        std::uint32_t frame_index;
    };

    struct VulkanSwapChain::EndFrameResult
    {
        VkResult vk_result;
    };
}