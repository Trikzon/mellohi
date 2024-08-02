#pragma once

#include <flecs.h>

namespace mellohi
{
    namespace phase
    {
        struct PreTick {};

        struct PreUpdate {};

        struct PreRender {};

        struct Render {};

        struct PostRender {};

        struct PostTick {};
    }

    struct Engine
    {
        explicit Engine(flecs::world &ecs);
    };
}
