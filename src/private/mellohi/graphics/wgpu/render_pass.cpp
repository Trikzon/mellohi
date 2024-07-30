#include "mellohi/graphics/wgpu/render_pass.hpp"

#include "mellohi/graphics/wgpu/queue.hpp"
#include "mellohi/graphics/wgpu/texture_view.hpp"

namespace mellohi::wgpu
{
    RenderPass::RenderPass(const Device &device, const Surface &surface, const vec2u framebuffer_size,
                           const vec3f clear_color)
    {
        m_device = std::make_shared<Device>(device);
        m_command_encoder = std::make_shared<CommandEncoder>(device, "Mellohi Render Pass Command Encoder");

        const Texture current_surface_texture{surface};
        const TextureView current_surface_texture_view{current_surface_texture};

        const WGPURenderPassColorAttachment render_pass_color_attachment
        {
            .nextInChain = nullptr,
            .view = current_surface_texture_view.get_raw_ptr(),
            .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
            .resolveTarget = nullptr,
            .loadOp = WGPULoadOp_Clear,
            .storeOp = WGPUStoreOp_Store,
            .clearValue = {clear_color.r, clear_color.g, clear_color.b, 1.0f},
        };

        // TODO: Move to a constant.
        constexpr WGPUTextureFormat depth_texture_format = WGPUTextureFormat_Depth24Plus;
        const WGPUTextureDescriptor depth_texture_descriptor
        {
            .nextInChain = nullptr,
            .label = "Mellohi Render Pass Depth Texture",
            .usage = WGPUTextureUsage_RenderAttachment,
            .dimension = WGPUTextureDimension_2D,
            .size = {framebuffer_size.x, framebuffer_size.y, 1},
            .format = depth_texture_format,
            .mipLevelCount = 1,
            .sampleCount = 1,
            .viewFormatCount = 1,
            .viewFormats = &depth_texture_format,
        };

        // TODO: Use wgpu::Texture instead of doing it manually.
        const WGPUTexture depth_texture = wgpuDeviceCreateTexture(device.get_raw_ptr(), &depth_texture_descriptor);

        WGPUTextureViewDescriptor depth_texture_view_descriptor
        {
            .nextInChain = nullptr,
            .label = "Mellohi Render Pass Depth Texture View",
            .format = depth_texture_format,
            .dimension = WGPUTextureViewDimension_2D,
            .baseMipLevel = 0,
            .mipLevelCount = 1,
            .baseArrayLayer = 0,
            .arrayLayerCount = 1,
            .aspect = WGPUTextureAspect_DepthOnly,
        };

        const WGPUTextureView depth_texture_view = wgpuTextureCreateView(depth_texture, &depth_texture_view_descriptor);

        WGPURenderPassDepthStencilAttachment render_pass_depth_stencil_attachment
        {
            .view = depth_texture_view,
            .depthLoadOp = WGPULoadOp_Clear,
            .depthStoreOp = WGPUStoreOp_Store,
            .depthClearValue = 1.0f,
            .depthReadOnly = false,
            .stencilLoadOp = WGPULoadOp_Undefined,
            .stencilStoreOp = WGPUStoreOp_Undefined,
            .stencilClearValue = 0,
            .stencilReadOnly = true,
        };

        WGPURenderPassDescriptor render_pass_descriptor
        {
            .nextInChain = nullptr,
            .label = "Mellohi Render Pass",
            .colorAttachmentCount = 1,
            .colorAttachments = &render_pass_color_attachment,
            .depthStencilAttachment = &render_pass_depth_stencil_attachment,
            .occlusionQuerySet = nullptr,
            .timestampWrites = nullptr,
        };

        m_wgpu_render_pass_encoder = wgpuCommandEncoderBeginRenderPass(m_command_encoder->get_raw_ptr(),
                                                                       &render_pass_descriptor);

        wgpuTextureViewRelease(depth_texture_view);
        wgpuTextureRelease(depth_texture);
    }

    RenderPass::~RenderPass()
    {
        if (m_wgpu_render_pass_encoder != nullptr)
        {
            wgpuRenderPassEncoderRelease(m_wgpu_render_pass_encoder);
            m_wgpu_render_pass_encoder = nullptr;
        }
    }

    RenderPass::RenderPass(const RenderPass &other)
    {
        m_device = other.m_device;
        m_command_encoder = other.m_command_encoder;

        m_wgpu_render_pass_encoder = other.m_wgpu_render_pass_encoder;
        if (m_wgpu_render_pass_encoder != nullptr)
        {
            wgpuRenderPassEncoderAddRef(m_wgpu_render_pass_encoder);
        }
    }

    RenderPass::RenderPass(RenderPass &&other) noexcept
    {
        std::swap(m_device, other.m_device);
        std::swap(m_command_encoder, other.m_command_encoder);
        std::swap(m_wgpu_render_pass_encoder, other.m_wgpu_render_pass_encoder);
    }

    auto RenderPass::operator=(const RenderPass &other) -> RenderPass &
    {
        if (this != &other)
        {
            m_device = other.m_device;
            m_command_encoder = other.m_command_encoder;

            if (m_wgpu_render_pass_encoder != nullptr)
            {
                wgpuRenderPassEncoderRelease(m_wgpu_render_pass_encoder);
            }

            m_wgpu_render_pass_encoder = other.m_wgpu_render_pass_encoder;
            if (m_wgpu_render_pass_encoder != nullptr)
            {
                wgpuRenderPassEncoderAddRef(m_wgpu_render_pass_encoder);
            }
        }

        return *this;
    }

    auto RenderPass::operator=(RenderPass &&other) noexcept -> RenderPass &
    {
        if (this != &other)
        {
            std::swap(m_device, other.m_device);
            std::swap(m_command_encoder, other.m_command_encoder);
            std::swap(m_wgpu_render_pass_encoder, other.m_wgpu_render_pass_encoder);
        }

        return *this;
    }

    auto RenderPass::draw(const u32 vertex_count) const -> void
    {
        wgpuRenderPassEncoderDraw(m_wgpu_render_pass_encoder, vertex_count, 1, 0, 0);
    }

    auto RenderPass::draw_indexed(const u32 index_count) const -> void
    {
        wgpuRenderPassEncoderDrawIndexed(m_wgpu_render_pass_encoder, index_count, 1, 0, 0, 0);
    }

    auto RenderPass::set_bind_group(const u32 group_idx, const BindGroup &bind_group,
                                    const u32 dynamic_idx) const -> void
    {
        const vector<u32> dynamic_offsets = bind_group.get_dynamic_offsets(dynamic_idx);
        wgpuRenderPassEncoderSetBindGroup(m_wgpu_render_pass_encoder, group_idx, bind_group.get_raw_ptr(),
                                          dynamic_offsets.size(), dynamic_offsets.data());
    }

    auto RenderPass::set_index_buffer(const IndexBuffer &index_buffer) const -> void
    {
        wgpuRenderPassEncoderSetIndexBuffer(m_wgpu_render_pass_encoder, index_buffer.get_raw_ptr(),
                                            index_buffer.get_wgpu_index_format(), 0, index_buffer.get_size_bytes());
    }

    auto RenderPass::set_pipeline(const RenderPipeline &render_pipeline) const -> void
    {
        wgpuRenderPassEncoderSetPipeline(m_wgpu_render_pass_encoder, render_pipeline.get_raw_ptr());
    }

    auto RenderPass::set_vertex_buffer(const u32 slot, const VertexBuffer &vertex_buffer) const -> void
    {
        wgpuRenderPassEncoderSetVertexBuffer(m_wgpu_render_pass_encoder, slot, vertex_buffer.get_raw_ptr(), 0,
                                             vertex_buffer.get_size_bytes());
    }

    auto RenderPass::end() const -> void
    {
        wgpuRenderPassEncoderEnd(m_wgpu_render_pass_encoder);

        const CommandBuffer command_buffer{*m_command_encoder, "Mellohi Render Pass Command Buffer"};
        const Queue queue{*m_device};
        queue.submit(command_buffer);
    }

    auto RenderPass::get_raw_ptr() const -> WGPURenderPassEncoder
    {
        return m_wgpu_render_pass_encoder;
    }
}
