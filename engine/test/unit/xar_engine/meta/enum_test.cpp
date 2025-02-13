#include <gtest/gtest.h>

#include <sstream>

#include <xar_engine/meta/enum_impl.hpp>


namespace enum_test::ns
{
    enum class EnumTestEnum
    {
        VALUE_0,
        VALUE_1,
        VALUE_2
    };
}

enum class EnumTestEnum
{
    VALUE_10,
    VALUE_11,
    VALUE_12
};

ENUM_TO_STRING(enum_test::ns::EnumTestEnum);
ENUM_TO_STRING_IMPL(enum_test::ns::EnumTestEnum,
                    enum_test::ns::EnumTestEnum::VALUE_0,
                    enum_test::ns::EnumTestEnum::VALUE_1,
                    enum_test::ns::EnumTestEnum::VALUE_2);

ENUM_TO_STRING(EnumTestEnum);
ENUM_TO_STRING_IMPL(EnumTestEnum,
                    EnumTestEnum::VALUE_10,
                    EnumTestEnum::VALUE_11,
                    EnumTestEnum::VALUE_12);

namespace
{
    TEST(enum_,
         enum_in_namespace__enum_to_string)
    {
        const auto* const value_0 = xar_engine::meta::enum_to_string(enum_test::ns::EnumTestEnum::VALUE_0);
        const auto* const value_1 = xar_engine::meta::enum_to_string(enum_test::ns::EnumTestEnum::VALUE_1);
        const auto* const value_2 = xar_engine::meta::enum_to_string(enum_test::ns::EnumTestEnum::VALUE_2);

        EXPECT_STREQ(value_0,
                     "enum_test::ns::EnumTestEnum::VALUE_0");
        EXPECT_STREQ(value_1,
                     "enum_test::ns::EnumTestEnum::VALUE_1");
        EXPECT_STREQ(value_2,
                     "enum_test::ns::EnumTestEnum::VALUE_2");
    }

    TEST(enum_,
         enum_in_namespace__enum_to_string_operator)
    {
        std::stringstream str_value_0;
        std::stringstream str_value_1;
        std::stringstream str_value_2;

        str_value_0 << enum_test::ns::EnumTestEnum::VALUE_0;
        str_value_1 << enum_test::ns::EnumTestEnum::VALUE_1;
        str_value_2 << enum_test::ns::EnumTestEnum::VALUE_2;

        EXPECT_STREQ(str_value_0.str().c_str(),
                     "enum_test::ns::EnumTestEnum::VALUE_0");
        EXPECT_STREQ(str_value_1.str().c_str(),
                     "enum_test::ns::EnumTestEnum::VALUE_1");
        EXPECT_STREQ(str_value_2.str().c_str(),
                     "enum_test::ns::EnumTestEnum::VALUE_2");
    }

    TEST(enum_,
         enum_in_namespace__enum_to_short_string)
    {
        const auto* const value_0 = xar_engine::meta::enum_to_short_string(enum_test::ns::EnumTestEnum::VALUE_0);
        const auto* const value_1 = xar_engine::meta::enum_to_short_string(enum_test::ns::EnumTestEnum::VALUE_1);
        const auto* const value_2 = xar_engine::meta::enum_to_short_string(enum_test::ns::EnumTestEnum::VALUE_2);

        EXPECT_STREQ(value_0,
                     "VALUE_0");
        EXPECT_STREQ(value_1,
                     "VALUE_1");
        EXPECT_STREQ(value_2,
                     "VALUE_2");
    }

    TEST(enum_,
         enum_without_namespace__enum_to_string)
    {
        const auto* const value_0 = xar_engine::meta::enum_to_string(EnumTestEnum::VALUE_10);
        const auto* const value_1 = xar_engine::meta::enum_to_string(EnumTestEnum::VALUE_11);
        const auto* const value_2 = xar_engine::meta::enum_to_string(EnumTestEnum::VALUE_12);

        EXPECT_STREQ(value_0,
                     "EnumTestEnum::VALUE_10");
        EXPECT_STREQ(value_1,
                     "EnumTestEnum::VALUE_11");
        EXPECT_STREQ(value_2,
                     "EnumTestEnum::VALUE_12");
    }

    TEST(enum_,
         enum_without_namespace__enum_to_string_operator)
    {
        std::stringstream str_value_0;
        std::stringstream str_value_1;
        std::stringstream str_value_2;

        str_value_0 << EnumTestEnum::VALUE_10;
        str_value_1 << EnumTestEnum::VALUE_11;
        str_value_2 << EnumTestEnum::VALUE_12;

        EXPECT_STREQ(str_value_0.str().c_str(),
                     "EnumTestEnum::VALUE_10");
        EXPECT_STREQ(str_value_1.str().c_str(),
                     "EnumTestEnum::VALUE_11");
        EXPECT_STREQ(str_value_2.str().c_str(),
                     "EnumTestEnum::VALUE_12");
    }

    TEST(enum_,
         enum_without_namespace__enum_to_short_string)
    {
        const auto* const value_0 = xar_engine::meta::enum_to_short_string(EnumTestEnum::VALUE_10);
        const auto* const value_1 = xar_engine::meta::enum_to_short_string(EnumTestEnum::VALUE_11);
        const auto* const value_2 = xar_engine::meta::enum_to_short_string(EnumTestEnum::VALUE_12);

        EXPECT_STREQ(value_0,
                     "VALUE_10");
        EXPECT_STREQ(value_1,
                     "VALUE_11");
        EXPECT_STREQ(value_2,
                     "VALUE_12");
    }

    TEST(enum_,
         non_existing_value__should_throw)
    {
        const auto non_existing_value = static_cast<enum_test::ns::EnumTestEnum>(0xFFFF);
        EXPECT_THROW(
            []()
            {
                return xar_engine::meta::enum_to_string(non_existing_value);
            }(),
            xar_engine::error::XarException);
    }
}