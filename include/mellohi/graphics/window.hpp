#pragma once

#include "mellohi/graphics/render_pass.hpp"

struct GLFWwindow;

namespace mellohi
{
    class Window
    {
    public:
        Window();
        ~Window();

        Window(const Window &other) = delete;
        Window(Window &&other) noexcept;
        Window & operator=(const Window &other) = delete;
        Window & operator=(Window &&other) noexcept;

        bool should_close() const;
        std::optional<RenderPass> begin_frame() const;
        void end_frame(RenderPass &render_pass) const;

        Surface & get_surface() const;
        Device & get_device() const;

        GLFWwindow * get_raw() const;

    private:
        GLFWwindow *m_glfw_window;
        Surface *m_surface;
        Device *m_device;
    };
}
