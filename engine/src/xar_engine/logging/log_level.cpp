#include <xar_engine/logging/log_level.hpp>

#include <ostream>

#include <xar_engine/error/exception_utils.hpp>


ENUM_TO_STRING_IMPL(xar_engine::logging::LoggingLevel,
                    xar_engine::logging::LoggingLevel::DEBUG,
                    xar_engine::logging::LoggingLevel::INFO,
                    xar_engine::logging::LoggingLevel::WARNING,
                    xar_engine::logging::LoggingLevel::ERROR,
                    xar_engine::logging::LoggingLevel::CRITICAL);
