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
        Device device;

        VkDescriptorPool vk_descriptor_pool;
    };

    VulkanDescriptorPool::State::State(const Parameters& parameters)
        : device(parameters.device)
        , vk_descriptor_pool(nullptr)
    {
        auto poolSizes = std::vector<VkDescriptorPoolSize>(2);
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(parameters.uniform_buffer_count);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(parameters.combined_image_sampler_count);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<std::int32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(parameters.max_descriptor_set_count);

        const auto create_descriptor_pool_result = vkCreateDescriptorPool(
            device.get_native(),
            &poolInfo,
            nullptr,
            &vk_descriptor_pool);

        XAR_THROW_IF(
            create_descriptor_pool_result != VK_SUCCESS,
            error::XarException,
            "failed to create descriptor pool!");
    }

    VulkanDescriptorPool::State::~State()
    {
        vkDestroyDescriptorPool(
            device.get_native(),
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