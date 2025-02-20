#include <xar_engine/input/input_event.hpp>


namespace xar_engine::input
{
    bool operator==(
        const xar_engine::input::KeyboardKeyEvent& left,
        const xar_engine::input::KeyboardKeyEvent& right)
    {
        return left.state == right.state && left.code == right.code;
    }

    bool operator==(
        const xar_engine::input::MouseButtonEvent& left,
        const xar_engine::input::MouseButtonEvent& right)
    {
        return left.state == right.state && left.code == right.code;
    }

    bool operator==(
        const xar_engine::input::MouseMotionEvent& left,
        const xar_engine::input::MouseMotionEvent& right)
    {
        return left.position_x == right.position_x && left.position_y == right.position_y;
    }

    bool operator==(
        const xar_engine::input::MouseScrollEvent& left,
        const xar_engine::input::MouseScrollEvent& right)
    {
        return left.delta_x == right.delta_x && left.delta_y == right.delta_y;
    }
}