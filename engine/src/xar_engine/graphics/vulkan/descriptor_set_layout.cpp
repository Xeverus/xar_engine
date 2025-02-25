#include <xar_engine/graphics/vulkan/descriptor_set_layout.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    struct VulkanDescriptorSetLayout::State
    {
    public:
        explicit State(const Parameters& parameters);

        ~State();

    public:
        Device device;
        VkDescriptorSetLayout vk_descriptor_set_layout;
    };

    VulkanDescriptorSetLayout::State::State(const VulkanDescriptorSetLayout::Parameters& parameters)
        : device(parameters.device)
        , vk_descriptor_set_layout(nullptr)
    {
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<std::uint32_t>(parameters.vk_bindings.size());
        layoutInfo.pBindings = parameters.vk_bindings.data();

        const auto result = vkCreateDescriptorSetLayout(
            device.get_native(),
            &layoutInfo,
            nullptr,
            &vk_descriptor_set_layout);
        XAR_THROW_IF(
            result != VK_SUCCESS,
            error::XarException,
            "failed to create descriptor set layout!");
    }

    VulkanDescriptorSetLayout::State::~State()
    {
        vkDestroyDescriptorSetLayout(
            device.get_native(),
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