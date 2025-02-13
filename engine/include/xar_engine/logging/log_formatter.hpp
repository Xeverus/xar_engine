#pragma once

#include <string>

#include <xar_engine/logging/log_level.hpp>

namespace xar_engine::logging
{
    class ILogFormatter
    {
    public:
        virtual ~ILogFormatter();

        [[nodiscard]]
        virtual std::string format(
            LogLevel log_level,
            const std::string& tag,
            const std::string& message) const = 0;
    };
}