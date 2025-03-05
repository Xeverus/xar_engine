#include <gtest/gtest.h>

#include <xar_engine/meta/ref_counting_singleton.hpp>


namespace
{
    class RefCountedTestClass
    {
    public:
        int value = 0;
    };


    TEST(ref_counting_singleton,
         first_call__returns_new_and_initialized_instance)
    {
        auto instance = xar_engine::meta::RefCountedSingleton::get_instance_t<RefCountedTestClass>();
        ASSERT_NE(instance,
                  nullptr);
        EXPECT_EQ(instance->value, 0);
    }


    TEST(ref_counting_singleton,
         update_value_and_get_instance_again__value_preserved)
    {
        auto instance = xar_engine::meta::RefCountedSingleton::get_instance_t<RefCountedTestClass>();
        EXPECT_EQ(instance->value, 0);

        instance->value = 100;
        EXPECT_EQ(instance->value, 100);

        instance = xar_engine::meta::RefCountedSingleton::get_instance_t<RefCountedTestClass>();
        EXPECT_EQ(instance->value, 100);
    }


    TEST(ref_counting_singleton,
         update_value_and_reset_instance__instance_reset_to_default_state)
    {
        auto instance = xar_engine::meta::RefCountedSingleton::get_instance_t<RefCountedTestClass>();
        EXPECT_EQ(instance->value, 0);

        instance->value = 100;
        EXPECT_EQ(instance->value, 100);

        instance.reset();
        instance = xar_engine::meta::RefCountedSingleton::get_instance_t<RefCountedTestClass>();
        EXPECT_EQ(instance->value, 0);
    }
}