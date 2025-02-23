#include <xar_engine/graphics/vulkan/vulkan_descriptor_set_layout.hpp>

#include <vector>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const Parameters& parameters)
        : _vk_device(parameters.vk_device)
        , _vk_descriptor_set_layout(nullptr)
    {
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<std::uint32_t>(parameters.vk_bindings.size());
        layoutInfo.pBindings = parameters.vk_bindings.data();

        const auto result = vkCreateDescriptorSetLayout(
            _vk_device,
            &layoutInfo,
            nullptr,
            &_vk_descriptor_set_layout);
        XAR_THROW_IF(
            result != VK_SUCCESS,
            error::XarException,
            "failed to create descriptor set layout!");
    }

    VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
    {
        vkDestroyDescriptorSetLayout(
            _vk_device,
            _vk_descriptor_set_layout,
            nullptr);
    }

    VkDescriptorSetLayout VulkanDescriptorSetLayout::get_native() const
    {
        return _vk_descriptor_set_layout;
    }
}