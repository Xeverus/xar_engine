#include <fmt/core.h>

#include <gtest/gtest.h>

#include <xar_engine/meta/enum_impl.hpp>

#include <xar_engine/os/application.hpp>

enum class ActionType
{
    APPLICATION_ON_RUN,
    APPLICATION_ON_UPDATE,
    APPLICATION_ON_CLOSE,

    WINDOW_ON_RUN,
    WINDOW_ON_UPDATE,
    WINDOW_ON_CLOSE,
};

ENUM_TO_STRING(ActionType);
ENUM_TO_STRING_IMPL(ActionType,
                    ActionType::APPLICATION_ON_RUN,
                    ActionType::APPLICATION_ON_UPDATE,
                    ActionType::APPLICATION_ON_CLOSE,
                    ActionType::WINDOW_ON_RUN,
                    ActionType::WINDOW_ON_UPDATE,
                    ActionType::WINDOW_ON_CLOSE);

struct ActionWithId
{
    ActionType action;
    std::string id;

    bool operator==(const ActionWithId& other) const
    {
        return action == other.action && id == other.id;
    }
};

std::ostream& operator<<(
    std::ostream& stream,
    const ActionWithId& action_with_id)
{
    return stream << fmt::format(
        "{{{},{}}}",
        xar_engine::meta::enum_to_short_string(action_with_id.action),
        action_with_id.id);
}

namespace
{
    TEST(application,
         lifecycle__no_windows)
    {
        const auto application = xar_engine::os::ApplicationFactory::make();
        ASSERT_NE(application,
                  nullptr);

        constexpr auto expected_update_counter = std::int32_t{5};

        const auto expected_call_order = std::vector<ActionType>{
            ActionType::APPLICATION_ON_RUN,
            ActionType::APPLICATION_ON_UPDATE,
            ActionType::APPLICATION_ON_UPDATE,
            ActionType::APPLICATION_ON_UPDATE,
            ActionType::APPLICATION_ON_UPDATE,
            ActionType::APPLICATION_ON_UPDATE,
            ActionType::APPLICATION_ON_CLOSE
        };
        auto actual_call_order = std::vector<ActionType>{};

        application->set_on_run(
            [&]()
            {
                actual_call_order.push_back(ActionType::APPLICATION_ON_RUN);
            });

        auto application_update_counter = std::int32_t{0};
        application->set_on_update(
            [&]()
            {
                actual_call_order.push_back(ActionType::APPLICATION_ON_UPDATE);

                ++application_update_counter;
                if (application_update_counter == expected_update_counter)
                {
                    application->request_close();
                }
            });

        application->set_on_close(
            [&]()
            {
                actual_call_order.push_back(ActionType::APPLICATION_ON_CLOSE);
            });

        application->run();

        EXPECT_EQ(expected_call_order,
                  actual_call_order);
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

        constexpr auto expected_update_counter = std::int32_t{5};

        const auto expected_call_order = std::vector<ActionType>{
            ActionType::APPLICATION_ON_RUN,
            ActionType::APPLICATION_ON_UPDATE,
            ActionType::WINDOW_ON_RUN,
            ActionType::WINDOW_ON_UPDATE,
            ActionType::APPLICATION_ON_UPDATE,
            ActionType::WINDOW_ON_UPDATE,
            ActionType::APPLICATION_ON_UPDATE,
            ActionType::WINDOW_ON_UPDATE,
            ActionType::APPLICATION_ON_UPDATE,
            ActionType::WINDOW_ON_UPDATE,
            ActionType::APPLICATION_ON_UPDATE,
            ActionType::WINDOW_ON_UPDATE,
            ActionType::WINDOW_ON_CLOSE,
            ActionType::APPLICATION_ON_CLOSE,
        };
        auto actual_call_order = std::vector<ActionType>{};

        application->set_on_run(
            [&]()
            {
                actual_call_order.push_back(ActionType::APPLICATION_ON_RUN);
            });

        std::int32_t application_update_counter = 0;
        application->set_on_update(
            [&]()
            {
                actual_call_order.push_back(ActionType::APPLICATION_ON_UPDATE);
                ++application_update_counter;
                if (application_update_counter == expected_update_counter)
                {
                    application->request_close();
                }
            });

        application->set_on_close(
            [&]()
            {
                actual_call_order.push_back(ActionType::APPLICATION_ON_CLOSE);
            });

        window->set_on_run(
            [&]()
            {
                actual_call_order.push_back(ActionType::WINDOW_ON_RUN);
            });

        window->set_on_update(
            [&]()
            {
                actual_call_order.push_back(ActionType::WINDOW_ON_UPDATE);
            });

        window->set_on_close(
            [&]()
            {
                actual_call_order.push_back(ActionType::WINDOW_ON_CLOSE);
            });

        application->run();

        EXPECT_EQ(expected_call_order,
                  actual_call_order);
    }

    TEST(application,
         lifecycle__with_three_windows)
    {
        const auto application = xar_engine::os::ApplicationFactory::make();
        ASSERT_NE(application,
                  nullptr);

        const auto windows = std::vector<std::shared_ptr<xar_engine::os::IWindow>>{
            application->make_window(),
            application->make_window(),
            application->make_window(),
        };
        for (const auto& window: windows)
        {
            ASSERT_NE(window,
                      nullptr);
        }

        constexpr auto expected_update_counter = std::int32_t{5};
        const auto app_id = std::string{"application"};
        const auto window_id_prefix = std::string{"window_"};
        const auto expected_call_order = std::vector<ActionWithId>{
            {ActionType::APPLICATION_ON_RUN,    app_id},
            {ActionType::APPLICATION_ON_UPDATE, app_id},
            {ActionType::WINDOW_ON_RUN,         window_id_prefix + "0"},
            {ActionType::WINDOW_ON_RUN,         window_id_prefix + "1"},
            {ActionType::WINDOW_ON_RUN,         window_id_prefix + "2"},
            {ActionType::WINDOW_ON_UPDATE,      window_id_prefix + "0"},
            {ActionType::WINDOW_ON_UPDATE,      window_id_prefix + "1"},
            {ActionType::WINDOW_ON_UPDATE,      window_id_prefix + "2"},
            {ActionType::APPLICATION_ON_UPDATE, app_id},
            {ActionType::WINDOW_ON_UPDATE,      window_id_prefix + "0"},
            {ActionType::WINDOW_ON_UPDATE,      window_id_prefix + "1"},
            {ActionType::WINDOW_ON_UPDATE,      window_id_prefix + "2"},
            {ActionType::APPLICATION_ON_UPDATE, app_id},
            {ActionType::WINDOW_ON_UPDATE,      window_id_prefix + "0"},
            {ActionType::WINDOW_ON_UPDATE,      window_id_prefix + "1"},
            {ActionType::WINDOW_ON_UPDATE,      window_id_prefix + "2"},
            {ActionType::APPLICATION_ON_UPDATE, app_id},
            {ActionType::WINDOW_ON_UPDATE,      window_id_prefix + "0"},
            {ActionType::WINDOW_ON_UPDATE,      window_id_prefix + "1"},
            {ActionType::WINDOW_ON_UPDATE,      window_id_prefix + "2"},
            {ActionType::APPLICATION_ON_UPDATE, app_id},
            {ActionType::WINDOW_ON_UPDATE,      window_id_prefix + "0"},
            {ActionType::WINDOW_ON_UPDATE,      window_id_prefix + "1"},
            {ActionType::WINDOW_ON_UPDATE,      window_id_prefix + "2"},
            {ActionType::WINDOW_ON_CLOSE,       window_id_prefix + "0"},
            {ActionType::WINDOW_ON_CLOSE,       window_id_prefix + "1"},
            {ActionType::WINDOW_ON_CLOSE,       window_id_prefix + "2"},
            {ActionType::APPLICATION_ON_CLOSE,  app_id},
        };
        auto actual_call_order = std::vector<ActionWithId>{};

        application->set_on_run(
            [&]()
            {
                actual_call_order.push_back({ActionType::APPLICATION_ON_RUN, app_id});
            });

        std::int32_t application_update_counter = 0;
        application->set_on_update(
            [&]()
            {
                actual_call_order.push_back({ActionType::APPLICATION_ON_UPDATE, app_id});
                ++application_update_counter;
                if (application_update_counter == expected_update_counter)
                {
                    application->request_close();
                }
            });

        application->set_on_close(
            [&]()
            {
                actual_call_order.push_back({ActionType::APPLICATION_ON_CLOSE, app_id});
            });

        for (auto i = 0; i < windows.size(); ++i)
        {
            const auto window_id = window_id_prefix + std::to_string(i);
            windows[i]->set_on_run(
                [&, window_id]()
                {
                    actual_call_order.push_back({ActionType::WINDOW_ON_RUN, window_id});
                });
            windows[i]->set_on_update(
                [&, window_id]()
                {
                    actual_call_order.push_back({ActionType::WINDOW_ON_UPDATE, window_id});
                });
            windows[i]->set_on_close(
                [&, window_id]()
                {
                    actual_call_order.push_back({ActionType::WINDOW_ON_CLOSE, window_id});
                });
        }

        application->run();

        EXPECT_EQ(expected_call_order,
                  actual_call_order);
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

        constexpr auto expected_update_counter = std::int32_t{5};
        constexpr auto recreate_in_frame = expected_update_counter;
        const auto app_id = std::string{"application"};
        const auto original_window_id = std::string{"window_original"};
        const auto recreated_window_id = std::string{"window_recreated"};
        const auto expected_call_order = std::vector<ActionWithId>{
            {ActionType::APPLICATION_ON_RUN,    app_id},
            {ActionType::APPLICATION_ON_UPDATE, app_id},
            {ActionType::WINDOW_ON_RUN,         original_window_id},
            {ActionType::WINDOW_ON_UPDATE,      original_window_id},
            {ActionType::APPLICATION_ON_UPDATE, app_id},
            {ActionType::WINDOW_ON_UPDATE,      original_window_id},
            {ActionType::APPLICATION_ON_UPDATE, app_id},
            {ActionType::WINDOW_ON_UPDATE,      original_window_id},
            {ActionType::APPLICATION_ON_UPDATE, app_id},
            {ActionType::WINDOW_ON_UPDATE,      original_window_id},
            {ActionType::APPLICATION_ON_UPDATE, app_id},
            {ActionType::WINDOW_ON_RUN,         recreated_window_id},
            {ActionType::WINDOW_ON_UPDATE,      recreated_window_id},
            {ActionType::WINDOW_ON_CLOSE,       original_window_id},
            {ActionType::APPLICATION_ON_UPDATE, app_id},
            {ActionType::WINDOW_ON_UPDATE,      recreated_window_id},
            {ActionType::APPLICATION_ON_UPDATE, app_id},
            {ActionType::WINDOW_ON_UPDATE,      recreated_window_id},
            {ActionType::APPLICATION_ON_UPDATE, app_id},
            {ActionType::WINDOW_ON_UPDATE,      recreated_window_id},
            {ActionType::APPLICATION_ON_UPDATE, app_id},
            {ActionType::WINDOW_ON_UPDATE,      recreated_window_id},
            {ActionType::APPLICATION_ON_UPDATE, app_id},
            {ActionType::WINDOW_ON_CLOSE,       recreated_window_id},
            {ActionType::APPLICATION_ON_CLOSE,  app_id},
        };
        auto actual_call_order = std::vector<ActionWithId>{};

        application->set_on_run(
            [&]()
            {
                actual_call_order.push_back({ActionType::APPLICATION_ON_RUN, app_id});
            });

        auto update_counter = std::int32_t{0};
        application->set_on_update(
            [&]()
            {
                actual_call_order.push_back({ActionType::APPLICATION_ON_UPDATE, app_id});

                ++update_counter;
                if (expected_update_counter * 2 == update_counter)
                {
                    window->request_close();
                    application->request_close();
                }
                if (recreate_in_frame != update_counter)
                {
                    return;
                }

                window->request_close();
                window = application->make_window();
                window->set_on_run(
                    [&]()
                    {
                        actual_call_order.push_back({ActionType::WINDOW_ON_RUN, recreated_window_id});
                    });
                window->set_on_update(
                    [&]()
                    {
                        actual_call_order.push_back({ActionType::WINDOW_ON_UPDATE, recreated_window_id});
                    });
                window->set_on_close(
                    [&]()
                    {
                        actual_call_order.push_back({ActionType::WINDOW_ON_CLOSE, recreated_window_id});
                    });
            });

        application->set_on_close(
            [&]()
            {
                actual_call_order.push_back({ActionType::APPLICATION_ON_CLOSE, app_id});
            });

        window->set_on_run(
            [&]()
            {
                actual_call_order.push_back({ActionType::WINDOW_ON_RUN, original_window_id});
            });
        window->set_on_update(
            [&]()
            {
                actual_call_order.push_back({ActionType::WINDOW_ON_UPDATE, original_window_id});
            });
        window->set_on_close(
            [&]()
            {
                actual_call_order.push_back({ActionType::WINDOW_ON_CLOSE, original_window_id});
            });

        application->run();

        EXPECT_EQ(expected_call_order,
                  actual_call_order);
    }
}