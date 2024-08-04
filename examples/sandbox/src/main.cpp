#include <mellohi.hpp>

using namespace mellohi;

auto main() -> int
{
    flecs::world ecs;

    ecs.import<Engine>();

    ecs.entity()
            .emplace<Mesh::Data>(ecs, AssetId{"mellohi:models/regular_cube.obj"});

    ecs.system<const Input>()
            .term_at(0).singleton()
            .kind<phase::PreUpdate>()
            .each([](const Input &input)
            {
                if (input.is_just_pressed(MouseButton::Left))
                {
                    MH_INFO("Left Pressed!");
                }
            });

    while (ecs.progress());
}
