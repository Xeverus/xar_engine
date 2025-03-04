#pragma once

#include <xar_engine/graphics/api/resource_reference.hpp>


namespace xar_engine::graphics::api
{
    enum class SwapChainTag;
    using SwapChainReference = TResourceReference<SwapChainTag>;

    enum class ESwapChainResult
    {
        OK,
        RECREATION_REQUIRED,
        ERROR,
    };
}