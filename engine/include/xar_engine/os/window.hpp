#pragma once

#include <functional>

namespace xar_engine::os
{
    class IWindow
    {
    public:
        using OnUpdate = std::function<void()>;

    public:
        virtual ~IWindow();

        virtual void set_on_update(OnUpdate&& on_update) = 0;

        virtual void request_close() = 0;

        [[nodiscard]]
        virtual bool close_requested() const = 0;
    };
}