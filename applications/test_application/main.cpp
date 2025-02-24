#include <xar_engine/logging/console_logger.hpp>
#include <xar_engine/logging/logger.hpp>

#include <xar_engine/meta/overloaded.hpp>

#include <xar_engine/os/application.hpp>


int main()
{
    const auto application = xar_engine::os::ApplicationFactory().make({"Test Application"});
    auto window = application->make_window({"Test Application"});
    auto renderer = window->make_renderer(xar_engine::graphics::RendererType::VULKAN);

    window->set_on_update(
        [&]()
        {
            renderer->update();
        });

    window->set_on_resize_event(
        [](
            const std::int32_t new_width,
            const std::int32_t new_height)
        {
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
                            "TestApp",
                            "button code={}, state={}",
                            xar_engine::meta::enum_to_string(event.code),
                            xar_engine::meta::enum_to_string(event.state));
                    },
                    [&](const xar_engine::input::MouseMotionEvent& event)
                    {
                        XAR_LOG(
                            xar_engine::logging::LogLevel::INFO,
                            "TestApp",
                            "motion x={}, y={}",
                            event.position_x,
                            event.position_y);
                    },
                    [&](const xar_engine::input::MouseScrollEvent& event)
                    {
                        XAR_LOG(
                            xar_engine::logging::LogLevel::WARNING,
                            "TestApp",
                            "scroll x={}, y={}",
                            event.delta_x,
                            event.delta_y);
                    }
                },
                event);
        });

    window->set_on_close(
        [&]()
        {
            application->request_close();
        });

    application->run();

    return 0;
}