#pragma once

#include <unordered_map>

#include <flecs.h>

#include "mellohi/buttons.hpp"
#include "mellohi/graphics/window.hpp"
#include "mellohi/types.hpp"

namespace mellohi
{
    namespace input
    {
        struct KeyboardEvent
        {
            KeyboardButton button;
            ButtonModifier modifier;
            ButtonAction action;
        };

        struct MouseEvent
        {
            MouseButton button;
            ButtonModifier modifier;
            ButtonAction action;
        };

        struct MouseMoveEvent
        {
            vec2f position;
        };

        struct Module
        {
            explicit Module(const flecs::world &world);
        };
    }

    class Input
    {
    public:
        explicit Input(const Window &window);

        bool is_pressed(KeyboardButton button) const;
        bool is_pressed(MouseButton button) const;
        bool is_just_pressed(KeyboardButton button) const;
        bool is_just_pressed(MouseButton button) const;
        vec2f get_vector(KeyboardButton neg_x, KeyboardButton pos_x, KeyboardButton neg_y, KeyboardButton pos_y) const;
        vec2f get_mouse_position() const;

    private:
        std::unordered_map<KeyboardButton, bool> m_keyboard_state;
        std::unordered_map<KeyboardButton, bool> m_immediate_keyboard_state;
        std::unordered_map<MouseButton, bool> m_mouse_state;
        std::unordered_map<MouseButton, bool> m_immediate_mouse_state;
        vec2f m_mouse_position{};

        void on_keyboard_input(const input::KeyboardEvent &event);
        void on_mouse_input(const input::MouseEvent &event);
        void on_mouse_move(const input::MouseMoveEvent &event);
        void end_frame();

        friend struct input::Module;
    };
}
