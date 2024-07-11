#pragma once

#include <flecs.h>
#include <mellohi.hpp>

using namespace mellohi;

struct Mesh
{
    struct RenderData
    {
        VertexBuffer* vertex_buffer;
        u32 vertex_count;
        Pipeline* pipeline;
        BindGroup* bind_group;

        RenderData(Device &device, Surface &surface, const AssetId &obj_asset_id);
        ~RenderData();
    };

    struct Uniforms
    {
        mat4x4f projection;
        mat4x4f view;
        mat4x4f model;
    };

    Mesh(const flecs::world &world);

    static flecs::entity create(const flecs::world &world, const AssetId &obj_asset_id);
};
