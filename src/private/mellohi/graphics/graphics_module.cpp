#include "mellohi/graphics/graphics_module.hpp"

#include "mellohi/core/engine_module.hpp"
#include "mellohi/graphics/glfw/glfw.hpp"
#include "mellohi/graphics/wgpu/render_pass.hpp"

namespace mellohi
{
    GraphicsModule::GraphicsModule(flecs::world &world)
    {
        world.component<events::FramebufferResized>("events::FramebufferResized");

        window = std::make_shared<glfw::Window>("Mellohi", vec2u{800, 600});
        const wgpu::Instance instance;
        surface = std::make_shared<wgpu::Surface>(instance, *window);
        const wgpu::Adapter adapter{instance, *surface};
        device = std::make_shared<wgpu::Device>(adapter, adapter.get_limits());

        surface->configure(*device, window->get_framebuffer_size(), false);
        window->set_framebuffer_size_callback([world](const vec2u framebuffer_size)
        {
            world.event<events::FramebufferResized>()
                    .id<GraphicsModule>()
                    .entity(world.singleton<GraphicsModule>())
                    .ctx({framebuffer_size})
                    .emit();
        });

        const auto create_render_pass_phase = world.entity("phases::CreateRenderPass")
                .add(flecs::Phase).add(flecs::Final)
                .depends_on<phases::PreUpdate>();

        const auto end_render_pass_phase = world.entity("phases::EndRenderPass")
                .add(flecs::Phase).add(flecs::Final)
                .depends_on<phases::PostRender>();

        world.system<const GraphicsModule>("systems::PollEvents")
                .term_at(0).singleton()
                .kind<phases::PreTick>()
                .each(systems::poll_events);

        world.system<GraphicsModule>("systems::CreateRenderPass")
                .term_at(0).singleton()
                .kind(create_render_pass_phase)
                .each(systems::create_render_pass);

        world.system<GraphicsModule>("systems::EndRenderPass")
                .term_at(0).singleton()
                .kind(end_render_pass_phase)
                .each(systems::end_render_pass);

        world.system<const GraphicsModule>("systems::Present")
                .term_at(0).singleton()
                .kind<phases::PostTick>()
                .each(systems::present);

        world.observer<const GraphicsModule>("observers::OnFramebufferResized")
                .term_at(0).singleton()
                .event<events::FramebufferResized>()
                .each(systems::on_framebuffer_resized);
    }

    namespace systems
    {
        auto poll_events(const flecs::iter &it, usize, const GraphicsModule &graphics) -> void
        {
            if (graphics.window->should_close())
            {
                it.world().quit();
            }

            glfw::Glfw::get().poll_events();
        }

        auto create_render_pass(GraphicsModule &graphics) -> void
        {
            graphics.render_pass = wgpu::RenderPass{
                *graphics.device, *graphics.surface, graphics.window->get_framebuffer_size(), vec3f{0.1f, 0.05f, 0.1f}
            };
        }

        auto end_render_pass(GraphicsModule &graphics) -> void
        {
            graphics.render_pass->end();

            graphics.render_pass.reset();
        }

        auto present(const GraphicsModule &graphics) -> void
        {
            graphics.surface->present();
            graphics.device->tick();
        }

        auto on_framebuffer_resized(flecs::iter &it, usize, const GraphicsModule &graphics) -> void
        {
            const auto event = static_cast<events::FramebufferResized *>(it.param());

            graphics.surface->configure(*graphics.device, event->framebuffer_size, false);
        }
    }
}
