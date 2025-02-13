#include <xar_engine/graphics/vulkan_test.hpp>

#include <xar_engine/logging/console_logger.hpp>
#include <xar_engine/logging/logger.hpp>

#include <xar_engine/meta/overloaded.hpp>

#include <xar_engine/os/application.hpp>

#include <iostream>


int main()
{
    const auto logger = std::make_unique<xar_engine::logging::ConsoleLogger>();
    const auto application = xar_engine::os::ApplicationFactory::make({"Test Application"});
    auto window = application->make_window({"Test Application"});

    const auto on_keyboard_event = [&](const xar_engine::input::KeyboardEvent& event)
    {
        std::visit(
            xar_engine::meta::Overloaded{
                [&](const xar_engine::input::KeyboardKeyEvent& event)
                {
                    XAR_LOG(
                        xar_engine::logging::LogLevel::DEBUG,
                        *logger,
                        "TestApp",
                        "key code={}, state={}",
                        xar_engine::meta::enum_to_string(event.code),
                        xar_engine::meta::enum_to_string(event.state));
                }
            },
            event);
    };

    const auto on_mouse_event = [&](const xar_engine::input::MouseEvent& event)
    {
        std::visit(
            xar_engine::meta::Overloaded{
                [&](const xar_engine::input::MouseButtonEvent& event)
                {
                    XAR_LOG(
                        xar_engine::logging::LogLevel::DEBUG,
                        *logger,
                        "TestApp",
                        "button code={}, state={}",
                        xar_engine::meta::enum_to_string(event.code),
                        xar_engine::meta::enum_to_string(event.state));
                },
                [&](const xar_engine::input::MouseMotionEvent& event)
                {
                    XAR_LOG(
                        xar_engine::logging::LogLevel::INFO,
                        *logger,
                        "TestApp",
                        "motion x={}, y={}",
                        event.position_x,
                        event.position_y);
                },
                [&](const xar_engine::input::MouseScrollEvent& event)
                {
                    XAR_LOG(
                        xar_engine::logging::LogLevel::WARNING,
                        *logger,
                        "TestApp",
                        "scroll x={}, y={}",
                        event.delta_x,
                        event.delta_y);
                }
            },
            event);
    };

    const auto on_window_close = [&application]()
    {
        application->request_close();
    };

    window->set_on_keyboard_event(on_keyboard_event);
    window->set_on_mouse_event(on_mouse_event);
    window->set_on_close(on_window_close);

    const auto on_application_run = []()
    {
       xar_engine::graphics::run_vulkan_test();
    };

    application->set_on_run(on_application_run);

    application->run();

    return 0;
}