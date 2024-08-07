#include "mellohi/graphics/camera_module.hpp"

#include "mellohi/core/engine_module.hpp"
#include "mellohi/core/transform_module.hpp"
#include "mellohi/graphics/graphics_module.hpp"

namespace mellohi
{
    CameraUniforms::CameraUniforms(const vec2u framebuffer_size, const mat4x4f &view)
    {
        projection = glm::perspective(glm::radians(45.0f),
                                      static_cast<f32>(framebuffer_size.x) / static_cast<f32>(framebuffer_size.y),
                                      CameraModule::NEAR, CameraModule::FAR);
        this->view = view;
    }

    CameraModule::CameraModule(flecs::world &world)
    {
        world.import<GraphicsModule>();
        world.import<TransformModule>();

        world.component<CameraUniforms>();
        world.component<tags::CurrentCamera>("tags::CurrentCamera");

        const auto &graphics = world.ensure<GraphicsModule>();

        bind_group = std::make_shared<wgpu::BindGroup>(*graphics.device, "Camera Bind Group");
        bind_group->add_binding(0, sizeof(CameraUniforms));

        world.prefab<prefabs::Camera>("prefabs::Camera")
                .emplace<Position>()
                .emplace<Rotation>(1.0f, 0.0f, 0.0f, 0.0f)
                .emplace<Transform>()
                .emplace<CameraUniforms>(graphics.window->get_framebuffer_size(), mat4x4f{1.0f})
                .add<tags::CurrentCamera>();

        world.system<const Position, const Rotation, Transform>("systems::UpdateTransformMatrix")
                .term_at(2).out()
                .with<CameraUniforms>()
                .kind<phases::PreUpdate>()
                .run(systems::update_camera_transform_matrix);

        world.system<const Transform, CameraUniforms>("systems::BindCamera")
                .with<tags::CurrentCamera>()
                .kind<phases::PreRender>()
                .run(systems::bind_camera);

        const auto camera_query = world.query_builder<CameraUniforms>("queries::CameraQuery")
                .cached()
                .build();

        world.observer("observers::OnFramebufferResized")
                .with<GraphicsModule>().singleton()
                .event<events::FramebufferResized>()
                .each([camera_query](flecs::iter &it, usize)
                {
                    const auto *event = static_cast<events::FramebufferResized *>(it.param());

                    camera_query.each([event](CameraUniforms &camera_uniforms)
                    {
                        systems::on_framebuffer_resized(*event, camera_uniforms);
                    });
                });
    }

    namespace systems
    {
        auto update_camera_transform_matrix(flecs::iter &it) -> void
        {
            while (it.next())
            {
                if (!it.changed())
                {
                    continue;
                }

                const auto position = it.field<const Position>(0);
                const auto rotation = it.field<const Rotation>(1);
                const auto transform = it.field<Transform>(2);

                for (const auto i: it)
                {
                    const mat4x4f t = glm::translate(mat4x4f{1.0f}, -position[i]);
                    const mat4x4f r = glm::toMat4(rotation[i]);

                    transform[i] = r * t;
                }
            }
        }

        auto bind_camera(flecs::iter &it) -> void
        {
            const auto &camera = it.world().ensure<CameraModule>();
            const auto &graphics = it.world().ensure<GraphicsModule>();

            while (it.next())
            {
                const auto transform = it.field<const Transform>(0);
                const auto camera_uniforms = it.field<CameraUniforms>(1);

                camera_uniforms->view = *transform;

                camera.bind_group->write(0, 0, &*camera_uniforms, sizeof(CameraUniforms));

                graphics.render_pass->set_bind_group(0, *camera.bind_group, 0);
            }
        }

        auto on_framebuffer_resized(const events::FramebufferResized event, CameraUniforms &camera_uniforms) -> void
        {
            camera_uniforms = CameraUniforms(event.framebuffer_size, camera_uniforms.view);
        }
    }
}
