#pragma once

#include <flecs.h>

#include "mellohi/graphics/graphics_module.hpp"

namespace mellohi
{
    struct CameraUniforms
    {
        mat4x4f projection{1.0f};
        mat4x4f view{1.0f};

        CameraUniforms(vec2u framebuffer_size, const mat4x4f &view);
        CameraUniforms() = default;
    };

    namespace tags
    {
        struct CurrentCamera {};
    }

    namespace prefabs
    {
        struct Camera {};
    }

    struct CameraModule
    {
        constexpr static f32 NEAR = 0.001f;
        constexpr static f32 FAR = 1000.0f;

        s_ptr<wgpu::BindGroup> bind_group;

        explicit CameraModule(flecs::world &world);

        static auto update_camera_transform_matrix(flecs::iter &it) -> void;
        static auto bind_camera(flecs::iter &it) -> void;

        static auto on_framebuffer_resized(events::FramebufferResized event, CameraUniforms &camera_uniforms) -> void;
    };
}
