#pragma once

#include "graphics/window.h"

namespace mellohi
{
    class Game
    {
    public:
        static Game & get();

        explicit Game(const std::string_view &game_namespace);
        virtual ~Game() = default;

        void run();

        Window & get_window() const;

    private:
        inline static Game *s_instance;

        std::unique_ptr<Window> m_window;

        virtual void on_run() { };
        virtual void on_update(wgpu::RenderPassEncoder render_pass) { };
    };
}
