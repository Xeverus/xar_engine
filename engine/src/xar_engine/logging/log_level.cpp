#include <xar_engine/logging/log_level.hpp>

#include <ostream>

#include <xar_engine/error/exception_utils.hpp>

namespace xar_engine::meta
{
    std::string enum_to_string(logging::LoggingLevel enum_value)
    {
        switch (enum_value)
        {
            case logging::LoggingLevel::DEBUG: return "DEBUG";
            case logging::LoggingLevel::INFO: return "INFO";
            case logging::LoggingLevel::WARNING: return "WARNING";
            case logging::LoggingLevel::ERROR: return "ERROR";
            case logging::LoggingLevel::CRITICAL: return "CRITICAL";
        }

        XAR_THROW(
            error::XarException,
            "LoggingLevel enum value doesn't define value '{}'",
            static_cast<int>(enum_value));
    }

    std::ostream& operator<<(
        std::ostream& stream,
        const logging::LoggingLevel enum_value)
    {
        if (stream.good())
        {
            stream << enum_to_string(enum_value);
        }

        return stream;
    }
}