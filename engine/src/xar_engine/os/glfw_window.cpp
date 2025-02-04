#include <xar_engine/os/glfw_window.hpp>

namespace xar_engine::os
{
    namespace
    {
        IWindow::OnUpdate empty_on_update = [](){};
    }

    GlfwWindow::GlfwWindow()
        : _on_update(empty_on_update)
        , _close_requested(false)
    {
    }

    void GlfwWindow::update()
    {
        if (close_requested())
        {
            return;
        }

        _on_update();
    }

    void GlfwWindow::set_on_update(OnUpdate&& on_update)
    {
        _on_update = std::move(on_update);
        if (!_on_update)
        {
            _on_update = empty_on_update;
        }
    }

    void GlfwWindow::request_close()
    {
        _close_requested = true;
    }

    bool GlfwWindow::close_requested() const
    {
        return _close_requested;
    }
}