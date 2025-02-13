#pragma once

#include <deque>

#include <GLFW/glfw3.h>

#include <xar_engine/os/window.hpp>

namespace xar_engine::os
{
    class GlfwWindow : public IWindow
    {
    public:
        GlfwWindow();
        ~GlfwWindow() override;

        void update();
        void close();

        void set_on_update(OnUpdate&& on_update) override;
        void set_on_close(OnClose&& on_close) override;

        void set_on_keyboard_event(OnKeyboardEvent&& on_keyboard_event) override;
        void set_on_mouse_event(OnMouseEvent&& on_mouse_event) override;

        void enqueue_keyboard_event(const input::KeyboardEvent& event) override;
        void enqueue_mouse_event(const input::MouseEvent& event) override;

        void request_close() override;

        [[nodiscard]]
        bool close_requested() const override;

    private:
        GLFWwindow* _native_glfw_window;

        OnUpdate _on_update;
        OnUpdate _on_close;
        OnKeyboardEvent _on_keyboard_event;
        OnMouseEvent _on_mouse_event;

        std::deque<input::KeyboardEvent> _keyboard_event_queue;
        std::deque<input::MouseEvent> _mouse_event_queue;
    };
}