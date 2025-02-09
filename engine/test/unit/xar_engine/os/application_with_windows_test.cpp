#include <gtest/gtest.h>

#include <xar_engine/os/application.hpp>

namespace
{
    TEST(application,
         lifecycle__no_windows)
    {
        const auto application = xar_engine::os::ApplicationFactory::make();
        ASSERT_NE(application,
                  nullptr);

        constexpr std::int32_t expected_update_counter = 5;
        constexpr std::int32_t expected_on_close_counter = 1;

        std::int32_t application_update_counter = 0;
        application->set_on_update(
            [&application_update_counter, &application]()
            {
                ++application_update_counter;
                if (application_update_counter == expected_update_counter)
                {
                    application->request_close();
                }
            });

        std::int32_t application_on_close_counter = 0;
        application->set_on_close(
            [&application_on_close_counter]()
            {
                ++application_on_close_counter;
            });

        application->run();

        EXPECT_EQ(application_update_counter,
                  expected_update_counter);
        EXPECT_EQ(application_on_close_counter,
                  expected_on_close_counter);
    }

    TEST(application,
         lifecycle__with_single_window)
    {
        const auto application = xar_engine::os::ApplicationFactory::make();
        ASSERT_NE(application,
                  nullptr);

        const auto window = application->make_window();
        ASSERT_NE(window,
                  nullptr);

        constexpr std::int32_t expected_update_counter = 5;
        constexpr std::int32_t expected_on_close_counter = 1;

        std::int32_t application_update_counter = 0;
        application->set_on_update(
            [&application_update_counter, &application]()
            {
                ++application_update_counter;
                if (application_update_counter == expected_update_counter)
                {
                    application->request_close();
                }
            });

        std::int32_t application_on_close_counter = 0;
        application->set_on_close(
            [&application_on_close_counter]()
            {
                ++application_on_close_counter;
            });

        std::int32_t window_update_counter = 0;
        window->set_on_update(
            [&window_update_counter]()
            {
                ++window_update_counter;
            });

        std::int32_t window_on_close_counter = 0;
        window->set_on_close(
            [&window_on_close_counter]()
            {
                ++window_on_close_counter;
            });

        application->run();

        EXPECT_EQ(application_update_counter,
                  expected_update_counter);
        EXPECT_EQ(window_update_counter,
                  expected_update_counter);

        EXPECT_EQ(application_on_close_counter,
                  expected_on_close_counter);
        EXPECT_EQ(window_on_close_counter,
                  expected_on_close_counter);
    }

    TEST(application,
         lifecycle__with_three_windows)
    {
        struct WindowWithCounters
        {
            std::shared_ptr<xar_engine::os::IWindow> window;
            std::int32_t on_update_counter;
            std::int32_t on_close_counter;
        };

        const auto application = xar_engine::os::ApplicationFactory::make();
        ASSERT_NE(application,
                  nullptr);

        auto windows_with_counters = std::vector<WindowWithCounters>{
            {application->make_window(), 0, 0},
            {application->make_window(), 0, 0},
            {application->make_window(), 0, 0},
        };
        for (const auto& window_with_counters: windows_with_counters)
        {
            ASSERT_NE(window_with_counters.window,
                      nullptr);
        }

        constexpr std::int32_t expected_update_counter = 5;
        constexpr std::int32_t expected_on_close_counter = 1;

        std::int32_t application_update_counter = 0;
        application->set_on_update(
            [&application_update_counter, &application]()
            {
                ++application_update_counter;
                if (application_update_counter == expected_update_counter)
                {
                    application->request_close();
                }
            });

        std::int32_t application_on_close_counter = 0;
        application->set_on_close(
            [&application_on_close_counter]()
            {
                ++application_on_close_counter;
            });

        for (auto& window_with_counters: windows_with_counters)
        {
            window_with_counters.window->set_on_update(
                [&window_with_counters]()
                {
                    ++window_with_counters.on_update_counter;
                });
            window_with_counters.window->set_on_close(
                [&window_with_counters]()
                {
                    ++window_with_counters.on_close_counter;
                });
        }

        application->run();

        EXPECT_EQ(application_update_counter,
                  expected_update_counter);
        EXPECT_EQ(application_on_close_counter,
                  expected_on_close_counter);

        for (auto& window_with_counters: windows_with_counters)
        {
            EXPECT_EQ(window_with_counters.on_update_counter,
                      expected_update_counter);
            EXPECT_EQ(window_with_counters.on_close_counter,
                      expected_on_close_counter);
        }
    }

    TEST(application,
         lifecycle__single_window_recreated)
    {
        const auto application = xar_engine::os::ApplicationFactory::make();
        ASSERT_NE(application,
                  nullptr);

        auto window = application->make_window();
        ASSERT_NE(window,
                  nullptr);

        decltype(window) recreated_window = nullptr;

        constexpr std::int32_t expected_application_update_counter = 9;
        constexpr std::int32_t expected_on_close_counter = 1;
        constexpr std::int32_t recreate_window_in_update_number = 5;

        constexpr std::int32_t expected_window_update_counter = 4;
        constexpr std::int32_t expected_recreate_window_update_counter = 5;

        std::int32_t window_update_counter = 0;
        std::int32_t recreated_window_update_counter = 0;

        std::int32_t window_on_close_counter = 0;
        std::int32_t recreated_window_on_close_counter = 0;

        std::int32_t application_update_counter = 0;
        application->set_on_update(
            [&]()
            {
                ++application_update_counter;

                if (application_update_counter != recreate_window_in_update_number)
                {
                    return;
                }

                window->request_close();
                window.reset();

                recreated_window = application->make_window();
                recreated_window->set_on_update(
                    [&]()
                    {
                        ++recreated_window_update_counter;
                        if (recreated_window_update_counter == expected_recreate_window_update_counter)
                        {
                            application->request_close();
                        }
                    });
                recreated_window->set_on_close(
                    [&recreated_window_on_close_counter]()
                    {
                        ++recreated_window_on_close_counter;
                    });
            });

        std::int32_t application_on_close_counter = 0;
        application->set_on_close(
            [&application_on_close_counter]()
            {
                ++application_on_close_counter;
            });

        window->set_on_update(
            [&window_update_counter]()
            {
                ++window_update_counter;
            });
        window->set_on_close(
            [&window_on_close_counter]()
            {
                ++window_on_close_counter;
            });

        application->run();

        EXPECT_EQ(application_update_counter,
                  expected_application_update_counter);
        EXPECT_EQ(window_update_counter,
                  expected_window_update_counter);
        EXPECT_EQ(recreated_window_update_counter,
                  expected_recreate_window_update_counter);

        EXPECT_EQ(application_on_close_counter,
                  expected_on_close_counter);
        EXPECT_EQ(window_on_close_counter,
                  expected_on_close_counter);
        EXPECT_EQ(recreated_window_on_close_counter,
                  expected_on_close_counter);
    }
}