#pragma once

#include <flecs.h>

#include "mellohi/core/buttons.hpp"

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

        struct CursorMoved
        {
            vec2f cursor_pos{};
        };
    }

    class InputModule
    {
    public:
        InputModule(flecs::world &world);

        auto is_pressed(KeyboardButton button) const -> bool;
        auto is_pressed(MouseButton button) const -> bool;
        auto is_just_pressed(KeyboardButton button) const -> bool;
        auto is_just_pressed(MouseButton button) const -> bool;
        auto get_vector(KeyboardButton neg_x, KeyboardButton pos_x, KeyboardButton neg_y,
                        KeyboardButton pos_y) const -> vec2f;
        auto get_cursor_pos() const -> vec2f;

    private:
        hash_map<KeyboardButton, bool> m_keyboard_state{};
        hash_map<KeyboardButton, bool> m_immediate_keyboard_state{};
        hash_map<MouseButton, bool> m_mouse_state{};
        hash_map<MouseButton, bool> m_immediate_mouse_state{};
        vec2f m_cursor_pos{};

        static auto clear_immediate_state(InputModule &input) -> void;

        static auto on_keyboard_button_pressed(flecs::iter &it, usize, InputModule &input) -> void;
        static auto on_mouse_button_pressed(flecs::iter &it, usize, InputModule &input) -> void;
        static auto on_cursor_moved(flecs::iter &it, usize, InputModule &input) -> void;
    };
}
