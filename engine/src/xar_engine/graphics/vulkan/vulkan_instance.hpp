#pragma once

#include <memory>

#include <volk.h>


namespace xar_engine::graphics::vulkan
{
    class VulkanInstance
    {
    public:
        VulkanInstance();
        VulkanInstance(VulkanInstance&& other) = delete;
        VulkanInstance(const VulkanInstance& other) = delete;
        VulkanInstance operator=(VulkanInstance&& other) = delete;
        VulkanInstance operator=(const VulkanInstance& other) = delete;

        ~VulkanInstance();

        [[nodiscard]]
        VkInstance get_native() const;

    private:
        VkInstance _vk_instance;
    };
}