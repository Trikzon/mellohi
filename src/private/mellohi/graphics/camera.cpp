#include "mellohi/graphics/camera.hpp"

#include "mellohi/core/engine.hpp"
#include "mellohi/core/transform.hpp"
#include "mellohi/graphics/graphics.hpp"
#include "mellohi/graphics/wgpu/queue.hpp"

namespace mellohi
{
    Camera::Uniforms::Uniforms(const vec2u framebuffer_size, const mat4x4f &view)
    {
        projection = glm::perspective(
            glm::radians(45.0f),
            static_cast<f32>(framebuffer_size.x) / static_cast<f32>(framebuffer_size.y),
            NEAR,
            FAR
        );

        this->view = view;
    }

    Camera::Camera(flecs::world &ecs)
    {
        ecs.import<Transform>();
        ecs.import<Graphics>();

        const glfw::Window *window = ecs.get<glfw::Window>();
        const wgpu::Device *device = ecs.get<wgpu::Device>();

        bind_group = std::make_shared<wgpu::BindGroup>(*device, "Camera Bind Group");
        bind_group->add_binding(0, sizeof(Uniforms));

        ecs.entity("Camera")
                .is_a<Transform::Prefab>()
                .set<Position>({0.0f, 0.0f, -3.0f})
                .emplace<Uniforms>(window->get_framebuffer_size(), mat4x4f{1.0f});

        ecs.system<const TransformMatrix, Uniforms>()
                .kind<phase::PreRender>()
                .run(pre_render);

        ecs.observer<Uniforms>()
                .with<glfw::Window>()
                .term_at(0).src("Camera")
                .event<event::WindowResized>()
                .each(on_window_resized);
    }

    auto Camera::pre_render(flecs::iter &it) -> void
    {
        const auto *camera = it.world().get<Camera>();
        const auto *render_pass = it.world().get<wgpu::RenderPass>();

        while (it.next())
        {
            const auto transform = it.field<const TransformMatrix>(0);
            const auto uniforms = it.field<Uniforms>(1);

            uniforms->view = *transform;

            camera->bind_group->write(0, 0, &*uniforms, sizeof(Uniforms));

            render_pass->set_bind_group(0, *camera->bind_group, 0);
        }
    }

    auto Camera::on_window_resized(flecs::iter &it, usize, Uniforms &uniforms) -> void
    {
        const auto event = static_cast<event::WindowResized *>(it.param());

        uniforms = Uniforms(event->new_size, uniforms.view);
    }
}
