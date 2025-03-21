#include <xar_engine/graphics/backend/unit/vulkan/vulkan_shader_unit.hpp>


namespace xar_engine::graphics::backend::unit::vulkan
{
    api::ShaderReference IVulkanShaderUnit::make_shader(const MakeShaderParameters& parameters)
    {
        return get_state()._vulkan_resource_storage.add(
            native::vulkan::VulkanShader{
                {
                    get_state()._vulkan_device,
                    parameters.shader_byte_code,
                }});
    }
}