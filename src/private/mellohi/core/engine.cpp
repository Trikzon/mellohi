#include "mellohi/core/engine.hpp"

#include <iostream>

#include "mellohi/core/transform.hpp"
#include "mellohi/graphics/camera.hpp"
#include "mellohi/graphics/graphics.hpp"
#include "mellohi/graphics/mesh.hpp"

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
        ecs.import<Graphics>();
        ecs.import<Camera>();
        ecs.import<Mesh>();
    }
}
