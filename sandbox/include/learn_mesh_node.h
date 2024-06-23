#pragma once

#include <mellohi/asset_id.h>
#include <mellohi/nodes/node.h>

using namespace mellohi;

// A mesh loaded from a basic custom model format used by Learn WebGPU for C++
// https://eliemichel.github.io/LearnWebGPU/basic-3d-rendering/input-geometry/loading-from-file.html
class LearnMeshNode final : public Node
{
public:
    explicit LearnMeshNode(const std::string_view &name = "LearnMeshNode");
    explicit LearnMeshNode(const std::optional<AssetId> &learn_mesh_asset_id, const std::string_view &name = "LearnMeshNode");

    void load_mesh(const AssetId &learn_mesh_asset_id);

private:
    std::unique_ptr<VertexBuffer> m_vertex_buffer;
    std::unique_ptr<IndexBuffer> m_index_buffer;
    std::unique_ptr<Pipeline> m_pipeline;

    void on_render(RenderPass &render_pass, double delta_time) override;
};
