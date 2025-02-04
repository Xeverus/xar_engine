#pragma once

#include <memory>
#include <vector>

#include <xar_engine/os/application.hpp>
#include <xar_engine/os/glfw_window.hpp>

namespace xar_engine::os
{
    class GlfwApplication : public IApplication
    {
    public:
        GlfwApplication();
        ~GlfwApplication() override;

        void run() override;

    private:
         std::vector<std::unique_ptr<GlfwWindow>> _glfw_windows;

    private:
        void update_windows();
        void handle_windows_close_request();
    };
}