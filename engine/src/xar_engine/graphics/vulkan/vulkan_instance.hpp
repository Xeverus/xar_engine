#pragma once

#include <volk.h>

namespace xar_engine::graphics::vulkan
{
    class VulkanInstance
    {
    public:
        VulkanInstance();
        ~VulkanInstance();

        [[nodiscard]]
        VkInstance get_native() const;

    private:
        VkInstance _vk_instance;
    };
}