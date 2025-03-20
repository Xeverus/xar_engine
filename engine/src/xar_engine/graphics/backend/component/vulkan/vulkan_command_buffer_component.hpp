#pragma once

#include <xar_engine/graphics/backend/component/command_buffer_component.hpp>

#include <xar_engine/graphics/backend/vulkan/vulkan_graphics_backend_state.hpp>


namespace xar_engine::graphics::backend::component::vulkan
{
    class IVulkanCommandBufferComponent
        : public ICommandBufferComponent
          , public ::xar_engine::graphics::backend::vulkan::SharedVulkanGraphicsBackendState
    {
    public:
        using SharedVulkanGraphicsBackendState::SharedVulkanGraphicsBackendState;

        std::vector<api::CommandBufferReference> make_command_buffer_list(const MakeCommandBufferParameters& parameters) override;

        void begin_command_buffer(const BeginCommandBufferParameters& parameters) override;
        void end_command_buffer(const EndCommandBufferParameters& parameters) override;
        void submit_command_buffer(const SubmitCommandBufferParameters& parameters) override;
    };
}