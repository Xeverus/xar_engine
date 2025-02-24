#include <xar_engine/os/service_register_impl.hpp>

#include <xar_engine/logging/console_logger.hpp>


namespace xar_engine::os
{
    ServiceRegisterImpl::ServiceRegisterImpl()
        : _global_logger(std::make_unique<logging::ConsoleLogger>())
    {
    }

    logging::ILogger& ServiceRegisterImpl::get_global_logger() const
    {
        return *_global_logger;
    }

    void ServiceRegisterImpl::set_global_logger(std::unique_ptr<logging::ILogger> global_logger)
    {
        if (global_logger != nullptr)
        {
            _global_logger = std::move(global_logger);
            return;
        }

        _global_logger = std::make_unique<logging::ConsoleLogger>();
    }
}
