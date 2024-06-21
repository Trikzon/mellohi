#include "mellohi/graphics/window.h"

#include <iostream>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_wgpu.h>

namespace mellohi
{
    Window::Window()
    {
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW." << std::endl;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_glfw_window = glfwCreateWindow(640, 480, "Mellohi", nullptr, nullptr);

        if (m_glfw_window == nullptr)
        {
            std::cerr << "Failed to create GLFW window." << std::endl;
        }

        wgpu::Instance instance = wgpuCreateInstance(nullptr);
        if (instance == nullptr)
        {
            std::cerr << "Failed to create WebGPU instance." << std::endl;
        }

        m_surface = std::make_unique<Surface>(instance, m_glfw_window);

        Adapter adapter(instance, *m_surface);
        instance.release();

        m_device = std::make_unique<Device>(adapter);

        int width, height;
        glfwGetFramebufferSize(m_glfw_window, &width, &height);
        m_surface->configure(width, height, *m_device, true);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::GetIO();

        ImGui_ImplGlfw_InitForOther(m_glfw_window, true);
        ImGui_ImplWGPU_InitInfo init_info;
        init_info.Device = m_device->get_unsafe();
        init_info.RenderTargetFormat = m_surface->get_texture_format();
        ImGui_ImplWGPU_Init(&init_info);
    }

    Window::~Window()
    {
        if (m_glfw_window != nullptr)
        {
            ImGui_ImplGlfw_Shutdown();
            ImGui_ImplWGPU_Shutdown();

            glfwDestroyWindow(m_glfw_window);
            glfwTerminate();
        }
    }

    Window::Window(Window&& other) noexcept : m_glfw_window(nullptr)
    {
        std::swap(m_glfw_window, other.m_glfw_window);
        std::swap(m_surface, other.m_surface);
        std::swap(m_device, other.m_device);
    }

    Window& Window::operator=(Window&& other) noexcept
    {
        if (this != &other)
        {
            std::swap(m_glfw_window, other.m_glfw_window);
            std::swap(m_surface, other.m_surface);
            std::swap(m_device, other.m_device);
        }

        return *this;
    }

    bool Window::should_close() const
    {
        return glfwWindowShouldClose(m_glfw_window);
    }

    std::optional<std::tuple<wgpu::CommandEncoder, wgpu::RenderPassEncoder>> Window::begin_frame() const
    {
        glfwPollEvents();

        auto current_surface_texture = get_surface().get_next_texture_view_unsafe();
        if (!current_surface_texture)
        {
            std::cout << "Skipped frame." << std::endl;

            return std::nullopt;
        }

        wgpu::CommandEncoderDescriptor command_encoder_descriptor = {};
        command_encoder_descriptor.label = "Mellohi Command Encoder";
        wgpu::CommandEncoder command_encoder = get_device().create_command_encoder_unsafe(command_encoder_descriptor);

        wgpu::RenderPassColorAttachment render_pass_color_attachment = {};
        render_pass_color_attachment.view = *current_surface_texture;
        render_pass_color_attachment.resolveTarget = nullptr;
        render_pass_color_attachment.loadOp = wgpu::LoadOp::Clear;
        render_pass_color_attachment.storeOp = wgpu::StoreOp::Store;
        render_pass_color_attachment.clearValue = wgpu::Color(0.05, 0.05, 0.05, 1.0);
        render_pass_color_attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

        wgpu::RenderPassDescriptor render_pass_descriptor = {};
        render_pass_descriptor.colorAttachmentCount = 1;
        render_pass_descriptor.colorAttachments = &render_pass_color_attachment;
        render_pass_descriptor.depthStencilAttachment = nullptr;
        render_pass_descriptor.timestampWrites = nullptr;
        wgpu::RenderPassEncoder render_pass = command_encoder.beginRenderPass(render_pass_descriptor);

        current_surface_texture->release();

        ImGui_ImplWGPU_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        return std::tuple(command_encoder, render_pass);
    }

    void Window::end_frame(wgpu::CommandEncoder command_encoder, wgpu::RenderPassEncoder render_pass) const
    {
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), render_pass);

        render_pass.end();
        render_pass.release();

        wgpu::CommandBufferDescriptor command_buffer_descriptor = {};
        command_buffer_descriptor.label = "Mellohi Command Buffer";
        wgpu::CommandBuffer command_buffer = command_encoder.finish(command_buffer_descriptor);
        command_encoder.release();

        wgpu::Queue queue = get_device().get_queue_unsafe();
        queue.submit(1, &command_buffer);
        queue.release();
        command_buffer.release();

        m_surface->present();
        m_device->tick();
    }

    Surface& Window::get_surface() const
    {
        return *m_surface;
    }

    Device& Window::get_device() const
    {
        return *m_device;
    }
}
