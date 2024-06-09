#include "mellohi/graphics/vertex_array.h"

#include <glad/glad.h>

#include "mellohi/log.h"

namespace mellohi
{
    VertexArray::VertexArray() : m_vao_id(0)
    {
        glGenVertexArrays(1, &m_vao_id);
    }

    VertexArray::~VertexArray()
    {
        glDeleteVertexArrays(1, &m_vao_id);
    }

    void VertexArray::bind() const
    {
        glBindVertexArray(m_vao_id);
    }

    // Requires the vao to be bound before calling.
    void VertexArray::draw() const
    {
        MH_ASSERT(m_index_buffer.has_value(), "Attempted to draw vertex array (id: {}) with no index buffer set.", m_vao_id);
        MH_ASSERT(!m_vertex_buffers.empty(), "Attempted to draw vertex array (id: {}) with no vertex buffers added.", m_vao_id);

        m_index_buffer->draw();
    }

    // Requires the vao to be bound before calling.
    void VertexArray::add_vertex_buffer(VertexBuffer&& vertex_buffer)
    {
        vertex_buffer.bind();

        m_vertex_buffers.push_back(std::move(vertex_buffer));
    }

    // Requires the vao to be bound before calling.
    void VertexArray::set_index_buffer(IndexBuffer&& index_buffer)
    {
        index_buffer.bind();

        m_index_buffer = std::move(index_buffer);
    }
}
