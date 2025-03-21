#pragma once

#include <xar_engine/graphics/backend/unit/command_buffer_unit.hpp>

#include <xar_engine/graphics/backend/vulkan/vulkan_graphics_backend_state.hpp>


namespace xar_engine::graphics::backend::unit::vulkan
{
    class IVulkanCommandBufferUnit
        : public ICommandBufferUnit
          , public backend::vulkan::SharedVulkanGraphicsBackendState
    {
    public:
        using SharedVulkanGraphicsBackendState::SharedVulkanGraphicsBackendState;

        std::vector<api::CommandBufferReference> make_command_buffer_list(const MakeCommandBufferParameters& parameters) override;

        void begin_command_buffer(const BeginCommandBufferParameters& parameters) override;
        void end_command_buffer(const EndCommandBufferParameters& parameters) override;
        void submit_command_buffer(const SubmitCommandBufferParameters& parameters) override;
    };
}