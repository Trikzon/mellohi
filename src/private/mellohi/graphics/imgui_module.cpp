#include "mellohi/graphics/imgui_module.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_wgpu.h>

#include "mellohi/core/engine_module.hpp"
#include "mellohi/core/input_module.hpp"

namespace mellohi
{
    ImGuiModule::ImGuiModule(flecs::world &world)
    {
        world.import<GraphicsModule>();
        world.import<InputModule>();

        const auto &graphics = world.ensure<GraphicsModule>();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui_ImplGlfw_InitForOther(graphics.window->get_raw_ptr(), true);
        ImGui_ImplWGPU_InitInfo init_info;
        init_info.Device = graphics.device->get_raw_ptr();
        // TODO: Use constants.
        init_info.RenderTargetFormat = WGPUTextureFormat_BGRA8Unorm;
        init_info.DepthStencilFormat = WGPUTextureFormat_Depth24Plus;
        ImGui_ImplWGPU_Init(&init_info);

        auto config = ImFontConfig();
        strcpy(config.Name, "JetBrains Mono");
        config.FontDataOwnedByAtlas = false;
        auto font_data = AssetId{"mellohi:fonts/JetBrainsMono.ttf"}.read_file_as_bytes();
        ImGui::GetIO().Fonts->AddFontFromMemoryTTF(font_data.data(), font_data.size(), 18.0f, &config);

        auto &style = ImGui::GetStyle();
        style.WindowBorderSize = 1.0f;
        style.WindowRounding = 4.0f;
        style.FrameBorderSize = 1.0f;
        style.FrameRounding = 4.0f;

        ImVec4 *colors = style.Colors;
        colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.54f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.40f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.40f, 0.40f, 0.40f, 0.67f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.40f, 0.16f, 0.48f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);

        world.system("systems::NewFrame")
                .kind<phases::PreRender>()
                .each(new_frame);

        world.system<GraphicsModule>("systems::EndFrame")
                .term_at(0).singleton()
                .kind<phases::PostRender>()
                .each(end_frame);
    }

    ImGuiModule::~ImGuiModule()
    {
        ImGui_ImplGlfw_Shutdown();
        ImGui_ImplWGPU_Shutdown();
    }

    auto ImGuiModule::new_frame() -> void
    {
        ImGui_ImplWGPU_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    auto ImGuiModule::end_frame(const GraphicsModule &graphics) -> void
    {
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), graphics.render_pass->get_raw_ptr());
    }
}
