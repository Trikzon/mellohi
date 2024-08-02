#pragma once

#include <flecs.h>

#include "mellohi/graphics/wgpu/bind_group.hpp"

namespace mellohi
{
    struct Camera
    {
        struct Uniforms
        {
            mat4x4f projection{1.0f};
            mat4x4f view{1.0f};

            Uniforms(vec2u framebuffer_size, const mat4x4f &view);
        };

        constexpr static f32 NEAR = 0.001f;
        constexpr static f32 FAR = 100.0f;

        s_ptr<wgpu::BindGroup> bind_group;

        explicit Camera(flecs::world &ecs);

        static auto pre_render(flecs::iter &it) -> void;
        static auto on_window_resized(flecs::iter &it, usize, Uniforms &uniforms) -> void;
    };
}
