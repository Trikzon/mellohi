#include <mellohi/core/engine.hpp>
#include <mellohi/core/logger.hpp>
#include <mellohi/events/event_dispatcher.hpp>
#include <mellohi/platform/window.hpp>

using namespace mellohi;

int main()
{
    MH_INFO("Hello, world!");

    auto &engine = Engine::get();
    engine.initialize();

    auto &event_dispatcher = engine.event_dispatcher();
    event_dispatcher.register_listener<WindowFocusChangedEvent>([](const WindowFocusChangedEvent &event)
    {
        event.window->title(std::format("Focused: {}", event.focused));
    });
    event_dispatcher.register_listener<KeyEvent>([](const KeyEvent &event)
    {
        event.window->title(format("{}, {}, {}", event.key, event.action, event.mod_flags));
    });
    event_dispatcher.register_listener<MouseButtonEvent>([](const MouseButtonEvent &event)
    {
        event.window->title(format("{}, {}, {}", event.button, event.action, event.mod_flags));
    });
    event_dispatcher.register_listener<MouseMotionEvent>([](const MouseMotionEvent &event)
    {
        event.window->title(format("{}, {}", event.delta.x, event.delta.y)); 
    });

    engine.run();

    return 0;
}

