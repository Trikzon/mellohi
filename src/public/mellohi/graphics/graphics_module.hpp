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
        s_ptr<wgpu::TimeQuerySet> time_query_set{nullptr};
        optional<wgpu::RenderPass> render_pass;

        explicit GraphicsModule(flecs::world &world);

        static auto poll_events(const flecs::iter &it, usize, const GraphicsModule &graphics) -> void;
        static auto create_render_pass(GraphicsModule &graphics) -> void;
        static auto end_render_pass(GraphicsModule &graphics) -> void;
        static auto present(const GraphicsModule &graphics) -> void;

        static auto on_framebuffer_resized(flecs::iter &it, usize, const GraphicsModule &graphics) -> void;
    };
}
