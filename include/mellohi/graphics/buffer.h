#pragma once

#include <vector>

#include <webgpu/webgpu.hpp>

#include "mellohi/graphics/device.h"

namespace mellohi
{
    class Buffer
    {
    public:
        template <typename T>
        Buffer(Device &device, const T &data, wgpu::BufferUsageFlags wgpu_usage_flags);
        template <typename T>
        Buffer(Device &device, const std::vector<T> &data, wgpu::BufferUsageFlags wgpu_usage_flags);
        virtual ~Buffer();

        Buffer(const Buffer &other) = delete;
        Buffer(Buffer &&other) noexcept;
        Buffer & operator=(const Buffer &other) = delete;
        Buffer & operator=(Buffer &&other) noexcept;

        template <typename T>
        void write(Device &device, const T &data) const;
        template <typename T>
        void write(Device &device, const std::vector<T> &data) const;

        size_t get_size_bytes() const;

        wgpu::Buffer get_unsafe() const;

    protected:
        wgpu::Buffer m_wgpu_buffer;
        size_t m_size_bytes;

    private:
        void create_buffer(Device &device, const void *data, wgpu::BufferUsageFlags wgpu_usage_flags);
        void write(Device &device, const void *data) const;
    };

    class VertexBuffer final : public Buffer
    {
    public:
        template <typename T>
        explicit VertexBuffer(Device &device, const std::vector<T> &data);

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

    class IndexBuffer final : public Buffer
    {
    public:
        explicit IndexBuffer(Device &device, const std::vector<uint16_t> &data);
        explicit IndexBuffer(Device &device, const std::vector<uint32_t> &data);

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

#include "buffer.hpp"
