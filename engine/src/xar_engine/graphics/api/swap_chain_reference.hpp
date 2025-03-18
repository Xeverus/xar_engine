#pragma once

#include <xar_engine/meta/enum.hpp>
#include <xar_engine/meta/resource_reference.hpp>


namespace xar_engine::graphics::api
{
    enum class SwapChainTag;
    using SwapChainReference = meta::TResourceReference<SwapChainTag>;


    enum class ESwapChainResult
    {
        OK,
        RECREATION_REQUIRED,
        ERROR,
    };
}


ENUM_TO_STRING(xar_engine::graphics::api::ESwapChainResult);