#pragma once

#include <xar_engine/math/vector.hpp>


namespace xar_engine::graphics
{
    class IWindowSurface
    {
    public:
        virtual ~IWindowSurface();

        virtual math::Vector2i32 get_pixel_size() const = 0;
    };
}