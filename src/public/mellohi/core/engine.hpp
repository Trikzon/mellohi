#pragma once

#include <flecs.h>

#include "mellohi/core/types.hpp"
#include "mellohi/graphics/glfw/window.hpp"
#include "mellohi/graphics/wgpu/render_pass.hpp"

namespace mellohi
{
    namespace phase
    {
        struct PreTick {};

        struct PreUpdate {};

        struct PreRender {};

        struct Render {};

        struct PostRender {};

        struct PostTick {};
    }

    namespace event
    {
        struct Resized
        {
            vec2u new_size{};
        };
    }

    struct Engine
    {
        explicit Engine(flecs::world &ecs);

        static auto pre_tick(const flecs::iter &it, usize, const glfw::Window &window) -> void;
        static auto pre_render(const flecs::iter &it, usize, const glfw::Window &window, const wgpu::Surface &surface,
                               const wgpu::Device &device) -> void;
        static auto post_render(const flecs::iter &it, usize) -> void;
        static auto post_tick(const wgpu::Surface &surface, const wgpu::Device &device) -> void;

        static auto on_window_resized(flecs::iter &it, usize, const wgpu::Device &device,
                                      const wgpu::Surface &surface) -> void;
    };
}
