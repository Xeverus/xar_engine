#include <xar_engine/os/glfw_application.hpp>

#include <GLFW/glfw3.h>

#include <xar_engine/error/exception_utils.hpp>
#include <xar_engine/os/glfw_window.hpp>

namespace xar_engine::os
{
    namespace
    {
        const IApplication::OnUpdate empty_on_update = []()
        {
        };
        const IApplication::OnUpdate empty_on_close = []()
        {
        };
    }

    GlfwApplication::GlfwApplication()
        : _on_update(empty_on_update)
        , _on_close(empty_on_close)
        , _current_close_requested(false)
        , _previous_close_requested(false)
    {
        const bool glfw_initialized = glfwInit();
        XAR_THROW_IF(
            !glfw_initialized,
            error::XarException,
            "Failed to initialize GLFW");
    }

    GlfwApplication::~GlfwApplication()
    {
        glfwTerminate();
    }

    std::shared_ptr<IWindow> GlfwApplication::make_window()
    {
        _glfw_windows.push_back(std::make_shared<GlfwWindow>());
        return _glfw_windows.back();
    }

    void GlfwApplication::set_on_update(OnUpdate&& on_update)
    {
        _on_update = std::move(on_update);
        if (!_on_update)
        {
            _on_update = empty_on_update;
        }
    }

    void GlfwApplication::set_on_close(OnClose&& on_close)
    {
        _on_close = std::move(on_close);
        if (!_on_close)
        {
            _on_close = empty_on_close;
        }
    }

    void GlfwApplication::request_close()
    {
        _previous_close_requested = true;
    }

    bool GlfwApplication::close_requested() const
    {
        return _current_close_requested;
    }

    void GlfwApplication::run()
    {
        while (!close_requested())
        {
            glfwPollEvents();
            _on_update();
            update_windows();
            handle_application_close_request();
            handle_windows_close_request();
        }

        _on_close();
    }

    void GlfwApplication::update_windows()
    {
        /**
         * A window can be created during update function and it may result in vector reallocation.
         * This is why using range-based loop is better idea as iterators may become invalid in the meantime.
         */
        for (std::size_t i = 0; i < _glfw_windows.size(); ++i)
        {
            _glfw_windows[i]->update();
        }
    }

    void GlfwApplication::handle_application_close_request()
    {
        if (_current_close_requested == _previous_close_requested)
        {
            return;
        }

        _current_close_requested = _previous_close_requested;

        for (std::size_t i = 0; i < _glfw_windows.size(); ++i)
        {
            _glfw_windows[i]->request_close();
        }
    }

    void GlfwApplication::handle_windows_close_request()
    {
        for (auto& glfw_window : _glfw_windows)
        {
            if (glfw_window->close_requested())
            {
                glfw_window->close();
            }
        }

        _glfw_windows.erase(
            std::remove_if(
                _glfw_windows.begin(),
                _glfw_windows.end(),
                [](const auto& glfw_window)
                {
                    return glfw_window->close_requested();
                }),
            _glfw_windows.end());
    }
}
