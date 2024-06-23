#pragma once

#include <deque>

#include "mellohi/nodes/node.h"

namespace mellohi
{
    class PerformanceHudNode final : public Node
    {
    public:
        explicit PerformanceHudNode(const std::string_view& name = "PerformanceHudNode");

    private:
        constexpr static double AVERAGING_PERIOD = 1.5;  // 1.5s averaging period
        constexpr static double OUTLIER_PERCENTAGE = 0.15;  // 15%

        std::deque<double> m_delta_times;
        double m_delta_time_sum;

        void on_update(double delta_time) override;
        void on_render(RenderPass &render_pass, double delta_time) override;

        static const char * backend_type_to_name(wgpu::BackendType wgpu_backend_type);
    };
}
