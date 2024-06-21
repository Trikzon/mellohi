#pragma once

#include "graphics/window.h"

namespace mellohi
{
    class Game
    {
    public:
        Game();
        virtual ~Game() = default;

        void run();

        Window& get_window() const;

    private:
        std::unique_ptr<Window> m_window;

        virtual void on_run() { };
        virtual void on_update(wgpu::RenderPassEncoder render_pass) { };
    };
}
