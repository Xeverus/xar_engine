#pragma once

#include <xar_engine/os/window.hpp>

namespace xar_engine::os
{
    class GlfwWindow : public IWindow
    {
    public:
        GlfwWindow();

        void update();

        void request_close() override;

        [[nodiscard]]
        bool close_requested() const override;

    private:
        bool _close_requested;
    };
}