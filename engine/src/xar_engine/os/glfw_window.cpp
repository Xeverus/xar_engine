#include <xar_engine/os/glfw_window.hpp>

#include <xar_engine/error/exception_utils.hpp>

#include <xar_engine/graphics/vulkan/vulkan_window_surface.hpp>

#include <xar_engine/graphics/vulkan/impl/vulkan_instance.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_renderer.hpp>

#include <xar_engine/meta/ref_counting_singleton.hpp>


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
        const IWindow::OnResize empty_on_resize = [](
            const int32_t,
            const int32_t)
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

        void glfw_framebuffer_resize_callback(
            GLFWwindow* const native_glfw_window,
            int width,
            int height)
        {
            auto* const glfw_window = get_window(native_glfw_window);
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

    GlfwWindow::GlfwWindow(Parameters parameters)
        : _native_glfw_window(nullptr)
        , _on_run(empty_on_run)
        , _on_update(empty_on_update)
        , _on_close(empty_on_close)
        , _on_resize(empty_on_resize)
        , _on_keyboard_event(empty_on_keyboard_event)
        , _on_mouse_event(empty_on_mouse_event)
        , _parameters(std::move(parameters))
    {
        glfwWindowHint(
            GLFW_CLIENT_API,
            GLFW_NO_API);

        _native_glfw_window = glfwCreateWindow(
            800,
            600,
            _parameters.title.c_str(),
            nullptr,
            nullptr);
        XAR_THROW_IF(
            _native_glfw_window == nullptr,
            error::XarException,
            "Failed to make GLFW window");

        glfwSetWindowUserPointer(
            _native_glfw_window,
            this);

        glfwSetFramebufferSizeCallback(
            _native_glfw_window,
            glfw_framebuffer_resize_callback);

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

        glfwSetWindowTitle(
            _native_glfw_window,
            _parameters.title.c_str());

        if (_parameters.graphics_backend_type != graphics::EGraphicsBackendType::VULKAN)
        {
            return;
        }

        auto vulkan_instance = meta::RefCountedSingleton::get_instance<graphics::vulkan::impl::VulkanInstance>();

        auto vk_surface_khr = VkSurfaceKHR{nullptr};
        const auto result = glfwCreateWindowSurface(
            vulkan_instance->get_native(),
            _native_glfw_window,
            nullptr,
            &vk_surface_khr);
        XAR_THROW_IF(
            result != VK_SUCCESS,
            error::XarException,
            "Failed to create window surface");

        _window_surface = std::make_shared<graphics::vulkan::VulkanWindowSurface>(
            vulkan_instance,
            vk_surface_khr,
            this);
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

        if (_resize_event)
        {
            _on_resize(
                _resize_event->new_width,
                _resize_event->new_height);
            _resize_event.reset();
        }

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

    void GlfwWindow::set_on_resize_event(OnResize&& on_resize)
    {
        _on_resize = std::move(on_resize);
        if (!_on_resize)
        {
            _on_resize = empty_on_resize;
        }
    }

    void GlfwWindow::enqueue_resize_event(
        const int32_t new_width,
        const int32_t new_height)
    {
        _resize_event = {new_width, new_height};
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

    const std::string& GlfwWindow::get_title() const
    {
        return _parameters.title;
    }

    const std::shared_ptr<graphics::IWindowSurface>& GlfwWindow::get_surface() const
    {
        return _window_surface;
    }

    math::Vector2i32 GlfwWindow::get_surface_pixel_size() const
    {
        std::int32_t width;
        std::int32_t height;
        glfwGetFramebufferSize(
            _native_glfw_window,
            &width,
            &height);

        return {
            width,
            height
        };
    }

    std::shared_ptr<graphics::IRenderer> GlfwWindow::TMP_MAKE_RENDERER() const
    {
        return std::make_shared<graphics::vulkan::impl::VulkanRenderer>(
            meta::RefCountedSingleton::get_instance<graphics::vulkan::impl::VulkanInstance>(),
            std::dynamic_pointer_cast<graphics::vulkan::VulkanWindowSurface>(get_surface()));
    }
}