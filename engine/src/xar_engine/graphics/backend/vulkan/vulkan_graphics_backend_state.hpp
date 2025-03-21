#pragma once

#include <xar_engine/graphics/backend/vulkan/vulkan_resource_storage.hpp>

#include <xar_engine/meta/shared_state.hpp>


namespace xar_engine::graphics::backend::vulkan
{
    struct VulkanGraphicsBackendState
    {
        std::shared_ptr<native::vulkan::VulkanInstance> vulkan_instance;
        std::vector<native::vulkan::VulkanPhysicalDevice> vulkan_physical_device_list;
        native::vulkan::VulkanDevice vulkan_device;
        native::vulkan::VulkanQueue vulkan_graphics_queue;
        native::vulkan::VulkanCommandBufferPool vulkan_command_buffer_pool;
        VulkanResourceStorage vulkan_resource_storage;
    };

    class SharedVulkanGraphicsBackendState
        : public meta::TSharedState<VulkanGraphicsBackendState>
    {
    public:
        using meta::TSharedState<VulkanGraphicsBackendState>::TSharedState;
    };
}