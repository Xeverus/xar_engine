#pragma once

#include <memory>

namespace xar_engine::os
{
    class IApplication
    {
    public:
        virtual ~IApplication();

        virtual void run() = 0;
    };

    class ApplicationFactory
    {
    public:
        static std::unique_ptr<IApplication> make();
    };
}