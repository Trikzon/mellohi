#include <iostream>

#include <imgui.h>

#include "mellohi/asset_id.h"
#include "mellohi/game.h"
#include "mellohi/graphics/buffer.h"
#include "mellohi/graphics/pipeline.h"

std::vector<float> vertex_data = {
    -0.5, -0.5,     1.0, 0.0, 0.0,
    +0.5, -0.5,     0.0, 1.0, 0.0,
    +0.5, +0.5,     0.0, 0.0, 1.0,
    -0.5, +0.5,     1.0, 1.0, 0.0,
};

std::vector<uint16_t> index_data = {
    0, 1, 2,  // Triangle 0
    0, 2, 3,  // Triangle 1
};
auto index_count = static_cast<uint32_t>(index_data.size());

class TestNode final : public mellohi::Node
{
public:
    TestNode() : Node("TestNode")
    {
        m_vertex_buffer = std::make_unique<mellohi::VertexBuffer>(vertex_data);
        m_vertex_buffer->add_attribute_vec2f();  // Position
        m_vertex_buffer->add_attribute_vec3f();  // Color

        m_index_buffer = std::make_unique<mellohi::IndexBuffer>(index_data);

        m_pipeline = std::make_unique<mellohi::Pipeline>(mellohi::AssetId::fromGame("square.wgsl"), *m_vertex_buffer);
    }

private:
    std::unique_ptr<mellohi::VertexBuffer> m_vertex_buffer;
    std::unique_ptr<mellohi::IndexBuffer> m_index_buffer;
    std::unique_ptr<mellohi::Pipeline> m_pipeline;

    void on_render(mellohi::RenderPass &render_pass, double delta_time) override
    {
        render_pass.set_pipeline(*m_pipeline);
        render_pass.set_vertex_buffer(0, *m_vertex_buffer);
        render_pass.set_index_buffer(*m_index_buffer);

        render_pass.draw_indexed();

        ImGui::ShowMetricsWindow();
    }
};

class Sandbox final : public mellohi::Game
{
public:
    Sandbox() : Game("sandbox")
    {
        get_root_node().add_child(std::make_shared<TestNode>());
    }
};

int main()
{
    const auto game = new Sandbox();

    game->run();

    delete game;
}
