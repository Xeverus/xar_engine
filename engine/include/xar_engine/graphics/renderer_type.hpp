#pragma once

#include <xar_engine/meta/enum.hpp>


namespace xar_engine::graphics
{
    enum class RendererType
    {
        VULKAN
    };
}

ENUM_TO_STRING(xar_engine::graphics::RendererType);