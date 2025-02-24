#pragma once

#include <memory>


namespace xar_engine::logging
{
    class ILogger;
}


namespace xar_engine::os
{
    class IServiceRegister
    {
    public:
        static IServiceRegister& get_instance();

    public:
        virtual ~IServiceRegister();

        [[nodiscard]]
        virtual logging::ILogger& get_global_logger() const = 0;

        virtual void set_global_logger(std::unique_ptr<logging::ILogger> global_logger) = 0;
    };
}
