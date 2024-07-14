#pragma once

#include <flecs.h>

#include "mellohi/graphics/bind_group.hpp"
#include "mellohi/types.hpp"

namespace mellohi
{
    struct CameraUniforms
    {
        mat4x4f projection{1.0f};
        mat4x4f view{1.0f};

        CameraUniforms();
    };

    struct CameraModule
    {
        BindGroup* bind_group = nullptr;

        explicit CameraModule(flecs::world& world);
        ~CameraModule();

        CameraModule(const CameraModule &other) = delete;
        CameraModule(CameraModule &&other) noexcept;
        CameraModule & operator=(const CameraModule &other) = delete;
        CameraModule & operator=(CameraModule &&other) noexcept;
    };
}
