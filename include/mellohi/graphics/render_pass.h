#pragma once

#include "mellohi/graphics/pipeline.h"

namespace mellohi
{
    class RenderPass
    {
    public:
        RenderPass(Device& device, wgpu::TextureView wgpu_texture_view, vec3f clear_color);

        ~RenderPass();

        RenderPass(const RenderPass &other);
        RenderPass(RenderPass &&other) noexcept;
        RenderPass &operator=(const RenderPass &other);
        RenderPass &operator=(RenderPass &&other) noexcept;

        void set_pipeline(const Pipeline &pipeline);
        void set_vertex_buffer(u32 slot, const VertexBuffer &vertex_buffer);
        void set_index_buffer(const IndexBuffer &index_buffer);
        void set_bind_group(Device &device, BindGroup &bind_group, u32 dynamic_idx);

        void draw_indexed(u32 index_count);

        void end(Device &device);

        wgpu::RenderPassEncoder get_unsafe() const;

    private:
        wgpu::CommandEncoder m_command_encoder;
        wgpu::RenderPassEncoder m_render_pass;
    };
}
