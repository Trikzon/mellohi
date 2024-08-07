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

    class MeshModule
    {
    public:
        explicit MeshModule(flecs::world &world);

        auto get_mesh_data(const AssetId &asset_id) -> MeshData &;

    private:
        s_ptr<wgpu::Device> m_device{nullptr};
        s_ptr<wgpu::BindGroup> m_camera_bind_group{nullptr}, m_model_bind_group{nullptr};
        s_ptr<wgpu::ShaderModule> m_shader_module{nullptr};
        hash_map<AssetId, MeshData> m_mesh_data{};
        usize m_dynamic_mesh_index{0};

        static auto reset_dynamic_mesh_index(MeshModule &mesh) -> void;
        static auto render(const GraphicsModule &graphics, MeshModule &mesh_module, const Transform &transform,
                           const Mesh &mesh) -> void;
    };
}
