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
    void on_key_event(GLFWwindow *glfw_window, i32 key, i32 scan_code, i32 action, i32 mods);
    void on_mouse_button_event(GLFWwindow *glfw_window, i32 button, i32 action, i32 mods);
    void on_mouse_motion_event(GLFWwindow *glfw_window, f64 x_pos, f64 y_pos);
    void on_window_resized(GLFWwindow *glfw_window, i32 width, i32 height);
}

#endif
