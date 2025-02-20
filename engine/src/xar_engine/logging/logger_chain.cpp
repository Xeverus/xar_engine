#include <xar_engine/logging/logger_chain.hpp>


namespace xar_engine::logging
{
    LoggerChain::LoggerChain(std::vector<std::unique_ptr<ILogger>> loggers)
        : _loggers(std::move(loggers))
    {
    }

    void LoggerChain::log_message(
        LogLevel log_level,
        const std::string& tag,
        const std::string& message)
    {
        for (auto& logger : _loggers)
        {
            logger->log_message(log_level, tag, message);
        }
    }
}
