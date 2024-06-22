#pragma once

#include <webgpu/webgpu.hpp>

#include "mellohi/graphics/buffer.h"
#include "mellohi/graphics/pipeline.h"

namespace mellohi
{
    class RenderPass
    {
    public:
        // TODO: Take color as vec3f.
        RenderPass(wgpu::TextureView wgpu_texture_view, float r, float g, float b);

        ~RenderPass();

        RenderPass(const RenderPass &other) = delete;
        RenderPass(RenderPass &&other) noexcept;
        RenderPass &operator=(const RenderPass &other) = delete;
        RenderPass &operator=(RenderPass &&other) noexcept;

        void set_pipeline(const Pipeline &pipeline);
        void set_vertex_buffer(size_t slot, const VertexBuffer &vertex_buffer);
        void set_index_buffer(const IndexBuffer &index_buffer);

        void draw_indexed();
        void end();

        wgpu::RenderPassEncoder get_unsafe() const;

    private:
        wgpu::CommandEncoder m_command_encoder;
        wgpu::RenderPassEncoder m_render_pass;
        size_t m_index_count;
    };
}
