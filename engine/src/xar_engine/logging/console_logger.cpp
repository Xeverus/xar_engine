#include <xar_engine/logging/console_logger.hpp>

#include <iostream>

#include <xar_engine/error/exception_utils.hpp>

#include <xar_engine/logging/default_log_formatter.hpp>


namespace xar_engine::logging
{
    namespace
    {
        std::ostream& select_stream(LogLevel logging_level)
        {
            switch (logging_level)
            {
                case LogLevel::DEBUG:
                case LogLevel::INFO:
                case LogLevel::WARNING:return std::cout;
                case LogLevel::ERROR:
                case LogLevel::CRITICAL:return std::cerr;
            }

            XAR_THROW(
                error::XarException,
                "LoggingLevel enum value doesn't define value '{}'",
                static_cast<int>(logging_level));
        }
    }

    ConsoleLogger::ConsoleLogger()
        : ConsoleLogger(nullptr)
    {
    }

    ConsoleLogger::ConsoleLogger(std::unique_ptr<ILogFormatter> log_formatter)
        : _log_formatter(std::move(log_formatter))
    {
        if (_log_formatter == nullptr)
        {
            _log_formatter = std::make_unique<DefaultLogFormatter>();
        }
    }

    void ConsoleLogger::log_message(
        LogLevel log_level,
        const std::string& tag,
        const std::string& message)
    {
        std::ostream& stream = select_stream(log_level);
        if (!stream.good())
        {
            return;
        }

        stream << _log_formatter->format(
            log_level,
            tag,
            message) << std::endl;
    }
}
