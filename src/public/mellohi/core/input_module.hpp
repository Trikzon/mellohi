#pragma once

#include <flecs.h>

#include "mellohi/core/buttons.hpp"
#include "mellohi/graphics/glfw/window.hpp"

namespace mellohi
{
    namespace events
    {
        struct KeyboardButtonPressed
        {
            KeyboardButton button;
            ButtonAction action;
            ButtonModifier modifier;
        };

        struct MouseButtonPressed
        {
            MouseButton button;
            ButtonAction action;
            ButtonModifier modifier;
        };

        struct UncapturedCursorMoved
        {
            vec2f cursor_pos{};
        };

        struct CapturedCursorMoved
        {
            vec2f cursor_delta{};
        };
    }

    class InputModule
    {
    public:
        InputModule(flecs::world &world);

        auto capture_cursor() -> void;
        auto uncapture_cursor() -> void;

        auto is_pressed(KeyboardButton button) const -> bool;
        auto is_pressed(MouseButton button) const -> bool;
        auto is_just_pressed(KeyboardButton button) const -> bool;
        auto is_just_pressed(MouseButton button) const -> bool;
        auto get_vector(KeyboardButton neg_x, KeyboardButton pos_x, KeyboardButton neg_y,
                        KeyboardButton pos_y) const -> vec2f;
        auto get_cursor_pos() const -> vec2f;
        auto is_cursor_captured() const -> bool;

    private:
        s_ptr<glfw::Window> m_window;
        hash_map<KeyboardButton, bool> m_keyboard_state{};
        hash_map<KeyboardButton, bool> m_immediate_keyboard_state{};
        hash_map<MouseButton, bool> m_mouse_state{};
        hash_map<MouseButton, bool> m_immediate_mouse_state{};
        vec2f m_cursor_pos{};
        bool m_cursor_captured{false};

        static auto clear_immediate_state(InputModule &input) -> void;
    };
}
