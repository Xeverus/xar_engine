#pragma once

#include <memory>
#include <vector>

#include <volk.h>

#include <xar_engine/graphics/vulkan/native/vulkan_device.hpp>


namespace xar_engine::graphics::vulkan::native
{
    class VulkanShader
    {
    public:
        struct Parameters;

    public:
        VulkanShader();
        explicit VulkanShader(const Parameters& parameters);

        ~VulkanShader();

        [[nodiscard]]
        VkShaderModule get_native() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct VulkanShader::Parameters
    {
        VulkanDevice vulkan_device;
        std::vector<char> shader_byte_code;
    };
}