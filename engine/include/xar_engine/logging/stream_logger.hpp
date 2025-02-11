#pragma once

#include <memory>

#include <xar_engine/logging/log_formatter.hpp>
#include <xar_engine/logging/logger.hpp>

namespace xar_engine::logging
{
    class StreamLogger : public ILogger
    {
    public:
        explicit StreamLogger(std::ostream& stream);
        StreamLogger(std::ostream& stream, std::unique_ptr<ILogFormatter> log_formatter);

        void log_message(
            LoggingLevel logging_level,
            const std::string& tag,
            const std::string& message) override;

    private:
        std::unique_ptr<ILogFormatter> _log_formatter;
        std::ostream& _stream;
    };

}