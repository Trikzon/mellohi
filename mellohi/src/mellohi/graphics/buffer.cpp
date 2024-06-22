#include "mellohi/graphics/buffer.h"

#include "mellohi/game.h"

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

    Buffer::Buffer(Buffer&& other) noexcept
    {
        std::swap(m_wgpu_buffer, other.m_wgpu_buffer);
    }

    Buffer& Buffer::operator=(Buffer&& other) noexcept
    {
        if (this != &other)
        {
            std::swap(m_wgpu_buffer, other.m_wgpu_buffer);
        }

        return *this;
    }

    wgpu::Buffer Buffer::get_unsafe() const
    {
        return m_wgpu_buffer;
    }

    void Buffer::create_buffer(const void* data, const size_t size,
                               const wgpu::BufferUsageFlags wgpu_usage_flags)
    {
        Device& device = Game::get().get_window().get_device();

        wgpu::BufferDescriptor descriptor;
        descriptor.size = size;
        descriptor.usage = wgpu_usage_flags;
        descriptor.mappedAtCreation = false;
        m_wgpu_buffer = device.create_buffer_unsafe(descriptor);

        {
            wgpu::Queue queue = device.get_queue_unsafe();
            queue.writeBuffer(m_wgpu_buffer, 0, data, size);
            queue.release();
        }
    }

    VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
        : Buffer(std::move(other)), m_stride(0)
    {
        std::swap(m_wgpu_vertex_attributes, other.m_wgpu_vertex_attributes);
        std::swap(m_stride, other.m_stride);
    }

    VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
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

    IndexBuffer::IndexBuffer(const std::vector<uint16_t>& data)
        : Buffer(data, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index),
          m_wgpu_format(wgpu::IndexFormat::Uint16) { }

    IndexBuffer::IndexBuffer(const std::vector<uint32_t>& data)
        : Buffer(data, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index),
          m_wgpu_format(wgpu::IndexFormat::Uint32) { }

    IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept
        : Buffer(std::move(other)), m_wgpu_format(other.m_wgpu_format) { }

    IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) noexcept
    {
        if (this != &other)
        {
            Buffer::operator=(std::move(other));
            std::swap(m_wgpu_format, other.m_wgpu_format);
        }

        return *this;
    }

    wgpu::IndexFormat IndexBuffer::get_wgpu_format() const
    {
        return m_wgpu_format;
    }
}
