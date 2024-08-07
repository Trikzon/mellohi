#include "mellohi/graphics/performance_hud_module.hpp"

#include <imgui.h>

#include "mellohi/core/engine_module.hpp"
#include "mellohi/graphics/imgui_module.hpp"

namespace mellohi
{
    const char * backend_type_to_name(const WGPUBackendType wpgu_backend_type)
    {
        switch (wpgu_backend_type)
        {
            case WGPUBackendType_WebGPU: return "WebGPU";
            case WGPUBackendType_D3D11: return "D3D11";
            case WGPUBackendType_D3D12: return "D3D12";
            case WGPUBackendType_Metal: return "Metal";
            case WGPUBackendType_Vulkan: return "Vulkan";
            case WGPUBackendType_OpenGL: return "OpenGL";
            case WGPUBackendType_OpenGLES: return "OpenGLES";
            default: return "Undefined";
        }
    }

    PerformanceHudModule::PerformanceHudModule(flecs::world &world)
    {
        world.import<ImGuiModule>();

        world.system<PerformanceHudModule>("systems::TrackDeltaTime")
                .term_at(0).singleton()
                .kind<phases::Update>()
                .each(systems::track_delta_time);

        world.system<PerformanceHudModule>("systems::CalculateDeltaTimeAvg")
                .term_at(0).singleton()
                .kind<phases::Update>()
                .interval(AVERAGING_PERIOD)
                .each(systems::calculate_delta_time_avg);

        world.system<const GraphicsModule, const PerformanceHudModule>("systems::RenderPerformanceHud")
                .term_at(0).singleton()
                .kind<phases::RenderImGui>()
                .each(systems::render_performance_hud);
    }

    namespace systems
    {
        auto track_delta_time(const flecs::iter &it, usize, PerformanceHudModule &performance_hud) -> void
        {
            performance_hud.delta_times.push_back(it.delta_time());
            performance_hud.delta_time_sum += it.delta_time();
        }

        auto calculate_delta_time_avg(PerformanceHudModule &performance_hud) -> void
        {
            if (!performance_hud.delta_times.empty())
            {
                performance_hud.delta_time_avg = performance_hud.delta_time_sum /
                                                 static_cast<f32>(performance_hud.delta_times.size());

                performance_hud.delta_times.clear();
                performance_hud.delta_time_sum = 0;
            }
        }

        auto render_performance_hud(const GraphicsModule &graphics, const PerformanceHudModule &performance_hud) -> void
        {
            static const auto properties = graphics.device->get_adapter().get_properties();

            constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration
                                                      | ImGuiWindowFlags_AlwaysAutoResize
                                                      | ImGuiWindowFlags_NoSavedSettings
                                                      | ImGuiWindowFlags_NoFocusOnAppearing
                                                      | ImGuiWindowFlags_NoNav
                                                      | ImGuiWindowFlags_NoMouseInputs;
            constexpr f32 margin = 10.0f;
            const ImGuiViewport *viewport = ImGui::GetMainViewport();
            const ImVec2 pos = viewport->Pos;
            const ImVec2 size = viewport->Size;
            ImGui::SetNextWindowPos({size.x - margin, pos.y + margin}, ImGuiCond_Always, {1.0, 0.0});
            ImGui::SetNextWindowSizeConstraints({225, 0}, {FLT_MAX, FLT_MAX});
            ImGui::SetNextWindowBgAlpha(0.35f);

            if (ImGui::Begin("Performance HUD", nullptr, window_flags))
            {
                constexpr ImVec4 red = {0.9, 0.6, 0.5, 1.0};
                constexpr ImVec4 yellow = {0.9, 0.9, 0.5, 1.0};
                constexpr ImVec4 white = {1.0, 1.0, 1.0, 1.0};

                std::string formatted = std::format("{} ({})", properties.name,
                                                    backend_type_to_name(properties.backendType));
                ImGui::TextUnformatted(formatted.c_str());

                formatted = std::format("[{}x{}]", graphics.window->get_framebuffer_size().x,
                                        graphics.window->get_framebuffer_size().y);
                ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize(formatted.c_str()).x);
                ImGui::TextUnformatted(formatted.c_str());

                ImGui::Separator();

                const ImVec4 color = performance_hud.delta_time_avg > PerformanceHudModule::MAX_DELTA_TIME
                                         ? red
                                         : performance_hud.delta_time_avg < PerformanceHudModule::TARGET_DELTA_TIME
                                               ? white
                                               : yellow;
                formatted = std::format("FPS: {:7.2f}", 1 / performance_hud.delta_time_avg);
                ImGui::TextColored(color, "%s", formatted.c_str());

                formatted = std::format("{:.2f} ms", performance_hud.delta_time_avg * 1000);
                ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize(formatted.c_str()).x);
                ImGui::TextColored(color, "%s", formatted.c_str());

                formatted = std::format("Draw Calls: {:d}", graphics.render_pass->get_draw_call_count());
                ImGui::TextUnformatted(formatted.c_str());

                formatted = std::format("{:d} tris", graphics.render_pass->get_triangle_count());
                ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize(formatted.c_str()).x);
                ImGui::TextUnformatted(formatted.c_str());
            }
            ImGui::End();
        }
    }
}
