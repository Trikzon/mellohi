#include "mellohi/graphics/window.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include "mellohi/log.h"

namespace mellohi
{
    Window::Window()
    {
        MH_ASSERT(glfwInit(), "Failed to initialize GLFW.");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);

        m_glfw_window = glfwCreateWindow(960, 540, "Mellohi", nullptr, nullptr);
        MH_ASSERT(m_glfw_window != nullptr, "Failed to create GLFW window.");

        glfwMakeContextCurrent(m_glfw_window);

        m_vsync_enabled = true;
        glfwSwapInterval(0);

        MH_ASSERT(
            gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)),
            "Failed to load OpenGL with GLAD."
        );

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& imgui_io = ImGui::GetIO();
        imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        ImGui_ImplGlfw_InitForOpenGL(m_glfw_window, true);
        ImGui_ImplOpenGL3_Init();
    }

    Window::~Window()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwTerminate();
    }

    void Window::begin_frame() const
    {
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Window::end_frame() const
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwPollEvents();
        glfwSwapBuffers(m_glfw_window);
    }

    bool Window::is_vsync_enabled() const
    {
        return m_vsync_enabled;
    }

    void Window::set_vsync(const bool enabled)
    {
        m_vsync_enabled = enabled;
        glfwSwapInterval(enabled ? 1 : 0);
    }

    bool Window::should_close() const
    {
        return glfwWindowShouldClose(m_glfw_window);
    }
}
