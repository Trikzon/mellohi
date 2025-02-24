#pragma once

#include "mellohi/core/types.hpp"

#include <typeindex>
#include <unordered_map>

namespace mellohi
{
    class Event;
    
    class EventDispatcher
    {
    public:
        using ListenerId = usize;
        using Callback = std::function<void(const Event &)>;
        
        template<typename T>
        ListenerId register_listener(std::function<void(const T &)> listener);
        void unregister_listener(ListenerId id);
        template<typename T>
        void dispatch_event(const Event &event);
        
    private:
        std::unordered_map<std::type_index, std::vector<std::pair<ListenerId, Callback>>> m_listeners;
        ListenerId m_next_listener_id = 0;
    };
    
    template<typename T>
    EventDispatcher::ListenerId EventDispatcher::register_listener(std::function<void(const T &)> listener)
    {
        const std::type_index type = typeid(T);
        
        const auto cast_wrapper = [listener](const Event& event)
        {
            listener(static_cast<const T&>(event));
        };
        m_listeners[type].emplace_back(m_next_listener_id, cast_wrapper);
        
        return m_next_listener_id++;
    }
    
    template<typename T>
    void EventDispatcher::dispatch_event(const Event &event)
    {
        const std::type_index type = typeid(T);
        if (m_listeners.contains(type))
        {
            for (const auto &[id, listener] : m_listeners[type])
            {
                listener(event);
            }
        }
    }
}
