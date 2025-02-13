#include <gtest/gtest.h>

#include <sstream>

#include <xar_engine/logging/log_formatter_mock.hpp>
#include <xar_engine/logging/stream_logger.hpp>

namespace
{
    TEST(stream_logger, empty_formatter__no_crash)
    {
        std::stringstream stream;
        xar_engine::logging::StreamLogger logger(stream);

        logger.log_message(
            xar_engine::logging::LogLevel::INFO,
            "tag",
            "message");
    }

    TEST(stream_logger, with_formatter__message_is_logged_with_correct_format)
    {
        const auto logging_level = xar_engine::logging::LogLevel::INFO;
        const auto tag = std::string{"some tag"};
        const auto message = std::string{"some message"};

        const auto formatted_message = fmt::format("{} {} {}", xar_engine::meta::enum_to_string(logging_level), tag, message);
        const auto expected_formatted_message = fmt::format("{}\n", formatted_message);

        auto log_formatter = std::make_unique<xar_engine::logging::LogFormatterMock>();
        EXPECT_CALL(*log_formatter, format(logging_level, tag, message)).WillOnce(::testing::Return(formatted_message));

        std::stringstream stream;
        auto logger = xar_engine::logging::StreamLogger{stream, std::move(log_formatter)};
        logger.log_message(
            logging_level,
            tag,
            message);

        EXPECT_EQ(stream.str(), expected_formatted_message);
    }
}