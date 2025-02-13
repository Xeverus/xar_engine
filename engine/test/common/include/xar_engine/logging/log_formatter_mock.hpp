#pragma once

#include <gmock/gmock.h>

#include <xar_engine/logging/log_formatter.hpp>

namespace xar_engine::logging
{
    class LogFormatterMock : public ILogFormatter
    {
    public:
        MOCK_METHOD(
            std::string,
            format,
            (
                LogLevel log_level,
                const std::string& tag,
                const std::string& message
            ),
            (const, override));
    };
}