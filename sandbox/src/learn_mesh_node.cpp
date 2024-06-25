#include "learn_mesh_node.h"

#include <sstream>

#include <GLFW/glfw3.h>

LearnMeshNode::LearnMeshNode(const std::string_view &name) : LearnMeshNode(std::nullopt, name) { }

LearnMeshNode::LearnMeshNode(const std::optional<AssetId> &learn_mesh_asset_id, const std::string_view &name)
    : Node(name), m_color{0.0, 0.0, 0.0, 1.0}
{
    if (learn_mesh_asset_id)
    {
        load_mesh(*learn_mesh_asset_id);
    }
}

void LearnMeshNode::load_mesh(const AssetId &learn_mesh_asset_id)
{
    std::string learn_mesh_contents = learn_mesh_asset_id.read_file_to_string();

    std::vector<float> point_data;
    std::vector<uint16_t> index_data;

    enum class Section
    {
        None,
        Points,
        Indices,
    };
    auto current_section = Section::None;

    float value;
    uint16_t index;
    std::string line;
    std::istringstream learn_mesh_stream(learn_mesh_contents);
    while (std::getline(learn_mesh_stream, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        if (line == "[points]")
        {
            current_section = Section::Points;
        }
        else if (line == "[indices]")
        {
            current_section = Section::Indices;
        }
        else if (line[0] == '#' || line.empty())
        {
            // NOP
        }
        else if (current_section == Section::Points)
        {
            std::istringstream line_stream(line);
            // Get x, y, r, g, b
            for (int i = 0; i < 5; ++i)
            {
                line_stream >> value;
                point_data.push_back(value);
            }
        }
        else if (current_section == Section::Indices)
        {
            std::istringstream line_stream(line);
            // Get corners #0, #1, and #2
            for (int i = 0; i < 3; ++i)
            {
                line_stream >> index;
                index_data.push_back(index);
            }
        }
    }

    m_vertex_buffer = std::make_unique<VertexBuffer>(point_data);
    m_vertex_buffer->add_attribute_vec2f();  // Position
    m_vertex_buffer->add_attribute_vec3f();  // Color

    m_index_buffer = std::make_unique<IndexBuffer>(index_data);

    MyUniforms my_uniforms{};
    my_uniforms.color = m_color;
    m_uniform_buffer = std::make_unique<UniformBuffer>(0, my_uniforms);

    m_pipeline = std::make_unique<Pipeline>(AssetId::fromGame("learn.wgsl"), *m_vertex_buffer);

    m_bind_group = std::make_unique<BindGroup>(*m_pipeline, *m_uniform_buffer);
}

void LearnMeshNode::set_color(const std::array<float, 4> &color)
{
    m_color = color;
}

void LearnMeshNode::on_render(RenderPass &render_pass, double delta_time)
{
    if (!m_vertex_buffer || !m_index_buffer || !m_uniform_buffer || !m_pipeline || !m_bind_group)
    {
        return;
    }

    MyUniforms my_uniforms{};
    my_uniforms.color = m_color;
    my_uniforms.time = static_cast<float>(glfwGetTime()) + offset;

    m_uniform_buffer->write(my_uniforms);

    render_pass.set_pipeline(*m_pipeline);
    render_pass.set_vertex_buffer(0, *m_vertex_buffer);
    render_pass.set_index_buffer(*m_index_buffer);
    render_pass.set_bind_group(*m_bind_group);

    render_pass.draw_indexed();
}
