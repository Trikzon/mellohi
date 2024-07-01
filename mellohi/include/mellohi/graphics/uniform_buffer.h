#pragma once

#include <webgpu/webgpu.hpp>

#include "mellohi/graphics/device.h"

namespace mellohi
{
    class UniformBuffer
    {
    public:
        UniformBuffer(Device &device, uint32_t binding_idx, uint32_t dynamic_size, uint32_t size_bytes);
        ~UniformBuffer();

        UniformBuffer(const UniformBuffer &other) = delete;
        UniformBuffer(UniformBuffer &&other) noexcept;
        UniformBuffer & operator=(const UniformBuffer &other) = delete;
        UniformBuffer & operator=(UniformBuffer &&other) noexcept;

        void write(Device &device, uint32_t dynamic_idx, const void *data) const;

        wgpu::BindGroupEntry get_wgpu_entry() const;
        wgpu::BindGroupLayoutEntry get_wgpu_layout() const;
        uint32_t get_binding_idx() const;
        size_t get_size_bytes() const;
        size_t get_stride_bytes() const;

        wgpu::Buffer get_unsafe() const;

    private:
        wgpu::Buffer m_wgpu_buffer;
        wgpu::BindGroupEntry m_wgpu_entry;
        wgpu::BindGroupLayoutEntry m_wgpu_layout;

        uint32_t m_binding_idx;
        size_t m_size_bytes;
        size_t m_stride_bytes;
    };
}
