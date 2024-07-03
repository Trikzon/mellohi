#pragma once

#include <webgpu/webgpu.hpp>

#include "mellohi/graphics/uniform_buffer.h"

namespace mellohi
{
    class BindGroup
    {
    public:
        explicit BindGroup(Device &device, uint32_t initial_dynamic_size = 3);
        ~BindGroup();

        BindGroup(const BindGroup &other) = delete;
        BindGroup(BindGroup &&other) noexcept;
        BindGroup &operator=(const BindGroup &other) = delete;
        BindGroup &operator=(BindGroup &&other) noexcept;

        void add(Device &device, uint32_t binding_idx, uint32_t size_bytes);
        void write(Device &device, uint32_t binding_idx, uint32_t dynamic_idx, const void *data);

        std::vector<uint32_t> get_dynamic_offsets(Device &device, uint32_t dynamic_idx);

        wgpu::BindGroup get_unsafe() const;
        wgpu::BindGroupLayout get_layout_unsafe() const;

    private:
        wgpu::BindGroup m_wgpu_bind_group;
        wgpu::BindGroupLayout m_wgpu_bind_group_layout;

        std::vector<UniformBuffer> m_uniform_buffers;
        uint32_t m_dynamic_size;

        void rebuild_bind_group(Device &device);
        void grow_dynamic_size(Device &device, uint32_t new_dynamic_size);
    };
}
