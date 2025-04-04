#include <gtest/gtest.h>

#include <xar_engine/algorithm/interval_container.hpp>


namespace
{
    class IntervalCollectionTest : testing::Test
    {
    };

    TEST(IntervalCollectionTest,
         basic)
    {
        xar_engine::algorithm::TIntervalCollection<int> interval_collection;
    }
}
