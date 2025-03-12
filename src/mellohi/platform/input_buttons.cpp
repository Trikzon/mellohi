#include "mellohi/platform/input_buttons.hpp"

namespace mellohi
{
    std::string_view to_string(const Key key)
    {
        switch (key)
        {
            case Key::Space:          return "Space";
            case Key::Apostrophe:     return "Apostrophe";
            case Key::Comma:          return "Comma";
            case Key::Minus:          return "Minus";
            case Key::Period:         return "Period";
            case Key::Slash:          return "Slash";
            case Key::Zero:           return "Zero";
            case Key::One:            return "One";
            case Key::Two:            return "Two";
            case Key::Three:          return "Three";
            case Key::Four:           return "Four";
            case Key::Five:           return "Five";
            case Key::Six:            return "Six";
            case Key::Seven:          return "Seven";
            case Key::Eight:          return "Eight";
            case Key::Nine:           return "Nine";
            case Key::Semicolon:      return "Semicolon";
            case Key::Equal:          return "Equal";
            case Key::A:              return "A";
            case Key::B:              return "B";
            case Key::C:              return "C";
            case Key::D:              return "D";
            case Key::E:              return "E";
            case Key::F:              return "F";
            case Key::G:              return "G";
            case Key::H:              return "H";
            case Key::I:              return "I";
            case Key::J:              return "J";
            case Key::K:              return "K";
            case Key::L:              return "L";
            case Key::M:              return "M";
            case Key::N:              return "N";
            case Key::O:              return "O";
            case Key::P:              return "P";
            case Key::Q:              return "Q";
            case Key::R:              return "R";
            case Key::S:              return "S";
            case Key::T:              return "T";
            case Key::U:              return "U";
            case Key::V:              return "V";
            case Key::W:              return "W";
            case Key::X:              return "X";
            case Key::Y:              return "Y";
            case Key::Z:              return "Z";
            case Key::LeftBracket:    return "LeftBracket";
            case Key::Backslash:      return "Backslash";
            case Key::RightBracket:   return "RightBracket";
            case Key::GraveAccent:    return "GraveAccent";
            case Key::Escape:         return "Escape";
            case Key::Enter:          return "Enter";
            case Key::Tab:            return "Tab";
            case Key::Backspace:      return "Backspace";
            case Key::Insert:         return "Insert";
            case Key::Delete:         return "Delete";
            case Key::Right:          return "Right";
            case Key::Left:           return "Left";
            case Key::Down:           return "Down";
            case Key::Up:             return "Up";
            case Key::PageUp:         return "PageUp";
            case Key::PageDown:       return "PageDown";
            case Key::Home:           return "Home";
            case Key::End:            return "End";
            case Key::CapsLock:       return "CapsLock";
            case Key::ScrollLock:     return "ScrollLock";
            case Key::NumLock:        return "NumLock";
            case Key::PrintScreen:    return "PrintScreen";
            case Key::Pause:          return "Pause";
            case Key::F1:             return "F1";
            case Key::F2:             return "F2";
            case Key::F3:             return "F3";
            case Key::F4:             return "F4";
            case Key::F5:             return "F5";
            case Key::F6:             return "F6";
            case Key::F7:             return "F7";
            case Key::F8:             return "F8";
            case Key::F9:             return "F9";
            case Key::F10:            return "F10";
            case Key::F11:            return "F11";
            case Key::F12:            return "F12";
            case Key::F13:            return "F13";
            case Key::F14:            return "F14";
            case Key::F15:            return "F15";
            case Key::F16:            return "F16";
            case Key::F17:            return "F17";
            case Key::F18:            return "F18";
            case Key::F19:            return "F19";
            case Key::F20:            return "F20";
            case Key::F21:            return "F21";
            case Key::F22:            return "F22";
            case Key::F23:            return "F23";
            case Key::F24:            return "F24";
            case Key::F25:            return "F25";
            case Key::NumPadZero:     return "NumPadZero";
            case Key::NumPadOne:      return "NumPadOne";
            case Key::NumPadTwo:      return "NumPadTwo";
            case Key::NumPadThree:    return "NumPadThree";
            case Key::NumPadFour:     return "NumPadFour";
            case Key::NumPadFive:     return "NumPadFive";
            case Key::NumPadSix:      return "NumPadSix";
            case Key::NumPadSeven:    return "NumPadSeven";
            case Key::NumPadEight:    return "NumPadEight";
            case Key::NumPadNine:     return "NumPadNine";
            case Key::NumPadDecimal:  return "NumPadDecimal";
            case Key::NumPadDivide:   return "NumPadDivide";
            case Key::NumPadMultiply: return "NumPadMultiply";
            case Key::NumPadSubtract: return "NumPadSubtract";
            case Key::NumPadAdd:      return "NumPadAdd";
            case Key::NumPadEnter:    return "NumPadEnter";
            case Key::NumPadEqual:    return "NumPadEqual";
            case Key::LeftShift:      return "LeftShift";
            case Key::LeftControl:    return "LeftControl";
            case Key::LeftAlt:        return "LeftAlt";
            case Key::LeftSuper:      return "LeftSuper";
            case Key::RightShift:     return "RightShift";
            case Key::RightControl:   return "RightControl";
            case Key::RightAlt:       return "RightAlt";
            case Key::RightSuper:     return "RightSuper";
            case Key::Menu:           return "Menu";
            default:                  return "None";
        }
    }

    std::ostream & operator<<(std::ostream &os, const Key key)
    {
        return os << "Key(" << to_string(key) << ")";
    }

    std::string_view to_string(const MouseButton mouse_button)
    {
        switch (mouse_button)
        {
            case MouseButton::Left: return "Left";
            case MouseButton::Right: return "Right";
            case MouseButton::Middle: return "Middle";
            case MouseButton::Four: return "Four";
            case MouseButton::Five: return "Five";
            case MouseButton::Six: return "Six";
            case MouseButton::Seven: return "Seven";
            case MouseButton::Eight: return "Eight";
            default: return "None";
        }
    }

    std::ostream & operator<<(std::ostream &os, const MouseButton mouse_button)
    {
        return os << "MouseButton(" << to_string(mouse_button) << ")";
    }

    ButtonModFlags operator|(const ButtonModFlags lhs, const ButtonModFlags rhs)
    {
        using T = std::underlying_type_t<ButtonModFlags>;
        return static_cast<ButtonModFlags>(static_cast<T>(lhs) | static_cast<T>(rhs));
    }

    ButtonModFlags & operator|=(ButtonModFlags &lhs, const ButtonModFlags rhs)
    {
        lhs = lhs | rhs;
        return lhs;
    }

    ButtonModFlags operator&(const ButtonModFlags lhs, const ButtonModFlags rhs)
    {
        using T = std::underlying_type_t<ButtonModFlags>;
        return static_cast<ButtonModFlags>(static_cast<T>(lhs) & static_cast<T>(rhs));
    }

    ButtonModFlags & operator&=(ButtonModFlags &lhs, const ButtonModFlags rhs)
    {
        lhs = lhs & rhs;
        return lhs;
    }

    bool operator!(const ButtonModFlags mod)
    {
        return static_cast<std::underlying_type_t<ButtonModFlags>>(mod) == 0;
    }

    std::string to_string(const ButtonModFlags button_mod_flags)
    {
        std::string result = "";

        if (!!(button_mod_flags & ButtonModFlags::Shift))
            result += ", Shift";
        if (!!(button_mod_flags & ButtonModFlags::Control))
            result += ", Control";
        if (!!(button_mod_flags & ButtonModFlags::Alt))
            result += ", Alt";
        if (!!(button_mod_flags & ButtonModFlags::Super))
            result += ", Super";
        if (!!(button_mod_flags & ButtonModFlags::CapsLock))
            result += ", CapsLock";
        if (!!(button_mod_flags & ButtonModFlags::NumLock))
            result += ", NumLock";

        if (result.empty())
            return "None";
        else
            return result.substr(2);
    }

    std::ostream & operator<<(std::ostream &os, const ButtonModFlags button_mod_flags)
    {
        return os << "ButtonModFlags(" << to_string(button_mod_flags) << ")";
    }

    std::string_view to_string(const ButtonAction button_action)
    {
        switch(button_action)
        {
            case ButtonAction::Press:   return "Press";
            case ButtonAction::Release: return "Release";
            case ButtonAction::Repeat:  return "Repeat";
            default:                    return "None";
        }
    }

    std::ostream & operator<<(std::ostream &os, const ButtonAction button_action)
    {
        return os << "ButtonAction(" << to_string(button_action) << ")";
    }
}

