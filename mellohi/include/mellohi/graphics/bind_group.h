#pragma once

#include <webgpu/webgpu.hpp>

#include "mellohi/graphics/buffer.h"

namespace mellohi
{
    class BindGroup
    {
    public:
        BindGroup(Device &device, const UniformBuffer &uniform_buffer);
        ~BindGroup();

        BindGroup(const BindGroup &other);
        BindGroup(BindGroup &&other) noexcept;
        BindGroup &operator=(const BindGroup &other);
        BindGroup &operator=(BindGroup &&other) noexcept;

        wgpu::BindGroup get_unsafe() const;
        wgpu::BindGroupLayout get_layout_unsafe() const;

    private:
        wgpu::BindGroup m_wgpu_bind_group;
        wgpu::BindGroupLayout m_wgpu_bind_group_layout;
    };
}
