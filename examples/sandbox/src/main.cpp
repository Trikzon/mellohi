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
    std::cout << R"(
   ___       ___       ___       ___       ___       ___       ___
  /\__\     /\  \     /\__\     /\__\     /\  \     /\__\     /\  \
 /::L_L_   /::\  \   /:/  /    /:/  /    /::\  \   /:/__/_   _\:\  \
/:/L:\__\ /::\:\__\ /:/__/    /:/__/    /:/\:\__\ /::\/\__\ /\/::\__\
\/_/:/  / \:\:\/  / \:\  \    \:\  \    \:\/:/  / \/\::/  / \::/\/__/
  /:/  /   \:\/  /   \:\__\    \:\__\    \::/  /    /:/  /   \:\__\
  \/__/     \/__/     \/__/     \/__/     \/__/     \/__/     \/__/  )" << std::endl << std::endl;

    const auto &glfw = glfw::Glfw::get_instance();
    const glfw::Window window{"Sandbox", {1280, 720}};

    const wgpu::Instance instance;
    const wgpu::Surface surface{instance, window};
    const wgpu::Adapter adapter{instance, surface};
    const wgpu::Device device{adapter, adapter.get_limits()};

    surface.configure(device, window.get_framebuffer_size(), false);

    window.set_framebuffer_size_callback([surface, device](const vec2u size) -> void
    {
        surface.configure(device, size, false);
    });

    const wgpu::ShaderModule shader_module{device, AssetId{"sandbox:shaders/basic.wgsl"}};

    const auto point_data = vector<tuple<vec2f, vec3f>>
    {
        {{-0.5, -0.5}, {1.0, 0.0, 0.0}},
        {{+0.5, -0.5}, {0.0, 1.0, 0.0}},
        {{+0.5, +0.5}, {0.0, 0.0, 1.0}},
        {{-0.5, +0.5}, {1.0, 1.0, 0.0}},
    };
    const wgpu::VertexBuffer point_buffer{device, "Position Buffer", point_data};
    point_buffer.add_attribute<vec2f>(0); // Position
    point_buffer.add_attribute<vec3f>(1); // Color

    const auto index_data = vector<u16>
    {
        0, 1, 2,
        0, 2, 3,
    };
    const wgpu::IndexBuffer index_buffer{device, "Index Buffer", index_data};

    const wgpu::BindGroup bind_group{device, "Uniforms Bind Group"};
    bind_group.add_binding(0, sizeof(Uniforms));
    constexpr Uniforms uniforms{{1.0f, 0.0f, 0.0f}};
    bind_group.write(0, 0, &uniforms, sizeof(Uniforms));

    const wgpu::RenderPipeline render_pipeline
    {
        device, "Render Pipeline", shader_module, {point_buffer}, {bind_group}
    };

    while (!window.should_close())
    {
        glfw.poll_events();

        wgpu::RenderPass render_pass{device, surface, window.get_framebuffer_size(), {0.05f, 0.05f, 0.05f}};

        render_pass.set_bind_group(0, bind_group, 0);
        render_pass.set_index_buffer(index_buffer);
        render_pass.set_pipeline(render_pipeline);
        render_pass.set_vertex_buffer(0, point_buffer);
        render_pass.draw_indexed(index_buffer.get_index_count());

        render_pass.end();

        surface.present();
        device.tick();
    }
}
