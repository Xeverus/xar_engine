#include <gtest/gtest.h>

#include <xar_engine/math/epsilon.hpp>


namespace
{
    const auto epsilon = xar_engine::math::epsilon::get_epsilon(0.0);
    const auto epsilon_x2 = epsilon * 2.0;

    TEST(epsilon, equal)
    {
        EXPECT_TRUE(xar_engine::math::epsilon::equal(0.0, 0.0));
        EXPECT_TRUE(xar_engine::math::epsilon::equal(0.0, 0.0 + epsilon));
        EXPECT_TRUE(xar_engine::math::epsilon::equal(0.0, 0.0 - epsilon));
        EXPECT_FALSE(xar_engine::math::epsilon::equal(0.0, 0.0 + epsilon_x2));
        EXPECT_FALSE(xar_engine::math::epsilon::equal(0.0, 0.0 - epsilon_x2));
    }

    TEST(epsilon, not_equal)
    {
        EXPECT_FALSE(xar_engine::math::epsilon::not_equal(0.0, 0.0));
        EXPECT_FALSE(xar_engine::math::epsilon::not_equal(0.0, 0.0 + epsilon));
        EXPECT_FALSE(xar_engine::math::epsilon::not_equal(0.0, 0.0 - epsilon));
        EXPECT_TRUE(xar_engine::math::epsilon::not_equal(0.0, 0.0 + epsilon_x2));
        EXPECT_TRUE(xar_engine::math::epsilon::not_equal(0.0, 0.0 - epsilon_x2));
    }

    TEST(epsilon, less)
    {
        EXPECT_FALSE(xar_engine::math::epsilon::less(0.0, 0.0));
        EXPECT_FALSE(xar_engine::math::epsilon::less(0.0, 0.0 + epsilon));
        EXPECT_FALSE(xar_engine::math::epsilon::less(0.0, 0.0 - epsilon));
        EXPECT_TRUE(xar_engine::math::epsilon::less(0.0, 0.0 + epsilon_x2));
        EXPECT_FALSE(xar_engine::math::epsilon::less(0.0, 0.0 - epsilon_x2));
    }

    TEST(epsilon, less_or_equal)
    {
        EXPECT_TRUE(xar_engine::math::epsilon::less_or_equal(0.0, 0.0));
        EXPECT_TRUE(xar_engine::math::epsilon::less_or_equal(0.0, 0.0 + epsilon));
        EXPECT_TRUE(xar_engine::math::epsilon::less_or_equal(0.0, 0.0 - epsilon));
        EXPECT_TRUE(xar_engine::math::epsilon::less_or_equal(0.0, 0.0 + epsilon_x2));
        EXPECT_FALSE(xar_engine::math::epsilon::less_or_equal(0.0, 0.0 - epsilon_x2));
    }

    TEST(epsilon, greater)
    {
        EXPECT_FALSE(xar_engine::math::epsilon::greater(0.0, 0.0));
        EXPECT_FALSE(xar_engine::math::epsilon::greater(0.0, 0.0 + epsilon));
        EXPECT_FALSE(xar_engine::math::epsilon::greater(0.0, 0.0 - epsilon));
        EXPECT_FALSE(xar_engine::math::epsilon::greater(0.0, 0.0 + epsilon_x2));
        EXPECT_TRUE(xar_engine::math::epsilon::greater(0.0, 0.0 - epsilon_x2));
    }

    TEST(epsilon, greater_or_equal)
    {
        EXPECT_TRUE(xar_engine::math::epsilon::greater_or_equal(0.0, 0.0));
        EXPECT_TRUE(xar_engine::math::epsilon::greater_or_equal(0.0, 0.0 + epsilon));
        EXPECT_TRUE(xar_engine::math::epsilon::greater_or_equal(0.0, 0.0 - epsilon));
        EXPECT_FALSE(xar_engine::math::epsilon::greater_or_equal(0.0, 0.0 + epsilon_x2));
        EXPECT_TRUE(xar_engine::math::epsilon::greater_or_equal(0.0, 0.0 - epsilon_x2));
    }
}