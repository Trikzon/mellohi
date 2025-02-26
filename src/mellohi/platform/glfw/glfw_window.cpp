#ifdef MH_PLATFORM_GLFW

#include "mellohi/core/engine.hpp"
#include "mellohi/events/event_dispatcher.hpp"
#include "mellohi/platform/glfw/glfw.hpp"
#include "mellohi/platform/glfw/glfw_window.hpp"

namespace mellohi
{
    Window::~Window()
    {
        Glfw::get().destroy_window(m_data->glfw_window);
    }
    
    void Window::initialize()
    {
        auto &glfw = Glfw::get();
        
        glfw.window_user_pointer(m_data->glfw_window, this);
        
        glfw.window_focus_changed_callback(m_data->glfw_window, on_focus_changed);
        glfw.window_framebuffer_resized_callback(m_data->glfw_window, on_framebuffer_resized);
        glfw.window_resized_callback(m_data->glfw_window, on_window_resized);
    }
    
    bool Window::focused() const
    {
        return Glfw::get().window_attribute(m_data->glfw_window, GLFW_FOCUSED);
    }
    
    uvec2 Window::framebuffer_size() const
    {
        return Glfw::get().window_framebuffer_size(m_data->glfw_window);
    }
    
    bool Window::should_close() const
    {
        return Glfw::get().window_should_close(m_data->glfw_window);
    }
    
    uvec2 Window::size() const
    {
        return Glfw::get().window_size(m_data->glfw_window);
    }
    
    void Window::size(const uvec2 size)
    {
        Glfw::get().window_size(m_data->glfw_window, size);
    }
    
    std::string Window::title() const
    {
        return Glfw::get().window_title(m_data->glfw_window);
    }
    
    void Window::title(const std::string &title)
    {
        return Glfw::get().window_title(m_data->glfw_window, title);
    }
    
    Window::Window(const uvec2 size, const bool resizable, const std::string &title)
    {
        m_data = std::make_unique<ImplData>();
        
        auto &glfw = Glfw::get();
        
        glfw.window_hint(GLFW_RESIZABLE, resizable);
        
        #ifdef MH_GRAPHICS_VULKAN
            glfw.window_hint(GLFW_CLIENT_API, GLFW_NO_API);
        #endif
        
        m_data->glfw_window = glfw.create_window(size, title);
    }
    
    void on_focus_changed(GLFWwindow *glfw_window, i32 focused)
    {
        Window *window = Glfw::get().window_user_pointer<Window>(glfw_window);
        
        auto &event_dispatcher = Engine::get().event_dispatcher();
        event_dispatcher.dispatch_event<WindowFocusChangedEvent>(
            WindowFocusChangedEvent{window->shared_from_this(), static_cast<bool>(focused)}
        );
}
    
    void on_framebuffer_resized(GLFWwindow *glfw_window, const i32 width, const i32 height)
    {
        MH_ASSERT(width >= 0 && height >= 0, "GLFW framebuffer width or height is negative.");
        
        Window *window = Glfw::get().window_user_pointer<Window>(glfw_window);
        
        auto &event_dispatcher = Engine::get().event_dispatcher();
        event_dispatcher.dispatch_event<FramebufferResizedEvent>(
            FramebufferResizedEvent{window->shared_from_this(), uvec2{width, height}}
        );
    }
    
    void on_window_resized(GLFWwindow *glfw_window, i32 width, i32 height)
    {
        MH_ASSERT(width >= 0 && height >= 0, "GLFW window width or height is negative.");
        
        Window *window = Glfw::get().window_user_pointer<Window>(glfw_window);
        
        auto &event_dispatcher = Engine::get().event_dispatcher();
        event_dispatcher.dispatch_event<WindowResizedEvent>(
            WindowResizedEvent{window->shared_from_this(), uvec2{width, height}}
        );
    }
}

#endif
