#include "mellohi/graphics/render_pass.h"

namespace mellohi
{
    RenderPass::RenderPass(Device &device, wgpu::TextureView wgpu_texture_view, const float r, const float g, const float b)
    {
        wgpu::CommandEncoderDescriptor command_encoder_descriptor = {};
        command_encoder_descriptor.label = "Mellohi Command Encoder";
        m_command_encoder = device.create_command_encoder_unsafe(command_encoder_descriptor);

        wgpu::RenderPassColorAttachment render_pass_color_attachment = {};
        render_pass_color_attachment.view = wgpu_texture_view;
        render_pass_color_attachment.resolveTarget = nullptr;
        render_pass_color_attachment.loadOp = wgpu::LoadOp::Clear;
        render_pass_color_attachment.storeOp = wgpu::StoreOp::Store;
        render_pass_color_attachment.clearValue = wgpu::Color(r, g, b, 1.0);
        render_pass_color_attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

        wgpu::RenderPassDescriptor render_pass_descriptor = {};
        render_pass_descriptor.colorAttachmentCount = 1;
        render_pass_descriptor.colorAttachments = &render_pass_color_attachment;
        render_pass_descriptor.depthStencilAttachment = nullptr;
        render_pass_descriptor.timestampWrites = nullptr;
        m_render_pass = m_command_encoder.beginRenderPass(render_pass_descriptor);
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

    void RenderPass::set_vertex_buffer(const size_t slot, const VertexBuffer &vertex_buffer)
    {
        m_render_pass.setVertexBuffer(slot, vertex_buffer.get_unsafe(), 0, vertex_buffer.get_size_bytes());
    }

    void RenderPass::set_index_buffer(const IndexBuffer &index_buffer)
    {
        m_render_pass.setIndexBuffer(index_buffer.get_unsafe(), index_buffer.get_wgpu_format(), 0, index_buffer.get_size_bytes());
    }

    void RenderPass::set_bind_group(const BindGroup &bind_group)
    {
        m_render_pass.setBindGroup(0, bind_group.get_unsafe(), 0, nullptr);
    }

    void RenderPass::draw_indexed(const size_t index_count)
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
