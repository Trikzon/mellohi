#pragma once

#include <mellohi/asset_id.h>
#include <mellohi/nodes/node.h>

using namespace mellohi;

// Generated with https://eliemichel.github.io/WebGPU-AutoLayout/
struct MyUniforms {
    float time; // at byte offset 0
    float _pad0[3];
    std::array<float, 4> color; // at byte offset 16
};

// A mesh loaded from a basic custom model format used by Learn WebGPU for C++
// https://eliemichel.github.io/LearnWebGPU/basic-3d-rendering/input-geometry/loading-from-file.html
class LearnMeshNode final : public Node
{
public:
    float offset = 0.0;

    explicit LearnMeshNode(const std::string_view &name = "LearnMeshNode");
    explicit LearnMeshNode(const std::optional<AssetId> &learn_mesh_asset_id, const std::string_view &name = "LearnMeshNode");

    void load_mesh(const AssetId &learn_mesh_asset_id);

    void set_color(const std::array<float, 4> &color);

private:
    std::unique_ptr<VertexBuffer> m_vertex_buffer;
    std::unique_ptr<IndexBuffer> m_index_buffer;
    std::unique_ptr<UniformBuffer> m_uniform_buffer;
    std::unique_ptr<Pipeline> m_pipeline;
    std::unique_ptr<BindGroup> m_bind_group;

    std::array<float, 4> m_color;

    void on_render(RenderPass &render_pass, double delta_time) override;
};
