#pragma once

#include "graphics/window.h"
#include "nodes/node.h"

namespace mellohi
{
    class App
    {
    public:
        explicit App(const std::string_view& app_name);
        virtual ~App() = default;

        void run();

        virtual void on_run() { };
        virtual void on_update() { };

    protected:
        Window m_window;
        Node m_root_node;
    };
}
