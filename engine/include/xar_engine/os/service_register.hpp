#pragma once

#include <memory>


namespace xar_engine::logging
{
    class ILogger;
}

namespace xar_engine::os
{
    class ServiceRegister
    {
    public:
        ServiceRegister();
        ~ServiceRegister();

        [[nodiscard]]
        logging::ILogger& get_global_logger() const;

        void set_global_logger(std::unique_ptr<logging::ILogger> global_logger);

    private:
        std::unique_ptr<logging::ILogger> _global_logger;
    };
}
