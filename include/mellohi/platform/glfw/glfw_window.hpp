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
}

#endif
