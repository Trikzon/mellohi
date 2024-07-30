#include "mellohi/graphics/glfw/window.hpp"

#include "mellohi/graphics/glfw/glfw.hpp"

namespace mellohi::glfw
{
    Window::Window(const str title, const vec2u size)
    {
        MH_ASSERT(size.x < std::numeric_limits<i32>::max() && size.y < std::numeric_limits<i32>::max(),
                  "Window size is too large.");

        Glfw::get_instance();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        MH_INFO("Creating GLFW window.");
        GLFWwindow *glfw_window = glfwCreateWindow(static_cast<i32>(size.x), static_cast<i32>(size.y),
                                                   string{title}.c_str(), nullptr, nullptr);

        MH_ASSERT(glfw_window != nullptr, "Failed to create GLFW window.");

        m_handle = std::make_shared<Handle>(glfw_window);

        glfwSetWindowUserPointer(*m_handle, m_handle.get());
    }

    auto Window::should_close() const -> bool
    {
        return glfwWindowShouldClose(*m_handle);
    }

    auto Window::get_title() const -> string
    {
        return glfwGetWindowTitle(*m_handle);
    }

    auto Window::set_title(const str title) const -> void
    {
        glfwSetWindowTitle(*m_handle, string{title}.c_str());
    }

    auto Window::get_pos() const -> vec2i
    {
        i32 x, y;
        glfwGetWindowPos(*m_handle, &x, &y);
        return {x, y};
    }

    auto Window::set_pos(const vec2i pos) const -> void
    {
        glfwSetWindowPos(*m_handle, pos.x, pos.y);
    }

    auto Window::get_size() const -> vec2u
    {
        i32 width, height;
        glfwGetWindowSize(*m_handle, &width, &height);
        MH_ASSERT(width >= 0 && height >= 0, "Window size is negative.");
        return {static_cast<u32>(width), static_cast<u32>(height)};
    }

    auto Window::set_size(const vec2u size) const -> void
    {
        MH_ASSERT(size.x < std::numeric_limits<i32>::max() && size.y < std::numeric_limits<i32>::max(),
                  "Attempted to set window size to a too large value.");
        glfwSetWindowSize(*m_handle, static_cast<i32>(size.x), static_cast<i32>(size.y));
    }

    auto Window::get_framebuffer_size() const -> vec2u
    {
        i32 width, height;
        glfwGetFramebufferSize(*m_handle, &width, &height);
        MH_ASSERT(width >= 0 && height >= 0, "Framebuffer size is negative.");
        return {static_cast<u32>(width), static_cast<u32>(height)};
    }

    auto Window::get_opacity() const -> f32
    {
        return glfwGetWindowOpacity(*m_handle);
    }

    auto Window::set_opacity(const f32 opacity) const -> void
    {
        MH_ASSERT(opacity >= 0.0f && opacity <= 1.0f, "Window opacity must be in the range [0, 1].");
        glfwSetWindowOpacity(*m_handle, opacity);
    }

    auto Window::set_framebuffer_size_callback(const FramebufferSizeCallback &callback) const -> void
    {
        m_handle->set_framebuffer_size_callback(callback);
    }

    auto Window::get_raw_ptr() const -> GLFWwindow *
    {
        return *m_handle;
    }

    Window::Handle::Handle(GLFWwindow *glfw_window) : m_glfw_window{glfw_window}
    {
    }

    Window::Handle::~Handle()
    {
        if (m_framebuffer_size_callback != nullptr)
        {
            glfwSetFramebufferSizeCallback(m_glfw_window, nullptr);
            m_framebuffer_size_callback = nullptr;
        }

        if (m_glfw_window != nullptr)
        {
            MH_INFO("Destroying GLFW window.");
            glfwDestroyWindow(m_glfw_window);
            m_glfw_window = nullptr;
        }
    }

    Window::Handle::operator GLFWwindow *() const
    {
        return m_glfw_window;
    }

    auto Window::Handle::set_framebuffer_size_callback(const FramebufferSizeCallback &callback) -> void
    {
        m_framebuffer_size_callback = callback;
        glfwSetFramebufferSizeCallback(m_glfw_window, [](GLFWwindow *window, const i32 width, const i32 height) -> void
        {
            const auto *handle = static_cast<Handle *>(glfwGetWindowUserPointer(window));
            MH_ASSERT(width >= 0 && height >= 0, "Framebuffer size is negative.");
            handle->m_framebuffer_size_callback({static_cast<u32>(width), static_cast<u32>(height)});
        });
    }
}
