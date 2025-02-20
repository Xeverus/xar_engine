#pragma once

#include <filesystem>
#include <fstream>
#include <memory>

#include <xar_engine/logging/log_formatter.hpp>
#include <xar_engine/logging/logger.hpp>


namespace xar_engine::logging
{
    class FileLogger : public ILogger
    {
    public:
        explicit FileLogger(const std::filesystem::path& filepath);
        FileLogger(const std::filesystem::path& filepath, std::unique_ptr<ILogFormatter> log_formatter);

        void log_message(
            LogLevel log_level,
            const std::string& tag,
            const std::string& message) override;

    private:
        std::unique_ptr<ILogFormatter> _log_formatter;
        std::fstream _file_stream;
    };

}