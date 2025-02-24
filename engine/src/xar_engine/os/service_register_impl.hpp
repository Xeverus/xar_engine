#pragma once

#include <xar_engine/os/service_register.hpp>


namespace xar_engine::logging
{
    class ILogger;
}

namespace xar_engine::os
{
    class ServiceRegisterImpl : public IServiceRegister
    {
    public:
        ServiceRegisterImpl();

        [[nodiscard]]
        logging::ILogger& get_global_logger() const override;

        void set_global_logger(std::unique_ptr<logging::ILogger> global_logger) override;

    private:
        std::unique_ptr<logging::ILogger> _global_logger;
    };
}
