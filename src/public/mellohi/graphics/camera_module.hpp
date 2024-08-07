#pragma once

#include <flecs.h>

#include "mellohi/core/input_module.hpp"
#include "mellohi/core/transform_module.hpp"
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
        constexpr static f32 MOVE_SPEED = 4.0f;
        constexpr static f32 ROTATE_SPEED = 0.1f;

        s_ptr<wgpu::BindGroup> bind_group;

        explicit CameraModule(flecs::world &world);

        static auto move_camera(const flecs::iter &it, usize, InputModule &input, Position &position,
                                const Rotation &rotation) -> void;
        static auto rotate_camera(Rotation &rotation, vec2f cursor_delta) -> void;
        static auto update_camera_transform_matrix(flecs::iter &it) -> void;
        static auto bind_camera(const GraphicsModule &graphics, const CameraModule &camera_module,
                                const Transform &transform, CameraUniforms &uniforms) -> void;

        static auto on_framebuffer_resized(events::FramebufferResized event, CameraUniforms &camera_uniforms) -> void;
    };
}
