#include "mellohi/camera.hpp"

#include <glm/gtx/string_cast.hpp>

#include "mellohi/input.hpp"
#include "mellohi/graphics/window.hpp"
#include "mellohi/transform.hpp"

namespace mellohi
{
    CameraUniforms::CameraUniforms()
    {
        constexpr f32 near = 0.001f;
        constexpr f32 far = 100.0f;
        constexpr f32 ratio = 640.0f / 480.0f;
        const float fov = 2.0f * glm::atan(1.0f / 2.0f);
        projection = glm::perspective(fov, ratio, near, far);
    }

    CameraModule::CameraModule(flecs::world &world)
    {
        world.import<TransformModule>();

        {
            const auto *window = world.get<Window>();

            bind_group = new BindGroup(window->get_device());
            bind_group->add_binding(window->get_device(), 0, sizeof(CameraUniforms));
        }

        world.entity()
            .emplace<CameraUniforms>()
            .is_a<TransformPrefab>()
            .set<Rotation>(normalize(angleAxis(glm::radians(-30.0f), vec3f{1.0f, 0.0f, 0.0f})))
            .set<Position>({0.0, 5.0, -10});

        world.system<Position, Rotation>()
            .with<CameraUniforms>()
            .run([](flecs::iter &it)
            {
                const auto input = it.world().get<const Input>();

                while (it.next())
                {
                    const auto rot_dir = input->get_vector(KeyboardButton::Down, KeyboardButton::Up, KeyboardButton::Right, KeyboardButton::Left);
                    const auto move_dir = input->get_vector(KeyboardButton::A, KeyboardButton::D, KeyboardButton::S, KeyboardButton::W);

                    if (rot_dir == vec2f{0.0f, 0.0f} && move_dir == vec2f{0.0f, 0.0f})
                    {
                        it.skip();
                        continue;
                    }

                    const auto position = it.field<Position>(0);
                    const auto rotation = it.field<Rotation>(1);

                    for (const auto i : it)
                    {
                        rotation[i] = angleAxis(glm::radians(rot_dir.x * 50 * it.delta_time()), vec3f{1.0f, 0.0f, 0.0f}) * rotation[i];
                        const auto y_axis = rotation[i] * vec3f{0.0f, 1.0f, 0.0f};
                        rotation[i] = angleAxis(glm::radians(rot_dir.y) * 75 * it.delta_time(), y_axis) * rotation[i];
                        rotation[i] = normalize(rotation[i]);

                        const auto m = vec3f{move_dir.x, 0.0f, move_dir.y} * rotation[i];
                        position[i] += m * 5.0f * it.delta_time();
                    }
                }
            });

        world.system<const CameraUniforms, const Transform>()
            .kind(flecs::PreStore)
            .run([](flecs::iter &it)
            {
                const auto *camera_module = it.world().get<CameraModule>();
                const auto *window = it.world().get<Window>();
                auto *render_pass = it.world().get_mut<RenderPass>();

                while (it.next())
                {
                    const auto camera = it.field<const CameraUniforms>(0);
                    const auto transform = it.field<const Transform>(1);

                    CameraUniforms camera_uniforms{};
                    camera_uniforms.view = *transform;
                    camera_uniforms.projection = camera->projection;

                    camera_module->bind_group->write(window->get_device(), 0, 0, &camera_uniforms);

                    render_pass->set_bind_group(window->get_device(), 0, *camera_module->bind_group, 0);
                }
            });
    }

    CameraModule::~CameraModule()
    {
        delete bind_group;
    }

    CameraModule::CameraModule(CameraModule &&other) noexcept
    {
        std::swap(bind_group, other.bind_group);
    }

    CameraModule & CameraModule::operator=(CameraModule &&other) noexcept
    {
        if (this != &other)
        {
            std::swap(bind_group, other.bind_group);
        }

        return *this;
    }
}
