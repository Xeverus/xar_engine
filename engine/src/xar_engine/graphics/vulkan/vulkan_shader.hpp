#pragma once

#include <cstdint>
#include <vector>

#include <volk.h>


namespace xar_engine::graphics::vulkan
{
    class VulkanShader
    {
    public:
        struct Parameters;

    public:
        explicit VulkanShader(const Parameters& parameters);
        ~VulkanShader();

        [[nodiscard]]
        VkShaderModule get_native() const;

    private:
        VkShaderModule _vk_shader_module;
        VkDevice _vk_device;
    };

    struct VulkanShader::Parameters
    {
        VkDevice vk_device;
        std::vector<char> shader_bytes;
    };
}