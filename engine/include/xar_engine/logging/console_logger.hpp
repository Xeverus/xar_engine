#pragma once

#include <memory>

#include <xar_engine/logging/log_formatter.hpp>
#include <xar_engine/logging/logger.hpp>


namespace xar_engine::logging
{
    class ConsoleLogger : public ILogger
    {
    public:
        ConsoleLogger();
        explicit ConsoleLogger(std::unique_ptr<ILogFormatter> log_formatter);

        void log_message(
            LogLevel log_level,
            const std::string& tag,
            const std::string& message) override;

    private:
        std::unique_ptr<ILogFormatter> _log_formatter;
    };

}