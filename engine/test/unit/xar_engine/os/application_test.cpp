#include <gtest/gtest.h>

#include <xar_engine/os/application.hpp>

namespace
{
    TEST(application, make__application_created)
    {
        const auto application = xar_engine::os::ApplicationFactory::make();
        EXPECT_NE(application, nullptr);
    }

    TEST(application, run_without_windows_and_request_close__application_closes)
    {
        const auto application = xar_engine::os::ApplicationFactory::make();
        ASSERT_NE(application, nullptr);

        application->set_on_update([&application](){
            application->request_close();
        });

        EXPECT_FALSE(application->close_requested());

        application->run();

        EXPECT_TRUE(application->close_requested());
    }

    TEST(application, run_with_one_window_and_request_close__application_closes)
    {
        const auto application = xar_engine::os::ApplicationFactory::make();
        ASSERT_NE(application, nullptr);

        const auto window = application->make_window();
        ASSERT_NE(window, nullptr);

        window->set_on_update([&application](){
            application->request_close();
        });

        EXPECT_FALSE(window->close_requested());
        EXPECT_FALSE(application->close_requested());

        application->run();

        EXPECT_TRUE(window->close_requested());
        EXPECT_TRUE(application->close_requested());
    }

    TEST(application, run_with_two_windows_and_request_close_after_few_frames__application_closes)
    {
        const auto application = xar_engine::os::ApplicationFactory::make();
        ASSERT_NE(application, nullptr);

        const auto window1 = application->make_window();
        ASSERT_NE(window1, nullptr);

        const auto window2 = application->make_window();
        ASSERT_NE(window2, nullptr);

        /*
         * Counter is not divisible by number of windows to avoid easy case.
         */
        constexpr int expected_counter = -1;
        int counter = 5;
        const auto on_update = [&application, &counter](){
            --counter;
            if (counter == 0)
            {
                application->request_close();
            }
        };

        window1->set_on_update(on_update);
        window2->set_on_update(on_update);

        EXPECT_FALSE(window1->close_requested());
        EXPECT_FALSE(window2->close_requested());
        EXPECT_FALSE(application->close_requested());

        application->run();

        EXPECT_EQ(counter, expected_counter);
        EXPECT_TRUE(window1->close_requested());
        EXPECT_TRUE(window2->close_requested());
        EXPECT_TRUE(application->close_requested());
    }
}