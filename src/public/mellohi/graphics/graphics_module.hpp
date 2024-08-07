#pragma once

#include <flecs.h>

#include "mellohi/graphics/wgpu/render_pass.hpp"

namespace mellohi
{
    namespace events
    {
        struct FramebufferResized
        {
            vec2u framebuffer_size{};
        };
    }

    struct GraphicsModule
    {
        s_ptr<glfw::Window> window{nullptr};
        s_ptr<wgpu::Surface> surface{nullptr};
        s_ptr<wgpu::Device> device{nullptr};
        optional<wgpu::RenderPass> render_pass;

        explicit GraphicsModule(flecs::world &world);
    };

    namespace systems
    {
        auto poll_events(const flecs::iter &it, usize, const GraphicsModule &graphics) -> void;
        auto create_render_pass(GraphicsModule &graphics) -> void;
        auto end_render_pass(GraphicsModule &graphics) -> void;
        auto present(const GraphicsModule &graphics) -> void;

        auto on_framebuffer_resized(flecs::iter &it, usize, const GraphicsModule &graphics) -> void;
    }
}
