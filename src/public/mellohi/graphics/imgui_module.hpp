#pragma once

#include <flecs.h>

#include "mellohi/graphics/graphics_module.hpp"

namespace mellohi
{
    struct ImGuiModule
    {
        explicit ImGuiModule(flecs::world &world);
        ~ImGuiModule();

        static auto new_frame() -> void;
        static auto end_frame(const GraphicsModule &graphics) -> void;
    };
}
