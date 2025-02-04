#include <xar_engine/os/glfw_application.hpp>

#include <GLFW/glfw3.h>

#include <xar_engine/error/exception_utils.hpp>

namespace xar_engine::os
{
    GlfwApplication::GlfwApplication()
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

    void GlfwApplication::run()
    {
        update_windows();
        handle_windows_close_request();
    }

    void GlfwApplication::update_windows()
    {
        for (auto& glfw_window : _glfw_windows)
        {
            glfw_window->update();
        }
    }

    void GlfwApplication::handle_windows_close_request()
    {
        const auto new_end = std::remove_if(
            _glfw_windows.begin(),
            _glfw_windows.end(),
            [](const auto& glfw_window)
            {
                return glfw_window->close_requested();
            });

        _glfw_windows.erase(
            new_end,
            _glfw_windows.end());
    }
}
