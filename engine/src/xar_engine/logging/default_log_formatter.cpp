#include <xar_engine/logging/default_log_formatter.hpp>

#include <fmt/core.h>

#include <xar_engine/time/time.hpp>

namespace xar_engine::logging
{
    std::string DefaultLogFormatter::format(
        const LoggingLevel logging_level,
        const std::string& tag,
        const std::string& message) const
    {
        return fmt::format(
            "[{}][{}ns][{}]: {}",
            meta::enum_to_string(logging_level),
            time::Time::since_startup(),
            tag,
            message);
    }
}
