#pragma once

#include <functional>
#include <memory>

#include <xar_engine/os/window.hpp>


namespace xar_engine::os
{
    class IApplication
    {
    public:
        struct Parameters;

        using OnClose = std::function<void()>;
        using OnRun = std::function<void()>;
        using OnUpdate = std::function<void()>;

    public:
        virtual ~IApplication();

        virtual std::shared_ptr<IWindow> make_window(IWindow::Parameters parameters) = 0;

        virtual void set_on_run(OnRun&& on_run) = 0;
        virtual void set_on_update(OnUpdate&& on_update) = 0;
        virtual void set_on_close(OnClose&& on_close) = 0;

        virtual void run() = 0;

        virtual void request_close() = 0;

        [[nodiscard]]
        virtual bool close_requested() const = 0;

        [[nodiscard]]
        virtual const std::string& get_name() const = 0;
    };

    struct IApplication::Parameters
    {
        std::string name;
    };

    class IApplicationFactory
    {
    public:
        virtual ~IApplicationFactory();

        [[nodiscard]]
        virtual std::unique_ptr<IApplication> make(IApplication::Parameters parameters) const = 0;
    };

    class ApplicationFactory
        : public IApplicationFactory
    {
    public:
        [[nodiscard]]
        std::unique_ptr<IApplication> make(IApplication::Parameters parameters) const override;
    };
}