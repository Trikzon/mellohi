#pragma once

#include "mellohi/core/types.hpp"
#include "mellohi/events/event.hpp"
#include "mellohi/platform/input_buttons.hpp"

#include <memory>

namespace mellohi
{
    class Window : public std::enable_shared_from_this<Window>
    {
    public:
        ~Window();
        
        void initialize();
        
        fvec2 cursor_pos() const;
        void cursor_pos(fvec2 cursor_pos);
        bool focused() const;
        uvec2 framebuffer_size() const;
        bool should_close() const;
        uvec2 size() const;
        void size(uvec2 size);
        std::string title() const;
        void title(const std::string &title);
    
    private:
        friend class Platform;
        
        struct ImplData;
        
        std::unique_ptr<ImplData> m_data;
        
        Window(uvec2 size, bool resizable, const std::string &title);
    };
    
    struct FramebufferResizedEvent : public Event
    {
        std::shared_ptr<Window> window;
        uvec2 framebuffer_size;
        
        FramebufferResizedEvent(std::shared_ptr<Window> window, uvec2 framebuffer_size);
    };
    
    struct KeyEvent : public Event
    {
        std::shared_ptr<Window> window;
        Key key;
        ButtonAction action;
        ButtonModFlags mod_flags;
        
        KeyEvent(std::shared_ptr<Window> window, Key key, ButtonAction action, ButtonModFlags mod_flags);
    };
    
    struct MouseButtonEvent : public Event
    {
        std::shared_ptr<Window> window;
        MouseButton button;
        ButtonAction action;
        ButtonModFlags mod_flags;
        
        MouseButtonEvent(std::shared_ptr<Window> window, MouseButton button, ButtonAction action,
            ButtonModFlags mod_flags);
    };
    
    struct MouseMotionEvent : public Event
    {
        std::shared_ptr<Window> window;
        fvec2 delta;
        
        MouseMotionEvent(std::shared_ptr<Window> window, fvec2 delta);
    };
    
    struct WindowFocusChangedEvent : public Event
    {
        std::shared_ptr<Window> window;
        bool focused;
        
        WindowFocusChangedEvent(std::shared_ptr<Window> window, bool focused);
    };
    
    struct WindowResizedEvent : public Event
    {
        std::shared_ptr<Window> window;
        uvec2 size;
        
        WindowResizedEvent(std::shared_ptr<Window> window, uvec2 size);
    };
}
