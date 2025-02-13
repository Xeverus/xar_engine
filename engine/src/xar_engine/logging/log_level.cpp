#include <xar_engine/logging/log_level.hpp>

#include <xar_engine/meta/enum_impl.hpp>


ENUM_TO_STRING_IMPL(xar_engine::logging::LogLevel,
                    xar_engine::logging::LogLevel::DEBUG,
                    xar_engine::logging::LogLevel::INFO,
                    xar_engine::logging::LogLevel::WARNING,
                    xar_engine::logging::LogLevel::ERROR,
                    xar_engine::logging::LogLevel::CRITICAL);
