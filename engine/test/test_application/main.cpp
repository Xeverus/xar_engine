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
                [](const xar_engine::os::KeyboardKeyEvent& key_event)
                {
                    std::cout << static_cast<int>(key_event.code) << ' ' << static_cast<int>(key_event.state)
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
    window->set_on_close(on_close);

    window.reset();

    application->run();

    return 0;
}