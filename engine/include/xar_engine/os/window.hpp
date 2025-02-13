#pragma once

#include <functional>

#include <xar_engine/input/input_event.hpp>

namespace xar_engine::os
{
    class IWindow
    {
    public:
        using OnRun = std::function<void()>;
        using OnUpdate = std::function<void()>;
        using OnClose = std::function<void()>;

        using OnKeyboardEvent = std::function<void(const input::KeyboardEvent& event)>;
        using OnMouseEvent = std::function<void(const input::MouseEvent& event)>;

    public:
        virtual ~IWindow();

        virtual void set_on_run(OnRun&& on_run) = 0;
        virtual void set_on_update(OnUpdate&& on_update) = 0;
        virtual void set_on_close(OnClose&& on_close) = 0;

        virtual void set_on_keyboard_event(OnKeyboardEvent&& on_keyboard_event) = 0;
        virtual void set_on_mouse_event(OnMouseEvent&& on_mouse_event) = 0;

        virtual void enqueue_keyboard_event(const input::KeyboardEvent& event) = 0;
        virtual void enqueue_mouse_event(const input::MouseEvent& event) = 0;

        virtual void request_close() = 0;

        [[nodiscard]]
        virtual bool close_requested() const = 0;
    };
}