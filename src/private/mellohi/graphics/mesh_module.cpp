#include "mellohi/graphics/mesh_module.hpp"

#include <tiny_obj_loader.h>

#include "mellohi/core/engine_module.hpp"
#include "mellohi/graphics/camera_module.hpp"
#include "mellohi/graphics/graphics_module.hpp"

namespace mellohi
{
    MeshData::MeshData(const wgpu::Device &device, const wgpu::ShaderModule &shader_module,
                       const vector<wgpu::BindGroup> &bind_groups, const AssetId &obj_file_id)
    {
        tinyobj::attrib_t attrib;
        vector<tinyobj::shape_t> shapes;
        vector<tinyobj::material_t> materials;

        string warn;
        string err;

        const string obj_file_path = obj_file_id.get_file_path().string();
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

        vertex_buffer = std::make_shared<wgpu::VertexBuffer>(
            device,
            obj_file_id.get_fully_qualified_id() + " Vertex Buffer",
            vertexData
        );

        vertex_buffer->add_attribute<vec3f>(0); // Position
        vertex_buffer->add_attribute<vec3f>(1); // Normal
        vertex_buffer->add_attribute<vec3f>(2); // Color;

        vertex_count = vertexData.size();

        render_pipeline = std::make_shared<wgpu::RenderPipeline>(
            device,
            obj_file_id.get_fully_qualified_id() + " Render Pipeline",
            shader_module,
            vector{*vertex_buffer},
            bind_groups
        );
    }

    MeshModule::MeshModule(flecs::world &world)
    {
        world.import<GraphicsModule>();
        world.import<CameraModule>();

        world.component<Mesh>();

        const auto &camera = world.ensure<CameraModule>();
        const auto &graphics = world.ensure<GraphicsModule>();

        m_device = graphics.device;
        m_camera_bind_group = camera.bind_group;
        m_model_bind_group = std::make_shared<wgpu::BindGroup>(*graphics.device, "Model Bind Group");
        m_model_bind_group->add_binding(0, sizeof(MeshUniforms));
        m_shader_module = std::make_shared<wgpu::ShaderModule>(*graphics.device, AssetId{"mellohi:shaders/mesh.wgsl"});

        world.prefab<prefabs::Mesh>("prefabs::Mesh")
                .is_a<prefabs::Transform>()
                .set<Mesh>({});

        world.system<MeshModule>("systems::ResetDynamicMeshIndex")
                .kind<phases::PreRender>()
                .each(reset_dynamic_mesh_index);

        world.system<const GraphicsModule, MeshModule, const Transform, const Mesh>("systems::Render")
                .term_at(0).singleton()
                .term_at(1).singleton()
                .kind<phases::Render>()
                .each(render);
    }

    auto MeshModule::get_mesh_data(const AssetId &asset_id) -> MeshData &
    {
        if (!m_mesh_data.contains(asset_id))
        {
            m_mesh_data.insert({
                asset_id,
                MeshData{*m_device, *m_shader_module, vector{*m_camera_bind_group, *m_model_bind_group}, asset_id}
            });
        }

        const auto it = m_mesh_data.find(asset_id);
        return it->second;
    }

    auto MeshModule::reset_dynamic_mesh_index(MeshModule &mesh) -> void
    {
        mesh.m_dynamic_mesh_index = 0;
    }

    auto MeshModule::render(const GraphicsModule &graphics, MeshModule &mesh_module, const Transform &transform,
                            const Mesh &mesh) -> void
    {
        const auto &mesh_data = mesh_module.get_mesh_data(mesh.asset_id);

        const MeshUniforms uniforms{
            .model = transform
        };
        mesh_module.m_model_bind_group->write(0, mesh_module.m_dynamic_mesh_index, &uniforms, sizeof(MeshUniforms));

        graphics.render_pass->set_bind_group(1, *mesh_module.m_model_bind_group, mesh_module.m_dynamic_mesh_index);
        graphics.render_pass->set_pipeline(*mesh_data.render_pipeline);
        graphics.render_pass->set_vertex_buffer(0, *mesh_data.vertex_buffer);
        graphics.render_pass->draw(mesh_data.vertex_count);

        mesh_module.m_dynamic_mesh_index += 1;
    }
}
