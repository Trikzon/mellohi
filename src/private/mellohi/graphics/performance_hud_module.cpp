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

    auto PerformanceHudModule::DeltaTimeStats::calculate() -> void
    {
        if (!delta_times.empty())
        {
            avg = sum / static_cast<f32>(delta_times.size());

            stddev = std::sqrt(std::max(0.0f, sum_squared / delta_times.size() - avg * avg));

            delta_times.clear();
            sum = 0;
            sum_squared = 0;
        }
    }

    PerformanceHudModule::PerformanceHudModule(flecs::world &world)
    {
        world.import<ImGuiModule>();

        world.system<const GraphicsModule, PerformanceHudModule>("systems::TrackDeltaTime")
                .term_at(0).singleton()
                .kind<phases::Update>()
                .each(track_delta_time);

        world.system<PerformanceHudModule>("systems::CalculateDeltaTimeAvg")
                .term_at(0).singleton()
                .kind<phases::Update>()
                .interval(AVERAGING_PERIOD)
                .each(calculate_delta_times);

        world.system<const GraphicsModule, const PerformanceHudModule>("systems::RenderPerformanceHud")
                .term_at(0).singleton()
                .kind<phases::RenderImGui>()
                .each(render);
    }

    auto PerformanceHudModule::track_delta_time(const flecs::iter &it, usize, const GraphicsModule &graphics,
                                                PerformanceHudModule &performance_hud) -> void
    {
        performance_hud.cpu_dt.delta_times.push_back(it.delta_time());
        performance_hud.cpu_dt.sum += it.delta_time();
        performance_hud.cpu_dt.sum_squared += it.delta_time() * it.delta_time();

        const auto gpu_elapsed_ms = graphics.time_query_set->get_elapsed_ms();
        performance_hud.gpu_dt.delta_times.push_back(gpu_elapsed_ms);
        performance_hud.gpu_dt.sum += gpu_elapsed_ms;
        performance_hud.gpu_dt.sum_squared += gpu_elapsed_ms * gpu_elapsed_ms;
    }

    auto PerformanceHudModule::calculate_delta_times(PerformanceHudModule &performance_hud) -> void
    {
        performance_hud.cpu_dt.calculate();
        performance_hud.gpu_dt.calculate();
    }

    auto PerformanceHudModule::render(const GraphicsModule &graphics,
                                      const PerformanceHudModule &performance_hud) -> void
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
        ImGui::SetNextWindowSizeConstraints({300, 0}, {FLT_MAX, FLT_MAX});
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

            const ImVec4 color = performance_hud.cpu_dt.avg > PerformanceHudModule::MAX_DELTA_TIME
                                     ? red
                                     : performance_hud.cpu_dt.avg < PerformanceHudModule::TARGET_DELTA_TIME
                                           ? white
                                           : yellow;
            formatted = std::format("CPU: {:>5.2f} ± {:>4.2f} ms", performance_hud.cpu_dt.avg * 1000,
                                    performance_hud.cpu_dt.stddev * 1000);
            ImGui::TextColored(color, "%s", formatted.c_str());

            formatted = std::format("{:>7.2f} fps", 1 / performance_hud.cpu_dt.avg);
            ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize(formatted.c_str()).x);
            ImGui::TextColored(color, "%s", formatted.c_str());

            formatted = std::format("GPU: {:>5.2f} ± {:>4.2f} ms", performance_hud.gpu_dt.avg,
                                    performance_hud.gpu_dt.stddev);
            ImGui::TextUnformatted(formatted.c_str());

            formatted = std::format("Draw Calls: {:d}", graphics.render_pass->get_draw_call_count());
            ImGui::TextUnformatted(formatted.c_str());

            formatted = std::format("{:d} tris", graphics.render_pass->get_triangle_count());
            ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize(formatted.c_str()).x);
            ImGui::TextUnformatted(formatted.c_str());
        }
        ImGui::End();
    }
}
