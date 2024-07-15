#pragma once

#include <flecs.h>

#include "mellohi/graphics/render_pass.hpp"

namespace mellohi
{
    class PostRender { };

    class Game
    {
    public:
        explicit Game(const std::string_view &game_namespace);

        void run() const;

        flecs::world & get_world();
    private:
        flecs::world m_world;
    };
}
