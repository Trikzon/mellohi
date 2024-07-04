#pragma once

#include <mellohi.h>

using namespace mellohi;

struct LearnMesh3D
{
    struct Time
    {
        f32 value;
    };

    struct Renderer
    {
        std::unique_ptr<VertexBuffer> vertex_buffer;
        std::unique_ptr<IndexBuffer> index_buffer;
        std::unique_ptr<Pipeline> pipeline;
        std::unique_ptr<BindGroup> bind_group;

        Renderer(Device &device, Surface &surface);
    };

    struct Uniforms
    {
        mat4x4f projection;
        mat4x4f view;
        mat4x4f model;
    };

    explicit LearnMesh3D(flecs::world &world);

    static flecs::entity create(const flecs::world &world, f32 time);
};
