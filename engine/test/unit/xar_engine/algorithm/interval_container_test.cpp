#include <gtest/gtest.h>

#include <xar_engine/algorithm/interval_container.hpp>


namespace
{
    using IntervalCollectionType = xar_engine::algorithm::TIntervalCollection<int>;
    using IntervalType = IntervalCollectionType::IntervalType;
}


namespace xar_engine::algorithm
{
    bool operator==(
        const xar_engine::algorithm::TInterval<int>& left,
        const xar_engine::algorithm::TInterval<int>& right)
    {
        return left.begin == right.begin && left.end == right.end;
    }
}


namespace
{
    class IntervalCollectionTest
        : public testing::Test
    {
    };

    TEST_F(IntervalCollectionTest,
           contains__empty_container__false)
    {
        IntervalCollectionType interval_collection;
        EXPECT_FALSE(interval_collection.contains(0));
    }

    TEST_F(IntervalCollectionTest,
           contains__filled_container__correct_result)
    {
        IntervalCollectionType interval_collection;
        interval_collection.add({0, 10});

        EXPECT_FALSE(interval_collection.contains(-1));
        EXPECT_TRUE(interval_collection.contains(0));
        EXPECT_TRUE(interval_collection.contains(10));
        EXPECT_FALSE(interval_collection.contains(11));
    }

    TEST_F(IntervalCollectionTest,
           to_vector__empty_container__empty_result)
    {
        IntervalCollectionType interval_collection;

        EXPECT_EQ(interval_collection.to_vector().size(), 0);
    }

    TEST_F(IntervalCollectionTest,
           to_vector__filled_container__correct_result)
    {
        IntervalCollectionType interval_collection;
        interval_collection.add({10, 20});
        interval_collection.add({0, 5});
        interval_collection.add({30, 35});

        const auto result = interval_collection.to_vector();
        ASSERT_EQ(result.size(), 3);
        EXPECT_EQ(result, (std::vector<IntervalType>{
            {0, 5}, {10, 20}, {30, 35},
        }));
    }

    TEST_F(IntervalCollectionTest,
           add)
    {
        IntervalCollectionType interval_collection;

        interval_collection.add({0, 5});
        EXPECT_EQ(interval_collection.to_vector(), (std::vector<IntervalType>{
            {0, 5},
        }));

        interval_collection.add({10, 15});
        EXPECT_EQ(interval_collection.to_vector(), (std::vector<IntervalType>{
            {0, 5}, {10, 15},
        }));

        interval_collection.add({2, 8});
        EXPECT_EQ(interval_collection.to_vector(), (std::vector<IntervalType>{
            {0, 8}, {10, 15},
        }));

        interval_collection.add({9, 12});
        EXPECT_EQ(interval_collection.to_vector(), (std::vector<IntervalType>{
            {0, 8}, {9, 15},
        }));

        interval_collection.add({8, 9});
        EXPECT_EQ(interval_collection.to_vector(), (std::vector<IntervalType>{
            {0, 15},
        }));
    }
}
