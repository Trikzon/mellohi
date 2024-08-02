#pragma once

#include <flecs.h>

#include "mellohi/graphics/glfw/window.hpp"
#include "mellohi/graphics/wgpu/render_pass.hpp"

namespace mellohi
{
    namespace event
    {
        struct WindowResized
        {
            vec2u new_size{};
        };
    }

    struct Graphics
    {
        explicit Graphics(flecs::world &ecs);

        static auto poll_events(const flecs::iter &it, usize, const glfw::Window &window) -> void;
        static auto create_render_pass(const flecs::iter &it, usize, const glfw::Window &window,
                                       const wgpu::Surface &surface, const wgpu::Device &device) -> void;
        static auto end_render_pass(const flecs::iter &it, usize) -> void;
        static auto present(const wgpu::Surface &surface, const wgpu::Device &device) -> void;
        static auto on_window_resized(flecs::iter &it, usize, const wgpu::Device &device,
                                      const wgpu::Surface &surface) -> void;
    };
}
