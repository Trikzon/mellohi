#pragma once

#include <deque>

#include <flecs.h>

#include "mellohi/types.hpp"

namespace mellohi
{
    struct PerformanceHud
    {
        constexpr static f32 AVERAGING_PERIOD = 1.0;  // 1s averaging period
        constexpr static f32 TARGET_DELTA_TIME = 1.0 / 75.0;  // 75 FPS
        constexpr static f32 MAX_DELTA_TIME = 1.0 / 60.0;  // 60 FPS

        struct Data
        {
            std::deque<f32> delta_times;
            f32 delta_time_sum;
            f32 average_delta_time;
        };

        explicit PerformanceHud(const flecs::world& world);
    };
}
