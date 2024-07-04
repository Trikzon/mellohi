#include <mellohi.hpp>

#include "learn3d_mesh.hpp"

using namespace mellohi;

int main()
{
    Game game("sandbox");

    auto &world = game.get_world();

    world.import<PerformanceHud>();

    world.import<LearnMesh3D>();
    LearnMesh3D::create(world, 0.0);

    game.run();
}
