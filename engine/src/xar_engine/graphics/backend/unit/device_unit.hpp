#pragma once

#include <xar_engine/graphics/api/format.hpp>


namespace xar_engine::graphics::backend::unit
{
    class IDeviceUnit
    {
    public:
        virtual ~IDeviceUnit();


        virtual void wait_idle() = 0;

        [[nodiscard]]
        virtual std::uint32_t get_sample_count() const = 0;

        [[nodiscard]]
        virtual api::EFormat find_depth_format() const = 0;
    };
}