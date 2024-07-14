#include "mellohi/transform.hpp"

#include <glm/gtx/quaternion.hpp>

namespace mellohi
{
    TransformModule::TransformModule(const flecs::world &world)
    {
        world.prefab<TransformPrefab>()
            .emplace_override<Position>()
            .emplace_override<Rotation>(1.0f, 0.0f, 0.0f, 0.0f)
            .emplace_override<Scale>(1.0f)
            .emplace_override<Transform>();

        world.system<const Position, const Rotation, const Scale, Transform>()
            .term_at(4).out()
            .kind(flecs::OnUpdate)
            .iter([](flecs::iter &it, const Position *position, const Rotation *rotation, const Scale *scale, Transform *transform)
            {
                if (!it.changed())
                {
                    return;
                }

                for (const auto i : it)
                {
                    const mat4x4f T = glm::translate(mat4x4f{1.0f}, -position[i]);
                    const mat4x4f R = glm::toMat4(rotation[i]);
                    const mat4x4f S = glm::scale(mat4x4f{1.0f}, scale[i]);

                    transform[i] = S * R * T;
                }
            });
    }
}
