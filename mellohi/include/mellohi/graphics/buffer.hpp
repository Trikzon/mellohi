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
}
