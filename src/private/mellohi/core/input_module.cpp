#include "mellohi/core/input_module.hpp"

#include "mellohi/core/engine_module.hpp"
#include "mellohi/graphics/graphics_module.hpp"

namespace mellohi
{
    InputModule::InputModule(flecs::world &world)
    {
        world.import<GraphicsModule>();

        world.component<events::KeyboardButtonPressed>("events::KeyboardButtonPressed");
        world.component<events::MouseButtonPressed>("events::MouseButtonPressed");
        world.component<events::CursorMoved>("events::MouseMoved");

        const auto &graphics = world.ensure<GraphicsModule>();

        graphics.window->set_keyboard_button_callback([world](const KeyboardButton button, const ButtonAction action,
                                                              const ButtonModifier modifier) -> void
        {
            world.event<events::KeyboardButtonPressed>()
                    .id<InputModule>()
                    .entity(world.singleton<InputModule>())
                    .ctx({button, action, modifier})
                    .emit();
        });

        graphics.window->set_mouse_button_callback([world](const MouseButton button, const ButtonAction action,
                                                           const ButtonModifier modifier) -> void
        {
            world.event<events::MouseButtonPressed>()
                    .id<InputModule>()
                    .entity(world.singleton<InputModule>())
                    .ctx({button, action, modifier})
                    .emit();
        });

        graphics.window->set_cursor_pos_callback([world](const vec2f cursor_pos) -> void
        {
            world.event<events::CursorMoved>()
                    .id<InputModule>()
                    .entity(world.singleton<InputModule>())
                    .ctx({cursor_pos})
                    .emit();
        });

        world.system<InputModule>("systems::ClearImmediateState")
                .term_at(0).singleton()
                .kind<phases::PostTick>()
                .each(systems::clear_immediate_state);

        world.observer<InputModule>("observers::OnKeyboardButtonPressed")
                .term_at(0).singleton()
                .event<events::KeyboardButtonPressed>()
                .each(systems::on_keyboard_button_pressed);

        world.observer<InputModule>("observers::OnMouseButtonPressed")
                .term_at(0).singleton()
                .event<events::MouseButtonPressed>()
                .each(systems::on_mouse_button_pressed);

        world.observer<InputModule>("observers::OnCursorMoved")
                .term_at(0).singleton()
                .event<events::CursorMoved>()
                .each(systems::on_cursor_moved);
    }

    auto InputModule::is_pressed(const KeyboardButton button) const -> bool
    {
        const auto it = keyboard_state.find(button);
        return it != keyboard_state.end() && it->second;
    }

    auto InputModule::is_pressed(const MouseButton button) const -> bool
    {
        const auto it = mouse_state.find(button);
        return it != mouse_state.end() && it->second;
    }

    auto InputModule::is_just_pressed(const KeyboardButton button) const -> bool
    {
        const auto it = immediate_keyboard_state.find(button);
        return it != immediate_keyboard_state.end() && it->second;
    }

    auto InputModule::is_just_pressed(const MouseButton button) const -> bool
    {
        const auto it = immediate_mouse_state.find(button);
        return it != immediate_mouse_state.end() && it->second;
    }

    auto InputModule::get_vector(const KeyboardButton neg_x, const KeyboardButton pos_x, const KeyboardButton neg_y,
                                 const KeyboardButton pos_y) const -> vec2f
    {
        float x = 0.0f, y = 0.0f;

        if (is_pressed(neg_x)) { x -= 1.0f; }
        if (is_pressed(pos_x)) { x += 1.0f; }
        if (is_pressed(neg_y)) { y -= 1.0f; }
        if (is_pressed(pos_y)) { y += 1.0f; }

        return {x, y};
    }

    auto InputModule::get_cursor_pos() const -> vec2f
    {
        return cursor_pos;
    }

    namespace systems
    {
        auto clear_immediate_state(InputModule &input) -> void
        {
            input.immediate_keyboard_state.clear();
            input.immediate_mouse_state.clear();
        }

        auto on_keyboard_button_pressed(flecs::iter &it, usize, InputModule &input) -> void
        {
            const auto *event = static_cast<events::KeyboardButtonPressed *>(it.param());

            input.keyboard_state[event->button] = event->action != ButtonAction::Release;
            input.immediate_keyboard_state[event->button] = event->action == ButtonAction::Press;
        }

        auto on_mouse_button_pressed(flecs::iter &it, usize, InputModule &input) -> void
        {
            const auto *event = static_cast<events::MouseButtonPressed *>(it.param());

            input.mouse_state[event->button] = event->action != ButtonAction::Release;
            input.immediate_mouse_state[event->button] = event->action == ButtonAction::Press;
        }

        auto on_cursor_moved(flecs::iter &it, usize, InputModule &input) -> void
        {
            const auto *event = static_cast<events::CursorMoved *>(it.param());

            input.cursor_pos = event->cursor_pos;
        }
    }
}
