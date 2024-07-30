#pragma once

#include "mellohi/graphics/wgpu/command_encoder.hpp"
#include "mellohi/graphics/wgpu/index_buffer.hpp"
#include "mellohi/graphics/wgpu/render_pipeline.hpp"

namespace mellohi::wgpu
{
    class RenderPass
    {
    public:
        RenderPass(const Device &device, const Surface &surface, vec2u framebuffer_size, vec3f clear_color);
        ~RenderPass();
        RenderPass(const RenderPass &other);
        RenderPass(RenderPass &&other) noexcept;
        auto operator=(const RenderPass &other) -> RenderPass &;
        auto operator=(RenderPass &&other) noexcept -> RenderPass &;

        auto draw(u32 vertex_count) const -> void;
        auto draw_indexed(u32 index_count) const -> void;
        auto set_bind_group(u32 group_idx, const BindGroup &bind_group, u32 dynamic_idx) const -> void;
        auto set_index_buffer(const IndexBuffer &index_buffer) const -> void;
        auto set_pipeline(const RenderPipeline &render_pipeline) const -> void;
        auto set_vertex_buffer(u32 slot, const VertexBuffer &vertex_buffer) const -> void;

        auto end() const -> void;

        auto get_raw_ptr() const -> WGPURenderPassEncoder;

    private:
        s_ptr<Device> m_device{nullptr};
        s_ptr<CommandEncoder> m_command_encoder{nullptr};
        WGPURenderPassEncoder m_wgpu_render_pass_encoder{nullptr};
    };
}
