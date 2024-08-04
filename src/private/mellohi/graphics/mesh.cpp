#include "mellohi/graphics/mesh.hpp"

#include <tiny_obj_loader.h>

#include "mellohi/core/engine.hpp"
#include "mellohi/core/transform.hpp"
#include "mellohi/graphics/camera.hpp"
#include "mellohi/graphics/graphics.hpp"

namespace mellohi
{
    struct Uniforms
    {
        mat4x4f model;
    };

    Mesh::Data::Data(const flecs::world &ecs, const AssetId &obj_file_id)
    {
        tinyobj::attrib_t attrib;
        vector<tinyobj::shape_t> shapes;
        vector<tinyobj::material_t> materials;

        string warn;
        string err;

        const string obj_file_path = obj_file_id.get_file_path();
        const bool ret = LoadObj(&attrib, &shapes, &materials, &warn, &err, obj_file_path.c_str());

        if (!warn.empty())
        {
            MH_WARN("{}", warn);
        }

        if (!err.empty())
        {
            MH_ERROR("{}", err);
        }

        MH_ASSERT(ret, "Failed to load OBJ file: {}", obj_file_id.get_fully_qualified_id());

        struct VertexAttributes
        {
            vec3f position;
            vec3f normal;
            vec3f color;
        };
        vector<VertexAttributes> vertexData;

        for (const auto &shape: shapes)
        {
            auto offset = vertexData.size();
            vertexData.resize(offset + shape.mesh.indices.size());

            for (auto i = 0; i < shape.mesh.indices.size(); ++i)
            {
                const auto &index = shape.mesh.indices[i];

                vertexData[offset + i].position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertexData[offset + i].normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };

                vertexData[offset + i].color = {
                    attrib.colors[3 * index.vertex_index + 0],
                    attrib.colors[3 * index.vertex_index + 1],
                    attrib.colors[3 * index.vertex_index + 2]
                };
            }
        }

        const auto *device = ecs.get<wgpu::Device>();

        vertex_buffer = std::make_shared<wgpu::VertexBuffer>(
            *device,
            obj_file_id.get_fully_qualified_id() + " Vertex Buffer",
            vertexData
        );

        vertex_buffer->add_attribute<vec3f>(0); // Position
        vertex_buffer->add_attribute<vec3f>(1); // Normal
        vertex_buffer->add_attribute<vec3f>(2); // Color;

        vertex_count = vertexData.size();

        const auto *mesh_module = ecs.get<Mesh>();
        const auto *camera_module = ecs.get<Camera>();

        const wgpu::ShaderModule shader_module{*device, AssetId{"mellohi:shaders/mesh.wgsl"}};
        render_pipeline = std::make_shared<wgpu::RenderPipeline>(
            *device,
            obj_file_id.get_fully_qualified_id() + " Render Pipeline",
            shader_module,
            vector<wgpu::VertexBuffer>{*vertex_buffer},
            vector<wgpu::BindGroup>{*camera_module->bind_group, *mesh_module->bind_group}
        );
    }

    Mesh::Mesh(flecs::world &ecs)
    {
        ecs.import<Graphics>();
        ecs.import<Transform>();
        ecs.import<Camera>();

        const auto *device = ecs.get<wgpu::Device>();

        bind_group = std::make_shared<wgpu::BindGroup>(*device, "Mesh Bind Group");
        bind_group->add_binding(0, sizeof(Uniforms));

        ecs.system<const Data>()
                .kind<phase::Render>()
                .run(render);
    }

    auto Mesh::render(flecs::iter &it) -> void
    {
        const auto *render_pass = it.world().get<wgpu::RenderPass>();
        const auto *mesh_module = it.world().get<Mesh>();

        while (it.next())
        {
            const auto data = it.field<const Data>(0);

            const mat4x4f S = scale(mat4x4f{1.0f}, vec3f{1.0f});
            const mat4x4f T = translate(mat4x4f{1.0f}, vec3f{0.0, 0.0, 0.0});
            const mat4x4f R = rotate(mat4x4f{1.0f}, static_cast<float>(glfwGetTime()), vec3f{0.0, 1.0, 0.0});
            // const mat4x4f R = rotate(mat4x4f{1.0f}, 0.0f, vec3f{0.0, 1.0, 0.0});

            Uniforms uniforms
            {
                .model = R * T * S
            };
            mesh_module->bind_group->write(0, 0, &uniforms, sizeof(Uniforms));

            render_pass->set_bind_group(1, *mesh_module->bind_group, 0);
            render_pass->set_pipeline(*data->render_pipeline);
            render_pass->set_vertex_buffer(0, *data->vertex_buffer);
            render_pass->draw(data->vertex_count);
        }
    }
}
