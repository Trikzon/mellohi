#include "mellohi/graphics/wgpu/uniform_buffer.hpp"

#include <webgpu/webgpu.hpp>

namespace mellohi::wgpu
{
    UniformBuffer::UniformBuffer(const Device &device, const string &label, const u32 binding_idx,
                                 const u32 dynamic_size, const u64 size_bytes)
    {
        MH_ASSERT(size_bytes % 4 == 0, "Uniform buffer size must be a multiple of 4 bytes.");
        m_size_bytes = size_bytes;

        m_stride_bytes = device.get_limits().minUniformBufferOffsetAlignment;
        m_stride_bytes *= m_size_bytes / m_stride_bytes + (m_size_bytes % m_stride_bytes == 0 ? 0 : 1);

        const WGPUBufferDescriptor buffer_descriptor
        {
            .nextInChain = nullptr,
            .label = label.c_str(),
            .usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst | WGPUBufferUsage_CopySrc,
            .size = m_stride_bytes * (dynamic_size - 1) + m_size_bytes,
            .mappedAtCreation = false,
        };

        m_wgpu_buffer = wgpuDeviceCreateBuffer(device.get_raw_ptr(), &buffer_descriptor);

        m_wgpu_entry = WGPUBindGroupEntry
        {
            .nextInChain = nullptr,
            .binding = binding_idx,
            .buffer = m_wgpu_buffer,
            .offset = 0,
            .size = m_stride_bytes,
            .sampler = nullptr,
            .textureView = nullptr,
        };

        m_wgpu_layout = WGPUBindGroupLayoutEntry
        {
            .nextInChain = nullptr,
            .binding = binding_idx,
            .visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment,
            .buffer = WGPUBufferBindingLayout
            {
                .nextInChain = nullptr,
                .type = WGPUBufferBindingType_Uniform,
                .hasDynamicOffset = true,
                .minBindingSize = m_stride_bytes,
            },
            .sampler = nullptr,
            .texture = nullptr,
            .storageTexture = nullptr,
        };
    }

    UniformBuffer::~UniformBuffer()
    {
        if (m_wgpu_buffer != nullptr)
        {
            wgpuBufferDestroy(m_wgpu_buffer);
            m_wgpu_buffer = nullptr;
        }

        m_wgpu_entry = {};
        m_wgpu_layout = {};
        m_binding_idx = 0;
        m_size_bytes = 0;
        m_stride_bytes = 0;
    }

    UniformBuffer::UniformBuffer(const UniformBuffer &other)
    {
        m_wgpu_buffer = other.m_wgpu_buffer;
        if (m_wgpu_buffer != nullptr)
        {
            wgpuBufferAddRef(m_wgpu_buffer);
        }

        m_wgpu_entry = other.m_wgpu_entry;
        m_wgpu_layout = other.m_wgpu_layout;
        m_binding_idx = other.m_binding_idx;
        m_size_bytes = other.m_size_bytes;
        m_stride_bytes = other.m_stride_bytes;
    }

    UniformBuffer::UniformBuffer(UniformBuffer &&other) noexcept
    {
        std::swap(m_wgpu_buffer, other.m_wgpu_buffer);
        m_wgpu_entry = other.m_wgpu_entry;
        m_wgpu_layout = other.m_wgpu_layout;
        m_binding_idx = other.m_binding_idx;
        m_size_bytes = other.m_size_bytes;
        m_stride_bytes = other.m_stride_bytes;
    }

    auto UniformBuffer::operator=(const UniformBuffer &other) -> UniformBuffer &
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

            m_wgpu_entry = other.m_wgpu_entry;
            m_wgpu_layout = other.m_wgpu_layout;
            m_binding_idx = other.m_binding_idx;
            m_size_bytes = other.m_size_bytes;
            m_stride_bytes = other.m_stride_bytes;
        }

        return *this;
    }

    auto UniformBuffer::operator=(UniformBuffer &&other) noexcept -> UniformBuffer &
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

    auto UniformBuffer::get_binding_idx() const -> u32
    {
        return m_binding_idx;
    }

    auto UniformBuffer::get_size_bytes() const -> u64
    {
        return m_size_bytes;
    }

    auto UniformBuffer::get_stride_bytes() const -> u64
    {
        return m_stride_bytes;
    }

    auto UniformBuffer::get_wgpu_entry() const -> const WGPUBindGroupEntry &
    {
        return m_wgpu_entry;
    }

    auto UniformBuffer::get_wgpu_layout() const -> const WGPUBindGroupLayoutEntry &
    {
        return m_wgpu_layout;
    }

    auto UniformBuffer::get_raw_ptr() const -> WGPUBuffer
    {
        return m_wgpu_buffer;
    }
}
