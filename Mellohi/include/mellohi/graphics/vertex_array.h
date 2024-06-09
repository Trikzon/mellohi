#pragma once

#include <optional>
#include <vector>

#include "mellohi/types.h"

#include "buffer.h"

namespace mellohi
{
    class VertexArray
    {
    public:
        VertexArray();
        ~VertexArray();

        void bind() const;

        void draw() const;

        void add_vertex_buffer(VertexBuffer&& vertex_buffer);
        void set_index_buffer(IndexBuffer&& index_buffer);

    private:
        u32 m_vao_id;
        std::vector<VertexBuffer> m_vertex_buffers;
        std::optional<IndexBuffer> m_index_buffer;
    };
}
