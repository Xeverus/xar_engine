#pragma once

#include <functional>
#include <memory>

#include <xar_engine/os/window.hpp>

namespace xar_engine::os
{
    class IApplication
    {
    public:
        using OnUpdate = std::function<void()>;

    public:
        virtual ~IApplication();

        virtual std::shared_ptr<IWindow> make_window() = 0;

        virtual void set_on_update(OnUpdate&& on_update) = 0;

        virtual void run() = 0;

        virtual void request_close() = 0;

        [[nodiscard]]
        virtual bool close_requested() const = 0;
    };

    class ApplicationFactory
    {
    public:
        static std::unique_ptr<IApplication> make();
    };
}