#include "mellohi/events/event_dispatcher.hpp"

namespace mellohi
{
    void EventDispatcher::unregister_listener(const EventDispatcher::ListenerId id)
    {
        for (auto &[type, vec] : m_listeners)
        {
            // Moves matching listeners to the end of the vec and returns an it right before them.
            const auto new_end = std::remove_if(vec.begin(), vec.end(), [id](const auto &pair)
            {
                return pair.first == id;
            });
            
            vec.erase(new_end, vec.end());
        }
    }
}
