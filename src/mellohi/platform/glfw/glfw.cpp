#include "GLFW/glfw3.h"
#ifdef MH_PLATFORM_GLFW

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
    
    fvec2 Glfw::window_cursor_pos(GLFWwindow *glfw_window) const
    {
        f64 x_pos, y_pos;
        glfwGetCursorPos(glfw_window, &x_pos, &y_pos);
        return {x_pos, y_pos};
    }
    
    void Glfw::window_cursor_pos(GLFWwindow *glfw_window, fvec2 cursor_pos)
    {
        glfwSetCursorPos(glfw_window, cursor_pos.x, cursor_pos.y);
    }
    
    u32 Glfw::window_attribute(GLFWwindow *glfw_window, u32 attribute) const
    {
        return glfwGetWindowAttrib(glfw_window, attribute);
    }
    
    void Glfw::window_attribute(GLFWwindow *glfw_window, const u32 attribute, const u32 value)
    {
        glfwSetWindowAttrib(glfw_window, attribute, value);
    }
    
    void Glfw::window_focus_changed_callback(GLFWwindow *glfw_window, GLFWwindowfocusfun callback)
    {
        glfwSetWindowFocusCallback(glfw_window, callback);
    }
    
    uvec2 Glfw::window_framebuffer_size(GLFWwindow *glfw_window) const
    {
        i32 width, height;
        glfwGetFramebufferSize(glfw_window, &width, &height);
        MH_ASSERT(width >= 0 && height >= 0, "GLFW framebuffer width or height is negative.");
        return uvec2{width, height};
    }
    
    void Glfw::window_framebuffer_resized_callback(GLFWwindow *glfw_window, const GLFWframebuffersizefun callback)
    {
        glfwSetFramebufferSizeCallback(glfw_window, callback);
    }
    
    void Glfw::window_hint(const u32 hint, const u32 value)
    {
        glfwWindowHint(hint, value);
    }
    
    void Glfw::window_key_event_callback(GLFWwindow *glfw_window, const GLFWkeyfun callback)
    {
        glfwSetKeyCallback(glfw_window, callback);
    }
    
    void Glfw::window_mouse_button_event_callback(GLFWwindow *glfw_window, const GLFWmousebuttonfun callback)
    {
        glfwSetMouseButtonCallback(glfw_window, callback);
    }
    
    void Glfw::window_mouse_motion_event_callback(GLFWwindow *glfw_window, const GLFWcursorposfun callback)
    {
        glfwSetCursorPosCallback(glfw_window, callback);
    }
    
    bool Glfw::window_should_close(GLFWwindow *glfw_window) const
    {
        return glfwWindowShouldClose(glfw_window);
    }
    
    uvec2 Glfw::window_size(GLFWwindow *glfw_window) const
    {
        i32 width, height;
        glfwGetWindowSize(glfw_window, &width, &height);
        MH_ASSERT(width >= 0 && height >= 0, "GLFW window width or height is negative.");
        return uvec2{width, height};
    }
    
    void Glfw::window_size(GLFWwindow *glfw_window, const uvec2 size)
    {
        glfwSetWindowSize(glfw_window, size.x, size.y);
    }
    
    void Glfw::window_resized_callback(GLFWwindow *glfw_window, const GLFWwindowsizefun callback)
    {
        glfwSetWindowSizeCallback(glfw_window, callback);
    }
    
    std::string Glfw::window_title(GLFWwindow *glfw_window) const
    {
        return glfwGetWindowTitle(glfw_window);
    }
    
    void Glfw::window_title(GLFWwindow *glfw_window, const std::string &title)
    {
        glfwSetWindowTitle(glfw_window, title.c_str());
    }
    
    void Glfw::window_user_pointer(GLFWwindow *glfw_window, void *user_pointer)
    {
        glfwSetWindowUserPointer(glfw_window, user_pointer);
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
