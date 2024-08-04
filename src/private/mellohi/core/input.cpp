#include "mellohi/core/input.hpp"

#include "mellohi/core/engine.hpp"
#include "mellohi/graphics/graphics.hpp"

namespace mellohi
{
    Input::Input(flecs::world &ecs)
    {
        ecs.import<Graphics>();

        const auto *window = ecs.get<glfw::Window>();

        window->set_keyboard_button_callback([ecs](const KeyboardButton button, const ButtonAction action,
                                                   const ButtonModifier modifier) -> void
        {
            ecs.event<event::KeyboardButtonPressed>()
                    .id<Input>()
                    .entity(ecs.singleton<Input>())
                    .ctx({button, action, modifier})
                    .emit();
        });

        window->set_mouse_button_callback([ecs](const MouseButton button, const ButtonAction action,
                                                const ButtonModifier modifier) -> void
        {
            ecs.event<event::MouseButtonPressed>()
                    .id<Input>()
                    .entity(ecs.singleton<Input>())
                    .ctx({button, action, modifier})
                    .emit();
        });

        window->set_cursor_pos_callback([ecs](const vec2f new_pos) -> void
        {
            ecs.event<event::MouseMoved>()
                    .id<Input>()
                    .entity(ecs.singleton<Input>())
                    .ctx({new_pos})
                    .emit();
        });

        ecs.system<Input>()
                .term_at(0).singleton()
                .kind<phase::PostTick>()
                .each(end_tick);

        ecs.observer<Input>()
                .term_at(0).singleton()
                .event<event::KeyboardButtonPressed>()
                .each(on_keyboard_button_pressed);

        ecs.observer<Input>()
                .term_at(0).singleton()
                .event<event::MouseButtonPressed>()
                .each(on_mouse_button_pressed);

        ecs.observer<Input>()
                .term_at(0).singleton()
                .event<event::MouseMoved>()
                .each(on_mouse_moved);
    }

    auto Input::is_pressed(const KeyboardButton button) const -> bool
    {
        const auto it = m_keyboard_state.find(button);
        return it != m_keyboard_state.end() && it->second;
    }

    bool Input::is_pressed(const MouseButton button) const
    {
        const auto it = m_mouse_state.find(button);
        return it != m_mouse_state.end() && it->second;
    }

    auto Input::is_just_pressed(const KeyboardButton button) const -> bool
    {
        const auto it = m_immediate_keyboard_state.find(button);
        return it != m_immediate_keyboard_state.end() && it->second;
    }

    bool Input::is_just_pressed(const MouseButton button) const
    {
        const auto it = m_immediate_mouse_state.find(button);
        return it != m_immediate_mouse_state.end() && it->second;
    }

    auto Input::get_vector(const KeyboardButton neg_x, const KeyboardButton pos_x, const KeyboardButton neg_y,
                           const KeyboardButton pos_y) const -> vec2u
    {
        float x = 0.0f, y = 0.0f;

        if (is_pressed(neg_x)) { x -= 1.0f; }
        if (is_pressed(pos_x)) { x += 1.0f; }
        if (is_pressed(neg_y)) { y -= 1.0f; }
        if (is_pressed(pos_y)) { y += 1.0f; }

        return {x, y};
    }

    vec2f Input::get_mouse_pos() const
    {
        return m_mouse_pos;
    }

    auto Input::on_keyboard_button_pressed(flecs::iter &it, usize, Input &input) -> void
    {
        const auto event = static_cast<event::KeyboardButtonPressed *>(it.param());

        input.m_keyboard_state[event->button] = event->action != ButtonAction::Release;
        input.m_immediate_keyboard_state[event->button] = event->action == ButtonAction::Press;
    }

    auto Input::on_mouse_button_pressed(flecs::iter &it, usize, Input &input) -> void
    {
        const auto event = static_cast<event::MouseButtonPressed *>(it.param());

        input.m_mouse_state[event->button] = event->action != ButtonAction::Release;
        input.m_immediate_mouse_state[event->button] = event->action == ButtonAction::Press;
    }

    auto Input::on_mouse_moved(flecs::iter &it, usize, Input &input) -> void
    {
        const auto event = static_cast<event::MouseMoved *>(it.param());

        input.m_mouse_pos = event->new_pos;
    }

    auto Input::end_tick(Input &input) -> void
    {
        input.m_immediate_keyboard_state.clear();
        input.m_immediate_mouse_state.clear();
    }
}
