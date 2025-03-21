#include <xar_engine/logging/stream_logger.hpp>

#include <xar_engine/logging/default_log_formatter.hpp>


namespace xar_engine::logging
{
    StreamLogger::StreamLogger(std::ostream& stream)
        : StreamLogger(
        stream,
        nullptr)
    {
    }

    StreamLogger::StreamLogger(
        std::ostream& stream,
        std::unique_ptr<ILogFormatter> log_formatter)
        : _log_formatter(std::move(log_formatter))
        , _stream(stream)
    {
        if (_log_formatter == nullptr)
        {
            _log_formatter = std::make_unique<DefaultLogFormatter>();
        }
    }

    void StreamLogger::log_message(
        LogLevel log_level,
        const std::string& tag,
        const std::string& message)
    {
        if (!_stream.good())
        {
            return;
        }

        _stream << _log_formatter->format(
            log_level,
            tag,
            message) << std::endl;
    }
}
