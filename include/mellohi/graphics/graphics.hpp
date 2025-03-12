#pragma once

#include "mellohi/platform/platform.hpp"

#include <memory>

namespace mellohi
{
    class Graphics
    {
    public:
        Graphics(const Platform &platform);
        ~Graphics();

        void draw_frame();

    private:
        struct ImplData;

        std::unique_ptr<ImplData> m_data;
    };
}

