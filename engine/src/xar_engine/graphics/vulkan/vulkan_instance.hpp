#pragma once

#include <volk.h>

namespace xar_engine::graphics::vulkan
{
    class VulkanInstance
    {
    public:
        VulkanInstance();
        ~VulkanInstance();

    private:
        VkInstance _vk_instance;
    };
}