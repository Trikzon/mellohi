#include "mellohi/core/transform.hpp"

#include "mellohi/core/engine.hpp"
#include "mellohi/core/log.hpp"

namespace mellohi
{
    Transform::Transform(const flecs::world &ecs)
    {
        ecs.prefab<Prefab>()
                .emplace<Position>()
                .emplace<Rotation>(1.0f, 0.0f, 0.0f, 0.0f)
                .emplace<Scale>(1.0f)
                .emplace<TransformMatrix>();

        ecs.system<const Position, const Rotation, const Scale, TransformMatrix>()
                .term_at(3).out()
                .kind<phase::PreUpdate>()
                .run(update_transform_matrix);
    }

    auto Transform::update_transform_matrix(flecs::iter &it) -> void
    {
        while (it.next())
        {
            if (!it.changed())
            {
                continue;
            }

            MH_INFO("Updating transform matrix.");

            const auto position = it.field<const Position>(0);
            const auto rotation = it.field<const Rotation>(1);
            const auto scale = it.field<const Scale>(2);
            const auto transform_matrix = it.field<TransformMatrix>(3);

            for (const auto i: it)
            {
                const mat4x4f T = glm::translate(mat4x4f{1.0f}, -position[i]);
                const mat4x4f R = glm::toMat4(rotation[i]);
                const mat4x4f S = glm::scale(mat4x4f{1.0f}, scale[i]);

                transform_matrix[i] = S * R * T;
            }
        }
    }
}
