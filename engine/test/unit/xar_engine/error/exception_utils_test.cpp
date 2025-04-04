#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include <xar_engine/error/exception_utils.hpp>


namespace
{
    TEST(exception_utils,
         format_message_without_args)
    {
        constexpr auto* const expected_value =
            "Reason: 1 != 2\n"
            "File: " __FILE__ "\n"
            "Line: 19\n"
            "Message: Hello World 123!";

        EXPECT_STREQ(
            XAR_FORMAT_EXCEPTION_MESSAGE(
                "1 != 2",
                "Hello World 123!").c_str(),
            expected_value);
    }

    TEST(exception_utils,
         format_message_with_args)
    {
        constexpr auto* const expected_value =
            "Reason: 1 != 2\n"
            "File: " __FILE__ "\n"
            "Line: 35\n"
            "Message: Hello World 123!";

        EXPECT_STREQ(
            XAR_FORMAT_EXCEPTION_MESSAGE(
                "1 != 2",
                "Hello {} {}!",
                "World",
                123).c_str(),
            expected_value);
    }

    TEST(exception_utils,
         throw__should_throw)
    {
        EXPECT_THROW([]()
                     {
                         XAR_THROW(
                             xar_engine::error::XarException,
                             "Should throw");
                     }(),
                     xar_engine::error::XarException);
    }

    TEST(exception_utils,
         throw_with_message__message_in_exception_should_match)
    {
        try
        {
            XAR_THROW(
                xar_engine::error::XarException,
                "Message in exception");
        }
        catch (const xar_engine::error::XarException& exception)
        {
            EXPECT_THAT(
                exception.what(),
                ::testing::ContainsRegex("Message in exception"));
        }
    }

    TEST(exception_utils,
         throw_if_true__should_throw)
    {
        EXPECT_THROW([]()
                     {
                         XAR_THROW_IF(
                             true,
                             xar_engine::error::XarException,
                             "Should throw");
                     }(),
                     xar_engine::error::XarException);
    }

    TEST(exception_utils,
         throw_if_false__should_not_throw)
    {
        XAR_THROW_IF(
            false,
            xar_engine::error::XarException,
            "Should not throw");
    }

    TEST(exception_utils,
         throw_if_with_message__message_in_exception_should_match)
    {
        try
        {
            XAR_THROW_IF(
                true,
                xar_engine::error::XarException,
                "Message in exception");
        }
        catch (const xar_engine::error::XarException& exception)
        {
            EXPECT_THAT(
                exception.what(),
                ::testing::ContainsRegex("Message in exception"));
        }
    }
}
