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
    event_dispatcher.register_listener<FramebufferResizedEvent>([](const FramebufferResizedEvent &event)
    {
        event.window->title(std::format("Framebuffer size: {}, {}", event.framebuffer_size.x, event.framebuffer_size.y));
    });
    
    engine.run();
    
    return 0;
}
