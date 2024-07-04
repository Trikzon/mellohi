#include "camera.h"

#include <glm/ext.hpp>

Camera::Camera(const flecs::world &world)
{
    constexpr f32 angle2 = 2.0 * 3.14 / 4.0;
    constexpr auto focal_point = vec3f(0.0, 0.0, -2.0);
    const mat4x4f R2 = rotate(mat4x4f(1.0f), -angle2, vec3f(1.0, 0.0, 0.0));
    const mat4x4f T2 = translate(mat4x4f(1.0f), -focal_point);

    constexpr f32 near = 0.001f;
    constexpr f32 far = 100.0f;
    constexpr f32 ratio = 640.0f / 480.0f;
    const float fov = 2.0f * glm::atan(1.0f / 2.0f);

    world.set<Data>(
        {
            .view = T2 * R2,
            .projection = glm::perspective(fov, ratio, near, far),
        });
}
