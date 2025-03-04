#pragma once

#include <memory>

#include <volk.h>

#include <xar_engine/graphics/vulkan/native/vulkan_instance.hpp>


namespace xar_engine::graphics::vulkan::native
{
    class VulkanSurface
    {
    public:
        struct Parameters;

    public:
        VulkanSurface();
        explicit VulkanSurface(const Parameters& parameters);

        ~VulkanSurface();

        [[nodiscard]]
        VkSurfaceKHR get_native() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct VulkanSurface::Parameters
    {
        std::shared_ptr<VulkanInstance> vulkan_instance;
        VkSurfaceKHR vk_surface_khr;
    };
}