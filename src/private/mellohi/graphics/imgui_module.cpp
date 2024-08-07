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

        world.system("systems::NewFrame")
                .kind<phases::PreRender>()
                .each(systems::new_frame);

        world.system<GraphicsModule>("systems::EndFrame")
                .term_at(0).singleton()
                .kind<phases::PostRender>()
                .each(systems::end_frame);
    }

    ImGuiModule::~ImGuiModule()
    {
        ImGui_ImplGlfw_Shutdown();
        ImGui_ImplWGPU_Shutdown();
    }

    namespace systems
    {
        auto new_frame() -> void
        {
            ImGui_ImplWGPU_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }

        auto end_frame(const GraphicsModule &graphics) -> void
        {
            ImGui::EndFrame();
            ImGui::Render();
            ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), graphics.render_pass->get_raw_ptr());
        }
    }
}
