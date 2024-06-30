#include "learn_mesh.h"

#include <sstream>

namespace learn_mesh
{
    // Generated with https://eliemichel.github.io/WebGPU-AutoLayout/
    struct MyUniforms {
        float time; // at byte offset 0
        float _pad0[3];
        std::array<float, 4> color; // at byte offset 16
    };

    Renderer::Renderer(Device &device, Surface &surface)
    {
        std::string learn_mesh_contents = AssetId::fromGame("webgpu.learn").read_file_to_string();

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
                // Get x, y, r, g, b
                for (int i = 0; i < 5; ++i)
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
        vertex_buffer->add_attribute_vec2f();  // Position
        vertex_buffer->add_attribute_vec3f();  // Color

        index_buffer = std::make_unique<IndexBuffer>(device, index_data);

        uniform_buffer = std::make_unique<UniformBuffer>(device, 0, MyUniforms{});

        bind_group = std::make_unique<BindGroup>(device, *uniform_buffer);

        pipeline = std::make_unique<Pipeline>(device, surface, AssetId::fromGame("learn.wgsl"), *vertex_buffer, *bind_group);
    }

    void init_systems(const flecs::world &world)
    {
        world.system<Time>()
            .write<Time>()
            .each([](Time &time)
            {
                time.value += 0.01;
            });

        world.system<const Window, RenderPass, const Renderer, const Time, const Color>()
            .term_at(1).singleton()
            .term_at(2).singleton()
            .kind(flecs::OnStore)
            .each([&](const Window &window, RenderPass &render_pass, const Renderer &renderer, const Time &time, const Color &color)
            {
                MyUniforms my_uniforms{};
                my_uniforms.time = time.value;
                my_uniforms.color = color.color;

                renderer.uniform_buffer->write(window.get_device(), my_uniforms);

                render_pass.set_pipeline(*renderer.pipeline);
                render_pass.set_vertex_buffer(0, *renderer.vertex_buffer);
                render_pass.set_index_buffer(*renderer.index_buffer);
                render_pass.set_bind_group(*renderer.bind_group);
                render_pass.draw_indexed(renderer.index_buffer->get_index_count());
            });
    }

    flecs::entity create(const flecs::world &world, const float time, const float r, const float g, const float b,
        const float a)
    {
        const auto window = world.get<Window>();

        return world.entity()
            .emplace<Renderer>(window->get_device(), window->get_surface())
            .set<Time>({time})
            .set<Color>({r, g, b, a});
    }
}
