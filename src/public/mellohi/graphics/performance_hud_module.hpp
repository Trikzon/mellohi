#pragma once

#include <flecs.h>

#include "mellohi/graphics/graphics_module.hpp"

namespace mellohi
{
    struct PerformanceHudModule
    {
        struct DeltaTimeStats
        {
            deque<f32> delta_times{};
            f32 sum, sum_squared, avg, stddev;

            auto calculate() -> void;
        };

        constexpr static f32 AVERAGING_PERIOD = 1.0; // 1s averaging period
        constexpr static f32 TARGET_DELTA_TIME = 1.0 / 75.0; // 75 FPS
        constexpr static f32 MAX_DELTA_TIME = 1.0 / 60.0; // 60 FPS

        DeltaTimeStats cpu_dt{};
        DeltaTimeStats gpu_dt{};

        explicit PerformanceHudModule(flecs::world &world);

        static auto track_delta_time(const flecs::iter &it, usize, const GraphicsModule &graphics,
                                     PerformanceHudModule &performance_hud) -> void;
        static auto calculate_delta_times(PerformanceHudModule &performance_hud) -> void;
        static auto render(const GraphicsModule &graphics, const PerformanceHudModule &performance_hud) -> void;
    };
}
