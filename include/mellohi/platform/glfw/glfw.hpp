#pragma once

#ifdef MH_PLATFORM_GLFW

#include "mellohi/core/logger.hpp"

#include <GLFW/glfw3.h>

namespace mellohi
{
    class Glfw
    {
    public:
        static Glfw & get();
        
        Glfw(const Glfw &other) = delete;
        void operator=(const Glfw &other) = delete;
        
        GLFWwindow * create_window(uvec2 size, const std::string &title);
        void destroy_window(GLFWwindow *glfw_window);
        void poll_events();
        uvec2 window_framebuffer_size(GLFWwindow *glfw_window) const;
        void window_framebuffer_size_callback(GLFWwindow *glfw_window, GLFWframebuffersizefun callback);
        void window_hint(u32 hint, u32 value);
        bool window_should_close(GLFWwindow *glfw_window) const;
        uvec2 window_size(GLFWwindow *glfw_window) const;
        void window_size(GLFWwindow *glfw_window, uvec2 size);
        std::string window_title(GLFWwindow *glfw_window) const;
        void window_title(GLFWwindow *glfw_window, const std::string &title);
        template<typename T>
        T * window_user_pointer(GLFWwindow *glfw_window);
        void window_user_pointer(GLFWwindow *glfw_window, void *user_pointer);
        
    private:
        Glfw();
        ~Glfw();
    };
    
    template<typename T>
    T * Glfw::window_user_pointer(GLFWwindow *glfw_window)
    {
        void * user_pointer = glfwGetWindowUserPointer(glfw_window);
        MH_ASSERT(user_pointer != nullptr, "GLFW window user pointer is null.");
        
        return static_cast<T *>(user_pointer);
    }
}

#endif
