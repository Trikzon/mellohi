#include "mellohi/game.h"

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

        bool should_run = true;
        while (!window->should_close() && should_run)
        {
            std::optional<RenderPass> render_pass = window->begin_frame();

            if (render_pass)
            {
                m_world.emplace<RenderPass>(*render_pass);
            }

            should_run &= m_world.progress();

            if (render_pass)
            {
                window->end_frame(*render_pass);

                m_world.remove<RenderPass>();
            }
        }
    }

    const flecs::world & Game::get_world() const
    {
        return m_world;
    }
}
