#pragma once

typedef struct GLFWwindow GLFWwindow;

namespace mellohi
{
    class Window
    {
    public:
        Window();
        ~Window();

        void begin_frame() const;
        void end_frame() const;

        bool is_vsync_enabled() const;
        void set_vsync(bool enabled);

        bool should_close() const;

    private:
        GLFWwindow* m_glfw_window;
        bool m_vsync_enabled;
    };
}
