#pragma once

namespace mellohi
{
    template<typename T>
    Buffer::Buffer(Device &device, const T &data, const wgpu::BufferUsageFlags wgpu_usage_flags)
    {
        m_size_bytes = sizeof(T);
        // The number of bytes copied must be a multiple of 4.
        m_size_bytes = (m_size_bytes + 3) & ~3;

        create_buffer(device, &data, wgpu_usage_flags);
    }

    template<typename T>
    Buffer::Buffer(Device &device, const std::vector<T> &data, const wgpu::BufferUsageFlags wgpu_usage_flags)
    {
        m_size_bytes = data.size() * sizeof(T);
        // The number of bytes copied must be a multiple of 4.
        m_size_bytes = (m_size_bytes + 3) & ~3;

        create_buffer(device, data.data(), wgpu_usage_flags);
    }

    template<typename T>
    void Buffer::write(Device &device, const T &data) const
    {
        write(device, static_cast<const void *>(&data));
    }

    template<typename T>
    void Buffer::write(Device &device, const std::vector<T> &data) const
    {
        write(device, data.data());
    }

    template<typename T>
    VertexBuffer::VertexBuffer(Device &device, const std::vector<T> &data)
        : Buffer(device, data, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex),
          m_stride(0) { }

    template<typename T>
    UniformBuffer::UniformBuffer(Device &device, const uint32_t binding, const T &data)
        : Buffer(device, data, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform)
    {
        m_wgpu_entry = wgpu::Default;
        m_wgpu_entry.binding = binding;
        m_wgpu_entry.buffer = m_wgpu_buffer;
        m_wgpu_entry.offset = 0;
        m_wgpu_entry.size = m_size_bytes;

        m_wgpu_layout = wgpu::Default;
        m_wgpu_layout.binding = binding;
        m_wgpu_layout.visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
        m_wgpu_layout.buffer.type = wgpu::BufferBindingType::Uniform;
        m_wgpu_layout.buffer.hasDynamicOffset = false;
        m_wgpu_layout.buffer.minBindingSize = m_size_bytes;
    }
}
