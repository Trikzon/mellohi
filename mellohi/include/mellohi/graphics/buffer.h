#pragma once

#include <vector>

#include <webgpu/webgpu.hpp>

namespace mellohi
{
    class Buffer
    {
    public:
        template <typename T>
        Buffer(const T &data, wgpu::BufferUsageFlags wgpu_usage_flags);
        template <typename T>
        Buffer(const std::vector<T> &data, wgpu::BufferUsageFlags wgpu_usage_flags);
        virtual ~Buffer();

        Buffer(const Buffer &other) = delete;
        Buffer(Buffer &&other) noexcept;
        Buffer & operator=(const Buffer &other) = delete;
        Buffer & operator=(Buffer &&other) noexcept;

        template <typename T>
        void write(const T &data) const;
        template <typename T>
        void write(const std::vector<T> &data) const;

        size_t get_size_bytes() const;

        wgpu::Buffer get_unsafe() const;

    protected:
        wgpu::Buffer m_wgpu_buffer;
        size_t m_size_bytes;

    private:
        void create_buffer(const void *data, wgpu::BufferUsageFlags wgpu_usage_flags);
        void write(const void *data) const;
    };

    template<typename T>
    Buffer::Buffer(const T &data, const wgpu::BufferUsageFlags wgpu_usage_flags)
    {
        m_size_bytes = sizeof(T);
        // The number of bytes copied must be a multiple of 4.
        m_size_bytes = (m_size_bytes + 3) & ~3;

        create_buffer(&data, wgpu_usage_flags);
    }

    template<typename T>
    Buffer::Buffer(const std::vector<T> &data, const wgpu::BufferUsageFlags wgpu_usage_flags)
    {
        m_size_bytes = data.size() * sizeof(T);
        // The number of bytes copied must be a multiple of 4.
        m_size_bytes = (m_size_bytes + 3) & ~3;

        create_buffer(data.data(), wgpu_usage_flags);
    }

    template<typename T>
    void Buffer::write(const T &data) const
    {
        write(static_cast<const void *>(&data));
    }

    template<typename T>
    void Buffer::write(const std::vector<T> &data) const
    {
        write(data.data());
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


    class UniformBuffer final : public Buffer
    {
    public:
        template<typename T>
        explicit UniformBuffer(uint32_t binding, const T &data);

        wgpu::BindGroupEntry get_wgpu_bind_group_entry() const;

    private:
        wgpu::BindGroupEntry m_wgpu_bind_group_entry;
    };

    template<typename T>
    UniformBuffer::UniformBuffer(const uint32_t binding, const T &data)
        : Buffer(data, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform)
    {
        m_wgpu_bind_group_entry = wgpu::Default;
        m_wgpu_bind_group_entry.binding = binding;
        m_wgpu_bind_group_entry.buffer = m_wgpu_buffer;
        m_wgpu_bind_group_entry.offset = 0;
        m_wgpu_bind_group_entry.size = m_size_bytes;
    }
}
