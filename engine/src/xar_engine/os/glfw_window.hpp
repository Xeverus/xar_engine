#pragma once

#include <deque>
#include <optional>

#include <GLFW/glfw3.h>

#include <xar_engine/os/window.hpp>

namespace xar_engine::os
{
    class GlfwWindow : public IWindow
    {
    public:
        explicit GlfwWindow(Parameters parameters);
        ~GlfwWindow() override;

        void run();
        void update();
        void close();

        [[nodiscard]]
        GLFWwindow* get_native() const;

        void set_on_run(OnRun&& on_run) override;
        void set_on_update(OnUpdate&& on_update) override;
        void set_on_close(OnClose&& on_close) override;

        void set_on_resize_event(OnResize&& on_resize) override;

        void enqueue_resize_event(int32_t new_width, int32_t new_height);

        void set_on_keyboard_event(OnKeyboardEvent&& on_keyboard_event) override;
        void set_on_mouse_event(OnMouseEvent&& on_mouse_event) override;

        void enqueue_keyboard_event(const input::KeyboardEvent& event) override;
        void enqueue_mouse_event(const input::MouseEvent& event) override;

        void request_close() override;

        [[nodiscard]]
        bool close_requested() const override;

        [[nodiscard]]
        const std::string& get_title() const override;

    private:
        struct ResizeEvent
        {
            std::int32_t new_width;
            std::int32_t new_height;
        };

    private:
        GLFWwindow* _native_glfw_window;

        OnRun _on_run;
        OnUpdate _on_update;
        OnClose _on_close;

        OnResize _on_resize;

        OnKeyboardEvent _on_keyboard_event;
        OnMouseEvent _on_mouse_event;

        std::optional<ResizeEvent> _resize_event;

        std::deque<input::KeyboardEvent> _keyboard_event_queue;
        std::deque<input::MouseEvent> _mouse_event_queue;

        Parameters _parameters;
    };
}