#pragma once

#include <xar_engine/graphics/backend/component/swap_chain_component.hpp>

#include <xar_engine/graphics/backend/vulkan/vulkan_graphics_backend_state.hpp>


namespace xar_engine::graphics::backend::component::vulkan
{
    class IVulkanSwapChainComponent
        : public ISwapChainComponent
          , public backend::vulkan::SharedVulkanGraphicsBackendState
    {
    public:
        using SharedVulkanGraphicsBackendState::SharedVulkanGraphicsBackendState;

        api::SwapChainReference make_swap_chain(const MakeSwapChainParameters& parameters) override;

        std::tuple<api::ESwapChainResult, std::uint32_t, std::uint32_t> begin_frame(const BeginFrameParameters& parameters) override;
        void begin_rendering(const BeginRenderingParameters& parameters) override;
        void end_rendering(const EndRenderingParameters& parameters) override;
        api::ESwapChainResult end_frame(const EndFrameParameters& parameters) override;
    };
}