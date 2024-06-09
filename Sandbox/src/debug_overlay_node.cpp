#include "debug_overlay_node.h"

#include <imgui.h>

void DebugOverlayNode::on_draw_imgui()
{
    constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMouseInputs;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 work_pos = viewport->WorkPos;
    ImVec2 window_pos;
    window_pos.x = work_pos.x + 10.0f;
    window_pos.y = work_pos.y + 10.0f;
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, ImVec2{});
    ImGui::SetNextWindowBgAlpha(0.35f);

    if (ImGui::Begin("Debug Info Overlay", nullptr, window_flags))
    {
        ImGui::Text("Mellohi Debug Info");
        ImGui::Separator();
        const ImGuiIO& imgui_io = ImGui::GetIO();
        ImGui::Text("%.2f fps (%.3f ms/frame)", imgui_io.Framerate, 1 / imgui_io.Framerate * 1000.0f);
    }
    ImGui::End();
}
