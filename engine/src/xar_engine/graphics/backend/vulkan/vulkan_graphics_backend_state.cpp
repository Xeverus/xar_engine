#include <xar_engine/graphics/backend/vulkan/vulkan_graphics_backend_state.hpp>


namespace xar_engine::graphics::backend::vulkan
{

    SharedVulkanGraphicsBackendState::SharedVulkanGraphicsBackendState(std::shared_ptr<VulkanGraphicsBackendState> state)
        : _state(std::move(state))
    {
        XAR_THROW_IF(
            _state == nullptr,
            error::XarException,
            "State cannot be null");
    }

    SharedVulkanGraphicsBackendState::~SharedVulkanGraphicsBackendState() = default;

    const VulkanGraphicsBackendState& SharedVulkanGraphicsBackendState::get_state() const
    {
        return *_state;
    }

    VulkanGraphicsBackendState& SharedVulkanGraphicsBackendState::get_state()
    {
        return *_state;
    }
}