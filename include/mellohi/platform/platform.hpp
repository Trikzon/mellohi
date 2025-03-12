#pragma once

#include "mellohi/platform/window.hpp"

namespace mellohi
{
    class Platform
    {
    public:
        Platform();
        ~Platform();

        std::shared_ptr<Window> create_window(uvec2 size, bool resizable, const std::string &title);
        void process_events();

    private:
        struct ImplData;

        std::unique_ptr<ImplData> m_data;
    };
}

