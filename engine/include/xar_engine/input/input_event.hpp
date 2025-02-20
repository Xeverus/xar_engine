#pragma once

#include <cstdint>
#include <variant>

#include <xar_engine/input/button.hpp>


namespace xar_engine::input
{
    struct KeyboardKeyEvent
    {
        ButtonCode code;
        ButtonState state;
    };

    bool operator==(
        const xar_engine::input::KeyboardKeyEvent& left,
        const xar_engine::input::KeyboardKeyEvent& right);

    using KeyboardEvent = std::variant<
        KeyboardKeyEvent
    >;


    struct MouseButtonEvent
    {
        ButtonCode code;
        ButtonState state;
    };

    bool operator==(
        const xar_engine::input::MouseButtonEvent& left,
        const xar_engine::input::MouseButtonEvent& right);

    struct MouseMotionEvent
    {
        std::int32_t position_x;
        std::int32_t position_y;
    };

    bool operator==(
        const xar_engine::input::MouseMotionEvent& left,
        const xar_engine::input::MouseMotionEvent& right);

    struct MouseScrollEvent
    {
        std::int32_t delta_x;
        std::int32_t delta_y;
    };

    bool operator==(
        const xar_engine::input::MouseScrollEvent& left,
        const xar_engine::input::MouseScrollEvent& right);

    using MouseEvent = std::variant<
        MouseButtonEvent,
        MouseMotionEvent,
        MouseScrollEvent
    >;
}