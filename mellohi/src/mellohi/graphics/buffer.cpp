#include "mellohi/graphics/buffer.h"

namespace mellohi
{
    Buffer::~Buffer()
    {
        if (m_wgpu_buffer != nullptr)
        {
            m_wgpu_buffer.destroy();
            m_wgpu_buffer.release();
        }
    }

    Buffer::Buffer(Buffer &&other) noexcept : m_size_bytes(0)
    {
        std::swap(m_wgpu_buffer, other.m_wgpu_buffer);
        std::swap(m_size_bytes, other.m_size_bytes);
    }

    Buffer & Buffer::operator=(Buffer &&other) noexcept
    {
        if (this != &other)
        {
            std::swap(m_wgpu_buffer, other.m_wgpu_buffer);
            std::swap(m_size_bytes, other.m_size_bytes);
        }

        return *this;
    }

    size_t Buffer::get_size_bytes() const
    {
        return m_size_bytes;
    }

    wgpu::Buffer Buffer::get_unsafe() const
    {
        return m_wgpu_buffer;
    }

    void Buffer::create_buffer(Device &device, const void *data, const wgpu::BufferUsageFlags wgpu_usage_flags)
    {
        wgpu::BufferDescriptor descriptor;
        descriptor.size = m_size_bytes;
        descriptor.usage = wgpu_usage_flags;
        descriptor.mappedAtCreation = false;
        m_wgpu_buffer = device.create_buffer_unsafe(descriptor);

        write(device, data);
    }

    void Buffer::write(Device &device, const void *data) const
    {
        wgpu::Queue queue = device.get_queue_unsafe();
        queue.writeBuffer(m_wgpu_buffer, 0, data, m_size_bytes);
        queue.release();
    }

    VertexBuffer::VertexBuffer(VertexBuffer &&other) noexcept
        : Buffer(std::move(other)), m_stride(0)
    {
        std::swap(m_wgpu_vertex_attributes, other.m_wgpu_vertex_attributes);
        std::swap(m_stride, other.m_stride);
    }

    VertexBuffer & VertexBuffer::operator=(VertexBuffer &&other) noexcept
    {
        if (this != &other)
        {
            Buffer::operator=(std::move(other));
            std::swap(m_wgpu_vertex_attributes, other.m_wgpu_vertex_attributes);
            std::swap(m_stride, other.m_stride);
        }

        return *this;
    }

    void VertexBuffer::add_attribute_vec2f()
    {
        add_attribute(wgpu::VertexFormat::Float32x2, 2 * sizeof(float));
    }

    void VertexBuffer::add_attribute_vec3f()
    {
        add_attribute(wgpu::VertexFormat::Float32x3, 3 * sizeof(float));
    }

    wgpu::VertexBufferLayout VertexBuffer::get_wgpu_layout() const
    {
        wgpu::VertexBufferLayout layout;
        layout.attributeCount = m_wgpu_vertex_attributes.size();
        layout.attributes = m_wgpu_vertex_attributes.data();
        layout.arrayStride = m_stride;
        layout.stepMode = wgpu::VertexStepMode::Vertex;

        return layout;
    }

    void VertexBuffer::add_attribute(const wgpu::VertexFormat wgpu_format, const size_t size)
    {
        wgpu::VertexAttribute attribute;
        attribute.shaderLocation = m_wgpu_vertex_attributes.size();
        attribute.format = wgpu_format;
        attribute.offset = m_stride;

        m_stride += size;
        m_wgpu_vertex_attributes.push_back(attribute);
    }

    IndexBuffer::IndexBuffer(Device &device, const std::vector<uint16_t> &data)
        : Buffer(device, data, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index),
          m_wgpu_format(wgpu::IndexFormat::Uint16), m_index_count(data.size()) { }

    IndexBuffer::IndexBuffer(Device &device, const std::vector<uint32_t> &data)
        : Buffer(device, data, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index),
          m_wgpu_format(wgpu::IndexFormat::Uint32), m_index_count(data.size()) { }

    IndexBuffer::IndexBuffer(IndexBuffer &&other) noexcept
        : Buffer(std::move(other)),
          m_wgpu_format(other.m_wgpu_format), m_index_count(other.m_index_count) { }

    IndexBuffer & IndexBuffer::operator=(IndexBuffer &&other) noexcept
    {
        if (this != &other)
        {
            Buffer::operator=(std::move(other));
            std::swap(m_wgpu_format, other.m_wgpu_format);
            std::swap(m_index_count, other.m_index_count);
        }

        return *this;
    }

    wgpu::IndexFormat IndexBuffer::get_wgpu_format() const
    {
        return m_wgpu_format;
    }

    size_t IndexBuffer::get_index_count() const
    {
        return m_index_count;
    }

    wgpu::BindGroupEntry UniformBuffer::get_wgpu_entry() const
    {
        return m_wgpu_entry;
    }

    wgpu::BindGroupLayoutEntry UniformBuffer::get_wgpu_layout() const
    {
        return m_wgpu_layout;
    }
}
