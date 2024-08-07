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
        world.component<events::UncapturedCursorMoved>("events::UncapturedCursorMoved");
        world.component<events::CapturedCursorMoved>("events::CapturedCursorMoved");

        const auto &graphics = world.ensure<GraphicsModule>();

        m_window = graphics.window;

        graphics.window->set_keyboard_button_callback([this, world](const KeyboardButton button,
                                                                    const ButtonAction action,
                                                                    const ButtonModifier modifier) -> void
        {
            this->m_keyboard_state[button] = action != ButtonAction::Release;
            this->m_immediate_keyboard_state[button] = action == ButtonAction::Press;

            world.event<events::KeyboardButtonPressed>()
                    .id<InputModule>()
                    .entity(world.singleton<InputModule>())
                    .ctx({button, action, modifier})
                    .emit();
        });

        graphics.window->set_mouse_button_callback([this, world](const MouseButton button, const ButtonAction action,
                                                                 const ButtonModifier modifier) -> void
        {
            this->m_mouse_state[button] = action != ButtonAction::Release;
            this->m_immediate_mouse_state[button] = action == ButtonAction::Press;

            world.event<events::MouseButtonPressed>()
                    .id<InputModule>()
                    .entity(world.singleton<InputModule>())
                    .ctx({button, action, modifier})
                    .emit();
        });

        graphics.window->set_cursor_pos_callback([this, world](const vec2f cursor_pos) -> void
        {
            this->m_cursor_pos = cursor_pos;

            if (this->m_cursor_captured)
            {
                glfwSetCursorPos(m_window->get_raw_ptr(), 0.0f, 0.0f);

                world.event<events::CapturedCursorMoved>()
                        .id<InputModule>()
                        .entity(world.singleton<InputModule>())
                        .ctx({cursor_pos})
                        .emit();
            } else
            {
                world.event<events::UncapturedCursorMoved>()
                        .id<InputModule>()
                        .entity(world.singleton<InputModule>())
                        .ctx({cursor_pos})
                        .emit();
            }
        });

        world.system<InputModule>("systems::ClearImmediateState")
                .term_at(0).singleton()
                .kind<phases::PostTick>()
                .each(clear_immediate_state);
    }

    auto InputModule::capture_cursor() -> void
    {
        glfwSetInputMode(m_window->get_raw_ptr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (glfwRawMouseMotionSupported())
        {
            glfwSetInputMode(m_window->get_raw_ptr(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }

        m_cursor_captured = true;

        glfwSetCursorPos(m_window->get_raw_ptr(), 0.0f, 0.0f);
        m_cursor_pos = {0.0f, 0.0f};
    }

    auto InputModule::uncapture_cursor() -> void
    {
        glfwSetInputMode(m_window->get_raw_ptr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        if (glfwRawMouseMotionSupported())
        {
            glfwSetInputMode(m_window->get_raw_ptr(), GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
        }

        m_cursor_captured = false;

        double x, y;
        glfwGetCursorPos(m_window->get_raw_ptr(), &x, &y);
        m_cursor_pos = {static_cast<float>(x), static_cast<float>(y)};
    }

    auto InputModule::is_pressed(const KeyboardButton button) const -> bool
    {
        const auto it = m_keyboard_state.find(button);
        return it != m_keyboard_state.end() && it->second;
    }

    auto InputModule::is_pressed(const MouseButton button) const -> bool
    {
        const auto it = m_mouse_state.find(button);
        return it != m_mouse_state.end() && it->second;
    }

    auto InputModule::is_just_pressed(const KeyboardButton button) const -> bool
    {
        const auto it = m_immediate_keyboard_state.find(button);
        return it != m_immediate_keyboard_state.end() && it->second;
    }

    auto InputModule::is_just_pressed(const MouseButton button) const -> bool
    {
        const auto it = m_immediate_mouse_state.find(button);
        return it != m_immediate_mouse_state.end() && it->second;
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
        return m_cursor_pos;
    }

    auto InputModule::is_cursor_captured() const -> bool
    {
        return m_cursor_captured;
    }

    auto InputModule::clear_immediate_state(InputModule &input) -> void
    {
        input.m_immediate_keyboard_state.clear();
        input.m_immediate_mouse_state.clear();
    }
}
