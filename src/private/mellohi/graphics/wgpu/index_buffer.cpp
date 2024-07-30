#include "mellohi/graphics/wgpu/index_buffer.hpp"

#include "mellohi/graphics/wgpu/queue.hpp"

namespace mellohi::wgpu
{
    IndexBuffer::IndexBuffer(const Device &device, const string &label, const vector<u16> &data)
    {
        m_size_bytes = data.size() * sizeof(u16);
        MH_ASSERT(m_size_bytes % 4 == 0, "Index buffer size must be a multiple of 4 bytes.");

        m_wgpu_index_format = WGPUIndexFormat_Uint16;
        m_index_count = data.size();

        create(device, label, data.data());
    }

    IndexBuffer::IndexBuffer(const Device &device, const string &label, const vector<u32> &data)
    {
        m_size_bytes = data.size() * sizeof(u32);
        MH_ASSERT(m_size_bytes % 4 == 0, "Index buffer size must be a multiple of 4 bytes.");

        m_wgpu_index_format = WGPUIndexFormat_Uint32;
        m_index_count = data.size();

        create(device, label, data.data());
    }

    IndexBuffer::~IndexBuffer()
    {
        if (m_wgpu_buffer != nullptr)
        {
            wgpuBufferRelease(m_wgpu_buffer);
            m_wgpu_buffer = nullptr;
        }

        m_size_bytes = 0;
        m_wgpu_index_format = WGPUIndexFormat_Undefined;
        m_index_count = 0;
    }

    IndexBuffer::IndexBuffer(const IndexBuffer &other)
    {
        m_wgpu_buffer = other.m_wgpu_buffer;
        if (m_wgpu_buffer != nullptr)
        {
            wgpuBufferAddRef(m_wgpu_buffer);
        }

        m_size_bytes = other.m_size_bytes;
        m_wgpu_index_format = other.m_wgpu_index_format;
        m_index_count = other.m_index_count;
    }

    IndexBuffer::IndexBuffer(IndexBuffer &&other) noexcept
    {
        std::swap(m_wgpu_buffer, other.m_wgpu_buffer);
        std::swap(m_size_bytes, other.m_size_bytes);
        std::swap(m_wgpu_index_format, other.m_wgpu_index_format);
        std::swap(m_index_count, other.m_index_count);
    }

    auto IndexBuffer::operator=(const IndexBuffer &other) -> IndexBuffer &
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
            m_wgpu_index_format = other.m_wgpu_index_format;
            m_index_count = other.m_index_count;
        }

        return *this;
    }

    auto IndexBuffer::operator=(IndexBuffer &&other) noexcept -> IndexBuffer &
    {
        if (this != &other)
        {
            std::swap(m_wgpu_buffer, other.m_wgpu_buffer);
            std::swap(m_size_bytes, other.m_size_bytes);
            std::swap(m_wgpu_index_format, other.m_wgpu_index_format);
            std::swap(m_index_count, other.m_index_count);
        }

        return *this;
    }

    auto IndexBuffer::get_size_bytes() const -> u64
    {
        return m_size_bytes;
    }

    auto IndexBuffer::get_index_count() const -> u32
    {
        return m_index_count;
    }

    auto IndexBuffer::get_wgpu_index_format() const -> WGPUIndexFormat
    {
        return m_wgpu_index_format;
    }

    auto IndexBuffer::get_raw_ptr() const -> WGPUBuffer
    {
        return m_wgpu_buffer;
    }

    auto IndexBuffer::create(const Device &device, const string &label, const void *data) -> void
    {
        const WGPUBufferDescriptor buffer_descriptor
        {
            .nextInChain = nullptr,
            .label = label.c_str(),
            .usage = WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst,
            .size = m_size_bytes,
            .mappedAtCreation = false,
        };

        m_wgpu_buffer = wgpuDeviceCreateBuffer(device.get_raw_ptr(), &buffer_descriptor);

        const Queue queue{device};
        queue.write_buffer(*this, data, m_size_bytes);
    }
}
