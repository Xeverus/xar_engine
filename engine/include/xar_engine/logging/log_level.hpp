#pragma once

#include <string>

namespace xar_engine::logging
{
    enum class LoggingLevel
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL,
    };
}

namespace xar_engine::meta
{
    std::string enum_to_string(logging::LoggingLevel enum_value);
    std::ostream& operator<<(std::ostream& stream, logging::LoggingLevel enum_value);
}
