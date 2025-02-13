#include <xar_engine/logging/default_log_formatter.hpp>

#include <fmt/core.h>

#include <xar_engine/time/time.hpp>

namespace xar_engine::logging
{
    std::string DefaultLogFormatter::format(
        const LogLevel log_level,
        const std::string& tag,
        const std::string& message) const
    {
        const auto level = meta::enum_to_short_string(log_level);
        const auto timestamp = time::Time::since_startup();

        return fmt::format(
            "[{}][{}ns][{}]: {}",
            level,
            timestamp,
            tag,
            message);
    }
}
