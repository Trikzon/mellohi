#include "mellohi/app.h"

#include "mellohi/asset_id.h"

namespace mellohi
{
    App::App(const std::string_view& app_name) : m_root_node("RootNode")
    {
        AssetId::set_default_namespace(app_name);
    }

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
