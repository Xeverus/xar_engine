#pragma once

#include <xar_engine/logging/log_formatter.hpp>

namespace xar_engine::logging
{
    class DefaultLogFormatter : public ILogFormatter
    {
    public:
        [[nodiscard]]
        std::string format(
            LoggingLevel logging_level,
            const std::string& tag,
            const std::string& message) const override;
    };
}