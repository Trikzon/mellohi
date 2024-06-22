#include "mellohi/game.h"

#include "mellohi/asset_id.h"

namespace mellohi
{
    Game & Game::get()
    {
        return *s_instance;
    }

    Game::Game(const std::string_view &game_namespace)
    {
        // TODO: Error if s_instance is not nullptr
        s_instance = this;

        AssetId::set_game_namespace(game_namespace);

        m_window = std::make_unique<Window>();
    }

    void Game::run()
    {
        on_run();

        while (!m_window->should_close())
        {
            if (auto render_pass = m_window->begin_frame())
            {
                on_update(*render_pass);

                m_window->end_frame(*render_pass);
            }
        }
    }

    Window & Game::get_window() const
    {
        return *m_window;
    }
}
