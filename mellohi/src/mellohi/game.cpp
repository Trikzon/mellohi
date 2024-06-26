#include "mellohi/game.h"

#include <GLFW/glfw3.h>

#include "mellohi/asset_id.h"
#include "mellohi/graphics/window.h"

namespace mellohi
{
    Game::Game(const std::string_view &game_namespace)
    {
        AssetId::set_game_namespace(game_namespace);

        m_world.add<Window>();
    }

    void Game::run() const
    {
        const auto window = m_world.get<Window>();

        double previous_time = glfwGetTime();

        bool should_run = true;
        while (!window->should_close() && should_run)
        {
            const double current_time = glfwGetTime();
            const double delta_time = current_time - previous_time;
            previous_time = current_time;

            std::optional<RenderPass> render_pass = window->begin_frame();

            // TODO: Fix crash that happens when there's no render pass.
            if (render_pass)
            {
                m_world.emplace<RenderPass>(*render_pass);
            }

            should_run &= m_world.progress(static_cast<float>(delta_time));

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
