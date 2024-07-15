#include "mellohi/game.hpp"

#include <GLFW/glfw3.h>

#include "mellohi/graphics/window.hpp"
#include "mellohi/input.hpp"

namespace mellohi
{
    Game::Game(const std::string_view &game_namespace)
    {
        AssetId::set_game_namespace(game_namespace);

        m_world.entity<PostRender>()
            .add(flecs::Final)
            .add(flecs::Phase)
            .depends_on(flecs::OnStore);

        m_world.add<Window>();

        const auto window = m_world.get<Window>();
        glfwSetWindowUserPointer(window->get_raw(), this);

        m_world.import<input::Module>();
    }

    void Game::run() const
    {
        const auto window = m_world.get<Window>();

        f64 previous_time = glfwGetTime();

        bool should_run = true;
        while (!window->should_close() && should_run)
        {
            const f64 current_time = glfwGetTime();
            const f64 delta_time = current_time - previous_time;
            previous_time = current_time;

            std::optional<RenderPass> render_pass = window->begin_frame();

            // TODO: Fix crash that happens when there's no render pass.
            if (render_pass)
            {
                m_world.emplace<RenderPass>(*render_pass);
            }

            should_run &= m_world.progress(static_cast<f32>(delta_time));

            if (render_pass)
            {
                window->end_frame(*render_pass);

                m_world.remove<RenderPass>();
            }
        }
    }

    flecs::world & Game::get_world()
    {
        return m_world;
    }
}
