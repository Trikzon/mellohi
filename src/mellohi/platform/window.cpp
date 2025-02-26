#include "mellohi/platform/window.hpp"

namespace mellohi
{
    FramebufferResizedEvent::FramebufferResizedEvent(const std::shared_ptr<Window> window, const uvec2 framebuffer_size)
        : window(window), framebuffer_size(framebuffer_size)
    {
        
    }
    
    WindowFocusChangedEvent::WindowFocusChangedEvent(std::shared_ptr<Window> window, bool focused)
        : window(window), focused(focused)
    {
        
    }
    
    WindowResizedEvent::WindowResizedEvent(std::shared_ptr<Window> window, uvec2 size)
        : window(window), size(size)
    {
        
    }
}
