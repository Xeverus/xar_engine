#include <xar_engine/os/service_register.hpp>

#include <xar_engine/logging/console_logger.hpp>


namespace xar_engine::os
{
    ServiceRegister::ServiceRegister()
        : _global_logger(std::make_unique<logging::ConsoleLogger>())
    {
    }

    ServiceRegister::~ServiceRegister() = default;

    logging::ILogger& ServiceRegister::get_global_logger() const
    {
        return *_global_logger;
    }

    void ServiceRegister::set_global_logger(std::unique_ptr<logging::ILogger> global_logger)
    {
        if (global_logger != nullptr)
        {
            _global_logger = std::move(global_logger);
            return;
        }

        _global_logger = std::make_unique<logging::ConsoleLogger>();
    }
}
