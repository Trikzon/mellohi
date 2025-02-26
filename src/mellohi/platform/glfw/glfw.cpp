#ifdef MH_PLATFORM_GLFW

#include "mellohi/core/logger.hpp"
#include "mellohi/platform/glfw/glfw.hpp"

namespace mellohi
{
    Glfw & Glfw::get()
    {
        static Glfw instance;
        return instance;
    }
    
    GLFWwindow * Glfw::create_window(const uvec2 size, const std::string &title)
    {
        return glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);
    }
    
    void Glfw::destroy_window(GLFWwindow *glfw_window)
    {
        glfwDestroyWindow(glfw_window);
    }
    
    void Glfw::poll_events()
    {
        glfwPollEvents();
    }
    
    void Glfw::window_hint(const u32 hint, const u32 value)
    {
        glfwWindowHint(hint, value);
    }
    
    bool Glfw::window_should_close(GLFWwindow *glfw_window)
    {
        return glfwWindowShouldClose(glfw_window);
    }
    
    Glfw::Glfw()
    {
        MH_ASSERT(glfwInit(), "Failed to initialize GLFW.");
    }
    
    Glfw::~Glfw()
    {
        glfwTerminate();
    }
}

#endif
