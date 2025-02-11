#include <xar_engine/os/button.hpp>

#include <ostream>

#include <xar_engine/error/exception_utils.hpp>

namespace xar_engine::meta
{
    std::string enum_to_string(os::ButtonCode enum_value)
    {
        switch (enum_value)
        {
            case os::ButtonCode::MOUSE_0: return "MOUSE_0";
            case os::ButtonCode::MOUSE_1: return "MOUSE_1";
            case os::ButtonCode::MOUSE_2: return "MOUSE_2";
            case os::ButtonCode::MOUSE_3: return "MOUSE_3";
            case os::ButtonCode::MOUSE_4: return "MOUSE_4";
            case os::ButtonCode::MOUSE_5: return "MOUSE_5";
            case os::ButtonCode::MOUSE_6: return "MOUSE_6";
            case os::ButtonCode::MOUSE_7: return "MOUSE_7";
            case os::ButtonCode::MOUSE_8: return "MOUSE_8";

            case os::ButtonCode::SPACE: return "SPACE";

            case os::ButtonCode::_0: return "_0";
            case os::ButtonCode::_1: return "_1";
            case os::ButtonCode::_2: return "_2";
            case os::ButtonCode::_3: return "_3";
            case os::ButtonCode::_4: return "_4";
            case os::ButtonCode::_5: return "_5";
            case os::ButtonCode::_6: return "_6";
            case os::ButtonCode::_7: return "_7";
            case os::ButtonCode::_8: return "_8";
            case os::ButtonCode::_9: return "_9";

            case os::ButtonCode::A: return "A";
            case os::ButtonCode::B: return "B";
            case os::ButtonCode::C: return "C";
            case os::ButtonCode::D: return "D";
            case os::ButtonCode::E: return "E";
            case os::ButtonCode::F: return "F";
            case os::ButtonCode::G: return "G";
            case os::ButtonCode::H: return "H";
            case os::ButtonCode::I: return "I";
            case os::ButtonCode::J: return "J";
            case os::ButtonCode::K: return "K";
            case os::ButtonCode::L: return "L";
            case os::ButtonCode::M: return "M";
            case os::ButtonCode::N: return "N";
            case os::ButtonCode::O: return "O";
            case os::ButtonCode::P: return "P";
            case os::ButtonCode::Q: return "Q";
            case os::ButtonCode::R: return "R";
            case os::ButtonCode::S: return "S";
            case os::ButtonCode::T: return "T";
            case os::ButtonCode::U: return "U";
            case os::ButtonCode::V: return "V";
            case os::ButtonCode::W: return "W";
            case os::ButtonCode::X: return "X";
            case os::ButtonCode::Y: return "Y";
            case os::ButtonCode::Z: return "Z";

            case os::ButtonCode::ESCAPE: return "ESCAPE";
            case os::ButtonCode::ENTER: return "ENTER";
            case os::ButtonCode::TAB: return "TAB";
            case os::ButtonCode::BACKSPACE: return "BACKSPACE";
        }

        XAR_THROW(
            error::XarException,
            "ButtonState enum value doesn't define value '{}'",
            static_cast<int>(enum_value));
    }

    std::ostream& operator<<(
        std::ostream& stream,
        const os::ButtonCode enum_value)
    {
        if (stream.good())
        {
            stream << enum_to_string(enum_value);
        }

        return stream;
    }

    std::string enum_to_string(os::ButtonState enum_value)
    {
        switch (enum_value)
        {
            case os::ButtonState::DOWN: return "DOWN";
            case os::ButtonState::REPEAT: return "REPEAT";
            case os::ButtonState::UP: return "UP";
        }

        XAR_THROW(
            error::XarException,
            "ButtonState enum value doesn't define value '{}'",
            static_cast<int>(enum_value));
    }

    std::ostream& operator<<(
        std::ostream& stream,
        const os::ButtonState enum_value)
    {
        if (stream.good())
        {
            stream << enum_to_string(enum_value);
        }

        return stream;
    }
}