#include "mellohi/nodes/performance_hud_node.h"

#include <format>

#include <imgui.h>

#include "mellohi/game.h"

namespace mellohi
{
    PerformanceHudNode::PerformanceHudNode(const std::string_view &name) : Node(name), m_delta_time_sum(0) { }

    void PerformanceHudNode::on_update(const double delta_time)
    {
        while (m_delta_time_sum > AVERAGING_PERIOD && !m_delta_times.empty())
        {
            m_delta_time_sum -= m_delta_times.front();
            m_delta_times.pop_front();
        }

        m_delta_times.push_back(delta_time);
        m_delta_time_sum += delta_time;
    }

    void PerformanceHudNode::on_render(RenderPass &render_pass, const double delta_time)
    {
        constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration
                                                | ImGuiWindowFlags_AlwaysAutoResize
                                                | ImGuiWindowFlags_NoSavedSettings
                                                | ImGuiWindowFlags_NoFocusOnAppearing
                                                | ImGuiWindowFlags_NoNav
                                                | ImGuiWindowFlags_NoMouseInputs;
        constexpr float margin = 10.0f;
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        const ImVec2 pos = viewport->Pos;
        const ImVec2 size = viewport->Size;
        ImGui::SetNextWindowPos({ size.x - margin, pos.y + margin }, ImGuiCond_Always, { 1.0, 0.0 });
        ImGui::SetNextWindowBgAlpha(0.35f);

        if (ImGui::Begin("Performance HUD", nullptr, window_flags))
        {
            constexpr ImVec4 red = { 0.9, 0.6, 0.5, 1.0 };
            constexpr ImVec4 green = { 0.7, 0.9, 0.5, 1.0 };
            constexpr ImVec4 blue = { 0.5, 0.7, 0.9, 1.0 };
            constexpr ImVec4 white = { 1.0, 1.0, 1.0, 1.0 };

            const double avg_delta_time = m_delta_time_sum / static_cast<double>(m_delta_times.size());
            const double min_delta_time = *std::min_element(m_delta_times.begin(), m_delta_times.end());
            const double max_delta_time = *std::max_element(m_delta_times.begin(), m_delta_times.end());

            const auto wgpu_properties = Game::get().get_window().get_device().get_wgpu_properties();
            ImGui::Text("%s (%s)", wgpu_properties.name, backend_type_to_name(wgpu_properties.backendType));

            std::string formatted = std::format("{:6.2f}", 1 / avg_delta_time);
            ImGui::Text("FPS: %s [", formatted.c_str()); ImGui::SameLine(0, 0);
            formatted = std::format("{:6.2f}", 1 / max_delta_time);
            bool outlier = max_delta_time > avg_delta_time * (1 + OUTLIER_PERCENTAGE);
            ImGui::TextColored(outlier ? red : white, "%s", formatted.c_str()); ImGui::SameLine();
            formatted = std::format("{:6.2f}", 1 / min_delta_time);
            outlier = min_delta_time < avg_delta_time * (1 - OUTLIER_PERCENTAGE);
            ImGui::TextColored(outlier ? red : white, "%s", formatted.c_str()); ImGui::SameLine(0, 0);
            ImGui::Text("]");

            formatted = std::format("{:6.2f}", avg_delta_time * 1000);
            ImGui::TextColored(blue, "dt:  %s [", formatted.c_str()); ImGui::SameLine(0, 0);
            formatted = std::format("{:6.2f}", min_delta_time * 1000);
            outlier = min_delta_time < avg_delta_time * (1 - OUTLIER_PERCENTAGE);
            ImGui::TextColored(outlier ? red : blue, "%s", formatted.c_str()); ImGui::SameLine();
            formatted = std::format("{:6.2f}", max_delta_time * 1000);
            outlier = max_delta_time > avg_delta_time * (1 + OUTLIER_PERCENTAGE);
            ImGui::TextColored(outlier ? red : blue, "%s", formatted.c_str()); ImGui::SameLine(0, 0);
            ImGui::TextColored(blue, "]");

            // ImGui::TextColored(green, "GPU: ");
            // ImGui::Text("Mem: ");
        }
        ImGui::End();
    }

    const char * PerformanceHudNode::backend_type_to_name(wgpu::BackendType wpgu_backend_type)
    {
        switch (wpgu_backend_type)
        {
            case wgpu::BackendType::WebGPU: return "WebGPU";
            case wgpu::BackendType::D3D11: return "D3D11";
            case wgpu::BackendType::D3D12: return "D3D12";
            case wgpu::BackendType::Metal: return "Metal";
            case wgpu::BackendType::Vulkan: return "Vulkan";
            case wgpu::BackendType::OpenGL: return "OpenGL";
            case wgpu::BackendType::OpenGLES: return "OpenGLES";
            default: return "Undefined";
        }
    }
}
