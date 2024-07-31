#pragma once

#include <flecs.h>

#include "mellohi/core/types.hpp"

namespace mellohi
{
    struct Position : vec3f
    {
        using vec3f::vec3f;
        using vec3f::operator=;

        Position(const vec3f &q) : vec3f(q) {};
    };

    struct Rotation : quat
    {
        using quat::quat;
        using quat::operator=;

        Rotation(const quat &q) : quat(q) {};
    };

    struct Scale : vec3f
    {
        using vec3f::vec3f;
        using vec3f::operator=;

        Scale(const vec3f &q) : vec3f(q) {};
    };

    struct TransformMatrix : mat4x4f
    {
        using mat4x4f::mat4x4f;
        using mat4x4f::operator=;

        TransformMatrix(const mat4x4f &q) : mat4x4f(q) {};
    };

    struct Transform
    {
        struct Prefab {};

        Transform(const flecs::world &ecs);

        static auto update_transform_matrix(flecs::iter &it) -> void;
    };
}
