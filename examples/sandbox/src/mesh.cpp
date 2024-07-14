#include "mesh.hpp"

#include <glm/ext.hpp>
#include <tiny_obj_loader.h>

Mesh::RenderData::RenderData(Device &device, Surface &surface, const AssetId &obj_asset_id, BindGroup *camera_bind_group)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    const std::string obj_path = obj_asset_id.get_file_path();
    const bool ret = LoadObj(&attrib, &shapes, &materials, &warn, &err, obj_path.c_str());

    if (!warn.empty()) {
        std::cerr << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        // TODO: Handle error
        return;
    }

    struct VertexAttributes
    {
        vec3f position;
        vec3f normal;
        vec3f color;
    };
    std::vector<VertexAttributes> vertexData;

    for (const auto& shape : shapes)
    {
        auto offset = vertexData.size();
        vertexData.resize(offset + shape.mesh.indices.size());

        for (auto i = 0; i < shape.mesh.indices.size(); ++i)
        {
            const auto& index = shape.mesh.indices[i];

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

    vertex_buffer = new VertexBuffer(device, vertexData);
    vertex_buffer->add_attribute_vec3f();  // Position
    vertex_buffer->add_attribute_vec3f();  // Normal
    vertex_buffer->add_attribute_vec3f();  // Color

    vertex_count = vertexData.size();

    bind_group = new BindGroup(device);
    bind_group->add_binding(device, 0, sizeof(ModelUniforms));

    const std::vector bind_groups = {camera_bind_group, bind_group};

    pipeline = new Pipeline(device, surface, AssetId::fromGame("shaders/mesh.wgsl"), *vertex_buffer, bind_groups);
}

Mesh::RenderData::~RenderData()
{
    delete vertex_buffer;
    delete pipeline;
    delete bind_group;
}

Mesh::Mesh(const flecs::world &world)
{
    world.system<const RenderData>()
        .kind(flecs::OnStore)
        .run([](flecs::iter &it)
        {
            const auto *window = it.world().get<Window>();
            auto *render_pass = it.world().get_mut<RenderPass>();

            while (it.next())
            {
                const auto render_data = it.field<const RenderData>(0);

                const mat4x4f S = scale(mat4x4f{1.0f}, vec3f{1.0f});
                const mat4x4f T1 = translate(mat4x4f{1.0f}, vec3f{0.0, 0.0, 0.0});
                // const mat4x4f R1 = rotate(mat4x4f{1.0f}, static_cast<float>(glfwGetTime()), vec3f{0.0, 1.0, 0.0});
                const mat4x4f R1 = rotate(mat4x4f{1.0f}, 0.0f, vec3f{0.0, 1.0, 0.0});

                ModelUniforms model_uniforms{};
                model_uniforms.transform = R1 * T1 * S;

                render_data->bind_group->write(window->get_device(), 0, 0, &model_uniforms);

                render_pass->set_pipeline(*render_data->pipeline);
                render_pass->set_vertex_buffer(0, *render_data->vertex_buffer);
                render_pass->set_bind_group(window->get_device(), 1, *render_data->bind_group, 0);
                render_pass->draw(render_data->vertex_count);
            }
        });
}

flecs::entity Mesh::create(const flecs::world &world, const AssetId &obj_asset_id)
{
    const auto *window = world.get<Window>();
    const auto *camera_module = world.get<CameraModule>();

    return world.entity()
        .emplace<RenderData>(window->get_device(), window->get_surface(), obj_asset_id, camera_module->bind_group);
}
