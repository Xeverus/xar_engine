#include <xar_engine/os/application.hpp>

#include <xar_engine/os/glfw_application.hpp>


namespace xar_engine::os
{
    IApplication::~IApplication() = default;


    IApplicationFactory::~IApplicationFactory() = default;


    std::unique_ptr<IApplication> ApplicationFactory::make(IApplication::Parameters parameters) const
    {
        return std::make_unique<GlfwApplication>(std::move(parameters));
    }
}