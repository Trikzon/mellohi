#include "mellohi/game.h"

#include <GLFW/glfw3.h>

#include "mellohi/asset_id.h"
#include "mellohi/nodes/performance_hud_node.h"

namespace mellohi
{
    Game::Game(const std::string_view &game_namespace)
    {
        // TODO: Error if s_instance is not nullptr
        s_instance = this;

        AssetId::set_game_namespace(game_namespace);

        m_window = std::make_unique<Window>();
        m_root_node = std::make_shared<Node>("Root");

        m_root_node->add_child(std::make_shared<PerformanceHudNode>());
    }

    Game & Game::get()
    {
        return *s_instance;
    }

    void Game::run()
    {
        on_run();

        double previous_time = glfwGetTime();

        while (!m_window->should_close())
        {
            if (auto render_pass = m_window->begin_frame())
            {
                const double current_time = glfwGetTime();
                const double delta_time = current_time - previous_time;
                previous_time = current_time;

                on_update(delta_time);
                m_root_node->update(delta_time);

                on_render(*render_pass, delta_time);
                m_root_node->render(*render_pass, delta_time);

                m_window->end_frame(*render_pass);
            }
        }
    }

    Window & Game::get_window() const
    {
        return *m_window;
    }

    Node & Game::get_root_node() const
    {
        return *m_root_node;
    }
}
