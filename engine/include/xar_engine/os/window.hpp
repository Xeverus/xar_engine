#pragma once

#include <functional>
#include <memory>

#include <xar_engine/graphics/renderer.hpp>
#include <xar_engine/graphics/renderer_type.hpp>

#include <xar_engine/input/input_event.hpp>

#include <xar_engine/math/vector.hpp>


namespace xar_engine::os
{
    class IWindow
    {
    public:
        struct Parameters;

        using OnRun = std::function<void()>;
        using OnUpdate = std::function<void()>;
        using OnClose = std::function<void()>;

        using OnResize = std::function<void(int32_t new_width, int32_t new_height)>;

        using OnKeyboardEvent = std::function<void(const input::KeyboardEvent& event)>;
        using OnMouseEvent = std::function<void(const input::MouseEvent& event)>;

    public:
        virtual ~IWindow();

        virtual void set_on_run(OnRun&& on_run) = 0;
        virtual void set_on_update(OnUpdate&& on_update) = 0;
        virtual void set_on_close(OnClose&& on_close) = 0;

        virtual void set_on_resize_event(OnResize&& on_resize) = 0;

        virtual void set_on_keyboard_event(OnKeyboardEvent&& on_keyboard_event) = 0;
        virtual void set_on_mouse_event(OnMouseEvent&& on_mouse_event) = 0;

        virtual void enqueue_keyboard_event(const input::KeyboardEvent& event) = 0;
        virtual void enqueue_mouse_event(const input::MouseEvent& event) = 0;

        virtual void request_close() = 0;

        virtual std::shared_ptr<graphics::IRenderer> make_renderer(graphics::RendererType renderer_type) = 0;

        [[nodiscard]]
        virtual bool close_requested() const = 0;
        [[nodiscard]]
        virtual const std::string& get_title() const = 0;
        [[nodiscard]]
        virtual math::Vector2i32 get_surface_pixel_size() const = 0;
    };

    struct IWindow::Parameters
    {
        std::string title;
    };
}