#include <xar_engine/graphics/vulkan/vulkan_shader.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    VulkanShader::VulkanShader(const VulkanShader::Parameters& parameters)
        : _vk_shader_module(nullptr)
        , _vk_device(parameters.vk_device)
    {
        auto create_info = VkShaderModuleCreateInfo{};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = parameters.shader_bytes.size();
        create_info.pCode = reinterpret_cast<const std::uint32_t*>(parameters.shader_bytes.data());

        const auto result = vkCreateShaderModule(
            parameters.vk_device,
            &create_info,
            nullptr,
            &_vk_shader_module);
        XAR_THROW_IF(
            result != VK_SUCCESS,
            error::XarException,
            "Failed to create shader");
    }

    VulkanShader::~VulkanShader()
    {
        if (_vk_shader_module != nullptr)
        {
            vkDestroyShaderModule(
                _vk_device,
                _vk_shader_module,
                nullptr);
        }
    }

    VkShaderModule VulkanShader::get_native() const
    {
        return _vk_shader_module;
    }
}