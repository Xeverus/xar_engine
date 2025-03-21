#pragma once

#include <xar_engine/graphics/api/command_buffer_reference.hpp>
#include <xar_engine/graphics/api/image_view_reference.hpp>
#include <xar_engine/graphics/api/swap_chain_reference.hpp>

#include <xar_engine/graphics/context/window_surface.hpp>


namespace xar_engine::graphics::backend::unit
{
    class ISwapChainUnit
    {
    public:
        struct MakeSwapChainParameters;
        struct BeginFrameParameters;
        struct BeginRenderingParameters;
        struct EndRenderingParameters;
        struct EndFrameParameters;

    public:
        virtual ~ISwapChainUnit();


        virtual api::SwapChainReference make_swap_chain(const MakeSwapChainParameters& parameters) = 0;

        virtual std::tuple<api::ESwapChainResult, std::uint32_t, std::uint32_t> begin_frame(const BeginFrameParameters& parameters) = 0;
        virtual void begin_rendering(const BeginRenderingParameters& parameters) = 0;
        virtual void end_rendering(const EndRenderingParameters& parameters) = 0;
        virtual api::ESwapChainResult end_frame(const EndFrameParameters& parameters) = 0;
    };


    struct ISwapChainUnit::MakeSwapChainParameters
    {
        std::shared_ptr<context::IWindowSurface> window_surface;
        std::uint32_t buffering_level;
    };

    struct ISwapChainUnit::BeginFrameParameters
    {
        api::SwapChainReference swap_chain;
    };

    struct ISwapChainUnit::BeginRenderingParameters
    {
        api::CommandBufferReference command_buffer;
        api::SwapChainReference swap_chain;
        std::uint32_t image_index;
        api::ImageViewReference color_image_view;
        api::ImageViewReference depth_image_view;
    };

    struct ISwapChainUnit::EndRenderingParameters
    {
        api::CommandBufferReference command_buffer;
        api::SwapChainReference swap_chain;
        std::uint32_t image_index;
    };

    struct ISwapChainUnit::EndFrameParameters
    {
        api::CommandBufferReference command_buffer;
        api::SwapChainReference swap_chain;
    };
}