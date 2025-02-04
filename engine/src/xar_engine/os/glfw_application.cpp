#include <xar_engine/os/glfw_application.hpp>

#include <GLFW/glfw3.h>

namespace xar_engine::os
{
    GlfwApplication::GlfwApplication()
    {
        const bool glfw_initialized = glfwInit();
        if (!glfw_initialized)
        {

        }
    }

    GlfwApplication::~GlfwApplication()
    {
        glfwTerminate();
    }
}
