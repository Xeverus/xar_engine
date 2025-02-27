#include <xar_engine/graphics/vulkan/impl/vulkan_descriptor_set.hpp>

#include <vector>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan::impl
{
    struct VulkanDescriptorSet::State
    {
    public:
        explicit State(const Parameters& parameters);

        ~State();

    public:
        VulkanDevice vulkan_device;

        std::vector<VkDescriptorSet> vk_descriptor_set_list;
    };

    VulkanDescriptorSet::State::State(const VulkanDescriptorSet::Parameters& parameters)
        : vulkan_device{parameters.vulkan_device}
        , vk_descriptor_set_list{}
    {
        auto vk_descriptor_set_allocate_info = VkDescriptorSetAllocateInfo{};
        vk_descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        vk_descriptor_set_allocate_info.descriptorPool = parameters.vulkan_descriptor_pool.get_native();
        vk_descriptor_set_allocate_info.descriptorSetCount = static_cast<uint32_t>(parameters.vk_descriptor_set_layout_list.size());
        vk_descriptor_set_allocate_info.pSetLayouts = parameters.vk_descriptor_set_layout_list.data();

        vk_descriptor_set_list.resize(parameters.vk_descriptor_set_layout_list.size());
        const auto vk_allocate_descritptor_sets_result = vkAllocateDescriptorSets(
            vulkan_device.get_native(),
            &vk_descriptor_set_allocate_info,
            vk_descriptor_set_list.data());
        XAR_THROW_IF(
            vk_allocate_descritptor_sets_result != VK_SUCCESS,
            error::XarException,
            "Allocate descriptor sets failed");
    }

    VulkanDescriptorSet::State::~State() = default;


    VulkanDescriptorSet::VulkanDescriptorSet()
        : _state(nullptr)
    {
    }

    VulkanDescriptorSet::VulkanDescriptorSet(const Parameters& parameters)
        : _state(std::make_shared<State>(parameters))
    {
    }

    VulkanDescriptorSet::~VulkanDescriptorSet() = default;

    void VulkanDescriptorSet::write(const std::vector<VkWriteDescriptorSet>& vk_write_descriptor_set_list)
    {
        vkUpdateDescriptorSets(
            _state->vulkan_device.get_native(),
            static_cast<std::uint32_t>(vk_write_descriptor_set_list.size()),
            vk_write_descriptor_set_list.data(),
            0,
            nullptr);
    }

    const std::vector<VkDescriptorSet>& VulkanDescriptorSet::get_native() const
    {
        return _state->vk_descriptor_set_list;
    }
}