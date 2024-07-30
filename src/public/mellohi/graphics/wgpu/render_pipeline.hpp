#pragma once

#include "mellohi/graphics/wgpu/bind_group.hpp"
#include "mellohi/graphics/wgpu/shader_module.hpp"
#include "mellohi/graphics/wgpu/vertex_buffer.hpp"

namespace mellohi::wgpu
{
    class RenderPipeline
    {
    public:
        RenderPipeline(const Device &device, const string &label, const ShaderModule &shader_module,
                       const vector<VertexBuffer> &vertex_buffers, const vector<BindGroup> &bind_groups);
        ~RenderPipeline();
        RenderPipeline(const RenderPipeline &other);
        RenderPipeline(RenderPipeline &&other) noexcept;
        auto operator=(const RenderPipeline &other) -> RenderPipeline &;
        auto operator=(RenderPipeline &&other) noexcept -> RenderPipeline &;

        auto get_raw_ptr() const -> WGPURenderPipeline;

    private:
        WGPURenderPipeline m_wgpu_render_pipeline{nullptr};
    };
}
