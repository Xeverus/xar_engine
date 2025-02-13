#include <xar_engine/os/glfw_window.hpp>

#include <xar_engine/error/exception_utils.hpp>

namespace xar_engine::os
{
    namespace
    {
        const IWindow::OnRun empty_on_run = []()
        {
        };
        const IWindow::OnUpdate empty_on_update = []()
        {
        };
        const IWindow::OnClose empty_on_close = []()
        {
        };
        const IWindow::OnKeyboardEvent empty_on_keyboard_event = [](const input::KeyboardEvent&)
        {
        };
        const IWindow::OnMouseEvent empty_on_mouse_event = [](const input::MouseEvent&)
        {
        };

        GlfwWindow* get_window(GLFWwindow* const native_glfw_window)
        {
            return static_cast<GlfwWindow*>(glfwGetWindowUserPointer(native_glfw_window));
        }

        input::ButtonState action_to_button_state(const int action)
        {
            auto button_state = input::ButtonState::DOWN;
            switch (action)
            {
                case GLFW_PRESS: button_state = input::ButtonState::DOWN;
                    break;
                case GLFW_RELEASE: button_state = input::ButtonState::UP;
                    break;
                case GLFW_REPEAT: button_state = input::ButtonState::REPEAT;
                    break;
                default: XAR_THROW(
                        error::XarException,
                        "Incorrect action for keyboard button event '{}'",
                        static_cast<int>(action));
            }

            return button_state;
        }

        void glfw_key_callback(
            GLFWwindow* const native_glfw_window,
            const int key,
            const int scancode,
            const int action,
            const int mode)
        {
            auto* const glfw_window = get_window(native_glfw_window);

            const auto button_code = static_cast<input::ButtonCode>(key);
            const auto button_state = action_to_button_state(action);

            glfw_window->enqueue_keyboard_event(
                input::KeyboardKeyEvent{
                    button_code,
                    button_state,
                });
        }

        void glfw_mouse_button_callback(
            GLFWwindow* const native_glfw_window,
            const int button,
            const int action,
            const int mods)
        {
            auto* const glfw_window = get_window(native_glfw_window);

            const auto button_code = static_cast<input::ButtonCode>(button);
            const auto button_state = action_to_button_state(action);

            glfw_window->enqueue_mouse_event(
                input::MouseButtonEvent{
                    button_code,
                    button_state,
                });
        }

        void glfw_cursor_position_callback(
            GLFWwindow* const native_glfw_window,
            const double x_position,
            const double y_position)
        {
            auto* const glfw_window = get_window(native_glfw_window);

            glfw_window->enqueue_mouse_event(
                input::MouseMotionEvent{
                    static_cast<std::int32_t>(x_position),
                    static_cast<std::int32_t>(y_position),
                });
        }

        void glfw_scroll_callback(
            GLFWwindow* const native_glfw_window,
            const double x_delta,
            const double y_delta)
        {
            auto* const glfw_window = get_window(native_glfw_window);

            glfw_window->enqueue_mouse_event(
                input::MouseScrollEvent{
                    static_cast<std::int32_t>(x_delta),
                    static_cast<std::int32_t>(y_delta),
                });
        }
    }

    GlfwWindow::GlfwWindow()
        : _native_glfw_window(nullptr)
        , _on_run(empty_on_run)
        , _on_update(empty_on_update)
        , _on_close(empty_on_close)
        , _on_keyboard_event(empty_on_keyboard_event)
        , _on_mouse_event(empty_on_mouse_event)
    {
        glfwWindowHint(
            GLFW_CONTEXT_VERSION_MAJOR,
            4);
        glfwWindowHint(
            GLFW_CONTEXT_VERSION_MINOR,
            6);
        glfwWindowHint(
            GLFW_OPENGL_PROFILE,
            GLFW_OPENGL_CORE_PROFILE);

        _native_glfw_window = glfwCreateWindow(
            800,
            600,
            "GLFW App",
            nullptr,
            nullptr);
        XAR_THROW_IF(
            _native_glfw_window == nullptr,
            error::XarException,
            "Failed to make GLFW window");

        glfwSetWindowUserPointer(
            _native_glfw_window,
            this);

        glfwSetKeyCallback(
            _native_glfw_window,
            glfw_key_callback);

        glfwSetMouseButtonCallback(
            _native_glfw_window,
            glfw_mouse_button_callback);
        glfwSetCursorPosCallback(
            _native_glfw_window,
            glfw_cursor_position_callback);
        glfwSetScrollCallback(
            _native_glfw_window,
            glfw_scroll_callback);

        glfwMakeContextCurrent(_native_glfw_window);
    }

    GlfwWindow::~GlfwWindow()
    {
        if (_native_glfw_window)
        {
            glfwDestroyWindow(_native_glfw_window);
            _native_glfw_window = nullptr;
        }
    }

    void GlfwWindow::run()
    {
        _on_run();
    }

    void GlfwWindow::update()
    {
        if (close_requested())
        {
            return;
        }

        for (const auto& keyboard_event: _keyboard_event_queue)
        {
            _on_keyboard_event(keyboard_event);
        }
        _keyboard_event_queue.clear();

        for (const auto& mouse_event: _mouse_event_queue)
        {
            _on_mouse_event(mouse_event);
        }
        _mouse_event_queue.clear();

        _on_update();

        glfwSwapBuffers(_native_glfw_window);
    }

    void GlfwWindow::close()
    {
        _on_close();
    }

    void GlfwWindow::set_on_run(IWindow::OnRun&& on_run)
    {
        _on_run = std::move(on_run);
        if (!_on_run)
        {
            _on_run = empty_on_run;
        }
    }

    void GlfwWindow::set_on_update(OnUpdate&& on_update)
    {
        _on_update = std::move(on_update);
        if (!_on_update)
        {
            _on_update = empty_on_update;
        }
    }

    void GlfwWindow::set_on_close(IWindow::OnClose&& on_close)
    {
        _on_close = std::move(on_close);
        if (!_on_close)
        {
            _on_close = empty_on_close;
        }
    }

    void GlfwWindow::set_on_keyboard_event(IWindow::OnKeyboardEvent&& on_keyboard_event)
    {
        _on_keyboard_event = std::move(on_keyboard_event);
        if (!_on_keyboard_event)
        {
            _on_keyboard_event = empty_on_keyboard_event;
        }
    }

    void GlfwWindow::set_on_mouse_event(IWindow::OnMouseEvent&& on_mouse_event)
    {
        _on_mouse_event = std::move(on_mouse_event);
        if (!_on_mouse_event)
        {
            _on_mouse_event = empty_on_mouse_event;
        }
    }

    void GlfwWindow::enqueue_keyboard_event(const input::KeyboardEvent& event)
    {
        _keyboard_event_queue.push_back(event);
    }

    void GlfwWindow::enqueue_mouse_event(const input::MouseEvent& event)
    {
        _mouse_event_queue.push_back(event);
    }

    void GlfwWindow::request_close()
    {
        glfwSetWindowShouldClose(
            _native_glfw_window,
            GLFW_TRUE);
    }

    bool GlfwWindow::close_requested() const
    {
        return glfwWindowShouldClose(_native_glfw_window);
    }
}