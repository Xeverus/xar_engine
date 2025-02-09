#pragma once

#include <functional>

#include <xar_engine/os/input_event.hpp>

namespace xar_engine::os
{
    class IWindow
    {
    public:
        using OnUpdate = std::function<void()>;
        using OnClose = std::function<void()>;

        using OnKeyboardEvent = std::function<void(const KeyboardEvent& event)>;
        using OnMouseEvent = std::function<void(const MouseEvent& event)>;

    public:
        virtual ~IWindow();

        virtual void set_on_update(OnUpdate&& on_update) = 0;
        virtual void set_on_close(OnClose&& on_close) = 0;

        virtual void set_on_keyboard_event(OnKeyboardEvent&& on_keyboard_event) = 0;
        virtual void set_on_mouse_event(OnMouseEvent&& on_mouse_event) = 0;

        virtual void request_close() = 0;

        [[nodiscard]]
        virtual bool close_requested() const = 0;
    };
}