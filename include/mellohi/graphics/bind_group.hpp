#pragma once

#include "mellohi/graphics/uniform_buffer.hpp"

namespace mellohi
{
    class BindGroup
    {
    public:
        explicit BindGroup(Device &device, u32 initial_dynamic_size = 3);
        ~BindGroup();

        BindGroup(const BindGroup &other) = delete;
        BindGroup(BindGroup &&other) noexcept;
        BindGroup &operator=(const BindGroup &other) = delete;
        BindGroup &operator=(BindGroup &&other) noexcept;

        void add_binding(Device &device, u32 binding_idx, u32 size_bytes);
        void write(Device &device, u32 binding_idx, u32 dynamic_idx, const void *data);

        std::vector<u32> get_dynamic_offsets(Device &device, u32 dynamic_idx);

        wgpu::BindGroup get_unsafe() const;
        wgpu::BindGroupLayout get_layout_unsafe() const;

    private:
        wgpu::BindGroup m_wgpu_bind_group;
        wgpu::BindGroupLayout m_wgpu_bind_group_layout;

        std::vector<UniformBuffer> m_uniform_buffers;
        u32 m_dynamic_size;

        void rebuild_bind_group(Device &device);
        void grow_dynamic_size(Device &device, u32 new_dynamic_size);
    };
}
