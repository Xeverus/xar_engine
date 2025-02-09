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

    bool operator==(const xar_engine::os::KeyboardKeyEvent& left, const xar_engine::os::KeyboardKeyEvent& right);

    using KeyboardEvent = std::variant<
        KeyboardKeyEvent
    >;

    struct MouseButtonEvent
    {
        ButtonCode code;
        ButtonState state;
    };

    bool operator==(const xar_engine::os::MouseButtonEvent& left, const xar_engine::os::MouseButtonEvent& right);

    struct MouseMotionEvent
    {
        std::int32_t position_x;
        std::int32_t position_y;
    };

    bool operator==(const xar_engine::os::MouseMotionEvent& left, const xar_engine::os::MouseMotionEvent& right);

    struct MouseScrollEvent
    {
        std::int32_t delta_x;
        std::int32_t delta_y;
    };

    bool operator==(const xar_engine::os::MouseScrollEvent& left, const xar_engine::os::MouseScrollEvent& right);

    using MouseEvent = std::variant<
        MouseButtonEvent,
        MouseMotionEvent,
        MouseScrollEvent
    >;
}