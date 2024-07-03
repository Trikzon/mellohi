#pragma once

#include <flecs.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

struct Camera
{
    struct Data
    {
        glm::mat4x4 projection;
        glm::mat4x4 view;
    };

    explicit Camera(const flecs::world &world);
};
