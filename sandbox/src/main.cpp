#include <iostream>

#include <mellohi/game.h>

#include "learn_mesh_node.h"

using namespace mellohi;

class Sandbox final : public Game
{
public:
    Sandbox() : Game("sandbox")
    {
        get_root_node().add_child(std::make_shared<LearnMeshNode>(AssetId::fromGame("webgpu.learn")));
    }
};

int main()
{
    const auto game = new Sandbox();

    game->run();

    delete game;
}
