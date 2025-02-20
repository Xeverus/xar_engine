#pragma once

#include <string>

#include <xar_engine/meta/enum.hpp>


namespace xar_engine::logging
{
    enum class LogLevel
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL,
    };
}

ENUM_TO_STRING(xar_engine::logging::LogLevel);
