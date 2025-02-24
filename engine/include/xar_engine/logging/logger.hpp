#pragma once

#include <fmt/core.h>

#include <xar_engine/logging/log_level.hpp>

#include <xar_engine/meta/singleton.hpp>

#include <xar_engine/os/service_register.hpp>


namespace xar_engine::logging
{
    class ILogger
    {
    public:
        virtual ~ILogger();

        virtual void log_message(
            LogLevel logging_level,
            const std::string& tag,
            const std::string& message) = 0;
    };


#define XAR_LOG_LEVEL xar_engine::logging::LogLevel::DEBUG

#define XAR_LOG_LOCAL(logging_level, logger, tag, message, ...) \
    do                                                    \
    {                                                     \
        if constexpr(logging_level >= XAR_LOG_LEVEL)      \
        {                                                 \
            (logger).log_message(logging_level, tag, ::fmt::format(message, ##__VA_ARGS__)); \
        }                                                 \
    }                                                     \
    while(false)
}

#define XAR_LOG(logging_level, tag, message, ...) \
    XAR_LOG_LOCAL(logging_level, ::xar_engine::meta::Singleton::get_instance<::xar_engine::os::ServiceRegister>().get_global_logger(), tag, message, ##__VA_ARGS__)
