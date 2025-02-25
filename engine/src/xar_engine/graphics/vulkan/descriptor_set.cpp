#include <xar_engine/graphics/vulkan/descriptor_set.hpp>

#include <vector>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    struct VulkanDescriptorSet::State
    {
    public:
        explicit State(const Parameters& parameters);

        ~State();

    public:
        Device device;

        std::vector<VkDescriptorSet> vk_descriptor_sets;
    };

    VulkanDescriptorSet::State::State(const VulkanDescriptorSet::Parameters& parameters)
        : device{parameters.device}
        , vk_descriptor_sets{}
    {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = parameters.descriptor_pool.get_native();
        allocInfo.descriptorSetCount = static_cast<uint32_t>(parameters.vk_layouts.size());
        allocInfo.pSetLayouts = parameters.vk_layouts.data();

        vk_descriptor_sets.resize(parameters.vk_layouts.size());
        const auto result = vkAllocateDescriptorSets(
            device.get_native(),
            &allocInfo,
            vk_descriptor_sets.data());
        XAR_THROW_IF(
            result != VK_SUCCESS,
            error::XarException,
            "failed to allocate descriptor sets!");
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

    void VulkanDescriptorSet::write(const std::vector<VkWriteDescriptorSet>& data)
    {
        vkUpdateDescriptorSets(
            _state->device.get_native(),
            static_cast<std::uint32_t>(data.size()),
            data.data(),
            0,
            nullptr);
    }

    const std::vector<VkDescriptorSet>& VulkanDescriptorSet::get_native() const
    {
        return _state->vk_descriptor_sets;
    }
}