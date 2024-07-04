#pragma once

#include "mellohi/graphics/device.hpp"

namespace mellohi
{
    class UniformBuffer
    {
    public:
        UniformBuffer(Device &device, u32 binding_idx, u32 dynamic_size, u32 size_bytes);
        ~UniformBuffer();

        UniformBuffer(const UniformBuffer &other) = delete;
        UniformBuffer(UniformBuffer &&other) noexcept;
        UniformBuffer & operator=(const UniformBuffer &other) = delete;
        UniformBuffer & operator=(UniformBuffer &&other) noexcept;

        void write(Device &device, u32 dynamic_idx, const void *data) const;

        wgpu::BindGroupEntry get_wgpu_entry() const;
        wgpu::BindGroupLayoutEntry get_wgpu_layout() const;
        u32 get_binding_idx() const;
        usize get_size_bytes() const;
        usize get_stride_bytes() const;

        wgpu::Buffer get_unsafe() const;

    private:
        wgpu::Buffer m_wgpu_buffer;
        wgpu::BindGroupEntry m_wgpu_entry;
        wgpu::BindGroupLayoutEntry m_wgpu_layout;

        u32 m_binding_idx;
        usize m_size_bytes;
        usize m_stride_bytes;
    };
}
