#include "mellohi/core/engine.hpp"

#include <iostream>

#include "mellohi/core/transform.hpp"
#include "mellohi/graphics/glfw/glfw.hpp"

namespace mellohi
{
    Engine::Engine(flecs::world &ecs)
    {
        std::cout << R"(
   ___       ___       ___       ___       ___       ___       ___
  /\__\     /\  \     /\__\     /\__\     /\  \     /\__\     /\  \
 /::L_L_   /::\  \   /:/  /    /:/  /    /::\  \   /:/__/_   _\:\  \
/:/L:\__\ /::\:\__\ /:/__/    /:/__/    /:/\:\__\ /::\/\__\ /\/::\__\
\/_/:/  / \:\:\/  / \:\  \    \:\  \    \:\/:/  / \/\::/  / \::/\/__/
  /:/  /   \:\/  /   \:\__\    \:\__\    \::/  /    /:/  /   \:\__\
  \/__/     \/__/     \/__/     \/__/     \/__/     \/__/     \/__/  )" << std::endl << std::endl;

        ecs.entity<phase::PreTick>().add(flecs::Phase).add(flecs::Final);
        ecs.entity<phase::PreUpdate>().add(flecs::Phase).add(flecs::Final).depends_on<phase::PreTick>();
        ecs.entity<phase::PreRender>().add(flecs::Phase).add(flecs::Final).depends_on<phase::PreUpdate>();
        ecs.entity<phase::Render>().add(flecs::Phase).add(flecs::Final).depends_on<phase::PreRender>();
        ecs.entity<phase::PostRender>().add(flecs::Phase).add(flecs::Final).depends_on<phase::Render>();
        ecs.entity<phase::PostTick>().add(flecs::Phase).add(flecs::Final).depends_on<phase::PostRender>();

        ecs.import<Transform>();

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
            ecs.event<event::Resized>().id<glfw::Window>().entity(ecs.singleton<glfw::Window>()).ctx({size}).emit();
        });

        ecs.system<const glfw::Window>()
                .term_at(0).singleton()
                .kind<phase::PreTick>()
                .each(pre_tick);

        ecs.system<const glfw::Window, const wgpu::Surface, const wgpu::Device>()
                .term_at(0).singleton()
                .term_at(1).singleton()
                .term_at(2).singleton()
                .kind<phase::PreRender>()
                .immediate()
                .each(pre_render);

        ecs.system()
                .kind<phase::PostRender>()
                .immediate()
                .each(post_render);

        ecs.system<const wgpu::Surface, const wgpu::Device>()
                .term_at(0).singleton()
                .term_at(1).singleton()
                .kind<phase::PostTick>()
                .each(post_tick);

        ecs.observer<const wgpu::Device, const wgpu::Surface>()
                .with<glfw::Window>()
                .term_at(0).singleton()
                .term_at(1).singleton()
                .event<event::Resized>()
                .each(on_window_resized);
    }

    auto Engine::pre_tick(const flecs::iter &it, usize, const glfw::Window &window) -> void
    {
        if (window.should_close())
        {
            it.world().quit();
        }

        glfw::Glfw::get_instance().poll_events();
    }

    auto Engine::pre_render(const flecs::iter &it, usize, const glfw::Window &window, const wgpu::Surface &surface,
                            const wgpu::Device &device) -> void
    {
        it.world().emplace<wgpu::RenderPass>(device, surface, window.get_framebuffer_size(),
                                             vec3f{0.05f, 0.05f, 0.05f});
    }

    auto Engine::post_render(const flecs::iter &it, usize) -> void
    {
        const auto *render_pass = it.world().get<wgpu::RenderPass>();
        if (render_pass)
        {
            render_pass->end();
        }

        it.world().remove<wgpu::RenderPass>();
    }

    auto Engine::post_tick(const wgpu::Surface &surface, const wgpu::Device &device) -> void
    {
        surface.present();
        device.tick();
    }

    auto Engine::on_window_resized(flecs::iter &it, usize, const wgpu::Device &device,
                                   const wgpu::Surface &surface) -> void
    {
        const auto event = static_cast<event::Resized *>(it.param());

        surface.configure(device, event->new_size, false);
    }
}
