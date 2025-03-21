#pragma once

#include <memory>
#include <vector>

#include <xar_engine/logging/log_formatter.hpp>
#include <xar_engine/logging/logger.hpp>


namespace xar_engine::logging
{
    class LoggerChain
        : public ILogger
    {
    public:
        explicit LoggerChain(std::vector<std::unique_ptr<ILogger>> loggers);

        void log_message(
            LogLevel log_level,
            const std::string& tag,
            const std::string& message) override;

    private:
        std::vector<std::unique_ptr<ILogger>> _loggers;
    };

}