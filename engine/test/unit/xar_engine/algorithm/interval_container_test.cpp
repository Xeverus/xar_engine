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
           add_case_1)
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

    TEST_F(IntervalCollectionTest,
           add_case_2)
    {
        IntervalCollectionType interval_collection;

        interval_collection.add({10, 20});
        EXPECT_EQ(interval_collection.to_vector(), (std::vector<IntervalType>{
            {10, 20},
        }));

        interval_collection.add({5, 25});
        EXPECT_EQ(interval_collection.to_vector(), (std::vector<IntervalType>{
            {5, 25},
        }));
    }

    TEST_F(IntervalCollectionTest,
           add_case_3)
    {
        IntervalCollectionType interval_collection;

        interval_collection.add({5, 25});
        EXPECT_EQ(interval_collection.to_vector(), (std::vector<IntervalType>{
            {5, 25},
        }));

        interval_collection.add({10, 20});
        EXPECT_EQ(interval_collection.to_vector(), (std::vector<IntervalType>{
            {5, 25},
        }));
    }

    TEST_F(IntervalCollectionTest,
           add_case_4)
    {
        IntervalCollectionType interval_collection;

        interval_collection.add({10, 20});
        EXPECT_EQ(interval_collection.to_vector(), (std::vector<IntervalType>{
            {10, 20},
        }));

        interval_collection.add({5, 15});
        EXPECT_EQ(interval_collection.to_vector(), (std::vector<IntervalType>{
            {5, 20},
        }));
    }

    TEST_F(IntervalCollectionTest,
           add_case_5)
    {
        IntervalCollectionType interval_collection;

        interval_collection.add({10, 20});
        EXPECT_EQ(interval_collection.to_vector(), (std::vector<IntervalType>{
            {10, 20},
        }));

        interval_collection.add({15, 25});
        EXPECT_EQ(interval_collection.to_vector(), (std::vector<IntervalType>{
            {10, 25},
        }));
    }

    TEST_F(IntervalCollectionTest,
           add_case_6)
    {
        IntervalCollectionType interval_collection;

        interval_collection.add({10, 20});
        EXPECT_EQ(interval_collection.to_vector(), (std::vector<IntervalType>{
            {10, 20},
        }));

        interval_collection.add({20, 30});
        EXPECT_EQ(interval_collection.to_vector(), (std::vector<IntervalType>{
            {10, 30},
        }));
    }

    TEST_F(IntervalCollectionTest,
           add_case_7)
    {
        IntervalCollectionType interval_collection;

        interval_collection.add({10, 20});
        interval_collection.add({30, 40});
        interval_collection.add({50, 60});
        EXPECT_EQ(interval_collection.to_vector(), (std::vector<IntervalType>{
            {10, 20}, {30, 40}, {50, 60}
        }));

        interval_collection.add({0, 70});
        EXPECT_EQ(interval_collection.to_vector(), (std::vector<IntervalType>{
            {0, 70},
        }));
    }

    TEST_F(IntervalCollectionTest,
           add_case_8)
    {
        IntervalCollectionType interval_collection;

        interval_collection.add({10, 20});
        interval_collection.add({30, 40});
        interval_collection.add({50, 60});
        interval_collection.add({70, 80});
        EXPECT_EQ(interval_collection.to_vector(), (std::vector<IntervalType>{
            {10, 20}, {30, 40}, {50, 60}, {70, 80}
        }));

        interval_collection.add({25, 65});
        EXPECT_EQ(interval_collection.to_vector(), (std::vector<IntervalType>{
            {10, 20}, {25, 65}, {70, 80}
        }));
    }
}
