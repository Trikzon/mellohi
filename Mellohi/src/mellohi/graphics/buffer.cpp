#include "mellohi/graphics/buffer.h"

#include <glad/glad.h>

#include "mellohi/log.h"

namespace mellohi
{
    VertexBuffer::VertexBuffer(const void* data, const i32 size) : m_vbo_id(0), m_stride(0)
    {
        glGenBuffers(1, &m_vbo_id);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }

    VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
        : m_vbo_id(other.m_vbo_id), m_attributes(std::move(other.m_attributes)), m_stride(other.m_stride)
    {
        other.m_vbo_id = 0;
    }

    VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
    {
        if (this != &other)
        {
            std::swap(m_vbo_id, other.m_vbo_id);
            std::swap(m_attributes, other.m_attributes);
            std::swap(m_stride, other.m_stride);
        }
        return *this;
    }

    VertexBuffer::~VertexBuffer()
    {
        glDeleteBuffers(1, &m_vbo_id);
    }

    void VertexBuffer::bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);

        MH_ASSERT(!m_attributes.empty(), "Attempted to bind vertex buffer (id: {}) with no attributes.", m_vbo_id);

        u32 offset = 0;
        for (u32 i = 0; i < m_attributes.size(); ++i)
        {
            const auto&[gl_type, count, size] = m_attributes[i];

            glEnableVertexAttribArray(i);
            // TODO: Support normalized attributes.
            glVertexAttribPointer(i, count, gl_type, GL_FALSE, m_stride, reinterpret_cast<const void *>(offset));
            offset += size;
        }
    }

    void VertexBuffer::add_attribute_f32(const i32 count)
    {
        const i32 size = count * static_cast<i32>(sizeof(float));

        m_attributes.push_back({ GL_FLOAT, count, size });
        m_stride += size;
    }

    IndexBuffer::IndexBuffer(const u32 *indices, const i32 count)
        : m_ibo_id(0), m_count(count), m_gl_type(GL_UNSIGNED_INT)
    {
        gen_index_buffer(indices, count * static_cast<i32>(sizeof(u32)));
    }

    IndexBuffer::IndexBuffer(const u16 *indices, const i32 count)
        : m_ibo_id(0), m_count(count), m_gl_type(GL_UNSIGNED_SHORT)
    {
        gen_index_buffer(indices, count * static_cast<i32>(sizeof(u16)));
    }

    IndexBuffer::IndexBuffer(const u8 *indices, const i32 count)
        : m_ibo_id(0), m_count(count), m_gl_type(GL_UNSIGNED_BYTE)
    {
        gen_index_buffer(indices, count * static_cast<i32>(sizeof(u8)));
    }

    IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept
        : m_ibo_id(other.m_ibo_id), m_count(other.m_count), m_gl_type(other.m_gl_type)
    {
        other.m_ibo_id = 0;
    }

    IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) noexcept
    {
        if (this != &other)
        {
            std::swap(m_ibo_id, other.m_ibo_id);
            std::swap(m_count, other.m_count);
            std::swap(m_gl_type, other.m_gl_type);
        }
        return *this;
    }

    IndexBuffer::~IndexBuffer()
    {
        glDeleteBuffers(1, &m_ibo_id);
    }

    void IndexBuffer::bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo_id);
    }

    void IndexBuffer::draw() const
    {
        glDrawElements(GL_TRIANGLES, m_count, m_gl_type, nullptr);
    }

    void IndexBuffer::gen_index_buffer(const void* indices, const i32 size)
    {
        glGenBuffers(1, &m_ibo_id);

        // GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO.
        // Binding with GL_COPY_WRITE_BUFFER allows the data to be loaded regardless of VAO state.
        glBindBuffer(GL_COPY_WRITE_BUFFER, m_ibo_id);
        glBufferData(GL_COPY_WRITE_BUFFER, size, indices, GL_STATIC_DRAW);
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    }
};
