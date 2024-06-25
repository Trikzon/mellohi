#pragma once

#include <webgpu/webgpu.hpp>

#include "mellohi/asset_id.h"
#include "mellohi/graphics/buffer.h"

namespace mellohi
{
    class Pipeline
    {
    public:
        Pipeline(const AssetId &shader_asset_id, VertexBuffer &vertex_buffer);
        ~Pipeline();

        Pipeline(const Pipeline &other) = delete;
        Pipeline(Pipeline &&other) noexcept;
        Pipeline &operator=(const Pipeline &other) = delete;
        Pipeline &operator=(Pipeline &&other) noexcept;

        wgpu::BindGroupLayout get_wgpu_bind_group_layout_unsafe(uint32_t group_idx);

        wgpu::RenderPipeline get_unsafe() const;

    private:
        wgpu::RenderPipeline m_wgpu_render_pipeline;
    };
}
