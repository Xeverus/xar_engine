#pragma once

#include <xar_engine/graphics/backend/unit/shader_unit.hpp>

#include <xar_engine/graphics/backend/vulkan/vulkan_graphics_backend_state.hpp>


namespace xar_engine::graphics::backend::unit::vulkan
{
    class IVulkanShaderUnit
        : public IShaderUnit
          , public backend::vulkan::SharedVulkanGraphicsBackendState
    {
    public:
        using SharedVulkanGraphicsBackendState::SharedVulkanGraphicsBackendState;

        api::ShaderReference make_shader(const MakeShaderParameters& parameters) override;
    };
}