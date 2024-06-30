#pragma once

#include <flecs.h>
#include <mellohi.h>

using namespace mellohi;

namespace learn_mesh
{
    struct Time
    {
        float value;
    };

    struct Color
    {
        std::array<float, 4> color;
    };

    struct Renderer
    {
        std::unique_ptr<VertexBuffer> vertex_buffer;
        std::unique_ptr<IndexBuffer> index_buffer;
        std::unique_ptr<UniformBuffer> uniform_buffer;
        std::unique_ptr<BindGroup> bind_group;
        std::unique_ptr<Pipeline> pipeline;

        Renderer(Device &device, Surface &surface);
    };

    void init_systems(const flecs::world &world);

    flecs::entity create(const flecs::world &world, float time, float r, float g, float b, float a);
}
