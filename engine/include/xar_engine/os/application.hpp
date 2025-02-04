#pragma once

#include <memory>

namespace xar_engine::os
{
    class IApplication
    {
    public:
        virtual ~IApplication();
    };

    class ApplicationFactory
    {
    public:
        static std::unique_ptr<IApplication> make();
    };
}