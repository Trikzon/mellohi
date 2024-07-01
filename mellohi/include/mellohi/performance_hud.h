#pragma once

#include <deque>

#include <flecs.h>

namespace mellohi
{
    struct PerformanceHud
    {
        constexpr static float AVERAGING_PERIOD = 1.0;  // 1s averaging period
        constexpr static float TARGET_DELTA_TIME = 1.0 / 75.0;  // 75 FPS
        constexpr static float MAX_DELTA_TIME = 1.0 / 60.0;  // 60 FPS

        struct Data
        {
            std::deque<float> delta_times;
            float delta_time_sum;
            float average_delta_time;
        };

        explicit PerformanceHud(const flecs::world& world);
    };
}
