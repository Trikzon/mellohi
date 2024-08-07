#include "mellohi/core/transform_module.hpp"

#include "mellohi/core/engine_module.hpp"

namespace mellohi
{
    TransformModule::TransformModule(const flecs::world &world)
    {
        world.component<Position>().member<f32>("x").member<f32>("y").member<f32>("z");
        world.component<Rotation>().member<f32>("x").member<f32>("y").member<f32>("z").member<f32>("w");
        world.component<Scale>().member<f32>("x").member<f32>("y").member<f32>("z");
        world.component<Transform>();

        world.prefab<prefabs::Transform>("prefabs::Transform")
                .emplace<Position>()
                .emplace<Rotation>(1.0f, 0.0f, 0.0f, 0.0f)
                .emplace<Scale>(1.0f)
                .emplace<Transform>();

        world.system<const Position, const Rotation, const Scale, Transform>("systems::UpdateTransformMatrix")
                .term_at(3).out()
                .kind<phases::PreUpdate>()
                .run(systems::update_transform_matrix);
    }

    namespace systems
    {
        auto update_transform_matrix(flecs::iter &it) -> void
        {
            while (it.next())
            {
                if (!it.changed())
                {
                    continue;
                }

                const auto position = it.field<const Position>(0);
                const auto rotation = it.field<const Rotation>(1);
                const auto scale = it.field<const Scale>(2);
                const auto transform = it.field<Transform>(3);

                for (const auto i: it)
                {
                    const mat4x4f t = glm::translate(mat4x4f{1.0f}, position[i]);
                    const mat4x4f r = glm::toMat4(rotation[i]);
                    const mat4x4f s = glm::scale(mat4x4f{1.0f}, scale[i]);

                    transform[i] = t * r * s;
                }
            }
        }
    }
}
