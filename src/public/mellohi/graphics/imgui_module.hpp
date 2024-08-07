#pragma once

#include <flecs.h>

#include "mellohi/graphics/graphics_module.hpp"

namespace mellohi
{
    struct ImGuiModule
    {
        explicit ImGuiModule(flecs::world &world);
        ~ImGuiModule();
    };

    namespace systems
    {
        auto new_frame() -> void;
        auto end_frame(const GraphicsModule &graphics) -> void;
    }
}
