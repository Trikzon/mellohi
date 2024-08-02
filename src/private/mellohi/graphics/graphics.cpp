#include "mellohi/graphics/graphics.hpp"

#include "mellohi/core/engine.hpp"
#include "mellohi/graphics/glfw.hpp"

namespace mellohi
{
    Graphics::Graphics(flecs::world &ecs)
    {
        const glfw::Window window{"Mellohi", vec2u{800, 600}};
        const wgpu::Instance instance;
        const wgpu::Surface surface{instance, window};
        const wgpu::Adapter adapter{instance, surface};
        const wgpu::Device device{adapter, adapter.get_limits()};

        ecs.emplace<glfw::Window>(window);
        ecs.emplace<wgpu::Surface>(surface);
        ecs.emplace<wgpu::Device>(device);

        surface.configure(device, window.get_framebuffer_size(), false);
        window.set_framebuffer_size_callback([ecs](const vec2u size) -> void
        {
            ecs.event<event::WindowResized>().id<glfw::Window>().entity(ecs.singleton<glfw::Window>()).ctx({size})
                    .emit();
        });

        const auto create_render_pass_phase = ecs.entity()
                .add(flecs::Phase).add(flecs::Final)
                .depends_on<phase::PreUpdate>();

        const auto end_render_pass_phase = ecs.entity()
                .add(flecs::Phase).add(flecs::Final)
                .depends_on<phase::PostRender>();

        ecs.system<const glfw::Window>()
                .term_at(0).singleton()
                .kind<phase::PreTick>()
                .each(poll_events);

        ecs.system<const glfw::Window, const wgpu::Surface, const wgpu::Device>()
                .term_at(0).singleton()
                .term_at(1).singleton()
                .term_at(2).singleton()
                .kind(create_render_pass_phase)
                .immediate()
                .each(create_render_pass);

        ecs.system()
                .kind(end_render_pass_phase)
                .immediate()
                .each(end_render_pass);

        ecs.system<const wgpu::Surface, const wgpu::Device>()
                .term_at(0).singleton()
                .term_at(1).singleton()
                .kind<phase::PostTick>()
                .each(present);

        ecs.observer<const wgpu::Device, const wgpu::Surface>()
                .with<glfw::Window>()
                .term_at(0).singleton()
                .term_at(1).singleton()
                .event<event::WindowResized>()
                .each(on_window_resized);
    }

    auto Graphics::poll_events(const flecs::iter &it, usize, const glfw::Window &window) -> void
    {
        if (window.should_close())
        {
            it.world().quit();
        }

        glfw::Glfw::get_instance().poll_events();
    }

    auto Graphics::create_render_pass(const flecs::iter &it, usize, const glfw::Window &window,
                                      const wgpu::Surface &surface, const wgpu::Device &device) -> void
    {
        it.world().emplace<wgpu::RenderPass>(device, surface, window.get_framebuffer_size(),
                                             vec3f{0.05f, 0.05f, 0.05f});
    }

    auto Graphics::end_render_pass(const flecs::iter &it, usize) -> void
    {
        const auto *render_pass = it.world().get<wgpu::RenderPass>();
        if (render_pass)
        {
            render_pass->end();
        }

        it.world().remove<wgpu::RenderPass>();
    }

    auto Graphics::present(const wgpu::Surface &surface, const wgpu::Device &device) -> void
    {
        surface.present();
        device.tick();
    }

    auto Graphics::on_window_resized(flecs::iter &it, usize, const wgpu::Device &device,
                                     const wgpu::Surface &surface) -> void
    {
        const auto event = static_cast<event::WindowResized *>(it.param());

        surface.configure(device, event->new_size, false);
    }
}
