#include <xar_engine/os/application.hpp>
#include <xar_engine/meta/overloaded.hpp>

#include <iostream>

int main()
{
    const auto application = xar_engine::os::ApplicationFactory::make();
    auto window = application->make_window();

    const auto on_keyboard_event = [](const xar_engine::os::KeyboardEvent& event)
    {
        std::visit(
            xar_engine::meta::Overloaded{
                [](const xar_engine::os::KeyboardKeyEvent& event)
                {
                    std::cout << static_cast<int>(event.code) << ' ' << static_cast<int>(event.state)
                              << std::endl;
                }
            },
            event);
    };

    const auto on_mouse_event = [](const xar_engine::os::MouseEvent& event)
    {
        std::visit(
            xar_engine::meta::Overloaded{
                [](const xar_engine::os::MouseButtonEvent& event)
                {
                    std::cout << static_cast<int>(event.code) << ' ' << static_cast<int>(event.state)
                              << std::endl;
                },
                [](const xar_engine::os::MouseMotionEvent& event)
                {
                    std::cout << static_cast<int>(event.position_x) << ' ' << static_cast<int>(event.position_y)
                              << std::endl;
                },
                [](const xar_engine::os::MouseScrollEvent& event)
                {
                    std::cout << static_cast<int>(event.delta_x) << ' ' << static_cast<int>(event.delta_y)
                              << std::endl;
                }
            },
            event);
    };

    const auto on_close = [&application]()
    {
        application->request_close();
    };

    window->set_on_keyboard_event(on_keyboard_event);
    window->set_on_mouse_event(on_mouse_event);
    window->set_on_close(on_close);

    application->run();

    return 0;
}