#pragma once

#ifdef MH_PLATFORM_GLFW

#include "mellohi/platform/window.hpp"

#include <GLFW/glfw3.h>

namespace mellohi
{
    struct Window::ImplData
    {
        GLFWwindow *glfw_window;
    };
    
    void on_focus_changed(GLFWwindow *glfw_window, i32 focused);
    void on_framebuffer_resized(GLFWwindow *glfw_window, i32 width, i32 height);
    void on_window_resized(GLFWwindow *glfw_window, i32 width, i32 height);
}

#endif
