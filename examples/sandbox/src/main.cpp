#include <mellohi.hpp>

using namespace mellohi;

auto main() -> int
{
    flecs::world world;

    world.import<flecs::stats>();
    world.set<flecs::Rest>({});

    world.import<EngineModule>();

    world.entity("Camera").is_a<prefabs::Camera>().set<Position>({0.0f, 0.0f, -3.0f});

    world.entity("Mesh")
            .is_a<prefabs::Mesh>()
            .set<Position>({2.0f, -0.5f, 0.0f});
    world.entity("Mesh2")
            .is_a<prefabs::Mesh>()
            .set<Mesh>({.asset_id = AssetId{"sandbox:models/fourareen.obj"}})
            .set<Position>({0.0f, -1.0f, 0.0f});

    world.system<Rotation>("RotateMesh")
            .with<Mesh>()
            .each([](const flecs::iter &it, const usize i, Rotation &rotation)
            {
                rotation *= glm::angleAxis(glm::radians(10.0f) * it.delta_time(), vec3f{0.0f, 1.0f, 0.0f});
            });

    while (world.progress()) {}
}
