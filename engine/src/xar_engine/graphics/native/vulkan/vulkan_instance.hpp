#pragma once

#include <memory>

#include <volk.h>

#include <xar_engine/graphics/native/vulkan/vulkan_physical_device.hpp>


namespace xar_engine::graphics::native::vulkan
{
    class VulkanInstance
    {
    public:
        VulkanInstance();

        ~VulkanInstance();


        [[nodiscard]]
        VkInstance get_native() const;

        [[nodiscard]]
        std::vector<VulkanPhysicalDevice> get_physical_device_list() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };
}
