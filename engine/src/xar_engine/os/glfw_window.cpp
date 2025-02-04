#include <xar_engine/os/glfw_window.hpp>

#include <xar_engine/error/exception_utils.hpp>

namespace xar_engine::os
{
    namespace
    {
        IWindow::OnUpdate empty_on_update = [](){};
    }

    GlfwWindow::GlfwWindow()
        : _on_update(empty_on_update)
        , _native_glfw_window(nullptr)
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        _native_glfw_window = glfwCreateWindow(800, 600, "GLFW App", nullptr, nullptr);
        XAR_THROW_IF(
            _native_glfw_window == nullptr,
            error::XarException,
            "Failed to make GLFW window");

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

    void GlfwWindow::update()
    {
        if (close_requested())
        {
            return;
        }

        _on_update();

        glfwSwapBuffers(_native_glfw_window);
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
        glfwSetWindowShouldClose(_native_glfw_window, GLFW_TRUE);
    }

    bool GlfwWindow::close_requested() const
    {
        return glfwWindowShouldClose(_native_glfw_window);
    }
}