#include <mellohi.hpp>

#include "mesh.hpp"

using namespace mellohi;

int main()
{
    const auto translation = mat4x4f(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            1, 2, 3, 1
    );

    Game game("sandbox");

    auto &world = game.get_world();

    world.import<PerformanceHud>();

    world.import<CameraModule>();

    world.import<Mesh>();
    Mesh::create(world, AssetId::fromGame("models/teapot.obj"));

    game.run();
}
