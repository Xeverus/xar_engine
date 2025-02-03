#include <gtest/gtest.h>

#include <xar_engine/version/version.hpp>

namespace
{
    TEST(version, returns_correct_version)
    {
       EXPECT_STREQ(xar_engine::version::get_version().c_str(), "0.0.0");
    }
}