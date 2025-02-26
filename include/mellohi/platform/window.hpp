#pragma once

#include "mellohi/core/types.hpp"
#include "mellohi/events/event.hpp"

#include <memory>

namespace mellohi
{
    class Window : public std::enable_shared_from_this<Window>
    {
    public:
        ~Window();
        
        void initialize();
        
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
}
