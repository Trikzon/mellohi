#include "mellohi/graphics/wgpu/vertex_buffer.hpp"

#include "mellohi/graphics/wgpu/queue.hpp"

namespace mellohi::wgpu
{
    VertexBuffer::~VertexBuffer()
    {
        if (m_wgpu_buffer != nullptr)
        {
            wgpuBufferRelease(m_wgpu_buffer);
            m_wgpu_buffer = nullptr;
        }

        m_size_bytes = 0;
        m_wgpu_vertex_attributes = std::make_shared<vector<WGPUVertexAttribute>>();
        m_stride_bytes = std::make_shared<u64>(0);
    }

    VertexBuffer::VertexBuffer(const VertexBuffer &other)
    {
        m_wgpu_buffer = other.m_wgpu_buffer;
        if (m_wgpu_buffer != nullptr)
        {
            wgpuBufferAddRef(m_wgpu_buffer);
        }

        m_size_bytes = other.m_size_bytes;
        m_wgpu_vertex_attributes = other.m_wgpu_vertex_attributes;
        m_stride_bytes = other.m_stride_bytes;
    }

    VertexBuffer::VertexBuffer(VertexBuffer &&other) noexcept
    {
        std::swap(m_wgpu_buffer, other.m_wgpu_buffer);
        std::swap(m_size_bytes, other.m_size_bytes);
        std::swap(m_wgpu_vertex_attributes, other.m_wgpu_vertex_attributes);
        std::swap(m_stride_bytes, other.m_stride_bytes);
    }

    auto VertexBuffer::operator=(const VertexBuffer &other) -> VertexBuffer &
    {
        if (this != &other)
        {
            if (m_wgpu_buffer != nullptr)
            {
                wgpuBufferRelease(m_wgpu_buffer);
            }

            m_wgpu_buffer = other.m_wgpu_buffer;
            if (m_wgpu_buffer != nullptr)
            {
                wgpuBufferAddRef(m_wgpu_buffer);
            }

            m_size_bytes = other.m_size_bytes;
            m_wgpu_vertex_attributes = other.m_wgpu_vertex_attributes;
            m_stride_bytes = other.m_stride_bytes;
        }

        return *this;
    }

    auto VertexBuffer::operator=(VertexBuffer &&other) noexcept -> VertexBuffer &
    {
        if (this != &other)
        {
            std::swap(m_wgpu_buffer, other.m_wgpu_buffer);
            std::swap(m_size_bytes, other.m_size_bytes);
            std::swap(m_wgpu_vertex_attributes, other.m_wgpu_vertex_attributes);
            std::swap(m_stride_bytes, other.m_stride_bytes);
        }

        return *this;
    }

    auto VertexBuffer::get_size_bytes() const -> u64
    {
        return m_size_bytes;
    }

    auto VertexBuffer::get_stride_bytes() const -> u64
    {
        return *m_stride_bytes;
    }

    auto VertexBuffer::get_wgpu_layout() const -> WGPUVertexBufferLayout
    {
        return WGPUVertexBufferLayout
        {
            .arrayStride = *m_stride_bytes,
            .stepMode = WGPUVertexStepMode_Vertex,
            .attributeCount = m_wgpu_vertex_attributes->size(),
            .attributes = m_wgpu_vertex_attributes->data(),
        };
    }

    auto VertexBuffer::get_raw_ptr() const -> WGPUBuffer
    {
        return m_wgpu_buffer;
    }

    auto VertexBuffer::create(const Device &device, const string &label, const void *data) -> void
    {
        const WGPUBufferDescriptor buffer_descriptor
        {
            .nextInChain = nullptr,
            .label = label.c_str(),
            .usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst,
            .size = m_size_bytes,
            .mappedAtCreation = false,
        };

        m_wgpu_buffer = wgpuDeviceCreateBuffer(device.get_raw_ptr(), &buffer_descriptor);

        const Queue queue{device};
        queue.write_buffer(*this, data, m_size_bytes);
    }

    auto VertexBuffer::add_attribute(const u32 shader_location, const WGPUVertexFormat format,
                                     const u32 size_bytes) const -> void
    {
        m_wgpu_vertex_attributes->push_back(WGPUVertexAttribute
            {
                .format = format,
                .offset = *m_stride_bytes,
                .shaderLocation = shader_location
            });

        *m_stride_bytes += size_bytes;
    }
}
