#pragma once

namespace xar_engine::os
{
    class IWindow
    {
    public:
        virtual ~IWindow();

        virtual void request_close() = 0;

        [[nodiscard]]
        virtual bool close_requested() const = 0;
    };
}