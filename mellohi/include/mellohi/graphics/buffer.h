#pragma once

#include <vector>

#include <webgpu/webgpu.hpp>

namespace mellohi
{
    class Buffer
    {
    public:
        template <typename T>
        Buffer(const std::vector<T> &data, wgpu::BufferUsageFlags wgpu_usage_flags);
        virtual ~Buffer();

        Buffer(const Buffer &other) = delete;
        Buffer(Buffer &&other) noexcept;
        Buffer & operator=(const Buffer &other) = delete;
        Buffer & operator=(Buffer &&other) noexcept;

        size_t get_size_bytes() const;

        wgpu::Buffer get_unsafe() const;

    private:
        wgpu::Buffer m_wgpu_buffer;
        size_t m_size_bytes;

        void create_buffer(const void *data, size_t size, wgpu::BufferUsageFlags wgpu_usage_flags);
    };

    template<typename T>
    Buffer::Buffer(const std::vector<T> &data, const wgpu::BufferUsageFlags wgpu_usage_flags)
    {
        m_size_bytes = data.size() * sizeof(T);

        // The number of bytes copied must be a multiple of 4.
        // So we round up to the next multiple.
        const size_t size = (m_size_bytes + 3) & ~3;
        create_buffer(data.data(), size, wgpu_usage_flags);
    }


    class VertexBuffer final : public Buffer
    {
    public:
        template <typename T>
        explicit VertexBuffer(const std::vector<T> &data);

        VertexBuffer(const VertexBuffer &other) = delete;
        VertexBuffer(VertexBuffer &&other) noexcept;
        VertexBuffer & operator=(const VertexBuffer &other) = delete;
        VertexBuffer & operator=(VertexBuffer &&other) noexcept;

        void add_attribute_vec2f();
        void add_attribute_vec3f();

        wgpu::VertexBufferLayout get_wgpu_layout() const;

    private:
        std::vector<wgpu::VertexAttribute> m_wgpu_vertex_attributes;
        size_t m_stride;

        void add_attribute(wgpu::VertexFormat wgpu_format, size_t size);
    };

    template<typename T>
    VertexBuffer::VertexBuffer(const std::vector<T> &data)
        : Buffer(data, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex),
          m_stride(0) { }


    class IndexBuffer final : public Buffer
    {
    public:
        explicit IndexBuffer(const std::vector<uint16_t> &data);
        explicit IndexBuffer(const std::vector<uint32_t> &data);

        IndexBuffer(const IndexBuffer &other) = delete;
        IndexBuffer(IndexBuffer &&other) noexcept;
        IndexBuffer & operator=(const IndexBuffer &other) = delete;
        IndexBuffer & operator=(IndexBuffer &&other) noexcept;

        wgpu::IndexFormat get_wgpu_format() const;
        size_t get_index_count() const;

    private:
        wgpu::IndexFormat m_wgpu_format;
        size_t m_index_count;
    };
}
