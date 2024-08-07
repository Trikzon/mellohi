#pragma once

#include <flecs.h>

namespace mellohi
{
    namespace phases
    {
        struct PreTick {};

        struct PreUpdate {};

        struct Update {};

        struct PreRender {};

        struct Render {};

        struct RenderImGui {};

        struct PostRender {};

        struct PostTick {};
    }

    struct EngineModule
    {
        explicit EngineModule(flecs::world &world);
    };
}
