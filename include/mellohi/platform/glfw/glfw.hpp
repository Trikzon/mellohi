#pragma once

#ifdef MH_PLATFORM_GLFW

#include "mellohi/core/types.hpp"

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
        void window_hint(u32 hint, u32 value);
        bool window_should_close(GLFWwindow *glfw_window);
        
    private:
        Glfw();
        ~Glfw();
    };
}

#endif
