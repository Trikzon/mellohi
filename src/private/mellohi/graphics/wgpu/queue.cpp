#include "mellohi/graphics/wgpu/queue.hpp"

namespace mellohi::wgpu
{
    Queue::Queue(const Device &device)
    {
        m_wgpu_queue = wgpuDeviceGetQueue(device.get_raw_ptr());
    }

    Queue::~Queue()
    {
        if (m_wgpu_queue != nullptr)
        {
            wgpuQueueRelease(m_wgpu_queue);
            m_wgpu_queue = nullptr;
        }
    }

    Queue::Queue(const Queue &other)
    {
        m_wgpu_queue = other.m_wgpu_queue;
        if (m_wgpu_queue != nullptr)
        {
            wgpuQueueAddRef(m_wgpu_queue);
        }
    }

    Queue::Queue(Queue &&other) noexcept
    {
        std::swap(m_wgpu_queue, other.m_wgpu_queue);
    }

    auto Queue::operator=(const Queue &other) -> Queue &
    {
        if (this != &other)
        {
            if (m_wgpu_queue != nullptr)
            {
                wgpuQueueRelease(m_wgpu_queue);
            }

            m_wgpu_queue = other.m_wgpu_queue;
            if (m_wgpu_queue != nullptr)
            {
                wgpuQueueAddRef(m_wgpu_queue);
            }
        }

        return *this;
    }

    auto Queue::operator=(Queue &&other) noexcept -> Queue &
    {
        if (this != &other)
        {
            std::swap(other.m_wgpu_queue, m_wgpu_queue);
        }

        return *this;
    }

    auto Queue::submit(const CommandBuffer &command_buffer) const -> void
    {
        const auto command_buffer_ptr = command_buffer.get_raw_ptr();
        wgpuQueueSubmit(m_wgpu_queue, 1, &command_buffer_ptr);
    }

    auto Queue::write_buffer(const IndexBuffer &index_buffer, const void *data, const usize size_bytes) const -> void
    {
        MH_ASSERT(index_buffer.get_size_bytes() == size_bytes, "Data size must match index buffer size.");
        wgpuQueueWriteBuffer(m_wgpu_queue, index_buffer.get_raw_ptr(), 0, data, size_bytes);
    }

    auto Queue::write_buffer(const UniformBuffer &uniform_buffer, const u32 dynamic_idx, const void *data,
                             const usize size_bytes) const -> void
    {
        MH_ASSERT(uniform_buffer.get_size_bytes() == size_bytes, "Data size must match uniform buffer size.");
        wgpuQueueWriteBuffer(m_wgpu_queue, uniform_buffer.get_raw_ptr(),
                             dynamic_idx * uniform_buffer.get_stride_bytes(), data, size_bytes);
    }

    auto Queue::write_buffer(const VertexBuffer &vertex_buffer, const void *data, const usize size_bytes) const -> void
    {
        MH_ASSERT(vertex_buffer.get_size_bytes() == size_bytes, "Data size must match vertex buffer size.");
        wgpuQueueWriteBuffer(m_wgpu_queue, vertex_buffer.get_raw_ptr(), 0, data, size_bytes);
    }

    auto Queue::get_raw_ptr() const -> WGPUQueue
    {
        return m_wgpu_queue;
    }
}
