#include <gtest/gtest.h>

#include <xar_engine/meta/overloaded.hpp>

#include <xar_engine/os/application.hpp>

namespace
{
    TEST(application,
         input__keyboard)
    {
        const auto application = xar_engine::os::ApplicationFactory::make();
        ASSERT_NE(application,
                  nullptr);

        const auto window = application->make_window();
        ASSERT_NE(window,
                  nullptr);

        std::int32_t update_counter = 0;

        using KeyboardEventVector = std::vector<xar_engine::os::KeyboardEvent>;
        const auto expected_update_events_lists = std::vector<KeyboardEventVector>{
            {
                xar_engine::os::KeyboardKeyEvent{
                    xar_engine::os::ButtonCode::B, xar_engine::os::ButtonState::DOWN
                },
                xar_engine::os::KeyboardKeyEvent{
                    xar_engine::os::ButtonCode::C, xar_engine::os::ButtonState::DOWN
                },
                xar_engine::os::KeyboardKeyEvent{
                    xar_engine::os::ButtonCode::C, xar_engine::os::ButtonState::UP
                },
                xar_engine::os::KeyboardKeyEvent{
                    xar_engine::os::ButtonCode::D, xar_engine::os::ButtonState::DOWN
                },
            },
            {
                xar_engine::os::KeyboardKeyEvent{
                    xar_engine::os::ButtonCode::D, xar_engine::os::ButtonState::UP
                },
                xar_engine::os::KeyboardKeyEvent{
                    xar_engine::os::ButtonCode::C, xar_engine::os::ButtonState::UP
                },
            },
        };
        auto update_events_lists = std::vector<KeyboardEventVector>{
            {},
            {},
        };

        application->set_on_update(
            [&]()
            {
                if (update_counter >= expected_update_events_lists.size())
                {
                    application->request_close();
                    return;
                }

                for (const auto& event: expected_update_events_lists[update_counter])
                {
                    window->enqueue_keyboard_event(event);
                }

                ++update_counter;
            });

        window->set_on_keyboard_event(
            [&](const xar_engine::os::KeyboardEvent& keyboard_event)
            {
                std::visit(
                    xar_engine::meta::Overloaded{
                        [&](const xar_engine::os::KeyboardKeyEvent& event)
                        {
                            const auto idx = update_counter - 1;
                            ASSERT_LE(idx,
                                      update_events_lists.size());

                            update_events_lists[idx].emplace_back(event);
                        }
                    },
                    keyboard_event);
            });

        application->run();

        EXPECT_EQ(update_events_lists,
                  expected_update_events_lists);
    }

    TEST(application,
         input__mouse)
    {
        const auto application = xar_engine::os::ApplicationFactory::make();
        ASSERT_NE(application,
                  nullptr);

        const auto window = application->make_window();
        ASSERT_NE(window,
                  nullptr);

        std::int32_t update_counter = 0;

        using MouseEventVector = std::vector<xar_engine::os::MouseEvent>;
        const auto expected_update_events_lists = std::vector<MouseEventVector>{
            {
                xar_engine::os::MouseButtonEvent{
                    xar_engine::os::ButtonCode::MOUSE_0, xar_engine::os::ButtonState::DOWN
                },
                xar_engine::os::MouseMotionEvent{10, 20},
                xar_engine::os::MouseButtonEvent{
                    xar_engine::os::ButtonCode::MOUSE_1, xar_engine::os::ButtonState::DOWN
                },
                xar_engine::os::MouseMotionEvent{30, 60},
                xar_engine::os::MouseScrollEvent{1, 1},
                xar_engine::os::MouseButtonEvent{
                    xar_engine::os::ButtonCode::MOUSE_0, xar_engine::os::ButtonState::UP
                },
            },
            {
                xar_engine::os::MouseButtonEvent{
                    xar_engine::os::ButtonCode::MOUSE_1, xar_engine::os::ButtonState::UP
                },
                xar_engine::os::MouseScrollEvent{1, 1},
                xar_engine::os::MouseMotionEvent{30, 60},
            },
        };
        auto update_events_lists = std::vector<MouseEventVector>{
            {},
            {},
        };

        window->set_on_mouse_event(
            [&](const xar_engine::os::MouseEvent& mouse_event)
            {
                std::visit(
                    xar_engine::meta::Overloaded{
                        [&](const xar_engine::os::MouseButtonEvent& event)
                        {
                            const auto idx = update_counter - 1;
                            ASSERT_LE(idx,
                                      update_events_lists.size());

                            update_events_lists[idx].emplace_back(event);
                        },
                        [&](const xar_engine::os::MouseMotionEvent& event)
                        {
                            const auto idx = update_counter - 1;
                            ASSERT_LE(idx,
                                      update_events_lists.size());

                            update_events_lists[idx].emplace_back(event);
                        },
                        [&](const xar_engine::os::MouseScrollEvent& event)
                        {
                            const auto idx = update_counter - 1;
                            ASSERT_LE(idx,
                                      update_events_lists.size());

                            update_events_lists[idx].emplace_back(event);
                        }
                    },
                    mouse_event);
            });

        application->set_on_update(
            [&]()
            {
                if (update_counter >= expected_update_events_lists.size())
                {
                    application->request_close();
                    return;
                }

                for (const auto& event: expected_update_events_lists[update_counter])
                {
                    window->enqueue_mouse_event(event);
                }

                ++update_counter;
            });

        application->run();

        EXPECT_EQ(update_events_lists,
                  expected_update_events_lists);
    }
}