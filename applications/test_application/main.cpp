#include <xar_engine/graphics/vulkan_test.hpp>

#include <xar_engine/logging/console_logger.hpp>
#include <xar_engine/logging/logger.hpp>

#include <xar_engine/meta/overloaded.hpp>

#include <xar_engine/os/application.hpp>

#include <iostream>


int main()
{
    const auto logger = std::make_unique<xar_engine::logging::ConsoleLogger>();
    const auto application = xar_engine::os::ApplicationFactory().make({"Test Application"});
    auto window = application->make_window({"Test Application"});

    window->set_on_run(
        [&]()
        {
            xar_engine::graphics::init_vulkan_test(*window);
        });

    window->set_on_update(
        [](){
            xar_engine::graphics::run_vulkan_test_frame();
        });

    window->set_on_resize_event([](const std::int32_t new_width, const std::int32_t new_height){
    });

    window->set_on_keyboard_event(
        [&](const xar_engine::input::KeyboardEvent& event)
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
        });

    window->set_on_mouse_event(
        [&](const xar_engine::input::MouseEvent& event)
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
        });

    window->set_on_close(
        [&application]()
        {
            xar_engine::graphics::cleanup_vulkan_test();
            application->request_close();
        });

    application->run();

    return 0;
}