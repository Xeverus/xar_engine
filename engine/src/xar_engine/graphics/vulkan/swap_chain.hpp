#pragma once

#include <memory>
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
        VulkanSwapChain();
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
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct VulkanSwapChain::Parameters
    {
        Device device;
        VkSurfaceKHR vk_surface_khr;
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
        std::uint32_t frame_index;
    };

    struct VulkanSwapChain::EndFrameResult
    {
        VkResult vk_result;
    };
}