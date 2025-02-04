#pragma once

#include <GLFW/glfw3.h>

#include <xar_engine/os/window.hpp>

namespace xar_engine::os
{
    class GlfwWindow : public IWindow
    {
    public:
        GlfwWindow();
        ~GlfwWindow() override;

        void update();

        void set_on_update(OnUpdate&& on_update) override;

        void request_close() override;

        [[nodiscard]]
        bool close_requested() const override;

    private:
        OnUpdate _on_update;
        GLFWwindow* _native_glfw_window;
    };
}