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

        std::shared_ptr<IWindow> make_window() override;

        void set_on_run(OnRun&& on_run) override;
        void set_on_update(OnUpdate&& on_update) override;
        void set_on_close(OnClose&& on_close) override;

        void run() override;

        void request_close() override;

        [[nodiscard]]
        bool close_requested() const override;

    private:
         std::vector<std::shared_ptr<GlfwWindow>> _glfw_windows;
         OnUpdate _on_run;
         OnUpdate _on_update;
         OnUpdate _on_close;
         bool _current_close_requested;
         bool _previous_close_requested;

    private:
        void update_windows();
        void handle_application_close_request();
        void handle_windows_close_request();
    };
}