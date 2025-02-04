#include <xar_engine/os/glfw_window.hpp>

namespace xar_engine::os
{
    GlfwWindow::GlfwWindow()
        : _close_requested(false)
    {
    }

    void GlfwWindow::update()
    {
        if (_close_requested)
        {
            return;
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