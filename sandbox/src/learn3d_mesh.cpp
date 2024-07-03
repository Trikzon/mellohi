#include "learn3d_mesh.h"

#include <sstream>

#include "camera.h"

LearnMesh3D::Renderer::Renderer(Device &device, Surface &surface)
{
    std::string learn_mesh_contents = AssetId::fromGame("pyramid.learn3d").read_file_to_string();

    std::vector<float> point_data;
    std::vector<uint16_t> index_data;

    enum class Section
    {
        None,
        Points,
        Indices,
    };
    auto current_section = Section::None;

    float value;
    uint16_t index;
    std::string line;
    std::istringstream learn_mesh_stream(learn_mesh_contents);
    while (std::getline(learn_mesh_stream, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        if (line == "[points]")
        {
            current_section = Section::Points;
        }
        else if (line == "[indices]")
        {
            current_section = Section::Indices;
        }
        else if (line[0] == '#' || line.empty())
        {
            // NOP
        }
        else if (current_section == Section::Points)
        {
            std::istringstream line_stream(line);
            // Get x, y, z, r, g, b
            for (int i = 0; i < 6; ++i)
            {
                line_stream >> value;
                point_data.push_back(value);
            }
        }
        else if (current_section == Section::Indices)
        {
            std::istringstream line_stream(line);
            // Get corners #0, #1, and #2
            for (int i = 0; i < 3; ++i)
            {
                line_stream >> index;
                index_data.push_back(index);
            }
        }
    }

    vertex_buffer = std::make_unique<VertexBuffer>(device, point_data);
    vertex_buffer->add_attribute_vec3f();  // Position
    vertex_buffer->add_attribute_vec3f();  // Color

    index_buffer = std::make_unique<IndexBuffer>(device, index_data);

    bind_group = std::make_unique<BindGroup>(device, 1);
    bind_group->add(device, 0, sizeof(Uniforms));

    pipeline = std::make_unique<Pipeline>(device, surface, AssetId::fromGame("learn3d.wgsl"),
        *vertex_buffer, bind_group.get(), 1);
}

LearnMesh3D::LearnMesh3D(flecs::world &world)
{
    world.import<Camera>();

    world.system<Time>()
        .write<Time>()
        .iter([](const flecs::iter &it, Time *time)
        {
            for (const auto i : it)
            {
                time[i].value += it.delta_time();
            }
        });

    {
        const auto window = world.get<Window>();
        world.emplace<Renderer>(window->get_device(), window->get_surface());
    }

    world.system<const Time>()
        .write<RenderPass>()
        .read<Renderer>()
        .read<Camera>()
        .kind(flecs::OnStore)
        .iter([](const flecs::iter &it, const Time *time)
        {
            const auto render_pass = it.world().get_mut<RenderPass>();
            const auto renderer = it.world().get<Renderer>();

            render_pass->set_pipeline(*renderer->pipeline);
            render_pass->set_vertex_buffer(0, *renderer->vertex_buffer);
            render_pass->set_index_buffer(*renderer->index_buffer);

            auto &device = it.world().get<Window>()->get_device();

            const auto camera = it.world().get<Camera::Data>();

            Uniforms uniforms{};
            uniforms.view = camera->view;
            uniforms.projection = camera->projection;

            for (const auto i : it)
            {
                const float angle1 = time[i].value;
                const glm::mat4x4 S = scale(glm::mat4x4(1.0f), glm::vec3(0.3f));
                const glm::mat4x4 T1 = translate(glm::mat4x4(1.0f), glm::vec3(0.5, 0.0, 0.0));
                const glm::mat4x4 R1 = rotate(glm::mat4x4(1.0f), angle1, glm::vec3(0.0, 0.0, 1.0));
                uniforms.model = R1 * T1 * S;

                renderer->bind_group->write(device, 0, i, &uniforms);
                render_pass->set_bind_group(device, *renderer->bind_group, i);
                render_pass->draw_indexed(renderer->index_buffer->get_index_count());
            }
        });
}

flecs::entity LearnMesh3D::create(const flecs::world &world, const float time)
{
    return world.entity()
        .set<Time>({time});
}
