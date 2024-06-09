#include <memory>

#include <mellohi/app.h>
#include <mellohi/graphics/buffer.h>
#include <mellohi/graphics/shader.h>
#include <mellohi/graphics/texture.h>
#include <mellohi/graphics/vertex_array.h>
#include <mellohi/math/mat4.h>
#include <mellohi/math/vec3.h>
#include <mellohi/types.h>

#include "debug_overlay_node.h"
#include "mellohi/asset_id.h"

using namespace mellohi;

constexpr f32 positions[] = {
    100.0f, 100.0f, 0.0f, 0.0f,
    200.0f, 100.0f, 1.0f, 0.0f,
    200.0f, 200.0f, 1.0f, 1.0f,
    100.0f, 200.0f, 0.0f, 1.0f,
};

constexpr u32 indices[] = {
    0, 1, 2,
    2, 3, 0,
};

class SandboxApp final : public App
{
public:
    SandboxApp() : App("sandbox") { }

private:
    std::unique_ptr<VertexArray> m_vertex_array;
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<Texture> m_texture;

    void on_run() override
    {
        m_root_node.add_child(std::make_shared<DebugOverlayNode>());

        m_vertex_array = std::make_unique<VertexArray>();
        {
            VertexBuffer vertex_buffer(positions, sizeof(float) * 4 * 4);
            vertex_buffer.add_attribute_f32(2);
            vertex_buffer.add_attribute_f32(2);

            IndexBuffer index_buffer(indices, 6);

            m_vertex_array->bind();
            m_vertex_array->add_vertex_buffer(std::move(vertex_buffer));
            m_vertex_array->set_index_buffer(std::move(index_buffer));
        }

        m_vertex_array->bind();
        m_shader = std::make_unique<Shader>(AssetId{"shaders/test.frag"}, AssetId{"shaders/test.vert"});
        m_shader->bind();

        m_texture = std::make_unique<Texture>(AssetId{"voxels/dirt.png"});
        m_texture->bind(0);
        m_shader->set_uniform_i32("u_Texture", 0);

        const mat4 proj = mat4::orthographic(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
        const mat4 view = mat4{1.0f}.translated(vec3{-100, 0, 0});
        const mat4 mvp = proj * view;
        m_shader->set_uniform_mat4("u_MVP", mvp);
    }

    void on_update() override
    {
        if (m_shader && m_vertex_array)
        {
            m_shader->bind();
            m_vertex_array->bind();
            m_vertex_array->draw();
        }
    }
};

int main()
{
    SandboxApp app;
    app.run();
}
