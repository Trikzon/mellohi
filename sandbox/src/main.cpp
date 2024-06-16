#include <cassert>
#include <iostream>
#include <string>

#include <imgui.h>
#include <backends/imgui_impl_wgpu.h>

#include "mellohi/graphics/window.h"

int main()
{
    mellohi::Window window;
    mellohi::Device& device = window.get_device();

    // wgpu::Queue queue = window.get_queue();
    wgpu::Queue queue = device.get_queue_unsafe();

    std::vector<float> vertex_data = {
        -0.5, -0.5,     1.0, 0.0, 0.0,
        +0.5, -0.5,     0.0, 1.0, 0.0,
        +0.5, +0.5,     0.0, 0.0, 1.0,
        -0.5, +0.5,     1.0, 1.0, 0.0,
    };

    std::vector<uint16_t> index_data = {
        0, 1, 2,  // Triangle 0
        0, 2, 3,  // Triangle 1
    };
    auto index_count = static_cast<uint32_t>(index_data.size());

    wgpu::BufferDescriptor buffer_descriptor;
    buffer_descriptor.size = vertex_data.size() * sizeof(float);
    buffer_descriptor.size = (buffer_descriptor.size + 3) & ~3;  // round up to the next multiple of 4
    buffer_descriptor.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex;
    buffer_descriptor.mappedAtCreation = false;
    // wgpu::Buffer vertex_buffer = window.create_buffer(buffer_descriptor);
    wgpu::Buffer vertex_buffer = device.create_buffer_unsafe(buffer_descriptor);

    queue.writeBuffer(vertex_buffer, 0, vertex_data.data(), buffer_descriptor.size);

    buffer_descriptor.size = index_data.size() * sizeof(uint16_t);
    buffer_descriptor.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index;
    // wgpu::Buffer index_buffer = window.create_buffer(buffer_descriptor);
    wgpu::Buffer index_buffer = device.create_buffer_unsafe(buffer_descriptor);

    queue.writeBuffer(index_buffer, 0, index_data.data(), buffer_descriptor.size);

    wgpu::VertexBufferLayout vertex_buffer_layout;
    std::vector<wgpu::VertexAttribute> vertex_attributes(2);

    // Position attribute
    vertex_attributes[0].shaderLocation = 0;
    vertex_attributes[0].format = wgpu::VertexFormat::Float32x2;
    vertex_attributes[0].offset = 0;

    // Color attribute
    vertex_attributes[1].shaderLocation = 1;
    vertex_attributes[1].format = wgpu::VertexFormat::Float32x3;
    vertex_attributes[1].offset = 2 * sizeof(float);

    vertex_buffer_layout.attributeCount = static_cast<uint32_t>(vertex_attributes.size());
    vertex_buffer_layout.attributes = vertex_attributes.data();
    vertex_buffer_layout.arrayStride = 5 * sizeof(float);
    vertex_buffer_layout.stepMode = wgpu::VertexStepMode::Vertex;

    std::string shader_code = R"(
        struct VertexInput {
            @location(0) position: vec2f,
            @location(1) color: vec3f,
        }

        struct FragmentInput {
            @builtin(position) position: vec4f,
            @location(0) color: vec3f,
        }

        @vertex
        fn vs_main(in: VertexInput) -> FragmentInput {
            var out: FragmentInput;
            let ratio = 640.0 / 480.0;
            out.position += vec4f(in.position.x, in.position.y * ratio, 0.0, 1.0);
            out.color = in.color;
            return out;
        }

        @fragment
        fn fs_main(in: FragmentInput) -> @location(0) vec4f {
            return vec4f(in.color, 1.0);
        }
    )";

    wgpu::ShaderModuleDescriptor shader_module_descriptor;
    wgpu::ShaderModuleWGSLDescriptor shader_module_wgsl_descriptor;
    shader_module_wgsl_descriptor.chain.next = nullptr;
    shader_module_wgsl_descriptor.chain.sType = wgpu::SType::ShaderModuleWGSLDescriptor;
    shader_module_wgsl_descriptor.code = shader_code.c_str();
    shader_module_descriptor.nextInChain = &shader_module_wgsl_descriptor.chain;

    wgpu::ShaderModule shader_module = device.create_shader_module_unsafe(shader_module_descriptor);

    wgpu::RenderPipelineDescriptor pipeline_descriptor;

    pipeline_descriptor.vertex.bufferCount = 1;
    pipeline_descriptor.vertex.buffers = &vertex_buffer_layout;
    pipeline_descriptor.vertex.module = shader_module;
    pipeline_descriptor.vertex.entryPoint = "vs_main";
    pipeline_descriptor.vertex.constantCount = 0;
    pipeline_descriptor.vertex.constants = nullptr;

    pipeline_descriptor.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
    pipeline_descriptor.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
    pipeline_descriptor.primitive.frontFace = wgpu::FrontFace::CCW;
    pipeline_descriptor.primitive.cullMode = wgpu::CullMode::None;

    wgpu::FragmentState fragment_state;
    fragment_state.module = shader_module;
    fragment_state.entryPoint = "fs_main";
    fragment_state.constantCount = 0;
    fragment_state.constants = nullptr;

    wgpu::BlendState blend_state;
    blend_state.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
    blend_state.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
    blend_state.color.operation = wgpu::BlendOperation::Add;
    blend_state.alpha.srcFactor = wgpu::BlendFactor::Zero;
    blend_state.alpha.dstFactor = wgpu::BlendFactor::One;
    blend_state.alpha.operation = wgpu::BlendOperation::Add;

    wgpu::ColorTargetState color_target;
    color_target.format = window.get_surface().get_texture_format();
    color_target.blend = &blend_state;
    color_target.writeMask = wgpu::ColorWriteMask::All;

    fragment_state.targetCount = 1;
    fragment_state.targets = &color_target;

    pipeline_descriptor.fragment = &fragment_state;

    pipeline_descriptor.depthStencil = nullptr;

    pipeline_descriptor.multisample.count = 1;
    pipeline_descriptor.multisample.mask = ~0u;
    pipeline_descriptor.multisample.alphaToCoverageEnabled = false;

    pipeline_descriptor.layout = nullptr;

    wgpu::RenderPipeline pipeline = device.create_render_pipeline_unsafe(pipeline_descriptor);
    shader_module.release();

    // ===== Main Loop =====
    while (!window.should_close())
    {
        window.begin_frame();

        auto target_view = window.get_surface().get_next_texture_view_unsafe();
        if (!target_view)
        {
            std::cout << "Skipped frame..." << std::endl;
            continue;
        }

        wgpu::CommandEncoderDescriptor encoder_descriptor = {};
        encoder_descriptor.label = "My command encoder";
        wgpu::CommandEncoder encoder = device.create_command_encoder_unsafe(encoder_descriptor);

        wgpu::RenderPassDescriptor render_pass_descriptor = {};

        wgpu::RenderPassColorAttachment render_pass_color_attachment = {};
        render_pass_color_attachment.view = *target_view;
        render_pass_color_attachment.resolveTarget = nullptr;
        render_pass_color_attachment.loadOp = wgpu::LoadOp::Clear;
        render_pass_color_attachment.storeOp = wgpu::StoreOp::Store;
        render_pass_color_attachment.clearValue = wgpu::Color{0.05, 0.05, 0.05, 1.0};
        render_pass_color_attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

        render_pass_descriptor.colorAttachmentCount = 1;
        render_pass_descriptor.colorAttachments = &render_pass_color_attachment;
        render_pass_descriptor.depthStencilAttachment = nullptr;
        render_pass_descriptor.timestampWrites = nullptr;

        wgpu::RenderPassEncoder render_pass = encoder.beginRenderPass(render_pass_descriptor);

        render_pass.setPipeline(pipeline);

        render_pass.setVertexBuffer(0, vertex_buffer, 0, vertex_data.size() * sizeof(float));
        render_pass.setIndexBuffer(index_buffer, wgpu::IndexFormat::Uint16, 0, index_data.size() * sizeof(uint16_t));

        render_pass.drawIndexed(index_count, 1, 0, 0, 0);

        ImGui::ShowMetricsWindow();

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), render_pass);

        render_pass.end();
        render_pass.release();

        wgpu::CommandBufferDescriptor command_buffer_descriptor = {};
        command_buffer_descriptor.label = "Command buffer";
        wgpu::CommandBuffer command_buffer = encoder.finish(command_buffer_descriptor);
        encoder.release();

        queue.submit(1, &command_buffer);
        command_buffer.release();

        target_view->release();

        window.end_frame();
    }

    // ===== Terminate =====
    vertex_buffer.release();
    index_buffer.release();

    pipeline.release();
    queue.release();
}
