#include <xar_engine/graphics/backend/component/vulkan/vulkan_shader_component.hpp>


namespace xar_engine::graphics::backend::component::vulkan
{
    api::ShaderReference IVulkanShaderComponent::make_shader(const MakeShaderParameters& parameters)
    {
        return get_state()._vulkan_resource_storage.add(
            native::vulkan::VulkanShader{
                {
                    get_state()._vulkan_device,
                    parameters.shader_byte_code,
                }});
    }
}