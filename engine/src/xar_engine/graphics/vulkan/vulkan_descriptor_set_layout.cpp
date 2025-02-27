#include <xar_engine/graphics/vulkan/vulkan_descriptor_set_layout.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    struct VulkanDescriptorSetLayout::State
    {
    public:
        explicit State(const Parameters& parameters);

        ~State();

    public:
        VulkanDevice vulkan_device;
        VkDescriptorSetLayout vk_descriptor_set_layout;
    };

    VulkanDescriptorSetLayout::State::State(const VulkanDescriptorSetLayout::Parameters& parameters)
        : vulkan_device(parameters.vulkan_device)
        , vk_descriptor_set_layout(nullptr)
    {
        auto vk_descriptor_set_layout_create_info = VkDescriptorSetLayoutCreateInfo{};
        vk_descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        vk_descriptor_set_layout_create_info.bindingCount = static_cast<std::uint32_t>(parameters.vk_descriptor_set_layout_binding_list.size());
        vk_descriptor_set_layout_create_info.pBindings = parameters.vk_descriptor_set_layout_binding_list.data();

        const auto vk_create_descriptor_set_layout_result = vkCreateDescriptorSetLayout(
            vulkan_device.get_native(),
            &vk_descriptor_set_layout_create_info,
            nullptr,
            &vk_descriptor_set_layout);
        XAR_THROW_IF(
            vk_create_descriptor_set_layout_result != VK_SUCCESS,
            error::XarException,
            "Create descriptor set layout failed");
    }

    VulkanDescriptorSetLayout::State::~State()
    {
        vkDestroyDescriptorSetLayout(
            vulkan_device.get_native(),
            vk_descriptor_set_layout,
            nullptr);
    }


    VulkanDescriptorSetLayout::VulkanDescriptorSetLayout()
        : _state(nullptr)
    {
    }

    VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const Parameters& parameters)
        : _state(std::make_shared<State>(parameters))
    {
    }

    VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() = default;

    VkDescriptorSetLayout VulkanDescriptorSetLayout::get_native() const
    {
        return _state->vk_descriptor_set_layout;
    }
}