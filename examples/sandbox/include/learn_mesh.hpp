#pragma once

#include <mellohi.hpp>

using namespace mellohi;

struct LearnMesh
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
        std::unique_ptr<Pipeline> pipeline;
        std::unique_ptr<BindGroup> bind_group;

        Renderer(Device &device, Surface &surface);
    };

    explicit LearnMesh(const flecs::world &world);

    static flecs::entity create(const flecs::world &world, float time, float r, float g, float b, float a);
};
