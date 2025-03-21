#pragma once

#include <gmock/gmock.h>

#include <xar_engine/logging/logger.hpp>


namespace xar_engine::logging
{
    class LoggerMock
        : public ILogger
    {
    public:
        MOCK_METHOD(
            void,
            log_message,
            (
                LogLevel log_level,
                const std::string& tag,
                const std::string& message
            ),
            (override));
    };
}