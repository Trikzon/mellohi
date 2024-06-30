#pragma once

#include <flecs.h>

#include "mellohi/graphics/render_pass.h"

namespace mellohi
{
    class Game
    {
    public:
        explicit Game(const std::string_view &game_namespace);

        void run() const;

        const flecs::world & get_world() const;
    private:
        flecs::world m_world;
    };
}
