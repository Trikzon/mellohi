#pragma once

#include <flecs.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <mellohi.h>

using namespace mellohi;

struct LearnMesh3D
{
    struct Time
    {
        float value;
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
        glm::mat4x4 projection;
        glm::mat4x4 view;
        glm::mat4x4 model;
    };

    explicit LearnMesh3D(flecs::world &world);

    static flecs::entity create(const flecs::world &world, float time);
};
