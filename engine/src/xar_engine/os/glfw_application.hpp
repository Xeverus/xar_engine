#pragma once

#include <xar_engine/os/application.hpp>

namespace xar_engine::os
{
    class GlfwApplication : public IApplication
    {
    public:
        GlfwApplication();
        ~GlfwApplication() override;
    };
}