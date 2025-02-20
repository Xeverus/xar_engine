#pragma once

#include <cstdint>


namespace xar_engine::time
{
    using Nanoseconds = std::int64_t;

    class Time
    {
    public:
        static Nanoseconds since_epoch();
        static Nanoseconds since_startup();
    };
}