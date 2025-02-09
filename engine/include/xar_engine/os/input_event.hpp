#pragma once

#include <cstdint>
#include <variant>

#include <xar_engine/os/button.hpp>

namespace xar_engine::os
{
    struct KeyboardKeyEvent
    {
        ButtonCode code;
        ButtonState state;
    };

    using KeyboardEvent = std::variant<
        KeyboardKeyEvent
    >;

    struct MouseButtonEvent
    {
        ButtonCode code;
        ButtonState state;
    };

    struct MouseMotionEvent
    {
        std::int32_t delta_x;
        std::int32_t delta_y;
    };

    struct MouseScrollEvent
    {
        std::int32_t delta_x;
        std::int32_t delta_y;
    };

    using MouseEvent = std::variant<
        MouseButtonEvent,
        MouseMotionEvent,
        MouseScrollEvent
    >;
}