#include "mellohi/performance_hud.hpp"

#include <format>

#include <imgui.h>

#include "mellohi/graphics/window.hpp"

namespace mellohi
{
    const char * backend_type_to_name(const wgpu::BackendType wpgu_backend_type)
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

    PerformanceHud::PerformanceHud(const flecs::world &world)
    {
        world.entity().add<Data>();

        world.system<Data>()
            .each([](const flecs::iter &it, usize, Data &data)
            {
                data.delta_times.push_back(it.delta_time());
                data.delta_time_sum += it.delta_time();
            });

        world.system<Data>()
            .interval(AVERAGING_PERIOD)
            .each([](Data &data)
            {
                if (!data.delta_times.empty())
                {
                    data.average_delta_time = data.delta_time_sum / static_cast<f32>(data.delta_times.size());

                    data.delta_times.clear();
                    data.delta_time_sum = 0.0;
                }
            });

        const auto& properties = world.get<Window>()->get_device().get_wgpu_properties();
        const char *device_name = properties.name;
        const char *backend_name = backend_type_to_name(properties.backendType);

        world.system<const Data>()
            .kind(flecs::OnStore)
            .each([device_name, backend_name](const Data &data)
            {
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
                ImGui::SetNextWindowPos({ size.x - margin, pos.y + margin }, ImGuiCond_Always, { 1.0, 0.0 });
                ImGui::SetNextWindowSizeConstraints({175, 0}, {FLT_MAX, FLT_MAX});
                ImGui::SetNextWindowBgAlpha(0.35f);

                if (ImGui::Begin("Performance HUD", nullptr, window_flags))
                {
                    constexpr ImVec4 red = { 0.9, 0.6, 0.5, 1.0 };
                    constexpr ImVec4 yellow = { 0.9, 0.9, 0.5, 1.0 };
                    constexpr ImVec4 white = { 1.0, 1.0, 1.0, 1.0 };

                    ImGui::TextUnformatted(device_name);
                    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize(backend_name).x);
                    ImGui::TextUnformatted(backend_name);

                    ImGui::Separator();

                    const ImVec4 color = data.average_delta_time > TARGET_DELTA_TIME ? red : data.average_delta_time < TARGET_DELTA_TIME ? white : yellow;
                    std::string formatted = std::format("FPS: {:7.2f}", 1 / data.average_delta_time);
                    ImGui::TextColored(color, "%s", formatted.c_str());
                    formatted = std::format("{:.2f} ms", data.average_delta_time * 1000);
                    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize(formatted.c_str()).x);
                    ImGui::TextColored(color, "%s", formatted.c_str());
                }
                ImGui::End();
            });
    }
}
