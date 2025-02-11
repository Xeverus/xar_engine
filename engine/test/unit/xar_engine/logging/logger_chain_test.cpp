#include <gtest/gtest.h>

#include <xar_engine/logging/logger_chain.hpp>
#include <xar_engine/logging/logger_mock.hpp>

namespace
{
    TEST(logger_chain, empty_list__no_crash)
    {
        auto logger = xar_engine::logging::LoggerChain{{}};

        logger.log_message(
            xar_engine::logging::LoggingLevel::INFO,
            "tag",
            "message");
    }

    TEST(logger_chain, all_loggers_are_called)
    {
        const auto logging_level = xar_engine::logging::LoggingLevel::INFO;
        const auto tag = std::string{"some tag"};
        const auto message = std::string{"some message"};

        const auto formatted_message = fmt::format("{} {} {}", xar_engine::meta::enum_to_string(logging_level), tag, message);
        const auto expected_formatted_message = fmt::format("{}\n", formatted_message);

        std::vector<std::unique_ptr<xar_engine::logging::ILogger>> logger_vector;
        for (auto i = 0; i < 3; ++i)
        {
            auto logger = std::make_unique<xar_engine::logging::LoggerMock>();
            EXPECT_CALL(*logger, log_message(logging_level, tag, message)).Times(1);

            logger_vector.push_back(std::move(logger));
        }

        auto logger_chain = xar_engine::logging::LoggerChain{std::move(logger_vector)};
        logger_chain.log_message(
            logging_level,
            tag,
            message);

    }
}