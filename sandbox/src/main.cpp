#include <iostream>
#include <string>

#include <imgui.h>

#include "mellohi/asset_id.h"
#include "mellohi/game.h"
#include "mellohi/graphics/buffer.h"
#include "mellohi/graphics/pipeline.h"
#include "mellohi/graphics/window.h"

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

class Sandbox final : public mellohi::Game
{
public:
    Sandbox() : Game("sandbox") { }

private:
    std::unique_ptr<mellohi::VertexBuffer> m_vertex_buffer;
    std::unique_ptr<mellohi::IndexBuffer> m_index_buffer;
    std::unique_ptr<mellohi::Pipeline> m_pipeline;

    void on_run() override
    {
        mellohi::Device& device = get_window().get_device();

        m_vertex_buffer = std::make_unique<mellohi::VertexBuffer>(device, vertex_data);
        m_vertex_buffer->add_attribute_vec2f();  // Position
        m_vertex_buffer->add_attribute_vec3f();  // Color

        m_index_buffer = std::make_unique<mellohi::IndexBuffer>(device, index_data);

        m_pipeline = std::make_unique<mellohi::Pipeline>(device, get_window().get_surface(), mellohi::AssetId::fromGame("square.wgsl"), *m_vertex_buffer);
    }

    void on_update(wgpu::RenderPassEncoder render_pass) override
    {
        render_pass.setPipeline(m_pipeline->get_unsafe());
        render_pass.setVertexBuffer(0, m_vertex_buffer->get_unsafe(), 0, vertex_data.size() * sizeof(float));
        render_pass.setIndexBuffer(m_index_buffer->get_unsafe(), m_index_buffer->get_wgpu_format(), 0, index_data.size() * sizeof(uint16_t));

        render_pass.drawIndexed(index_count, 1, 0, 0, 0);

        ImGui::ShowMetricsWindow();
    }
};

int main()
{
    Sandbox game;

    game.run();
}
