#pragma once

#include <webgpu/webgpu.hpp>

#include "buffer.h"
#include "device.h"

namespace mellohi
{
    class Pipeline
    {
    public:
        Pipeline(Device& device, const Surface& surface, const std::string& shader_code, VertexBuffer& vertex_buffer);
        ~Pipeline();

        wgpu::RenderPipeline get_unsafe() const;

    private:
        wgpu::RenderPipeline m_wgpu_render_pipeline;
    };
}
