#include "camera.h"

Camera::Camera(const flecs::world &world)
{
    constexpr float angle2 = 2.0 * 3.14 / 4.0;
    constexpr auto focal_point = glm::vec3(0.0, 0.0, -2.0);
    const glm::mat4x4 R2 = rotate(glm::mat4x4(1.0f), -angle2, glm::vec3(1.0, 0.0, 0.0));
    const glm::mat4x4 T2 = translate(glm::mat4x4(1.0f), -focal_point);

    constexpr float near = 0.001f;
    constexpr float far = 100.0f;
    constexpr float ratio = 640.0f / 480.0f;
    const float fov = 2.0f * glm::atan(1.0f / 2.0f);

    world.set<Data>(
        {
            .view = T2 * R2,
            .projection = glm::perspective(fov, ratio, near, far),
        });
}
