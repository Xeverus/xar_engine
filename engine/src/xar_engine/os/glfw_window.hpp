#pragma once

#include <xar_engine/os/window.hpp>

namespace xar_engine::os
{
    class GlfwWindow : public IWindow
    {
    public:
        GlfwWindow();

        void update();

        void set_on_update(OnUpdate&& on_update) override;

        void request_close() override;

        [[nodiscard]]
        bool close_requested() const override;

    private:
        OnUpdate _on_update;
        bool _close_requested;
    };
}