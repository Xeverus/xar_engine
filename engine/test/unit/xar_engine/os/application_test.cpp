#include <gtest/gtest.h>

#include <xar_engine/os/application.hpp>

namespace
{
    TEST(application, make)
    {
        const auto application = xar_engine::os::ApplicationFactory::make();
        EXPECT_NE(application, nullptr);
    }
}