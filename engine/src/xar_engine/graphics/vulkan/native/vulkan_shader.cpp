#include <xar_engine/graphics/vulkan/native/vulkan_shader.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan::native
{
    struct VulkanShader::State
    {
    public:
        explicit State(const Parameters& parameters);

        ~State();

    public:
        VulkanDevice vulkan_device;

        VkShaderModule vk_shader_module;
    };

    VulkanShader::State::State(const Parameters& parameters)
        : vulkan_device(parameters.vulkan_device)
        , vk_shader_module(nullptr)
    {
        auto vk_shader_module_create_info = VkShaderModuleCreateInfo{};
        vk_shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vk_shader_module_create_info.codeSize = parameters.shader_byte_code.size();
        vk_shader_module_create_info.pCode = reinterpret_cast<const std::uint32_t*>(parameters.shader_byte_code.data());

        const auto vk_create_shader_module_result = vkCreateShaderModule(
            parameters.vulkan_device.get_native(),
            &vk_shader_module_create_info,
            nullptr,
            &vk_shader_module);
        XAR_THROW_IF(
            vk_create_shader_module_result != VK_SUCCESS,
            error::XarException,
            "vkCreateShaderModule failed");
    }

    VulkanShader::State::~State()
    {
        vkDestroyShaderModule(
            vulkan_device.get_native(),
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