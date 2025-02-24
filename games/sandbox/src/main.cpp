#include <mellohi/core/engine.hpp>
#include <mellohi/core/logger.hpp>

using namespace mellohi;

struct TestEvent : public Event
{
    const f32 x, y;
    
    TestEvent(const f32 x, const f32 y) : x(x), y(y) {}
};

int main()
{
    MH_INFO("Hello, world!");
    
    auto &engine = Engine::get();
    
    auto &event_dispatcher = engine.event_dispatcher();
    
    const auto l_id = event_dispatcher.register_listener<TestEvent>([](const TestEvent &event)
    {
        MH_INFO("Event: {}, {}", event.x, event.y);
    });
    
    event_dispatcher.dispatch_event<TestEvent>(TestEvent{1.0f, 2.0f});
    
    event_dispatcher.unregister_listener(l_id);
    
    event_dispatcher.dispatch_event<TestEvent>(TestEvent{3.0f, 4.0f});
    
    return 0;
}
