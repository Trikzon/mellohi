#include <cassert>
#include <iostream>
#include <string>

#include <GLFW/glfw3.h>
#include <glfw3webgpu.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_wgpu.h>
#define WEBGPU_CPP_IMPLEMENTATION
#include <webgpu/webgpu.hpp>

wgpu::TextureView get_next_surface_texture_view(wgpu::Surface& surface)
{
    wgpu::SurfaceTexture surface_texture;
    surface.getCurrentTexture(&surface_texture);
    if (surface_texture.status != wgpu::SurfaceGetCurrentTextureStatus::Success)
    {
        return nullptr;
    }
    wgpu::Texture texture = surface_texture.texture;

    wgpu::TextureViewDescriptor view_descriptor;
    view_descriptor.label = "Surface texture view";
    view_descriptor.format = texture.getFormat();
    view_descriptor.dimension = wgpu::TextureViewDimension::_2D;
    view_descriptor.baseMipLevel = 0;
    view_descriptor.mipLevelCount = 1;
    view_descriptor.baseArrayLayer = 0;
    view_descriptor.arrayLayerCount = 1;
    view_descriptor.aspect = wgpu::TextureAspect::All;
    wgpu::TextureView target_view = texture.createView(view_descriptor);

    return target_view;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(640, 480, "Voxelfi", nullptr, nullptr);

    wgpu::Instance instance = wgpuCreateInstance(nullptr);

    wgpu::Surface surface = glfwGetWGPUSurface(instance, window);

    std::cout << "Requesting adapter..." << std::endl;
    wgpu::RequestAdapterOptions adapter_options = {};
    adapter_options.compatibleSurface = surface;
    wgpu::Adapter adapter = instance.requestAdapter(adapter_options);
    std::cout << "Got adapter: " << adapter << std::endl;
    instance.release();

    std::cout << "Requesting device..." << std::endl;
    wgpu::DeviceDescriptor device_descriptor = {};
    device_descriptor.label = "My Device";
    device_descriptor.requiredFeatureCount = 0;
    wgpu::RequiredLimits required_limits = wgpu::Default;
    required_limits.limits.maxBindGroups = 2;
    device_descriptor.requiredLimits = &required_limits;
    device_descriptor.defaultQueue.nextInChain = nullptr;
    device_descriptor.defaultQueue.label = "The default queue";
    device_descriptor.deviceLostCallback = [](WGPUDeviceLostReason reason, const char* message, void*)
    {
        std::cout << "Device lost: reason " << reason;
        if (message) std::cout << " (" << message << ")";
        std::cout << std::endl;
    };
    wgpu::Device device = adapter.requestDevice(device_descriptor);
    std::cout << "Got device: " << device << std::endl;
    adapter.release();

    std::unique_ptr<wgpu::ErrorCallback> uncaptured_error_callback_handle = device.setUncapturedErrorCallback(
        [](wgpu::ErrorType type, const char* message)
        {
            std::cout << "Uncaptured device error: type " << type;
            if (message) std::cout << " (" << message << ")";
            std::cout << std::endl;
        }
    );

    wgpu::Queue queue = device.getQueue();

    wgpu::SurfaceConfiguration surface_config = {};
    surface_config.width = 640;
    surface_config.height = 480;
    surface_config.usage = wgpu::TextureUsage::RenderAttachment;
    wgpu::TextureFormat surface_format = surface.getPreferredFormat(adapter);
    surface_config.format = surface_format;

    surface_config.viewFormatCount = 0;
    surface_config.viewFormats = nullptr;
    surface_config.device = device;
    surface_config.presentMode = wgpu::PresentMode::Fifo;
    surface_config.alphaMode = wgpu::CompositeAlphaMode::Auto;

    surface.configure(surface_config);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO();

    ImGui_ImplGlfw_InitForOther(window, true);
    ImGui_ImplWGPU_InitInfo init_info;
    init_info.Device = device;
    init_info.RenderTargetFormat = surface_format;
    ImGui_ImplWGPU_Init(&init_info);

    std::string shader_code = R"(
        @vertex
        fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4f {
            var p = vec2f(0.0, 0.0);
            if (in_vertex_index == 0u) {
                p = vec2f(-0.5, -0.5);
            } else if (in_vertex_index == 1u) {
                p = vec2f(0.5, -0.5);
            } else {
                p = vec2f(0.0, 0.5);
            }
            return vec4f(p, 0.0, 1.0);
        }

        @fragment
        fn fs_main() -> @location(0) vec4f {
            return vec4f(0.0, 0.4, 1.0, 1.0);
        }
    )";

    wgpu::ShaderModuleDescriptor shader_module_descriptor;
    wgpu::ShaderModuleWGSLDescriptor shader_module_wgsl_descriptor;
    shader_module_wgsl_descriptor.chain.next = nullptr;
    shader_module_wgsl_descriptor.chain.sType = wgpu::SType::ShaderModuleWGSLDescriptor;
    shader_module_wgsl_descriptor.code = shader_code.c_str();
    shader_module_descriptor.nextInChain = &shader_module_wgsl_descriptor.chain;

    wgpu::ShaderModule shader_module = device.createShaderModule(shader_module_descriptor);

    wgpu::RenderPipelineDescriptor pipeline_descriptor;

    pipeline_descriptor.vertex.bufferCount = 0;
    pipeline_descriptor.vertex.buffers = nullptr;
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
    color_target.format = surface_format;
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

    wgpu::RenderPipeline pipeline = device.createRenderPipeline(pipeline_descriptor);
    shader_module.release();

    // ===== Main Loop =====
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        wgpu::TextureView target_view = get_next_surface_texture_view(surface);
        if (target_view == nullptr)
        {
            std::cout << "Skipped frame..." << std::endl;
            continue;
        }

        wgpu::CommandEncoderDescriptor encoder_descriptor = {};
        encoder_descriptor.label = "My command encoder";
        wgpu::CommandEncoder encoder = device.createCommandEncoder(encoder_descriptor);

        wgpu::RenderPassDescriptor render_pass_descriptor = {};

        wgpu::RenderPassColorAttachment render_pass_color_attachment = {};
        render_pass_color_attachment.view = target_view;
        render_pass_color_attachment.resolveTarget = nullptr;
        render_pass_color_attachment.loadOp = wgpu::LoadOp::Clear;
        render_pass_color_attachment.storeOp = wgpu::StoreOp::Store;
        render_pass_color_attachment.clearValue = wgpu::Color{0.1, 0.1, 0.3, 1.0};
        render_pass_color_attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

        render_pass_descriptor.colorAttachmentCount = 1;
        render_pass_descriptor.colorAttachments = &render_pass_color_attachment;
        render_pass_descriptor.depthStencilAttachment = nullptr;
        render_pass_descriptor.timestampWrites = nullptr;

        wgpu::RenderPassEncoder render_pass = encoder.beginRenderPass(render_pass_descriptor);

        render_pass.setPipeline(pipeline);
        render_pass.draw(3, 1, 0, 0);

        ImGui_ImplWGPU_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

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

        target_view.release();

        surface.present();
        device.tick();
    }

    // ===== Terminate =====
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplWGPU_Shutdown();

    pipeline.release();
    surface.unconfigure();
    queue.release();
    surface.release();
    device.release();
    glfwDestroyWindow(window);
    glfwTerminate();
}
