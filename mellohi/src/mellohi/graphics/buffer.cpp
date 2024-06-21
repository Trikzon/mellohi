#include "mellohi/graphics/buffer.h"

namespace mellohi
{
    Buffer::~Buffer()
    {
        m_wgpu_buffer.release();
        m_wgpu_buffer.destroy();
    }

    wgpu::Buffer Buffer::get_unsafe() const
    {
        return m_wgpu_buffer;
    }

    void Buffer::create_buffer(Device& device, const void* data, const size_t size,
                               const wgpu::BufferUsageFlags wgpu_usage_flags)
    {
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

    IndexBuffer::IndexBuffer(Device& device, const std::vector<uint16_t>& data)
        : Buffer(device, data, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index),
          m_wgpu_format(wgpu::IndexFormat::Uint16) { }

    IndexBuffer::IndexBuffer(Device& device, const std::vector<uint32_t>& data)
        : Buffer(device, data, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index),
          m_wgpu_format(wgpu::IndexFormat::Uint32) { }

    wgpu::IndexFormat IndexBuffer::get_wgpu_format() const
    {
        return m_wgpu_format;
    }
}
