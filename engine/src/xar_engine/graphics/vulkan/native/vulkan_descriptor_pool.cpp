#include <xar_engine/graphics/vulkan/native/vulkan_descriptor_pool.hpp>

#include <vector>

#include <xar_engine/error/exception_utils.hpp>

#include <xar_engine/graphics/vulkan/native/vulkan_descriptor_set.hpp>


namespace xar_engine::graphics::vulkan::native
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
        auto vk_descriptor_pool_size_list = std::vector<VkDescriptorPoolSize>{};
        if (parameters.uniform_buffer_count > 0)
        {
            vk_descriptor_pool_size_list.emplace_back(
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                parameters.uniform_buffer_count);
        }
        if (parameters.combined_image_sampler_count > 0)
        {
            vk_descriptor_pool_size_list.emplace_back(
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                parameters.combined_image_sampler_count);
        }

        auto vk_descriptor_pool_create_info = VkDescriptorPoolCreateInfo{};
        vk_descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        vk_descriptor_pool_create_info.poolSizeCount = static_cast<std::int32_t>(vk_descriptor_pool_size_list.size());
        vk_descriptor_pool_create_info.pPoolSizes = vk_descriptor_pool_size_list.data();
        vk_descriptor_pool_create_info.maxSets = static_cast<uint32_t>(parameters.max_descriptor_set_count);
        vk_descriptor_pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;

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

    std::vector<VulkanDescriptorSet> VulkanDescriptorPool::make_descriptor_set_list(
        const std::vector<VkDescriptorSetLayout>& vk_descriptor_set_layout_list)
    {
        auto vk_descriptor_set_allocate_info = VkDescriptorSetAllocateInfo{};
        vk_descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        vk_descriptor_set_allocate_info.descriptorPool = _state->vk_descriptor_pool;
        vk_descriptor_set_allocate_info.descriptorSetCount = static_cast<uint32_t>(vk_descriptor_set_layout_list.size());
        vk_descriptor_set_allocate_info.pSetLayouts = vk_descriptor_set_layout_list.data();

        auto vk_descriptor_set_list = std::vector<VkDescriptorSet>(vk_descriptor_set_layout_list.size());
        const auto vk_allocate_descriptor_sets_result = vkAllocateDescriptorSets(
            _state->vulkan_device.get_native(),
            &vk_descriptor_set_allocate_info,
            vk_descriptor_set_list.data());
        XAR_THROW_IF(
            vk_allocate_descriptor_sets_result != VK_SUCCESS,
            error::XarException,
            "Allocate descriptor sets failed");

        auto vulkan_descriptor_set = std::vector<VulkanDescriptorSet>{};
        vulkan_descriptor_set.reserve(vk_descriptor_set_layout_list.size());
        for (auto vk_descriptor_set: vk_descriptor_set_list)
        {
            vulkan_descriptor_set.emplace_back(
                VulkanDescriptorSet::Parameters{
                    *this,
                    vk_descriptor_set,
                });
        }

        return vulkan_descriptor_set;
    }

    const VulkanDevice& VulkanDescriptorPool::get_device() const
    {
        return _state->vulkan_device;
    }

    VulkanDevice& VulkanDescriptorPool::get_device()
    {
        return _state->vulkan_device;
    }

    VkDescriptorPool VulkanDescriptorPool::get_native() const
    {
        return _state->vk_descriptor_pool;
    }
}