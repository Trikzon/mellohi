#pragma once

#include <webgpu/webgpu.hpp>

#include "device.h"

struct GLFWwindow;

namespace mellohi
{
    class Window
    {
    public:
        Window();
        ~Window();

        Window(const Window& other) = delete;
        Window(Window&& other) noexcept;
        Window& operator=(const Window& other) = delete;
        Window& operator=(Window&& other) noexcept;

        bool should_close() const;
        void begin_frame();
        void end_frame();

        Surface& get_surface() const;
        Device& get_device() const;

    private:
        GLFWwindow* m_glfw_window;
        std::unique_ptr<Surface> m_surface;
        std::unique_ptr<Device> m_device;
    };
}
