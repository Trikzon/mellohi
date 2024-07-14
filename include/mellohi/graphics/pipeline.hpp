#pragma once

#include "mellohi/asset_id.hpp"
#include "mellohi/graphics/bind_group.hpp"
#include "mellohi/graphics/buffer.hpp"

namespace mellohi
{
    class Pipeline
    {
    public:
        Pipeline(Device &device, Surface &surface, const AssetId &shader_asset_id, VertexBuffer &vertex_buffer,
            const std::vector<BindGroup *> &bind_groups);
        ~Pipeline();

        Pipeline(const Pipeline &other) = delete;
        Pipeline(Pipeline &&other) noexcept;
        Pipeline &operator=(const Pipeline &other) = delete;
        Pipeline &operator=(Pipeline &&other) noexcept;

        wgpu::RenderPipeline get_unsafe() const;

    private:
        wgpu::RenderPipeline m_wgpu_render_pipeline;
    };
}
