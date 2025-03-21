#pragma once


#include <xar_engine/graphics/backend/unit/device_unit.hpp>

#include <xar_engine/graphics/backend/vulkan/vulkan_graphics_backend_state.hpp>


namespace xar_engine::graphics::backend::unit::vulkan
{
    class IVulkanDeviceUnit
        : public IDeviceUnit
          , public backend::vulkan::SharedVulkanGraphicsBackendState
    {
    public:
        using SharedVulkanGraphicsBackendState::SharedVulkanGraphicsBackendState;


        void wait_idle() override;

        [[nodiscard]]
        std::uint32_t get_sample_count() const override;

        [[nodiscard]]
        api::EFormat find_depth_format() const override;
    };
}