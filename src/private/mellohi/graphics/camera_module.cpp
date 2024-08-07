#include "mellohi/graphics/camera_module.hpp"

#include "mellohi/core/engine_module.hpp"
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
        world.import<InputModule>();
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

        world.system<InputModule, Position, const Rotation>("systems::MoveCamera")
                .term_at(0).singleton()
                .with<tags::CurrentCamera>()
                .kind<phases::Update>()
                .each(move_camera);

        const auto rotation_query = world.query_builder<Rotation>("queries::RotationQuery")
                .with<tags::CurrentCamera>()
                .cached()
                .build();

        world.observer("observers::RotateCamera")
                .with<InputModule>().singleton()
                .event<events::CapturedCursorMoved>()
                .each([rotation_query](flecs::iter &it, usize)
                {
                    const auto *event = static_cast<events::CapturedCursorMoved *>(it.param());

                    rotation_query.each([event](Rotation &rotation)
                    {
                        rotate_camera(rotation, event->cursor_delta);
                    });
                });

        world.system<const Position, const Rotation, Transform>("systems::UpdateTransformMatrix")
                .term_at(2).out()
                .with<CameraUniforms>()
                .kind<phases::PreUpdate>()
                .run(update_camera_transform_matrix);

        world.system<const GraphicsModule, const CameraModule, const Transform, CameraUniforms>("systems::BindCamera")
                .term_at(0).singleton()
                .term_at(1).singleton()
                .with<tags::CurrentCamera>()
                .kind<phases::PreRender>()
                .each(bind_camera);

        const auto uniforms_query = world.query_builder<CameraUniforms>("queries::UniformsQuery")
                .cached()
                .build();

        world.observer("observers::OnFramebufferResized")
                .with<GraphicsModule>().singleton()
                .event<events::FramebufferResized>()
                .each([uniforms_query](flecs::iter &it, usize)
                {
                    const auto *event = static_cast<events::FramebufferResized *>(it.param());

                    uniforms_query.each([event](CameraUniforms &camera_uniforms)
                    {
                        on_framebuffer_resized(*event, camera_uniforms);
                    });
                });
    }

    auto CameraModule::move_camera(const flecs::iter &it, usize, InputModule &input, Position &position,
                                   const Rotation &rotation) -> void
    {
        if (input.is_just_pressed(KeyboardButton::Escape))
        {
            if (input.is_cursor_captured())
            {
                input.uncapture_cursor();
            } else
            {
                input.capture_cursor();
            }
        }

        if (input.is_cursor_captured())
        {
            auto move_vec = input.get_vector(KeyboardButton::A, KeyboardButton::D,
                                             KeyboardButton::S, KeyboardButton::W);
            if (move_vec != vec2f{0.0f, 0.0f})
            {
                move_vec = glm::normalize(move_vec);

                auto direction = vec3f{move_vec.x, 0.0f, move_vec.y} * rotation * vec3f{1.0f, 0.0f, 1.0f};
                direction = glm::normalize(direction);

                position += direction * MOVE_SPEED * it.delta_time();
            }

            if (input.is_pressed(KeyboardButton::Space))
            {
                position.y += MOVE_SPEED * it.delta_time();
            }

            if (input.is_pressed(KeyboardButton::LeftShift))
            {
                position.y -= MOVE_SPEED * it.delta_time();
            }
        }
    }

    auto CameraModule::rotate_camera(Rotation &rotation, const vec2f cursor_delta) -> void
    {
        rotation = glm::angleAxis(glm::radians(-cursor_delta.y * ROTATE_SPEED), vec3f{1.0f, 0.0f, 0.0f}) * rotation;
        const auto world_y_axis = rotation * vec3f{0.0f, 1.0f, 0.0f};
        rotation = glm::angleAxis(glm::radians(-cursor_delta.x * ROTATE_SPEED), world_y_axis) * rotation;
    }

    auto CameraModule::update_camera_transform_matrix(flecs::iter &it) -> void
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

    auto CameraModule::bind_camera(const GraphicsModule &graphics, const CameraModule &camera_module,
                                   const Transform &transform, CameraUniforms &uniforms) -> void
    {
        uniforms.view = transform;

        camera_module.bind_group->write(0, 0, &uniforms, sizeof(CameraUniforms));

        graphics.render_pass->set_bind_group(0, *camera_module.bind_group, 0);
    }

    auto CameraModule::on_framebuffer_resized(const events::FramebufferResized event,
                                              CameraUniforms &camera_uniforms) -> void
    {
        camera_uniforms = CameraUniforms(event.framebuffer_size, camera_uniforms.view);
    }
}
