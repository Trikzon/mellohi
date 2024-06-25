#pragma once

#include <webgpu/webgpu.hpp>

#include "mellohi/graphics/buffer.h"
#include "mellohi/graphics/pipeline.h"

namespace mellohi
{
    class BindGroup
    {
    public:
        BindGroup(Pipeline &pipeline, const UniformBuffer &uniform_buffer);
        ~BindGroup();

        wgpu::BindGroup get_unsafe() const;

    private:
        wgpu::BindGroup m_wgpu_bind_group;
    };
}
