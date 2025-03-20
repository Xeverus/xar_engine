#pragma once

#include <xar_engine/graphics/backend/component/shader_component.hpp>

#include <xar_engine/graphics/backend/vulkan/vulkan_graphics_backend_state.hpp>


namespace xar_engine::graphics::backend::component::vulkan
{
    class IVulkanShaderComponent
        : public IShaderComponent
          , public backend::vulkan::SharedVulkanGraphicsBackendState
    {
    public:
        using SharedVulkanGraphicsBackendState::SharedVulkanGraphicsBackendState;

        api::ShaderReference make_shader(const MakeShaderParameters& parameters) override;
    };
}