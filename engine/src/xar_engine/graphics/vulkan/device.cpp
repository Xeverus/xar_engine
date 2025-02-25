#include <xar_engine/graphics/vulkan/device.hpp>

#include <vector>

#include <xar_engine/error/exception_utils.hpp>

#include <xar_engine/logging/logger.hpp>


namespace xar_engine::graphics::vulkan
{
    namespace
    {
        constexpr auto logging_tag = "vulkan::Device";
    }


    struct Device::State
    {
    public:
        explicit State(const Parameters& parameters);
        ~State();

    public:
        PhysicalDevice physical_device;

        VkDevice vk_device;
        VkQueue vk_graphics_queue;
        std::uint32_t graphics_queue_family_index;
    };

    Device::State::State(const Parameters& parameters)
        : physical_device(parameters.physical_device)
        , vk_device(nullptr)
        , vk_graphics_queue(nullptr)
        , graphics_queue_family_index(0)
    {
        const auto queue_priority = 1.0f;

        auto queue_create_info = VkDeviceQueueCreateInfo{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = graphics_queue_family_index;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;

        auto device_features = VkPhysicalDeviceFeatures{};
        device_features.samplerAnisotropy = VK_TRUE;
        device_features.sampleRateShading = VK_TRUE;

        const auto physical_device_extension_names = std::vector<const char*>{
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
        };

        auto dynamic_rendering_feature = VkPhysicalDeviceDynamicRenderingFeaturesKHR{};
        dynamic_rendering_feature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
        dynamic_rendering_feature.dynamicRendering = VK_TRUE;

        auto device_create_info = VkDeviceCreateInfo{};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pQueueCreateInfos = &queue_create_info;
        device_create_info.queueCreateInfoCount = 1;
        device_create_info.pEnabledFeatures = &device_features;
        device_create_info.enabledExtensionCount = physical_device_extension_names.size();
        device_create_info.ppEnabledExtensionNames = physical_device_extension_names.data();
        device_create_info.pNext = &dynamic_rendering_feature;

        XAR_LOG(
            logging::LogLevel::DEBUG,
            logging_tag,
            "{} calling vkCreateDevice",
            XAR_OBJECT_ID(this));
        const auto vk_create_device_result = vkCreateDevice(
            physical_device.get_native(),
            &device_create_info,
            nullptr,
            &vk_device);
        XAR_THROW_IF(
            vk_create_device_result != VK_SUCCESS,
            error::XarException,
            "Vulkan device creation failed");

        XAR_LOG(
            logging::LogLevel::DEBUG,
            logging_tag,
            "{} calling vkGetDeviceQueue",
            XAR_OBJECT_ID(this));
        vkGetDeviceQueue(
            vk_device,
            graphics_queue_family_index,
            0,
            &vk_graphics_queue);
    }

    Device::State::~State()
    {
        vkDestroyDevice(
            vk_device,
            nullptr);
    }


    Device::Device()
        : _state(nullptr)
    {
    }

    Device::Device(const Device::Parameters& parameters)
        : _state(std::make_shared<State>(parameters))
    {
    }

    Device::~Device() = default;

    void Device::wait_idle() const
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

    VkDevice Device::get_native() const
    {
        return _state->vk_device;
    }

    std::uint32_t Device::get_graphics_family_index() const
    {
        return _state->graphics_queue_family_index;
    }

    VkQueue Device::get_graphics_queue() const
    {
        return _state->vk_graphics_queue;
    }
}