#include <xar_engine/graphics/vulkan/vulkan_descriptor_pool.hpp>

#include <vector>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    VulkanDescriptorPool::VulkanDescriptorPool(const Parameters& parameters)
        : _vk_device(parameters.vk_device)
        , _vk_descriptor_pool(nullptr)
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
            _vk_device,
            &poolInfo,
            nullptr,
            &_vk_descriptor_pool);

        XAR_THROW_IF(
            create_descriptor_pool_result != VK_SUCCESS,
            error::XarException,
            "failed to create descriptor pool!");
    }

    VulkanDescriptorPool::~VulkanDescriptorPool()
    {
        vkDestroyDescriptorPool(
            _vk_device,
            _vk_descriptor_pool,
            nullptr);
    }

    VkDescriptorPool VulkanDescriptorPool::get_native() const
    {
        return _vk_descriptor_pool;
    }
}