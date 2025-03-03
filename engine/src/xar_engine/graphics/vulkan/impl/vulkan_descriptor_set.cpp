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
        VkDescriptorSet vk_descriptor_set;
    };

    VulkanDescriptorSet::State::State(const VulkanDescriptorSet::Parameters& parameters)
        : vulkan_device{parameters.vulkan_device}
        , vk_descriptor_set{parameters.vk_descriptor_set}
    {
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

    VkDescriptorSet VulkanDescriptorSet::get_native() const
    {
        return _state->vk_descriptor_set;
    }
}