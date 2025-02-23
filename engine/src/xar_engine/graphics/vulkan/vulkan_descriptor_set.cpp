#include <xar_engine/graphics/vulkan/vulkan_descriptor_set.hpp>

#include <vector>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    VulkanDescriptorSet::VulkanDescriptorSet(const Parameters& parameters)
        : _vk_device(parameters.vk_device)
        , _vk_descriptor_sets({})
    {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = parameters.vk_descriptor_pool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(parameters.vk_layouts.size());
        allocInfo.pSetLayouts = parameters.vk_layouts.data();

        _vk_descriptor_sets.resize(parameters.vk_layouts.size());
        const auto result = vkAllocateDescriptorSets(
            _vk_device,
            &allocInfo,
            _vk_descriptor_sets.data());
        XAR_THROW_IF(
            result != VK_SUCCESS,
            error::XarException,
            "failed to allocate descriptor sets!");
    }

    VulkanDescriptorSet::~VulkanDescriptorSet() = default;

    void VulkanDescriptorSet::write(const std::vector<VkWriteDescriptorSet>& data)
    {
        vkUpdateDescriptorSets(
            _vk_device,
            static_cast<std::uint32_t>(data.size()),
            data.data(),
            0,
            nullptr);
    }

    const std::vector<VkDescriptorSet>& VulkanDescriptorSet::get_native() const
    {
        return _vk_descriptor_sets;
    }
}