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

        m_surface->configure(640, 480, *m_device, true);

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

    void Window::begin_frame()
    {
        glfwPollEvents();

        ImGui_ImplWGPU_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Window::end_frame()
    {
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
