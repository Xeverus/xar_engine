#pragma once

#include <xar_engine/graphics/api/command_buffer_reference.hpp>
#include <xar_engine/graphics/api/image_view_reference.hpp>
#include <xar_engine/graphics/api/swap_chain_reference.hpp>

#include <xar_engine/graphics/context/window_surface.hpp>


namespace xar_engine::graphics::backend::component
{
    class ISwapChainComponent
    {
    public:
        struct MakeSwapChainParameters;
        struct BeginFrameParameters;
        struct BeginRenderingParameters;
        struct EndRenderingParameters;
        struct EndFrameParameters;

    public:
        virtual ~ISwapChainComponent();


        virtual api::SwapChainReference make_swap_chain(const MakeSwapChainParameters& parameters) = 0;

        virtual std::tuple<api::ESwapChainResult, std::uint32_t, std::uint32_t> begin_frame(const BeginFrameParameters& parameters) = 0;
        virtual void begin_rendering(const BeginRenderingParameters& parameters) = 0;
        virtual void end_rendering(const EndRenderingParameters& parameters) = 0;
        virtual api::ESwapChainResult end_frame(const EndFrameParameters& parameters) = 0;
    };


    struct ISwapChainComponent::MakeSwapChainParameters
    {
        std::shared_ptr<context::IWindowSurface> window_surface;
        std::uint32_t buffering_level;
    };

    struct ISwapChainComponent::BeginFrameParameters
    {
        api::SwapChainReference swap_chain;
    };

    struct ISwapChainComponent::BeginRenderingParameters
    {
        api::CommandBufferReference command_buffer;
        api::SwapChainReference swap_chain;
        std::uint32_t image_index;
        api::ImageViewReference color_image_view;
        api::ImageViewReference depth_image_view;
    };

    struct ISwapChainComponent::EndRenderingParameters
    {
        api::CommandBufferReference command_buffer;
        api::SwapChainReference swap_chain;
        std::uint32_t image_index;
    };

    struct ISwapChainComponent::EndFrameParameters
    {
        api::CommandBufferReference command_buffer;
        api::SwapChainReference swap_chain;
    };
}