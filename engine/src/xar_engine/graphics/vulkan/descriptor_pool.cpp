#include <xar_engine/graphics/vulkan/descriptor_pool.hpp>

#include <vector>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    struct VulkanDescriptorPool::State
    {
    public:
        explicit State(const Parameters& parameters);

        ~State();

    public:
        VulkanDevice vulkan_device;

        VkDescriptorPool vk_descriptor_pool;
    };

    VulkanDescriptorPool::State::State(const Parameters& parameters)
        : vulkan_device(parameters.vulkan_device)
        , vk_descriptor_pool(nullptr)
    {
        auto vk_descriptor_pool_size_list = std::vector<VkDescriptorPoolSize>(2);
        vk_descriptor_pool_size_list[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        vk_descriptor_pool_size_list[0].descriptorCount = static_cast<uint32_t>(parameters.uniform_buffer_count);
        vk_descriptor_pool_size_list[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        vk_descriptor_pool_size_list[1].descriptorCount = static_cast<uint32_t>(parameters.combined_image_sampler_count);

        auto vk_descriptor_pool_create_info = VkDescriptorPoolCreateInfo{};
        vk_descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        vk_descriptor_pool_create_info.poolSizeCount = static_cast<std::int32_t>(vk_descriptor_pool_size_list.size());
        vk_descriptor_pool_create_info.pPoolSizes = vk_descriptor_pool_size_list.data();
        vk_descriptor_pool_create_info.maxSets = static_cast<uint32_t>(parameters.max_descriptor_set_count);

        const auto vk_create_descriptor_pool_result = vkCreateDescriptorPool(
            vulkan_device.get_native(),
            &vk_descriptor_pool_create_info,
            nullptr,
            &vk_descriptor_pool);

        XAR_THROW_IF(
            vk_create_descriptor_pool_result != VK_SUCCESS,
            error::XarException,
            "Create descriptor pool failed");
    }

    VulkanDescriptorPool::State::~State()
    {
        vkDestroyDescriptorPool(
            vulkan_device.get_native(),
            vk_descriptor_pool,
            nullptr);
    }


    VulkanDescriptorPool::VulkanDescriptorPool()
        : _state(nullptr)
    {
    }

    VulkanDescriptorPool::VulkanDescriptorPool(const Parameters& parameters)
        : _state(std::make_shared<State>(parameters))
    {
    }

    VulkanDescriptorPool::~VulkanDescriptorPool() = default;

    VkDescriptorPool VulkanDescriptorPool::get_native() const
    {
        return _state->vk_descriptor_pool;
    }
}