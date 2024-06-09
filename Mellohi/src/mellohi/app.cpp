#include "mellohi/app.h"

namespace mellohi
{
    App::App() : m_root_node("RootNode") { }

    void App::run()
    {
        on_run();

        while (!m_window.should_close())
        {
            m_window.begin_frame();

            on_update();
            m_root_node.update();

            m_root_node.draw_imgui();

            m_window.end_frame();
        }
    }
}
