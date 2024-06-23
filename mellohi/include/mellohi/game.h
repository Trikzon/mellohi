#pragma once

#include "mellohi/graphics/render_pass.h"
#include "mellohi/graphics/window.h"
#include "mellohi/nodes/node.h"

namespace mellohi
{
    class Game
    {
    public:
        explicit Game(const std::string_view &game_namespace);
        virtual ~Game() = default;

        static Game & get();

        void run();

        Window & get_window() const;
        Node & get_root_node() const;

    private:
        inline static Game *s_instance;

        std::unique_ptr<Window> m_window;
        std::shared_ptr<Node> m_root_node;

        virtual void on_run() { }
        virtual void on_update(double delta_time) { }
        virtual void on_render(RenderPass &render_pass, double delta_time) { }
    };
}
