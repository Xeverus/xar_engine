#include <xar_engine/logging/file_logger.hpp>

#include <fstream>

#include <xar_engine/logging/default_log_formatter.hpp>


namespace xar_engine::logging
{
    FileLogger::FileLogger(const std::filesystem::path& filepath)
        : FileLogger(
        filepath,
        nullptr)
    {
    }

    FileLogger::FileLogger(
        const std::filesystem::path& filepath,
        std::unique_ptr<ILogFormatter> log_formatter)
        : _log_formatter(std::move(log_formatter))
        , _file_stream(
            filepath,
            std::ios::out)
    {
        if (_log_formatter == nullptr)
        {
            _log_formatter = std::make_unique<DefaultLogFormatter>();
        }
    }

    void FileLogger::log_message(
        LogLevel log_level,
        const std::string& tag,
        const std::string& message)
    {
        if (!_file_stream.good())
        {
            return;
        }

        _file_stream << _log_formatter->format(
            log_level,
            tag,
            message) << std::endl;
    }
}
