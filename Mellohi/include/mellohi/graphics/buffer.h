#pragma once

#include <vector>

#include "mellohi/types.h"

namespace mellohi
{
    class VertexBuffer
    {
    public:
        VertexBuffer(const void* data, i32 size);
        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer(VertexBuffer&& other) noexcept;
        VertexBuffer& operator=(const VertexBuffer&) = delete;
        VertexBuffer& operator=(VertexBuffer&& other) noexcept;
        ~VertexBuffer();

        void bind() const;

        void add_attribute_f32(i32 count);

    private:
        struct VertexAttribute
        {
            u32 gl_type;
            i32 count;
            i32 size;
        };

        u32 m_vbo_id;
        std::vector<VertexAttribute> m_attributes;
        i32 m_stride;
    };

    class IndexBuffer
    {
    public:
        IndexBuffer(const u32* indices, i32 count);
        IndexBuffer(const u16* indices, i32 count);
        IndexBuffer(const u8* indices, i32 count);
        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer(IndexBuffer&& other) noexcept;
        IndexBuffer& operator=(const IndexBuffer&) = delete;
        IndexBuffer& operator=(IndexBuffer&& other) noexcept;
        ~IndexBuffer();

        void bind() const;

        void draw() const;

    private:
        u32 m_ibo_id;
        i32 m_count;
        u32 m_gl_type;

        void gen_index_buffer(const void* indices, i32 size);
    };
};
