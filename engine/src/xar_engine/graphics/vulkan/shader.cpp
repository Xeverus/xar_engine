#include <xar_engine/graphics/vulkan/shader.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    struct VulkanShader::State
    {
    public:
        explicit State(const Parameters& parameters);

        ~State();

    public:
        Device device;

        VkShaderModule vk_shader_module;
    };

    VulkanShader::State::State(const Parameters& parameters)
        : device(parameters.device)
        , vk_shader_module(nullptr)
    {
        auto create_info = VkShaderModuleCreateInfo{};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = parameters.shader_bytes.size();
        create_info.pCode = reinterpret_cast<const std::uint32_t*>(parameters.shader_bytes.data());

        const auto result = vkCreateShaderModule(
            parameters.device.get_native(),
            &create_info,
            nullptr,
            &vk_shader_module);
        XAR_THROW_IF(
            result != VK_SUCCESS,
            error::XarException,
            "Failed to create shader");
    }

    VulkanShader::State::~State()
    {
        vkDestroyShaderModule(
            device.get_native(),
            vk_shader_module,
            nullptr);
    }


    VulkanShader::VulkanShader()
        : _state(nullptr)
    {
    }

    VulkanShader::VulkanShader(const VulkanShader::Parameters& parameters)
        : _state(std::make_shared<State>(parameters))
    {
    }

    VulkanShader::~VulkanShader() = default;

    VkShaderModule VulkanShader::get_native() const
    {
        return _state->vk_shader_module;
    }
}