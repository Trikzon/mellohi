#include "mellohi/transform.hpp"

#include <glm/gtx/quaternion.hpp>

namespace mellohi
{
    TransformModule::TransformModule(const flecs::world &world)
    {
        world.prefab<TransformPrefab>()
            .emplace<Position>()
            .emplace<Rotation>(1.0f, 0.0f, 0.0f, 0.0f)
            .emplace<Scale>(1.0f)
            .emplace<Transform>();

        world.system<const Position, const Rotation, const Scale, Transform>()
            .term_at(3).out()
            .kind(flecs::OnUpdate)
            .run([](flecs::iter &it)
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

                    for (const auto i : it)
                    {
                        const mat4x4f T = glm::translate(mat4x4f{1.0f}, -position[i]);
                        const mat4x4f R = glm::toMat4(rotation[i]);
                        const mat4x4f S = glm::scale(mat4x4f{1.0f}, scale[i]);

                        transform[i] = S * R * T;
                    }
                }
            });
    }
}
