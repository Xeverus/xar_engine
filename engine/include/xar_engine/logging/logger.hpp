#pragma once

#include <fmt/core.h>

#include <xar_engine/logging/log_level.hpp>

namespace xar_engine::logging
{
    class ILogger
    {
    public:
        virtual ~ILogger();

        virtual void log_message(
            LoggingLevel logging_level,
            const std::string& tag,
            const std::string& message) = 0;
    };


#define XAR_LOG(logger, logging_level, tag, message, ...) \
    (logger).log_message(logging_level, tag, fmt::format(message, __VA_ARGS__))

}