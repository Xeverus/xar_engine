#include <xar_engine/os/input_event.hpp>

namespace xar_engine::os
{
    bool operator==(const xar_engine::os::KeyboardKeyEvent& left, const xar_engine::os::KeyboardKeyEvent& right)
    {
        return left.state == right.state && left.code == right.code;
    }

    bool operator==(const xar_engine::os::MouseButtonEvent& left, const xar_engine::os::MouseButtonEvent& right)
    {
        return left.state == right.state && left.code == right.code;
    }

    bool operator==(const xar_engine::os::MouseMotionEvent& left, const xar_engine::os::MouseMotionEvent& right)
    {
        return left.position_x == right.position_x && left.position_y == right.position_y;
    }

    bool operator==(const xar_engine::os::MouseScrollEvent& left, const xar_engine::os::MouseScrollEvent& right)
    {
        return left.delta_x == right.delta_x && left.delta_y == right.delta_y;
    }
}