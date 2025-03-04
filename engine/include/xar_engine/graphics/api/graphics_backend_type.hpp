#pragma once

#include <xar_engine/meta/enum.hpp>


namespace xar_engine::graphics::api
{
    enum class EGraphicsBackendType
    {
        VULKAN,
    };
}

ENUM_TO_STRING(xar_engine::graphics::api::EGraphicsBackendType);