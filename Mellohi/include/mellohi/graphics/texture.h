#pragma once

#include <string>

#include "mellohi/types.h"

namespace mellohi
{
    class Texture
    {
    public:
        explicit Texture(const std::string& path);
        ~Texture();

        void bind(u32 slot = 0) const;

    private:
        u32 m_texture_id;
        i32 m_width, m_height, m_channels;
    };
}
