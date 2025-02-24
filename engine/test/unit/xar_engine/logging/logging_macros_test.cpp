#include <gtest/gtest.h>

#include <xar_engine/logging/logger.hpp>

#include <xar_engine/logging/logger_mock.hpp>


namespace
{
    TEST(logging_macros,
         XAR_LOG_LOCAL__works)
    {
        xar_engine::logging::LoggerMock logger;
        EXPECT_CALL(logger,
                    log_message(
                        xar_engine::logging::LogLevel::ERROR,
                        "local-tag-1",
                        "some message!")).Times(1);
        EXPECT_CALL(logger,
                    log_message(
                        xar_engine::logging::LogLevel::CRITICAL,
                        "local-tag-2",
                        "some message 123!")).Times(1);

        XAR_LOG_LOCAL(xar_engine::logging::LogLevel::ERROR,
                      logger,
                      "local-tag-1",
                      "some message!");
        XAR_LOG_LOCAL(xar_engine::logging::LogLevel::CRITICAL,
                      logger,
                      "local-tag-2",
                      "some message {}!",
                      123);
    }

    TEST(logging_macros,
         XAR_LOG__works)
    {
        XAR_LOG(xar_engine::logging::LogLevel::DEBUG,
                "global-tag-1",
                "some message!");
    }
}