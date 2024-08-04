#pragma once

#include <flecs.h>

#include "mellohi/core/buttons.hpp"

namespace mellohi
{
    namespace event
    {
        struct KeyboardButtonPressed
        {
            KeyboardButton button{};
            ButtonAction action{};
            ButtonModifier modifier{};
        };

        struct MouseButtonPressed
        {
            MouseButton button{};
            ButtonAction action{};
            ButtonModifier modifier{};
        };

        struct MouseMoved
        {
            vec2f new_pos{};
        };
    }

    class Input
    {
    public:
        Input(flecs::world &ecs);

        auto is_pressed(KeyboardButton button) const -> bool;
        bool is_pressed(MouseButton button) const;
        auto is_just_pressed(KeyboardButton button) const -> bool;
        bool is_just_pressed(MouseButton button) const;
        auto get_vector(KeyboardButton neg_x, KeyboardButton pos_x, KeyboardButton neg_y,
                        KeyboardButton pos_y) const -> vec2u;
        vec2f get_mouse_pos() const;

    private:
        hash_map<KeyboardButton, bool> m_keyboard_state{};
        hash_map<KeyboardButton, bool> m_immediate_keyboard_state{};
        hash_map<MouseButton, bool> m_mouse_state{};
        hash_map<MouseButton, bool> m_immediate_mouse_state{};
        vec2f m_mouse_pos{};

        static auto on_keyboard_button_pressed(flecs::iter &it, usize, Input &input) -> void;
        static auto on_mouse_button_pressed(flecs::iter &it, usize, Input &input) -> void;
        static auto on_mouse_moved(flecs::iter &it, usize, Input &input) -> void;
        static auto end_tick(Input &input) -> void;
    };
}
