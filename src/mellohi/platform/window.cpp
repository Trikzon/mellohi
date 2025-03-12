#include "mellohi/platform/window.hpp"

namespace mellohi
{
    FramebufferResizedEvent::FramebufferResizedEvent(const std::shared_ptr<Window> window, const uvec2 framebuffer_size)
        : window(window), framebuffer_size(framebuffer_size)
    {

    }

    KeyEvent::KeyEvent(const std::shared_ptr<Window> window, const Key key, const ButtonAction action,
        const ButtonModFlags mod_flags) : window(window), key(key), mod_flags(mod_flags), action(action)
    {

    }

    MouseButtonEvent::MouseButtonEvent(const std::shared_ptr<Window> window, const MouseButton button,
        const ButtonAction action, const ButtonModFlags mod_flags)
        : window(window), button(button), action(action), mod_flags(mod_flags)
    {

    }

    MouseMotionEvent::MouseMotionEvent(std::shared_ptr<Window> window, fvec2 delta) : window(window), delta(delta)
    {

    }

    WindowFocusChangedEvent::WindowFocusChangedEvent(const std::shared_ptr<Window> window, const bool focused)
        : window(window), focused(focused)
    {

    }

    WindowResizedEvent::WindowResizedEvent(const std::shared_ptr<Window> window, const uvec2 size)
        : window(window), size(size)
    {

    }
}

