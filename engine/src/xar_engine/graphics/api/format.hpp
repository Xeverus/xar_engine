#pragma once

#include <xar_engine/meta/enum.hpp>


namespace xar_engine::graphics::api
{
    enum class EFormat
    {
        D32_SIGNED_FLOAT,
        R32G32_SIGNED_FLOAT,
        R32G32B32_SIGNED_FLOAT,
        R8G8B8A8_SRGB,
    };
}

ENUM_TO_STRING(xar_engine::graphics::api::EFormat);
