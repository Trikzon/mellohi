#include "mellohi/graphics/render_pass.hpp"

namespace mellohi
{
    RenderPass::RenderPass(Device &device, wgpu::TextureView wgpu_texture_view, vec3f clear_color)
    {
        wgpu::CommandEncoderDescriptor command_encoder_descriptor = {};
        command_encoder_descriptor.label = "Mellohi Command Encoder";
        m_command_encoder = device.create_command_encoder_unsafe(command_encoder_descriptor);

        wgpu::RenderPassColorAttachment color_attachment = {};
        color_attachment.view = wgpu_texture_view;
        color_attachment.resolveTarget = nullptr;
        color_attachment.loadOp = wgpu::LoadOp::Clear;
        color_attachment.storeOp = wgpu::StoreOp::Store;
        color_attachment.clearValue = wgpu::Color(clear_color.r, clear_color.g, clear_color.b, 1.0);
        color_attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

        // TODO: Don't make the depth texture here, but instead pass it in.
        wgpu::TextureFormat depth_texture_format = wgpu::TextureFormat::Depth24Plus;
        wgpu::TextureDescriptor depth_texture_descriptor = {};
        depth_texture_descriptor.dimension = wgpu::TextureDimension::_2D;
        depth_texture_descriptor.format = depth_texture_format;
        depth_texture_descriptor.mipLevelCount = 1;
        depth_texture_descriptor.sampleCount = 1;
        depth_texture_descriptor.size = {640, 480, 1};
        depth_texture_descriptor.usage = wgpu::TextureUsage::RenderAttachment;
        depth_texture_descriptor.viewFormatCount = 1;
        depth_texture_descriptor.viewFormats = reinterpret_cast<WGPUTextureFormat *>(&depth_texture_format);
        wgpu::Texture depth_texture = device.create_texture_unsafe(depth_texture_descriptor);

        wgpu::TextureViewDescriptor depth_texture_view_descriptor = {};
        depth_texture_view_descriptor.aspect = wgpu::TextureAspect::DepthOnly;
        depth_texture_view_descriptor.baseArrayLayer = 0;
        depth_texture_view_descriptor.arrayLayerCount = 1;
        depth_texture_view_descriptor.baseMipLevel = 0;
        depth_texture_view_descriptor.mipLevelCount = 1;
        depth_texture_view_descriptor.dimension = wgpu::TextureViewDimension::_2D;
        depth_texture_view_descriptor.format = depth_texture_format;
        wgpu::TextureView depth_texture_view = depth_texture.createView(depth_texture_view_descriptor);

        wgpu::RenderPassDepthStencilAttachment depth_stencil_attachment = {};
        depth_stencil_attachment.view = depth_texture_view;
        depth_stencil_attachment.depthClearValue = 1.0f;
        depth_stencil_attachment.depthLoadOp = wgpu::LoadOp::Clear;
        depth_stencil_attachment.depthStoreOp = wgpu::StoreOp::Store;
        depth_stencil_attachment.depthReadOnly = false;

        // Stencil is mandatory, but not used here.
        depth_stencil_attachment.stencilClearValue = 0;
        // On Dawn, the stencil ops must be set to Undefined if the stencil is not used.
        depth_stencil_attachment.stencilLoadOp = wgpu::LoadOp::Undefined;
        depth_stencil_attachment.stencilStoreOp = wgpu::StoreOp::Undefined;
        depth_stencil_attachment.stencilReadOnly = true;

        wgpu::RenderPassDescriptor render_pass_descriptor = {};
        render_pass_descriptor.colorAttachmentCount = 1;
        render_pass_descriptor.colorAttachments = &color_attachment;
        render_pass_descriptor.depthStencilAttachment = &depth_stencil_attachment;
        render_pass_descriptor.timestampWrites = nullptr;
        m_render_pass = m_command_encoder.beginRenderPass(render_pass_descriptor);

        depth_texture_view.release();
        depth_texture.release();
    }

    RenderPass::~RenderPass()
    {
        if (m_render_pass != nullptr)
        {
            m_render_pass.release();
        }

        if (m_command_encoder != nullptr)
        {
            m_command_encoder.release();
        }
    }

    RenderPass::RenderPass(const RenderPass &other)
    {
        m_command_encoder = other.m_command_encoder;
        m_command_encoder.addRef();
        m_render_pass = other.m_render_pass;
        m_render_pass.addRef();
    }

    RenderPass::RenderPass(RenderPass &&other) noexcept
    {
        std::swap(m_command_encoder, other.m_command_encoder);
        std::swap(m_render_pass, other.m_render_pass);
    }

    RenderPass & RenderPass::operator=(const RenderPass &other)
    {
        if (this != &other)
        {
            this->~RenderPass();

            m_command_encoder = other.m_command_encoder;
            m_command_encoder.addRef();
            m_render_pass = other.m_render_pass;
            m_render_pass.addRef();
        }

        return *this;
    }

    RenderPass & RenderPass::operator=(RenderPass &&other) noexcept
    {
        if (this != &other)
        {
            std::swap(m_command_encoder, other.m_command_encoder);
            std::swap(m_render_pass, other.m_render_pass);
        }

        return *this;
    }

    void RenderPass::set_pipeline(const Pipeline &pipeline)
    {
        m_render_pass.setPipeline(pipeline.get_unsafe());
    }

    void RenderPass::set_vertex_buffer(const u32 slot, const VertexBuffer &vertex_buffer)
    {
        m_render_pass.setVertexBuffer(slot, vertex_buffer.get_unsafe(), 0, vertex_buffer.get_size_bytes());
    }

    void RenderPass::set_index_buffer(const IndexBuffer &index_buffer)
    {
        m_render_pass.setIndexBuffer(index_buffer.get_unsafe(), index_buffer.get_wgpu_format(), 0, index_buffer.get_size_bytes());
    }

    void RenderPass::set_bind_group(Device &device, const u32 group_idx, BindGroup &bind_group, const u32 dynamic_idx)
    {
        const std::vector<u32> dynamic_offsets = bind_group.get_dynamic_offsets(device, dynamic_idx);
        m_render_pass.setBindGroup(group_idx, bind_group.get_unsafe(), dynamic_offsets.size(), dynamic_offsets.data());
    }

    void RenderPass::draw(const u32 vertex_count)
    {
        m_render_pass.draw(vertex_count, 1, 0, 0);
    }

    void RenderPass::draw_indexed(const u32 index_count)
    {
        m_render_pass.drawIndexed(index_count, 1, 0, 0, 0);
    }

    void RenderPass::end(Device &device)
    {
        m_render_pass.end();

        wgpu::CommandBufferDescriptor command_buffer_descriptor = {};
        command_buffer_descriptor.label = "Mellohi Command Buffer";
        wgpu::CommandBuffer command_buffer = m_command_encoder.finish(command_buffer_descriptor);

        wgpu::Queue queue = device.get_queue_unsafe();
        queue.submit(1, &command_buffer);
        queue.release();
        command_buffer.release();
    }

    wgpu::RenderPassEncoder RenderPass::get_unsafe() const
    {
        return m_render_pass;
    }
}
