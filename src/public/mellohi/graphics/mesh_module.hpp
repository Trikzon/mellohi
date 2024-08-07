#pragma once

#include <flecs.h>

#include "mellohi/core/transform_module.hpp"
#include "mellohi/graphics/graphics_module.hpp"
#include "mellohi/graphics/wgpu/render_pipeline.hpp"
#include "mellohi/graphics/wgpu/vertex_buffer.hpp"

namespace mellohi
{
    struct MeshUniforms
    {
        mat4x4f model;
    };

    struct MeshData
    {
        u32 vertex_count;
        s_ptr<wgpu::VertexBuffer> vertex_buffer;
        s_ptr<wgpu::RenderPipeline> render_pipeline;

        MeshData(const wgpu::Device &device, const wgpu::ShaderModule &shader_module,
                 const vector<wgpu::BindGroup> &bind_groups, const AssetId &obj_file_id);
    };

    struct Mesh
    {
        AssetId asset_id{"mellohi:models/default_cube.obj"};
    };

    namespace prefabs
    {
        struct Mesh {};
    }

    struct MeshModule
    {
        s_ptr<wgpu::Device> device{nullptr};
        s_ptr<wgpu::BindGroup> camera_bind_group{nullptr}, model_bind_group{nullptr};
        s_ptr<wgpu::ShaderModule> shader_module{nullptr};
        hash_map<AssetId, MeshData> mesh_data{};
        usize dynamic_mesh_index{0};

        explicit MeshModule(flecs::world &world);

        auto get_mesh_data(const AssetId &asset_id) -> MeshData &;
    };

    namespace systems
    {
        auto reset_dynamic_mesh_index(MeshModule &mesh) -> void;
        auto render(const GraphicsModule &graphics, MeshModule &mesh_module, const Transform &transform,
                    const Mesh &mesh) -> void;
    }
}
