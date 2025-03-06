#include <xar_engine/graphics/vulkan/native/vulkan_device.hpp>

#include <vector>

#include <xar_engine/error/exception_utils.hpp>

#include <xar_engine/logging/logger.hpp>


namespace xar_engine::graphics::vulkan::native
{
    namespace
    {
        constexpr auto logging_tag = "vulkan::Device";
    }


    struct VulkanDevice::State
    {
    public:
        explicit State(const Parameters& parameters);
        ~State();

    public:
        VulkanPhysicalDevice vulkan_physical_device;

        VkDevice vk_device;
        std::uint32_t graphics_queue_family_index;
    };

    VulkanDevice::State::State(const Parameters& parameters)
        : vulkan_physical_device(parameters.vulkan_physical_device)
        , vk_device(nullptr)
        , graphics_queue_family_index(0)
    {
        const auto queue_priority = 1.0f;

        auto vk_device_queue_create_info = VkDeviceQueueCreateInfo{};
        vk_device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        vk_device_queue_create_info.queueFamilyIndex = graphics_queue_family_index;
        vk_device_queue_create_info.queueCount = 1;
        vk_device_queue_create_info.pQueuePriorities = &queue_priority;

        auto vk_physical_device_features = VkPhysicalDeviceFeatures{};
        vk_physical_device_features.samplerAnisotropy = VK_TRUE;
        vk_physical_device_features.sampleRateShading = VK_TRUE;

        const auto physical_device_extension_names = std::vector<const char*>{
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        };

        auto vk_physical_device_dynamic_rendering_features_khr = VkPhysicalDeviceDynamicRenderingFeaturesKHR{};
        vk_physical_device_dynamic_rendering_features_khr.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
        vk_physical_device_dynamic_rendering_features_khr.dynamicRendering = VK_TRUE;

        auto vk_device_create_info = VkDeviceCreateInfo{};
        vk_device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        vk_device_create_info.pQueueCreateInfos = &vk_device_queue_create_info;
        vk_device_create_info.queueCreateInfoCount = 1;
        vk_device_create_info.pEnabledFeatures = &vk_physical_device_features;
        vk_device_create_info.enabledExtensionCount = physical_device_extension_names.size();
        vk_device_create_info.ppEnabledExtensionNames = physical_device_extension_names.data();
        vk_device_create_info.pNext = &vk_physical_device_dynamic_rendering_features_khr;

        XAR_LOG(
            logging::LogLevel::DEBUG,
            logging_tag,
            "{} calling vkCreateDevice",
            XAR_OBJECT_ID(this));
        const auto vk_create_device_result = vkCreateDevice(
            vulkan_physical_device.get_vk_physical_device(),
            &vk_device_create_info,
            nullptr,
            &vk_device);
        XAR_THROW_IF(
            vk_create_device_result != VK_SUCCESS,
            error::XarException,
            "vkCreateDevice failed");

        XAR_LOG(
            logging::LogLevel::DEBUG,
            logging_tag,
            "{} calling vkGetDeviceQueue",
            XAR_OBJECT_ID(this));
    }

    VulkanDevice::State::~State()
    {
        vkDestroyDevice(
            vk_device,
            nullptr);
    }


    VulkanDevice::VulkanDevice()
        : _state(nullptr)
    {
    }

    VulkanDevice::VulkanDevice(const VulkanDevice::Parameters& parameters)
        : _state(std::make_shared<State>(parameters))
    {
    }

    VulkanDevice::~VulkanDevice() = default;

    const VulkanPhysicalDevice& VulkanDevice::get_physical_device() const
    {
        return _state->vulkan_physical_device;
    }

    void VulkanDevice::wait_idle() const
    {
        XAR_LOG(
            logging::LogLevel::DEBUG,
            logging_tag,
            "{} calling vkDeviceWaitIdle...",
            XAR_OBJECT_ID(this));
        vkDeviceWaitIdle(_state->vk_device);
        XAR_LOG(
            logging::LogLevel::DEBUG,
            logging_tag,
            "{} calling vkDeviceWaitIdle finished",
            XAR_OBJECT_ID(this));
    }

    VkDevice VulkanDevice::get_native() const
    {
        return _state->vk_device;
    }

    std::uint32_t VulkanDevice::get_graphics_family_index() const
    {
        return _state->graphics_queue_family_index;
    }
}