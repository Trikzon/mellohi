#include <mellohi.h>

#include "learn3d_mesh.h"
#include "learn_mesh.h"

using namespace mellohi;

int main()
{
    Game game("sandbox");

    auto &world = game.get_world();

    world.import<PerformanceHud>();

    // world.import<LearnMesh>();
    // LearnMesh::create(world, 0.0, 1.0, 0.0, 0.0, 0.75);
    // LearnMesh::create(world, 1.5, 0.0, 1.0, 0.0, 0.75);
    // LearnMesh::create(world, 3.0, 0.0, 0.0, 1.0, 0.75);
    // LearnMesh::create(world, 4.5, 1.0, 1.0, 1.0, 0.75);

    world.import<LearnMesh3D>();
    LearnMesh3D::create(world, 0.0);

    game.run();
}
