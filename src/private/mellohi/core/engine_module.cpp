#include "mellohi/core/engine_module.hpp"

#include <iostream>

#include "mellohi/graphics/camera_module.hpp"
#include "mellohi/core/input_module.hpp"
#include "mellohi/core/transform_module.hpp"
#include "mellohi/graphics/graphics_module.hpp"
#include "mellohi/graphics/imgui_module.hpp"
#include "mellohi/graphics/mesh_module.hpp"
#include "mellohi/graphics/performance_hud_module.hpp"

namespace mellohi
{
        EngineModule::EngineModule(flecs::world &world)
        {
                std::cout << R"(
   ___       ___       ___       ___       ___       ___       ___
  /\__\     /\  \     /\__\     /\__\     /\  \     /\__\     /\  \
 /::L_L_   /::\  \   /:/  /    /:/  /    /::\  \   /:/__/_   _\:\  \
/:/L:\__\ /::\:\__\ /:/__/    /:/__/    /:/\:\__\ /::\/\__\ /\/::\__\
\/_/:/  / \:\:\/  / \:\  \    \:\  \    \:\/:/  / \/\::/  / \::/\/__/
  /:/  /   \:\/  /   \:\__\    \:\__\    \::/  /    /:/  /   \:\__\
  \/__/     \/__/     \/__/     \/__/     \/__/     \/__/     \/__/  )" << std::endl << std::endl;

                world.entity<phases::PreTick>("phases::PreTick")
                                .add(flecs::Phase).add(flecs::Final);
                world.entity<phases::PreUpdate>("phases::PreUpdate")
                                .add(flecs::Phase).add(flecs::Final).depends_on<phases::PreTick>();
                world.entity<phases::Update>("phases::Update")
                                .add(flecs::Phase).add(flecs::Final).depends_on<phases::PreUpdate>();
                world.entity<phases::PreRender>("phases::PreRender")
                                .add(flecs::Phase).add(flecs::Final).depends_on<phases::Update>();
                world.entity<phases::Render>("phases::Render")
                                .add(flecs::Phase).add(flecs::Final).depends_on<phases::PreRender>();
                world.entity<phases::RenderImGui>("phases::RenderImGui")
                                .add(flecs::Phase).add(flecs::Final).depends_on<phases::Render>();
                world.entity<phases::PostRender>("phases::PostRender")
                                .add(flecs::Phase).add(flecs::Final).depends_on<phases::RenderImGui>();
                world.entity<phases::PostTick>("phases::PostTick")
                                .add(flecs::Phase).add(flecs::Final).depends_on<phases::PostRender>();

                world.import<CameraModule>();
                world.import<GraphicsModule>();
                world.import<ImGuiModule>();
                world.import<InputModule>();
                world.import<MeshModule>();
                world.import<PerformanceHudModule>();
                world.import<TransformModule>();
        }
}
