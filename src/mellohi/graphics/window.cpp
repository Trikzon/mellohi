#include "mellohi/graphics/window.h"

#include <GLFW/glfw3.h>
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
        m_surface->configure(width, height, *m_device, false);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::GetIO();

        ImGui_ImplGlfw_InitForOther(m_glfw_window, true);
        ImGui_ImplWGPU_InitInfo init_info;
        init_info.Device = m_device->get_unsafe();
        init_info.RenderTargetFormat = m_surface->get_wgpu_texture_format();
        init_info.DepthStencilFormat = wgpu::TextureFormat::Depth24Plus;
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

    Window::Window(Window &&other) noexcept : m_glfw_window(nullptr)
    {
        std::swap(m_glfw_window, other.m_glfw_window);
        std::swap(m_surface, other.m_surface);
        std::swap(m_device, other.m_device);
    }

    Window & Window::operator=(Window &&other) noexcept
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

    std::optional<RenderPass> Window::begin_frame() const
    {
        glfwPollEvents();

        auto current_surface_texture = get_surface().get_current_wgpu_texture_unsafe();
        if (!current_surface_texture)
        {
            std::cout << "Skipped frame." << std::endl;

            return std::nullopt;
        }

        RenderPass render_pass(*m_device, *current_surface_texture, vec3f(0.05f));

        current_surface_texture->release();

        ImGui_ImplWGPU_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        return render_pass;
    }

    void Window::end_frame(RenderPass &render_pass) const
    {
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), render_pass.get_unsafe());

        render_pass.end(*m_device);

        m_surface->present();
        m_device->tick();
    }

    Surface & Window::get_surface() const
    {
        return *m_surface;
    }

    Device & Window::get_device() const
    {
        return *m_device;
    }
}
