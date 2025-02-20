#pragma once

#include <cstdint>


namespace xar_engine::time
{
    using Nanoseconds = std::int64_t;

    Nanoseconds since_epoch();
    Nanoseconds since_startup();
}