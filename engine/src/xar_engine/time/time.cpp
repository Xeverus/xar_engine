#include <xar_engine/time/time.hpp>

#include <chrono>

namespace xar_engine::time
{
    namespace
    {
        const Nanoseconds startup_time = Time::since_epoch();
    }

    Nanoseconds Time::since_startup()
    {
        return since_epoch() - startup_time;
    }

    Nanoseconds Time::since_epoch()
    {
        const auto now = std::chrono::steady_clock::now();
        const auto now_ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
        return now_ns.time_since_epoch().count();
    }
}