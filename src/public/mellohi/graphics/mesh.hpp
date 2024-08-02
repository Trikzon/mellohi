#pragma once

#include <flecs.h>

#include "mellohi/graphics/wgpu/bind_group.hpp"
#include "mellohi/graphics/wgpu/render_pipeline.hpp"
#include "mellohi/graphics/wgpu/vertex_buffer.hpp"

namespace mellohi
{
    struct Mesh
    {
        struct Data
        {
            s_ptr<wgpu::VertexBuffer> vertex_buffer;
            u32 vertex_count;
            s_ptr<wgpu::RenderPipeline> render_pipeline;

            Data(const flecs::world &ecs, const AssetId &obj_file_id);
        };

        struct Uniforms
        {
            mat4x4f model;
        };

        s_ptr<wgpu::BindGroup> bind_group;

        Mesh(flecs::world &ecs);

        static auto render(flecs::iter &it) -> void;
    };
}
