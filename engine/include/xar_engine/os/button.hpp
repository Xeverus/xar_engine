#pragma once

#include <string>

#include <xar_engine/meta/enum.hpp>

namespace xar_engine::os
{
    enum class ButtonCode
    {
        MOUSE_0 = 0,
        MOUSE_1, MOUSE_2, MOUSE_3, MOUSE_4, MOUSE_5,
        MOUSE_6, MOUSE_7, MOUSE_8,

        MOUSE_LEFT = MOUSE_0,
        MOUSE_RIGHT = MOUSE_1,
        MOUSE_MIDDLE = MOUSE_2,

        SPACE = 32,

        _0 = 48,
        _1, _2, _3, _4, _5,
        _6, _7, _8, _9,

        A = 65,
        B, C, D, E, F,
        G, H, I, J, K,
        L, M, N, O, P,
        Q, R, S, T, U,
        V, W, X, Y, Z,

        ESCAPE = 256,
        ENTER = 257,
        TAB = 258,
        BACKSPACE = 259,
    };

    enum class ButtonState
    {
        DOWN,
        REPEAT,
        UP,
    };
}

ENUM_TO_STRING(xar_engine::os::ButtonCode);
ENUM_TO_STRING(xar_engine::os::ButtonState);
