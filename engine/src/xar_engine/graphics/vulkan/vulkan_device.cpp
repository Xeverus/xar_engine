#include <xar_engine/graphics/vulkan/vulkan_device.hpp>

#include <vector>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    VulkanDevice::VulkanDevice(const VulkanDevice::Parameters& parameters)
        : _vk_device(nullptr)
        , _vk_graphics_queue(nullptr)
        , _graphics_family_index(0)
    {
        const auto graphics_queue_family = 0;
        float queue_priority = 1.0f;

        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = graphics_queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;

        VkPhysicalDeviceFeatures device_features{};
        device_features.samplerAnisotropy = VK_TRUE;
        device_features.sampleRateShading = VK_TRUE;

        std::vector<const char*> physical_device_extension_names = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
        };

        VkDeviceCreateInfo device_create_info{};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pQueueCreateInfos = &queue_create_info;
        device_create_info.queueCreateInfoCount = 1;
        device_create_info.pEnabledFeatures = &device_features;
        device_create_info.enabledExtensionCount = physical_device_extension_names.size();
        device_create_info.ppEnabledExtensionNames = physical_device_extension_names.data();
        VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_feature{};
        dynamic_rendering_feature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
        dynamic_rendering_feature.dynamicRendering = VK_TRUE;

        device_create_info.pNext = &dynamic_rendering_feature;

        const auto device_create_result = vkCreateDevice(
            parameters.vk_physical_device,
            &device_create_info,
            nullptr,
            &_vk_device);
        XAR_THROW_IF(
            device_create_result != VK_SUCCESS,
            error::XarException,
            "Vulkan device creation failed");

        vkGetDeviceQueue(
            _vk_device,
            graphics_queue_family,
            0,
            &_vk_graphics_queue);
    }

    VulkanDevice::~VulkanDevice()
    {
        vkDestroyDevice(
            _vk_device,
            nullptr);
    }

    void VulkanDevice::wait_idle() const
    {
        vkDeviceWaitIdle(_vk_device);
    }

    VkDevice VulkanDevice::get_native() const
    {
        return _vk_device;
    }

    std::uint32_t VulkanDevice::get_graphics_family_index() const
    {
        return _graphics_family_index;
    }

    VkQueue VulkanDevice::get_graphics_queue() const
    {
        return _vk_graphics_queue;
    }
}