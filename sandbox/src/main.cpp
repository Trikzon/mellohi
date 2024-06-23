#include <iostream>

#include <imgui.h>

#include "learn_mesh_node.h"
#include "mellohi/asset_id.h"
#include "mellohi/game.h"

using namespace mellohi;

class TestNode final : public Node
{
public:
    TestNode() : Node("TestNode") { }

private:
    void on_render(RenderPass &render_pass, double delta_time) override
    {
        ImGui::ShowMetricsWindow();
    }
};

class Sandbox final : public Game
{
public:
    Sandbox() : Game("sandbox")
    {
        get_root_node().add_child(std::make_shared<TestNode>());

        const auto learn_mesh_node = std::make_shared<LearnMeshNode>();
        learn_mesh_node->load_mesh(AssetId::fromGame("webgpu.learn"));
        get_root_node().add_child(learn_mesh_node);
    }
};

int main()
{
    const auto game = new Sandbox();

    game->run();

    delete game;
}
