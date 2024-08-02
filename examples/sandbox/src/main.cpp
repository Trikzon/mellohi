#include <mellohi.hpp>

using namespace mellohi;

auto main() -> int
{
    flecs::world ecs;

    ecs.import<Engine>();

    ecs.entity()
            .emplace<Mesh::Data>(ecs, AssetId{"mellohi:models/regular_cube.obj"});

    while (ecs.progress());
}
