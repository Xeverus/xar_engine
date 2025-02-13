#include <gtest/gtest.h>

#include <xar_engine/logging/log_formatter_mock.hpp>
#include <xar_engine/logging/file_logger.hpp>

namespace
{
    TEST(file_logger, empty_formatter__no_crash)
    {
        const auto filepath = std::string{"test.file_logger.empty_formatter__no_crash.txt"};
        auto logger = xar_engine::logging::FileLogger{filepath};

        logger.log_message(
            xar_engine::logging::LogLevel::INFO,
            "tag",
            "message");
    }

    TEST(file_logger, with_formatter__message_is_logged_with_correct_format)
    {
        const auto logging_level = xar_engine::logging::LogLevel::INFO;
        const auto tag = std::string{"some tag"};
        const auto message = std::string{"some message"};

        const auto formatted_message = fmt::format("{} {} {}", xar_engine::meta::enum_to_string(logging_level), tag, message);

        auto log_formatter = std::make_unique<xar_engine::logging::LogFormatterMock>();
        EXPECT_CALL(*log_formatter, format(logging_level, tag, message)).WillOnce(::testing::Return(formatted_message));

        const auto filepath = std::string{"test.file_logger.with_formatter__message_is_logged_with_correct_format.txt"};
        {
            auto logger = xar_engine::logging::FileLogger{filepath, std::move(log_formatter)};
            logger.log_message(
                logging_level,
                tag,
                message);
        }

        std::fstream file(filepath, std::ios::in);
        ASSERT_TRUE(file.is_open());

        std::string line;
        std::getline(file, line);
        EXPECT_EQ(line, formatted_message);
    }
}