#include <mellohi.hpp>

#include "mesh.hpp"

using namespace mellohi;

int main()
{
    Game game("sandbox");

    auto &world = game.get_world();

    world.import<PerformanceHud>();

    world.import<CameraModule>();

    world.import<Mesh>();
    Mesh::create(world, AssetId::fromGame("models/teapot.obj"));

    game.run();
}
