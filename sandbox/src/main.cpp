#include <iostream>

#include <mellohi/game.h>

#include "learn_mesh_node.h"

using namespace mellohi;

class Sandbox final : public Game
{
public:
    Sandbox() : Game("sandbox")
    {
        const auto first_node = std::make_shared<LearnMeshNode>(AssetId::fromGame("webgpu.learn"));
        first_node->set_color({1.0, 0.0, 0.0, 0.75});
        get_root_node().add_child(first_node);

        const auto second_node = std::make_shared<LearnMeshNode>(AssetId::fromGame("webgpu.learn"), "SecondLearnMeshNode");
        second_node->set_color({0.0, 1.0, 0.0, 0.75});
        second_node->offset = 2.0;
        get_root_node().add_child(second_node);

        const auto third_node = std::make_shared<LearnMeshNode>(AssetId::fromGame("webgpu.learn"), "SecondLearnMeshNode");
        third_node->set_color({0.0, 0.0, 1.0, 0.75});
        third_node->offset = 4.0;
        get_root_node().add_child(third_node);
    }
};

int main()
{
    const auto game = new Sandbox();

    game->run();

    delete game;
}
