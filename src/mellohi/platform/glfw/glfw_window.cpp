#ifdef MH_PLATFORM_GLFW

#include "mellohi/core/engine.hpp"
#include "mellohi/events/event_dispatcher.hpp"
#include "mellohi/platform/glfw/glfw.hpp"
#include "mellohi/platform/glfw/glfw_input_buttons.hpp"
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
        glfw.window_key_event_callback(m_data->glfw_window, on_key_event);
        glfw.window_mouse_button_event_callback(m_data->glfw_window, on_mouse_button_event);
        glfw.window_mouse_motion_event_callback(m_data->glfw_window, on_mouse_motion_event);
        glfw.window_resized_callback(m_data->glfw_window, on_window_resized);
    }
    
    fvec2 Window::cursor_pos() const
    {
        return Glfw::get().window_cursor_pos(m_data->glfw_window);
    }
    
    void Window::cursor_pos(const fvec2 cursor_pos)
    {
        Glfw::get().window_cursor_pos(m_data->glfw_window, cursor_pos);
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

    void on_focus_changed(GLFWwindow *glfw_window, const i32 focused)
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

    void on_key_event(GLFWwindow *glfw_window, const i32 key, const i32 scan_code, const i32 action, const i32 mods)
    {
        Window *window = Glfw::get().window_user_pointer<Window>(glfw_window);

        auto &event_dispatcher = Engine::get().event_dispatcher();
        event_dispatcher.dispatch_event<KeyEvent>(KeyEvent{
            window->shared_from_this(), to_mh_key(key), to_mh_button_action(action), to_mh_button_mod_flags(mods)
        });
    }
    
    void on_mouse_button_event(GLFWwindow *glfw_window, const i32 button, const i32 action, const i32 mods)
    {
        Window *window = Glfw::get().window_user_pointer<Window>(glfw_window);
        
        auto &event_dispatcher = Engine::get().event_dispatcher();
        event_dispatcher.dispatch_event<MouseButtonEvent>(MouseButtonEvent{
            window->shared_from_this(),
            to_mh_mouse_button(button), to_mh_button_action(action), to_mh_button_mod_flags(mods)
        });
    }
    
    void on_mouse_motion_event(GLFWwindow *glfw_window, const f64 x_pos, const f64 y_pos)
    {
        static fvec2 last_cursor_pos;
        
        Window *window = Glfw::get().window_user_pointer<Window>(glfw_window);
        
        fvec2 delta = fvec2{x_pos, y_pos} - last_cursor_pos;
        last_cursor_pos = {x_pos, y_pos};
                
        auto &event_dispatcher = Engine::get().event_dispatcher();
        event_dispatcher.dispatch_event<MouseMotionEvent>(MouseMotionEvent{window->shared_from_this(), delta});
    }

    void on_window_resized(GLFWwindow *glfw_window, const i32 width, const i32 height)
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
