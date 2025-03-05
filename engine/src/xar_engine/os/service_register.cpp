#include <xar_engine/os/service_register.hpp>

#include <xar_engine/logging/logger.hpp>

#include <xar_engine/meta/singleton.hpp>

#include <xar_engine/os/service_register_impl.hpp>


namespace xar_engine::os
{
    IServiceRegister& IServiceRegister::get_instance()
    {
        return meta::TSingleton::get_instance<ServiceRegisterImpl>();
    }

    IServiceRegister::~IServiceRegister() = default;
}
