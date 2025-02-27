#pragma once

#include <memory>

#include <volk.h>

#include <xar_engine/graphics/vulkan/vulkan_physical_device.hpp>


namespace xar_engine::graphics::vulkan
{
    class VulkanInstance
    {
    public:
        VulkanInstance();

        ~VulkanInstance();

        [[nodiscard]]
        std::vector<VulkanPhysicalDevice> get_physical_device_list() const;

        [[nodiscard]]
        VkInstance get_native() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };
}
