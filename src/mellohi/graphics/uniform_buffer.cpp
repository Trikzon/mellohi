#include "mellohi/graphics/uniform_buffer.hpp"

namespace mellohi
{
    UniformBuffer::UniformBuffer(Device &device, const u32 binding_idx, const u32 dynamic_size, const u32 size_bytes)
        : m_binding_idx(binding_idx), m_size_bytes(size_bytes)
    {
        // The number of bytes copied must be a multiple of 4.
        m_size_bytes = (m_size_bytes + 3) & ~3;

        m_stride_bytes = device.get_hardware_limits().minUniformBufferOffsetAlignment;
        // The stride must be a multiple the minimum uniform buffer offset alignment.
        m_stride_bytes *= m_size_bytes / m_stride_bytes + (m_size_bytes % m_stride_bytes == 0 ? 0 : 1);

        wgpu::BufferDescriptor descriptor;
        descriptor.size = m_stride_bytes * (dynamic_size - 1) + m_size_bytes;
        descriptor.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::CopySrc | wgpu::BufferUsage::Uniform;
        descriptor.mappedAtCreation = false;
        m_wgpu_buffer = device.create_buffer_unsafe(descriptor);

        m_wgpu_entry = wgpu::Default;
        m_wgpu_entry.binding = binding_idx;
        m_wgpu_entry.buffer = m_wgpu_buffer;
        m_wgpu_entry.offset = 0;
        m_wgpu_entry.size = m_size_bytes;

        m_wgpu_layout = wgpu::Default;
        m_wgpu_layout.binding = binding_idx;
        m_wgpu_layout.visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
        m_wgpu_layout.buffer.type = wgpu::BufferBindingType::Uniform;
        m_wgpu_layout.buffer.hasDynamicOffset = true;
        m_wgpu_layout.buffer.minBindingSize = m_size_bytes;
    }

    UniformBuffer::~UniformBuffer()
    {
        if (m_wgpu_buffer != nullptr)
        {
            m_wgpu_buffer.release();
        }
    }

    UniformBuffer::UniformBuffer(UniformBuffer &&other) noexcept
        : m_binding_idx(other.m_binding_idx), m_size_bytes(other.m_size_bytes), m_stride_bytes(other.m_stride_bytes)
    {
        std::swap(m_wgpu_buffer, other.m_wgpu_buffer);
        std::swap(m_wgpu_entry, other.m_wgpu_entry);
        std::swap(m_wgpu_layout, other.m_wgpu_layout);
    }

    UniformBuffer & UniformBuffer::operator=(UniformBuffer &&other) noexcept
    {
        if (this != &other)
        {
            std::swap(m_wgpu_buffer, other.m_wgpu_buffer);
            std::swap(m_wgpu_entry, other.m_wgpu_entry);
            std::swap(m_wgpu_layout, other.m_wgpu_layout);
            std::swap(m_binding_idx, other.m_binding_idx);
            std::swap(m_size_bytes, other.m_size_bytes);
            std::swap(m_stride_bytes, other.m_stride_bytes);
        }

        return *this;
    }

    void UniformBuffer::write(Device &device, const u32 dynamic_idx, const void *data) const
    {
        wgpu::Queue queue = device.get_queue_unsafe();
        queue.writeBuffer(m_wgpu_buffer, m_stride_bytes * dynamic_idx, data, m_size_bytes);
        queue.release();
    }

    wgpu::BindGroupEntry UniformBuffer::get_wgpu_entry() const
    {
        return m_wgpu_entry;
    }

    wgpu::BindGroupLayoutEntry UniformBuffer::get_wgpu_layout() const
    {
        return m_wgpu_layout;
    }

    u32 UniformBuffer::get_binding_idx() const
    {
        return m_binding_idx;
    }

    usize UniformBuffer::get_size_bytes() const
    {
        return m_size_bytes;
    }

    usize UniformBuffer::get_stride_bytes() const
    {
        return m_stride_bytes;
    }

    wgpu::Buffer UniformBuffer::get_unsafe() const
    {
        return m_wgpu_buffer;
    }
}
