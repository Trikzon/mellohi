#include "mellohi/input.hpp"

#include "mellohi/game.hpp"

namespace mellohi
{

    namespace input
    {
        Module::Module(const flecs::world &world)
        {
            {
                const auto *window = world.get<Window>();
                world.emplace<Input>(*window);
            }

            {
                auto *input = world.get_mut<Input>();

                world.observer<const KeyboardEvent>()
                    .event(flecs::OnSet)
                    .each([input](const KeyboardEvent &event)
                    {
                        input->on_keyboard_input(event);
                    });
                world.observer<const MouseEvent>()
                    .event(flecs::OnSet)
                    .each([input](const MouseEvent &event)
                    {
                        input->on_mouse_input(event);
                    });
                world.observer<const MouseMoveEvent>()
                    .event(flecs::OnSet)
                    .each([input](const MouseMoveEvent &event)
                    {
                        input->on_mouse_move(event);
                    });
            }

            world.system<const KeyboardEvent>()
                .kind(flecs::OnStore)
                .each([](const flecs::entity e, const KeyboardEvent &event)
                {
                    e.destruct();
                });
            world.system<const MouseEvent>()
                .kind(flecs::OnStore)
                .each([](const flecs::entity e, const MouseEvent &event)
                {
                    e.destruct();
                });
            world.system<const MouseMoveEvent>()
                .kind(flecs::OnStore)
                .each([](const flecs::entity e, const MouseMoveEvent &event)
                {
                    e.destruct();
                });
            world.system<Input>()
                .kind(flecs::OnStore)
                .each([](Input &input)
                {
                    input.end_frame();
                });
        }
    }

    void on_glfw_set_key(GLFWwindow *window, const i32 key, const i32 scancode, const i32 action, const i32 mods)
    {
        const auto &world = static_cast<Game *>(glfwGetWindowUserPointer(window))->get_world();

        world.entity()
            .set<input::KeyboardEvent>({
                .button = static_cast<KeyboardButton>(key),
                .modifier = static_cast<ButtonModifier>(mods),
                .action = static_cast<ButtonAction>(action)
            });
    }

    void on_glfw_set_mouse_button(GLFWwindow* window, int button, int action, int mods)
    {
        const auto &world = static_cast<Game *>(glfwGetWindowUserPointer(window))->get_world();

        world.entity()
            .set<input::MouseEvent>({
                .button = static_cast<MouseButton>(button),
                .modifier = static_cast<ButtonModifier>(mods),
                .action = static_cast<ButtonAction>(action)
            });
    }

    void on_glfw_set_cursor_pos(GLFWwindow* window, double x_pos, double y_pos)
    {
        const auto &world = static_cast<Game *>(glfwGetWindowUserPointer(window))->get_world();

        world.entity()
            .set<input::MouseMoveEvent>({
                .position = {x_pos, y_pos}
            });
    }

    Input::Input(const Window &window)
    {
        glfwSetKeyCallback(window.get_raw(), on_glfw_set_key);
        glfwSetMouseButtonCallback(window.get_raw(), on_glfw_set_mouse_button);
        glfwSetCursorPosCallback(window.get_raw(), on_glfw_set_cursor_pos);
    }

    bool Input::is_pressed(const KeyboardButton button) const
    {
        const auto it = m_keyboard_state.find(button);
        return it != m_keyboard_state.end() && it->second;
    }

    bool Input::is_pressed(const MouseButton button) const
    {
        const auto it = m_mouse_state.find(button);
        return it != m_mouse_state.end() && it->second;
    }

    bool Input::is_just_pressed(const KeyboardButton button) const
    {
        const auto it = m_immediate_keyboard_state.find(button);
        return it != m_immediate_keyboard_state.end() && it->second;
    }

    bool Input::is_just_pressed(const MouseButton button) const
    {
        const auto it = m_immediate_mouse_state.find(button);
        return it != m_immediate_mouse_state.end() && it->second;
    }

    vec2f Input::get_mouse_position() const
    {
        return m_mouse_position;
    }

    void Input::on_keyboard_input(const input::KeyboardEvent &event)
    {
        m_keyboard_state[event.button] = event.action == ButtonAction::Press;
        m_immediate_keyboard_state[event.button] = event.action == ButtonAction::Press;
    }

    void Input::on_mouse_input(const input::MouseEvent &event)
    {
        m_mouse_state[event.button] = event.action == ButtonAction::Press;
        m_immediate_mouse_state[event.button] = event.action == ButtonAction::Press;
    }

    void Input::on_mouse_move(const input::MouseMoveEvent &event)
    {
        m_mouse_position = event.position;
    }

    void Input::end_frame()
    {
        m_immediate_keyboard_state.clear();
        m_immediate_mouse_state.clear();
    }
}
