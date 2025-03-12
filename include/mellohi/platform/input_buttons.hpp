#pragma once

#include "mellohi/core/types.hpp"

namespace mellohi
{
    enum class Key : u32
    {
        None,
        Space,
        Apostrophe,
        Comma,
        Minus,
        Period,
        Slash,
        Zero,
        One,
        Two,
        Three,
        Four,
        Five,
        Six,
        Seven,
        Eight,
        Nine,
        Semicolon,
        Equal,
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        LeftBracket,
        Backslash,
        RightBracket,
        GraveAccent,
        Escape,
        Enter,
        Tab,
        Backspace,
        Insert,
        Delete,
        Right,
        Left,
        Down,
        Up,
        PageUp,
        PageDown,
        Home,
        End,
        CapsLock,
        ScrollLock,
        NumLock,
        PrintScreen,
        Pause,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,
        F25,
        NumPadZero,
        NumPadOne,
        NumPadTwo,
        NumPadThree,
        NumPadFour,
        NumPadFive,
        NumPadSix,
        NumPadSeven,
        NumPadEight,
        NumPadNine,
        NumPadDecimal,
        NumPadDivide,
        NumPadMultiply,
        NumPadSubtract,
        NumPadAdd,
        NumPadEnter,
        NumPadEqual,
        LeftShift,
        LeftControl,
        LeftAlt,
        LeftSuper,
        RightShift,
        RightControl,
        RightAlt,
        RightSuper,
        Menu,
    };

    std::string_view to_string(Key key);
    std::ostream & operator<<(std::ostream &os, Key key);

    enum class MouseButton : u32
    {
        None,
        Left,
        Right,
        Middle,
        Four,
        Five,
        Six,
        Seven,
        Eight,
    };

    std::string_view to_string(MouseButton mouse_button);
    std::ostream & operator<<(std::ostream &os, MouseButton mouse_button);

    enum class ButtonModFlags : u32
    {
        None     = 0,
        Shift    = 1,
        Control  = 2,
        Alt      = 4,
        Super    = 8,
        CapsLock = 16,
        NumLock  = 32,
    };

    ButtonModFlags operator|(ButtonModFlags lhs, ButtonModFlags rhs);
    ButtonModFlags & operator|=(ButtonModFlags &lhs, ButtonModFlags rhs);
    ButtonModFlags operator&(ButtonModFlags lhs, ButtonModFlags rhs);
    ButtonModFlags & operator&=(ButtonModFlags &lhs, ButtonModFlags rhs);
    bool operator!(ButtonModFlags mod);

    std::string to_string(ButtonModFlags button_mod_flags);
    std::ostream & operator<<(std::ostream &os, ButtonModFlags button_mod_flags);

    enum class ButtonAction : u32
    {
        None,
        Press,
        Release,
        Repeat,
    };

    std::string_view to_string(ButtonAction button_action);
    std::ostream & operator<<(std::ostream &os, ButtonAction button_action);
}

