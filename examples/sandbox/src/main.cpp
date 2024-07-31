#include <iostream>

#include <mellohi.hpp>

using namespace mellohi;

// https://eliemichel.github.io/WebGPU-AutoLayout/
struct Uniforms
{
    vec3f color{};
    float _pad0;
};

auto main() -> int
{
    flecs::world ecs;

    ecs.import<Engine>();

    ecs.entity().is_a<Transform::Prefab>();

    const auto *device = ecs.get<wgpu::Device>();

    const wgpu::ShaderModule shader_module{*device, AssetId{"sandbox:shaders/basic.wgsl"}};

    const auto point_data = vector<tuple<vec2f, vec3f>>
    {
        {{-0.5, -0.5}, {1.0, 0.0, 0.0}},
        {{+0.5, -0.5}, {0.0, 1.0, 0.0}},
        {{+0.5, +0.5}, {0.0, 0.0, 1.0}},
        {{-0.5, +0.5}, {1.0, 1.0, 0.0}},
    };
    const wgpu::VertexBuffer point_buffer{*device, "Position Buffer", point_data};
    point_buffer.add_attribute<vec2f>(0); // Position
    point_buffer.add_attribute<vec3f>(1); // Color

    const auto index_data = vector<u16>
    {
        0, 1, 2,
        0, 2, 3,
    };
    const wgpu::IndexBuffer index_buffer{*device, "Index Buffer", index_data};

    const wgpu::BindGroup bind_group{*device, "Uniforms Bind Group"};
    bind_group.add_binding(0, sizeof(Uniforms));
    constexpr Uniforms uniforms{{1.0f, 0.0f, 0.0f}};
    bind_group.write(0, 0, &uniforms, sizeof(Uniforms));

    const wgpu::RenderPipeline render_pipeline
    {
        *device, "Render Pipeline", shader_module, {point_buffer}, {bind_group}
    };

    ecs.system<const wgpu::RenderPass>()
            .term_at(0).singleton()
            .kind<phase::Render>()
            .each([bind_group, index_buffer, render_pipeline, point_buffer](const wgpu::RenderPass &render_pass) -> void
            {
                render_pass.set_bind_group(0, bind_group, 0);
                render_pass.set_index_buffer(index_buffer);
                render_pass.set_pipeline(render_pipeline);
                render_pass.set_vertex_buffer(0, point_buffer);
                render_pass.draw_indexed(index_buffer.get_index_count());
            });

    while (ecs.progress());
}
