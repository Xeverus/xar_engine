#include <xar_engine/graphics/vulkan/vulkan_instance.hpp>

#include <xar_engine/error/exception_utils.hpp>

namespace xar_engine::graphics::vulkan
{
    namespace
    {
        void initialize_volk_once()
        {
            static auto volk_already_initialized = false;
            if (volk_already_initialized)
            {
                return;
            }

            const auto result = volkInitialize();
            XAR_THROW_IF(
                result != VK_SUCCESS,
                error::XarException,
                "Failed to initialize Volk");

            volk_already_initialized = true;
        }

        VkInstance make_vk_instance()
        {
            VkInstance vk_instance = nullptr;

            VkApplicationInfo vk_application_info = {};
            vk_application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            vk_application_info.pEngineName = "eng-name";
            vk_application_info.engineVersion = VK_MAKE_VERSION(0,
                                                                0,
                                                                0);
            vk_application_info.apiVersion = VK_API_VERSION_1_3;
            vk_application_info.pApplicationName = "app-name";

            VkInstanceCreateInfo vk_instance_create_info = {};
            vk_instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            vk_instance_create_info.pNext = nullptr;
            vk_instance_create_info.pApplicationInfo = &vk_application_info;

            const auto result = vkCreateInstance(
                &vk_instance_create_info,
                nullptr,
                &vk_instance);
            XAR_THROW_IF(
                result != VK_SUCCESS,
                error::XarException,
                "Failed to create Vulkan instance");

            return vk_instance;
        }
    }

    VulkanInstance::VulkanInstance()
        : _vk_instance(nullptr)
    {
        initialize_volk_once();

        _vk_instance = make_vk_instance();
        volkLoadInstance(_vk_instance);
    }

    VulkanInstance::~VulkanInstance()
    {
        if (_vk_instance != nullptr)
        {
            vkDestroyInstance(
                _vk_instance,
                nullptr);
        }
    }
}